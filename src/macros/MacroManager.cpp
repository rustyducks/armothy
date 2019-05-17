/*
 * MacroManager.cpp
 *
 *  Created on: 10 févr. 2019
 *      Author: Fabien-B
 */

#include <MacroManager.h>
#include "CatchMacro.h"
#include "TakeAndStoreMacro.h"
#include "Armothy.h"
#include "HomeMacro.h"
#include "PutDownMacro.h"


namespace armothy {

MacroManager::MacroManager() {
	_armothy = nullptr;
	currentMacro = nullptr;
	macroState = FINISHED;
	_statusByte = 0;
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
		_statusByte = (uint8_t)currentMacro->doIt();
		if(_statusByte & AbstractMacro::MACRO_STATUS_FINISHED) {
			currentMacro->leave();
			macroState = FINISHED;
		}
		break;
	case FINISHED:
		break;
	}
}

void MacroManager::setMacro(MacrosNumber macroNb, Communication::uArg *args) {
	if(macroState != FINISHED) {
		currentMacro->leave();
	}

	_statusByte = 0;		//TODO replace by an explicit reset of the status byte ?

	float stackHeight;
	int stack;
	int rotation_drop;

	switch(macroNb) {
	case CATCH_MACRO:
		currentMacro = new CatchMacro(_armothy);
		break;
	case TAKE_AND_STORE_MACRO:
		stackHeight = args[0].f;
		stack = args[1].ui;
		currentMacro = new TakeAndStoreMacro(_armothy, stackHeight, stack);
		break;
	case HOME_MACRO:
		currentMacro = new HomeMacro(_armothy);
		break;
	case PUT_DOWN_MACRO:
		stackHeight = args[0].f;
		stack = args[1].ui;
		rotation_drop = args[2].i;
		currentMacro = new PutDownMacro(_armothy, stackHeight, stack, rotation_drop);
		break;
	default:
		//Unknown macro !
		break;
	}

	macroState = IDLE;
}

} /* namespace armothy */

void armothy::MacroManager::stop() {
	if(currentMacro != nullptr && macroState != FINISHED) {
		currentMacro->leave();
		macroState = FINISHED;
	}
}
