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
constexpr int DCMotor::MIN_INTEGRAL;
constexpr int DCMotor::MAX_INTEGRAL;

DCMotor * dcmotor = nullptr;

DCMotor::DCMotor() : _isHoming(false), _inc(0), _goal_inc(0), _prevError(0), _integralError(0){
}

void DCMotor::home(){
	if (digitalRead(VERTICAL_LIMIT_SWITCH)){
		// Motor already in limit. Just go down until switch deactivates, then go up
		_inc = 0;
		_isHoming = true;
		_goal_inc = 500 * INC_PER_MM;
	}else{
		// Motor not in limit, go up until switch is activated
		_goal_inc = -500 * INC_PER_MM;
		_isHoming = false;
	}
}

void DCMotor::stop(){
	_goal_inc = _inc;
	_prevError = 0;
	_integralError = 0;
}

void DCMotor::go_to(double position){
	_goal_inc = position * INC_PER_MM;
}

float DCMotor::get_position(){
	return _inc / INC_PER_MM;
}

bool DCMotor::isMoving(){
	return abs(_inc - _goal_inc) >= 10;
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
	attachInterrupt(VERTICAL_LIMIT_SWITCH, ISR_STOP, RISING);
	home();
}

void DCMotor::loop(){
	int error = _goal_inc - _inc;
	int derivError = error - _prevError;
	_integralError = clamp(MIN_INTEGRAL, _integralError + error, MAX_INTEGRAL);
	_prevError = error;

	float command = KP * error + KI * _integralError + KP * derivError;
	int boundedCommand = clamp(-255, (int)command, 255);

	digitalWrite(VERTICAL_MOTOR_DIR_A, sig(boundedCommand));
	digitalWrite(VERTICAL_MOTOR_DIR_B, !sig(boundedCommand));
	analogWrite(VERTICAL_MOTOR_PWM, abs(boundedCommand));

	if (_isHoming && !digitalRead(VERTICAL_LIMIT_SWITCH)){
		// Was homing and now the limit switch is off, go up until it turns on again.
		_isHoming = false;
		home();
	}
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

void DCMotor::isr_limit_switch(){
	_inc = 0;
	_goal_inc = 0;
	_prevError = 0;
	_integralError = 0;
}

void ISR_INC1(){
	dcmotor->isr_inc1();
}

void ISR_INC2(){
	dcmotor->isr_inc2();
}

void ISR_STOP(){
	dcmotor->isr_limit_switch();
}
