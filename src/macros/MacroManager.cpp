/*
 * MacroManager.cpp
 *
 *  Created on: 10 févr. 2019
 *      Author: Fabien-B
 */

#include <macros/MacroManager.h>

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

void MacroManager::setMacro(AbstractMacro* macro) {
	if(macroState != FINISHED) {
		currentMacro->leave();
	}
	currentMacro = macro;
	macroState = IDLE;
}

} /* namespace armothy */
