/*
 * Armothy.cpp
 *
 *  Created on: 3 déc. 2018
 *      Author: guilhem
 */

#include "Armothy.h"
#include "params.h"

Armothy m_armothy;

Armothy::Armothy() : _dynamixels(Dynamixel){
}

void Armothy::home(){
	_zAxisMotor.home();
	_dynamixels.move(Z_AXIS_DYNAMIXEL_ID, DYNAMIXEL_TO_0_1);
	_dynamixels.move(Y_AXIS_DYNAMIXEL_ID, DYNAMIXEL_TO_0_2);
}

void Armothy::startPump() {
	// TODO
}

void Armothy::stopPump() {
	//TODO
}

Armothy::ePumpState Armothy::getPumpState() {
	//TODO
	return PUMP_OFF;
}

void Armothy::openValve() {
	//TODO
}

void Armothy::closeValve() {
	//TODO
}

Armothy::eValveState Armothy::getValveState() {
	//TODO
	return VALVE_CLOSED;
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
	}
}
