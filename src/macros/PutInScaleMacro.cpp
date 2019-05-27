/*
 * PutInBalanceMacro.cpp
 *
 *  Created on: 10 févr. 2019
 *      Author: robot
 */

#include <PutInScaleMacro.h>
#include "Armothy.h"
#include "utils.h"

namespace armothy {

PutInScaleMacro::PutInScaleMacro(Armothy * arm, float stack_height, int stack, int rotation_drop, float drop_height) :AbstractMacro(arm) {
	pressure_time = 0;
	stackHeight = stack_height;
	_stack = (Stack) stack;
	rotation_time = 0;
	rotationDrop = rotation_drop;
	safeHeight = 0;
	dropHeight = drop_height;
	state = INIT_RAISING;
}

PutInScaleMacro::~PutInScaleMacro() {
	// TODO Auto-generated destructor stub
}

void PutInScaleMacro::init() {
	_armothy->closeValve();
	_armothy->startPump();
	state = INIT_RAISING;
	safeHeight = stackHeight-10;
}

int PutInScaleMacro::doIt() {
	float z = _armothy->getDoF(Armothy::PRISMATIC_Z_AXIS);

	switch(state) {
	case INIT_RAISING:
		_armothy->sendActuatorCommand(Armothy::PRISMATIC_Z_AXIS, safeHeight);
		if(abs(z-safeHeight) < 3) {
			if(_stack == LEFT) {
				_armothy->sendActuatorCommand(Armothy::REVOLUTE_Z_AXIS, 315);
			} else {
				_armothy->sendActuatorCommand(Armothy::REVOLUTE_Z_AXIS, -315);
			}
			_armothy->sendActuatorCommand(Armothy::REVOLUTE_Y_AXIS, 0);
			rotation_time = millis();
			state = INIT_ROTATIONS;
		}
		break;
	case INIT_ROTATIONS:
		if(millis() - rotation_time > 600) {	//TODO: use _armothy->get_dof(Armothy::REVOLUTE_Z_AXIS)
			state = STACK_DESCENT;
		}
		break;
	case STACK_DESCENT:
		if(_armothy->getPressure() > 30) {	//no atom catched yet
			_armothy->sendActuatorCommand(Armothy::PRISMATIC_Z_AXIS, 90);
		}
		else {
			safeHeight = min(z-10, dropHeight);
			safeHeight = max(0, safeHeight);
			_armothy->sendActuatorCommand(Armothy::PRISMATIC_Z_AXIS, safeHeight);
			state = RAISING;
		}
		break;
	case RAISING:
		if(abs(z-safeHeight) < 3) {
			_armothy->sendActuatorCommand(Armothy::REVOLUTE_Y_AXIS, 315);
			rotation_time = millis();
			state = ROTATION_Y;
		}
		break;
	case ROTATION_Y:
		if(millis() - rotation_time > 600) {
			_armothy->sendActuatorCommand(Armothy::REVOLUTE_Z_AXIS, rotationDrop);
			rotation_time = millis();
			state = ROTATION;
		}
		break;
	case ROTATION:
		if(millis() - rotation_time > 600) {	//TODO: use _armothy->get_dof(Armothy::REVOLUTE_Z_AXIS)
			//_armothy->sendActuatorCommand(Armothy::PRISMATIC_Z_AXIS, 100);		// Should we descend to drop the atom ?
			state = PUT_DESCENT;
		}
		break;
	case PUT_DESCENT:
		_armothy->openValve();
		_armothy->stopPump();
		state = DROPING;
		break;
	case DROPING:
		if(_armothy->getPressure() > 60) { //pressure return to normal
			pressure_time = millis();
			state = WAIT_ATOM_DROP;
		}
		break;
	case WAIT_ATOM_DROP:
		if(millis() - pressure_time > 100) { //atom released
			_armothy->sendActuatorCommand(Armothy::PRISMATIC_Z_AXIS, safeHeight);
			state = RAISING_BACK;
		}
		break;
	case RAISING_BACK:
		if(abs(z-safeHeight) < 3) {
			_armothy->sendActuatorCommand(Armothy::REVOLUTE_Z_AXIS, 0);
			rotation_time = millis();
			state = ROTATION_BACK;
		}
		break;
	case ROTATION_BACK:
		if(millis() - rotation_time > 600) {	//TODO: use _armothy->get_dof(Armothy::REVOLUTE_Z_AXIS)
			return MACRO_STATUS_FINISHED;
		}
	}

	return MACRO_STATUS_RUNNING;
}

void PutInScaleMacro::leave() {
}

} /* namespace armothy */
