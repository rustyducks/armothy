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
constexpr float DCMotor::Z_MAX;
constexpr float DCMotor::KP_SPEED;
constexpr float DCMotor::KI_SPEED;

DCMotor * dcmotor = nullptr;

DCMotor::DCMotor() : _inc(0), _prev_inc(0),
		_goal(0), _speed(0), _prevError(0), _integralError(0),
		_speedSetPoint(0), _speedControlActivated(true), _limitReached(false), _intSpeedError(0),
		_current(0){
}

void DCMotor::stop(){
	_speedSetPoint = 0;
	_goal = _inc / INC_PER_MM;
	_prevError = 0;
	_integralError = 0;
	_intSpeedError = 0;
	digitalWrite(VERTICAL_MOTOR_A, 0);
	digitalWrite(VERTICAL_MOTOR_B, 0);
}

void DCMotor::go_to(float position){
	_goal = clamp((float)0, position, Z_MAX);
}

float DCMotor::get_position(){
	return _inc / INC_PER_MM;
}

bool DCMotor::isMoving(){
	if(_speedControlActivated) {
		return abs(_speedSetPoint) >= 1.0;
	} else {
		return abs(_goal - _inc/INC_PER_MM) > 1.0;
	}
			//TODO 0.2
}

void DCMotor::setup(){
	dcmotor = this;
	pinMode(VERTICAL_MOTOR_EF, INPUT_PULLUP);
	pinMode(VERTICAL_MOTOR_A, OUTPUT);
	pinMode(VERTICAL_MOTOR_B, OUTPUT);
	setMotorCommand(0);
	pinMode(VERTICAL_ENCODER_A, INPUT);
	pinMode(VERTICAL_ENCODER_B, INPUT);
	pinMode(VERTICAL_LIMIT_SWITCH, INPUT_PULLUP);
	pinMode(MOT_Z_CURRENT_SENSOR, INPUT);
	attachInterrupt(VERTICAL_ENCODER_A, ISR_INC1, RISING);
	attachInterrupt(VERTICAL_ENCODER_B, ISR_INC2, RISING);
	attachInterrupt(VERTICAL_LIMIT_SWITCH, ISR_LIMIT_SWITCH, CHANGE);
	_limitReached = digitalRead(VERTICAL_LIMIT_SWITCH) ? true : false;
}

void DCMotor::loop(){
	if(_speedControlActivated) {
		speedControl();
	} else {
		positionControl();
	}
}

void DCMotor::speedControl() {
	cli();
	int inc = _inc;
	sei();
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
		_prev_inc = _inc;
	}
	_speedControlActivated = speedControlActivated;
}

void DCMotor::positionControl() {
	cli();
	int inc = _inc;
	sei();
	float error = _goal - inc/INC_PER_MM;
	float derivError = error - _prevError;
	_integralError = clamp(MIN_INTEGRAL, _integralError + error, MAX_INTEGRAL);
	_prevError = error;

	float command = KP * error + KI * _integralError + KP * derivError;

	setMotorCommand(command);
}

void DCMotor::isr_inc1(){
	if (digitalRead(VERTICAL_ENCODER_B)){
		_inc++;
	}else{
		_inc--;
	}
}

void DCMotor::isr_inc2(){
	if (digitalRead(VERTICAL_ENCODER_A)){
		_inc--;
	}else{
		_inc++;
	}
}

void ISR_INC1(){
	dcmotor->isr_inc1();
}

void ISR_INC2(){
	dcmotor->isr_inc2();
}

void ISR_LIMIT_SWITCH(){
	if(digitalRead(VERTICAL_LIMIT_SWITCH)) {
		dcmotor->setLimitReached(true);
	} else {
		dcmotor->setLimitReached(false);
	}

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
