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

float TakeAndStoreMacro::stackHeight = 100;

TakeAndStoreMacro::TakeAndStoreMacro(Armothy * arm) :AbstractMacro(arm) {
	atomHeight = 0;
	safeHeight = 0;
	rotation_time = 0;
	state = INITIAL_DESCENT;
}

TakeAndStoreMacro::~TakeAndStoreMacro() {
	// TODO Auto-generated destructor stub
}

void TakeAndStoreMacro::init() {
	_armothy->closeValve();
	_armothy->startPump();
}

bool TakeAndStoreMacro::doIt() {
	float z = _armothy->getDoF(Armothy::PRISMATIC_Z_AXIS);

	switch(state) {
	case INITIAL_DESCENT:
		if(_armothy->getPressure() > 30) {	//no atom catched yet
			_armothy->sendActuatorCommand(Armothy::PRISMATIC_Z_AXIS, 155);
		}
		else {
			safeHeight = clamp((float)0, TakeAndStoreMacro::stackHeight, z-10);
			_armothy->sendActuatorCommand(Armothy::PRISMATIC_Z_AXIS, safeHeight);
			state = RAISING;
		}
		break;
	case RAISING:
		Serial.println("raising !");
		if(abs(z-safeHeight) < 2) {
			_armothy->sendActuatorCommand(Armothy::REVOLUTE_Z_AXIS, -320);
			rotation_time = millis();
			state = ROTATION;
		}
		break;
	case ROTATION:
		if(millis() - rotation_time > 500) {	//TODO: use _armothy->get_dof(Armothy::REVOLUTE_Z_AXIS)
			_armothy->sendActuatorCommand(Armothy::PRISMATIC_Z_AXIS, TakeAndStoreMacro::stackHeight);
			state = STORE_DESCENT;
		}
		break;
	case STORE_DESCENT:
		if(abs(z-TakeAndStoreMacro::stackHeight) < 2) {
			_armothy->openValve();
			_armothy->stopPump();
			state = STORE;
		}
		break;
	case STORE:
		if(_armothy->getPressure() > 30) { //atom released
			_armothy->closeValve();
			return true;
		}
	}

	return false;
}

void TakeAndStoreMacro::leave() {
}

} /* namespace armothy */
