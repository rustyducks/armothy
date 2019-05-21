/*
 * DCMotor.cpp
 *
 *  Created on: 3 déc. 2018
 *      Author: guilhem
 */

#include "DCMotor.h"
#include "params.h"
#include "utils.h"

//#define sig(x) ((x) > 0) ? HIGH : LOW

constexpr float DCMotor::INC_PER_MM;
constexpr float DCMotor::KP;
constexpr float DCMotor::KI;
constexpr float DCMotor::KD;
constexpr float DCMotor::MIN_INTEGRAL;
constexpr float DCMotor::MAX_INTEGRAL;
constexpr float DCMotor::_Z_MAX;
constexpr float DCMotor::KP_SPEED;
constexpr float DCMotor::KI_SPEED;

DCMotor * dcmotor = nullptr;

volatile uint32_t v_read;   // Some variables have to be read before they can be changed.
	//Needs to be global volatile otherwise the compiler will optimize the read.

static inline int readZInc() {
	noInterrupts();	    // Variables can change if FTM interrrupt occurs
	uint16_t count16 = FTM1_CNT;
	interrupts();   // Turn interrupts back on before return
	return count16 - COUNTER_ZERO_VALUE;
}

DCMotor::DCMotor() : _prev_inc(0),
		_goal(0), _speed(0), _prevError(0), _integralError(0),
		_speedSetPoint(0), _speedControlActivated(true), _limitReached(false), _intSpeedError(0),
		_current(0){
}

void DCMotor::stop(){
	_speedSetPoint = 0;
	_goal = readZInc() / INC_PER_MM;
	_prevError = 0;
	_integralError = 0;
	_intSpeedError = 0;
	digitalWrite(VERTICAL_MOTOR_A, 0);
	digitalWrite(VERTICAL_MOTOR_B, 0);
}

void DCMotor::go_to(float position){
	_goal = clamp((float)0, position, _Z_MAX);
}

float DCMotor::get_position(){
	return readZInc() / INC_PER_MM;
}

bool DCMotor::isMoving(){
	if(_speedControlActivated) {
		return abs(_speedSetPoint) >= 1.0;
	} else {
		return abs(_goal - readZInc()/INC_PER_MM) > 1.0;
	}
			//TODO 0.2
}

void DCMotor::setup(){
	dcmotor = this;
	pinMode(VERTICAL_MOTOR_EF, INPUT_PULLUP);
	pinMode(VERTICAL_MOTOR_A, OUTPUT);
	pinMode(VERTICAL_MOTOR_B, OUTPUT);
	setMotorCommand(0);
	pinMode(VERTICAL_LIMIT_SWITCH, INPUT_PULLUP);
	pinMode(MOT_Z_CURRENT_SENSOR, INPUT);
	attachInterrupt(VERTICAL_LIMIT_SWITCH, ISR_LIMIT_SWITCH, CHANGE);
	_limitReached = digitalRead(VERTICAL_LIMIT_SWITCH) ? true : false;


	PORTA_PCR12 = 0x00000712;   //Alt7-QD_FTM1,FilterEnable,Pulldown
	PORTA_PCR13 = 0x00000712;   //Alt7-QD_FTM1,FilterEnable,Pulldown

    //Set FTMEN to be able to write registers
	FTM1_MODE=0x04;	    // Write protect disable - reset value
	FTM1_MODE=0x05;	    // Set FTM Enable

    // Set registers written in pins_teensy.c back to default
    FTM1_CNT = 0;
    FTM1_MOD = 0;
    FTM1_C0SC =0;
    FTM1_C1SC =0;
    FTM1_SC = 0;

    // Set registers to count quadrature
    FTM1_FILTER=0x22;	// 2x4 clock filters on both channels
    FTM1_MOD=0xFFFF;	// Maximum value of counter
    FTM1_CNTIN=COUNTER_ZERO_VALUE;	//set initial value to 32000 (the middle to not bother with overflows)
    FTM1_CNT=0;		// Updates counter with CNTIN
    FTM1_CNTIN=0;	//set minimum value to 0

    // Set Registers for output compare mode
    FTM1_COMBINE=0;	    // Reset value, make sure
    FTM1_C0SC=0x10;	    // Bit 4 Channel Mode

    FTM1_QDCTRL=0b11100001;	    // Quadrature control		0xE1
    //        Filter enabled, phase A inverted (positive count down) ,QUADEN set

    // Write Protect Enable
    FTM1_FMS=0x40;		// Write Protect, WPDIS=1

    //NVIC_ENABLE_IRQ(IRQ_FTM1);
}

void DCMotor::loop(){
	if(_speedControlActivated) {
		speedControl();
	} else {
		positionControl();
	}
}

void DCMotor::speedControl() {
	int inc = readZInc();
	_speed = (inc - _prev_inc) / (DC_MOTOR_PERIOD * INC_PER_MM);
	float speedError = _speed - _speedSetPoint;
	_intSpeedError = clamp((float)-255, _intSpeedError + speedError, (float)255);
	float command = speedError * KP_SPEED + _intSpeedError * KI_SPEED;

	if(_speedSetPoint == 0) {
		command = 0;
		_intSpeedError = 0;
	}
	setMotorCommand(-command);

	_prev_inc = inc;
}

void DCMotor::setSpeedControlActivated(bool speedControlActivated) {
	if(speedControlActivated) {
		_intSpeedError = 0;
		_speedSetPoint = 0;
		_prev_inc = readZInc();
	}
	_speedControlActivated = speedControlActivated;
}

void DCMotor::positionControl() {
	int inc = readZInc();
	float error = _goal - inc/INC_PER_MM;
	float derivError = error - _prevError;
	_integralError = clamp(MIN_INTEGRAL, _integralError + error, MAX_INTEGRAL);
	_prevError = error;

	float command = KP * error + KI * _integralError + KP * derivError;

	setMotorCommand(command);
}

void ISR_LIMIT_SWITCH(){
	if(digitalRead(VERTICAL_LIMIT_SWITCH)) {
		dcmotor->setLimitReached(true);
	} else {
		dcmotor->setLimitReached(false);
	}

	dcmotor->zeroFTM();
}

void DCMotor::setMotorCommand(int command) {
	if(abs(_current) > MAX_CURRENT_VALUE) {
		command = 0;
		Serial.println("OverCurrent!!!");
	}
	command = clamp(-127, command, 127);
	if(abs(command) < 20) {
		digitalWrite(VERTICAL_MOTOR_A, HIGH);
		digitalWrite(VERTICAL_MOTOR_B, LOW);
	}
	else {
		digitalWrite(VERTICAL_MOTOR_A, LOW);
		analogWrite(VERTICAL_MOTOR_B, command+127);
	}
}

void DCMotor::checkCurrent() {
	int value = analogRead(MOT_Z_CURRENT_SENSOR) - CURRENT_ANALOG_OFFSET;
	_current = 0.8*_current + 0.2 * value;	//low pass filter
}

void DCMotor::zeroFTM() {
    // Turn off counter to disable interrupts and clear any
    //   overflow and compare bits

    v_read=FTM1_FMS;	// Requires reading before WPDIS can be set

    noInterrupts();
    FTM1_MODE=0x05;	// Write Protect Disable with FTMEN set
    FTM1_QDCTRL=0xE0;	// Disable QUADEN, filter still enabled
                        //   Turns off counter so no interrupts
    interrupts();

    FTM1_CNTIN=COUNTER_ZERO_VALUE;	//set initial value to 32000 (the middle to not bother with overflows)
    FTM1_CNT=0;		// Updates counter with CNTIN
    FTM1_CNTIN=0;	//set minimum value to 0

    FTM1_C0SC=0x10;	// Disable Channel compare int, clear compare bit
			//   Leaves Mode bit for compare set

    // Enable counter again
    FTM1_QDCTRL=0xE1;	// QUADEN
    FTM1_FMS=0x40;  	// Write Protect, WPDIS=1
}
