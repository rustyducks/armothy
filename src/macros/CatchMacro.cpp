/*
 * CatchMacro.cpp
 *
 *  Created on: 10 févr. 2019
 *      Author: robot
 */

#include <CatchMacro.h>
#include "Armothy.h"

namespace armothy {

CatchMacro::CatchMacro(Armothy * arm) :AbstractMacro(arm){
	// TODO Auto-generated constructor stub

}

CatchMacro::~CatchMacro() {
	// TODO Auto-generated destructor stub
}

void CatchMacro::init() {
	_armothy->closeValve();
	_armothy->startPump();
}

int CatchMacro::doIt() {
	if(_armothy->getPressure() < 30) {
		return MACRO_STATUS_FINISHED;
	}
	return MACRO_STATUS_RUNNING;
}

void CatchMacro::leave() {
}

} /* namespace armothy */
