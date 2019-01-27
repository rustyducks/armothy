/*
 * Armothy.cpp
 *
 *  Created on: 3 déc. 2018
 *      Author: guilhem
 */

#include "Armothy.h"
#include "params.h"

using namespace armothy;

Armothy::Armothy() : _dynamixels(3, Serial3), _lastCommunicationTime(0), _lastDebugTime(0), _lastDcMotorTime(0), _lastSuccionTime(0), _lastDebugLedTime(0), _debugLedState(false){

}

void Armothy::setup(){
	pinMode(DEBUG_LED, OUTPUT);
	digitalWrite(DEBUG_LED, _debugLedState);
	communication.setup(this);
	_debugInterface.setup(this);
	_zAxisMotor.setup();
	_succionCup.setup();
}

void Armothy::loop(){
	unsigned long time = millis();
	if (time - _lastCommunicationTime >= COMMUNICATION_PERIOD){
		communication.loop();
		_lastCommunicationTime = time;
	}
	if (time - _lastDebugTime >= DEBUG_INTERFACE_PERIOD){
		_debugInterface.loop();
		_lastDebugTime = time;
	}
	if (time - _lastDcMotorTime >= DC_MOTOR_PERIOD){
		_zAxisMotor.loop();
		_lastDcMotorTime = time;
	}
	if (time - _lastSuccionTime >= SUCCION_CUP_PERIOD){
		_succionCup.loop();
		_lastSuccionTime = time;
	}

	if (time - _lastDebugLedTime >= 1000){
		_debugLedState ^= 1;
		digitalWrite(DEBUG_LED, _debugLedState);
		_lastDebugLedTime = time;
	}
}

void Armothy::home(){
	_zAxisMotor.home();
	_dynamixels.move(Z_AXIS_DYNAMIXEL_ID, DYNAMIXEL_TO_0_1);
	_dynamixels.move(Y_AXIS_DYNAMIXEL_ID, DYNAMIXEL_TO_0_2);
}

void Armothy::startPump() {
	_succionCup.startPump();
}

void Armothy::stopPump() {
	_succionCup.stopPump();
}

Armothy::ePumpState Armothy::getPumpState() {
	return (ePumpState)_succionCup.getPumpState();
}

void Armothy::openValve() {
	_succionCup.openValve();
}

void Armothy::closeValve() {
	_succionCup.closeValve();
}

Armothy::eValveState Armothy::getValveState() {
	return (eValveState)_succionCup.getValveState();
}

void Armothy::sendActuatorCommand(eJoint joint, float command) {
	switch(joint){
	case PRISMATIC_Z_AXIS:
		_zAxisMotor.go_to(command);
		break;
	case REVOLUTE_Z_AXIS:
		_dynamixels.move(Z_AXIS_DYNAMIXEL_ID, command + DYNAMIXEL_TO_0_1);
		break;
	case REVOLUTE_Y_AXIS:
		_dynamixels.move(Y_AXIS_DYNAMIXEL_ID, command + DYNAMIXEL_TO_0_2);
		break;
	}
}

float Armothy::getDoF(eJoint joint) {
	switch(joint){
	case PRISMATIC_Z_AXIS:
		return _zAxisMotor.get_position();
		break;
	case REVOLUTE_Z_AXIS:
		return _dynamixels.readPosition(Z_AXIS_DYNAMIXEL_ID) - DYNAMIXEL_TO_0_1;
		break;
	case REVOLUTE_Y_AXIS:
		return _dynamixels.readPosition(Y_AXIS_DYNAMIXEL_ID) - DYNAMIXEL_TO_0_2;
		break;
	default:
		return 0;
		break;
	}
}

void Armothy::emergencyStop(){
	_zAxisMotor.stop();
	_dynamixels.move(Z_AXIS_DYNAMIXEL_ID, _dynamixels.readPosition(Z_AXIS_DYNAMIXEL_ID));
	_dynamixels.move(Y_AXIS_DYNAMIXEL_ID, _dynamixels.readPosition(Y_AXIS_DYNAMIXEL_ID));
	_succionCup.stopPump();
	_succionCup.openValve();
}

bool Armothy::isMoving(){
	return _zAxisMotor.isMoving(); // Maybe also check the Dynamixels
}

float Armothy::getPressure(){
	return _succionCup.getPressure();
}
