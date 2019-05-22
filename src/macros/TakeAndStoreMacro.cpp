/*
 * TakeAndStore.cpp
 *
 *  Created on: 10 févr. 2019
 *      Author: robot
 */

#include <TakeAndStoreMacro.h>
#include "Armothy.h"
#include "utils.h"

namespace armothy {

TakeAndStoreMacro::TakeAndStoreMacro(Armothy * arm, float stack_height, int stack) :AbstractMacro(arm) {
	pressure_time = 0;
	stackHeight = stack_height;
	_stack = (Stack) stack;
	atomHeight = 0;
	safeHeight = 0;
	rotation_time = 0;
	arm_lowest_time = 0;
	arm_lowest_reached = false;
	state = INITIAL_DESCENT;
}

TakeAndStoreMacro::~TakeAndStoreMacro() {
	// TODO Auto-generated destructor stub
}

void TakeAndStoreMacro::init() {
	_armothy->closeValve();
	_armothy->startPump();
	arm_lowest_reached = false;
	state = INITIAL_DESCENT;
	Serial.println("INITIAL DESCENT");
}

int TakeAndStoreMacro::doIt() {
	MacroStatus returnCode = MACRO_STATUS_RUNNING;
	float z = _armothy->getDoF(Armothy::PRISMATIC_Z_AXIS);
	switch(state) {
	case INITIAL_DESCENT:
		if(_armothy->getPressure() > 30) {	//no atom catched yet
			_armothy->sendActuatorCommand(Armothy::PRISMATIC_Z_AXIS, 155);
			if(!arm_lowest_reached && abs(z - Z_MAX) < 5) {
				arm_lowest_time = millis();
				arm_lowest_reached = true;
			}
			if(arm_lowest_reached && millis() - arm_lowest_time > 500) {
				state = RAISING_ERROR;
				Serial.println(z);
				Serial.println("RAISING_ERROR");
			}
		}
		else {
			atomHeight = z;
			safeHeight = clamp((float)0, stackHeight, atomHeight-10);
			_armothy->sendActuatorCommand(Armothy::PRISMATIC_Z_AXIS, safeHeight);
			state = RAISING;
		}
		break;
	case RAISING:
		// higher than 10mm above the atom, it should be safe to move by now
		if(z < atomHeight - 10) {
			returnCode = MACRO_STATUS_RUNNING_SAFE;
		}
		if(abs(z-safeHeight) < 3) {
			if(_stack == LEFT) {
				_armothy->sendActuatorCommand(Armothy::REVOLUTE_Z_AXIS, 315);
			} else {
				_armothy->sendActuatorCommand(Armothy::REVOLUTE_Z_AXIS, -315);
			}
			rotation_time = millis();
			state = ROTATION;
			returnCode = MACRO_STATUS_RUNNING_SAFE;
		}
		break;
	case ROTATION:
		returnCode = MACRO_STATUS_RUNNING_SAFE;
		if(millis() - rotation_time > 600) {	//TODO: use _armothy->get_dof(Armothy::REVOLUTE_Z_AXIS)
			_armothy->sendActuatorCommand(Armothy::PRISMATIC_Z_AXIS, stackHeight);
			state = STORE_DESCENT;
		}
		break;
	case STORE_DESCENT:
		returnCode = MACRO_STATUS_RUNNING_SAFE;
		if(abs(z-stackHeight) < 3) {
			_armothy->openValve();
			_armothy->stopPump();
			state = STORE;
		}
		break;
	case STORE:
		returnCode = MACRO_STATUS_RUNNING_SAFE;
		if(_armothy->getPressure() > 60) { //pressure return to normal
			pressure_time = millis();
			state = WAIT_ATOM_DROP;
		}
		break;
	case WAIT_ATOM_DROP:
		returnCode = MACRO_STATUS_RUNNING_SAFE;
		if(millis() - pressure_time > 100) { //atom released
			_armothy->sendActuatorCommand(Armothy::PRISMATIC_Z_AXIS, safeHeight);
			state = RAISING_BACK;
		}
		break;
	case RAISING_BACK:
		returnCode = MACRO_STATUS_RUNNING_SAFE;
		if(abs(z-safeHeight) < 3) {
			_armothy->sendActuatorCommand(Armothy::REVOLUTE_Z_AXIS, 0);
			rotation_time = millis();
			_armothy->closeValve();
			state = ROTATION_BACK;
		}
		break;
	case ROTATION_BACK:
		returnCode = MACRO_STATUS_RUNNING_SAFE;
		if(millis() - rotation_time > 500) {	//TODO: use _armothy->get_dof(Armothy::REVOLUTE_Z_AXIS)
			returnCode = MACRO_STATUS_FINISHED;
		}
		break;
	case RAISING_ERROR:
		_armothy->sendActuatorCommand(Armothy::PRISMATIC_Z_AXIS, safeHeight);
		_armothy->closeValve();
		_armothy->stopPump();
		returnCode = MACRO_STATUS_ERROR;
		break;
	}

	return returnCode;
}

void TakeAndStoreMacro::leave() {
}

} /* namespace armothy */
