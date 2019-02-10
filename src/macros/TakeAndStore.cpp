/*
 * TakeAndStore.cpp
 *
 *  Created on: 10 févr. 2019
 *      Author: robot
 */

#include <TakeAndStore.h>
#include "Armothy.h"

namespace armothy {

TakeAndStore::TakeAndStore() {
	// TODO Auto-generated constructor stub

}

TakeAndStore::~TakeAndStore() {
	// TODO Auto-generated destructor stub
}

void TakeAndStore::init() {
	_armothy->closeValve();
	_armothy->startPump();
}

bool TakeAndStore::doIt() {
	float z = _armothy->getDoF(Armothy::PRISMATIC_Z_AXIS);
	if(_armothy->getPressure() > 30) {	//no atom catched yet
		_armothy->sendActuatorCommand(Armothy::PRISMATIC_Z_AXIS, z+5);
		return true;
	}
	else {

	}
	return false;
}

void TakeAndStore::leave() {
}

} /* namespace armothy */
