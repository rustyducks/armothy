/*
 * MacroManager.cpp
 *
 *  Created on: 10 févr. 2019
 *      Author: Fabien-B
 */

#include <MacroManager.h>
#include "CatchMacro.h"

namespace armothy {

MacroManager::MacroManager() {
	_armothy = nullptr;
	currentMacro = nullptr;
	macroState = FINISHED;
}

MacroManager::~MacroManager() {

}

void MacroManager::setup(Armothy * arm) {
	_armothy = arm;
}

void MacroManager::loop() {
	switch(macroState) {
	case IDLE:
		currentMacro->init();
		macroState = RUNNING;
		break;
	case RUNNING:
		if(currentMacro->doIt()) {
			currentMacro->leave();
			macroState = FINISHED;
		}
		break;
	case FINISHED:
		break;
	}
}

void MacroManager::setMacro(Macros macroNb) {
	if(macroState != FINISHED) {
		currentMacro->leave();
	}

	switch(macroNb) {
	case CATCH_MACRO:
		currentMacro = new CatchMacro(_armothy);
		break;
	default:
		//Unknown macro !
		break;
	}

	macroState = IDLE;
}

} /* namespace armothy */
