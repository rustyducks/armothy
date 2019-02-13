/*
 * DCMotor.cpp
 *
 *  Created on: 3 déc. 2018
 *      Author: guilhem
 */

#include "DCMotor.h"
#include "params.h"
#include "utils.h"

#define sig(x) ((x) > 0) ? HIGH : LOW

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
		_speedSetPoint(0), _speedControlActivated(true), _limitReached(false), _intSpeedError(0){
}

void DCMotor::stop(){
	_speedSetPoint = 0;
	_goal = _inc / INC_PER_MM;
	_prevError = 0;
	_integralError = 0;
	_intSpeedError = 0;
	digitalWrite(VERTICAL_MOTOR_DIR_A, 0);
	digitalWrite(VERTICAL_MOTOR_DIR_B, 0);
	analogWrite(VERTICAL_MOTOR_PWM, 0);
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
	pinMode(VERTICAL_MOTOR_PWM, OUTPUT);
	pinMode(VERTICAL_MOTOR_DIR_A, OUTPUT);
	pinMode(VERTICAL_MOTOR_DIR_B, OUTPUT);
	pinMode(VERTICAL_ENCODER_A, INPUT);
	pinMode(VERTICAL_ENCODER_B, INPUT);
	pinMode(VERTICAL_LIMIT_SWITCH, INPUT_PULLUP);
	analogWriteFrequency(VERTICAL_MOTOR_PWM, 549.3164);
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
	int boundedCommand = clamp(-255, (int)command, 255);

	if(_speedSetPoint == 0) {
		boundedCommand = 0;
		_intSpeedError = 0;
	}
	digitalWrite(VERTICAL_MOTOR_DIR_A, !sig(boundedCommand));
	digitalWrite(VERTICAL_MOTOR_DIR_B, sig(boundedCommand));
	analogWrite(VERTICAL_MOTOR_PWM, abs(boundedCommand));

	_prev_inc = inc;
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
	int boundedCommand = clamp(-255, (int)command, 255);

	Serial.print(error);
	Serial.print("\t");
	Serial.print(command);
	Serial.print("\t");
	Serial.println(inc);

	digitalWrite(VERTICAL_MOTOR_DIR_A, sig(boundedCommand));
	digitalWrite(VERTICAL_MOTOR_DIR_B, !sig(boundedCommand));
	analogWrite(VERTICAL_MOTOR_PWM, abs(boundedCommand));
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

