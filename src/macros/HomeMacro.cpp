/*
 * HomeMacro.cpp
 *
 *  Created on: 12 févr. 2019
 *      Author: robot
 */

#include <HomeMacro.h>
#include "Armothy.h"
#include "DCMotor.h"
#include "Arduino.h"
#include "params.h"

namespace armothy {

//constexpr float SPEED = -10.0;

HomeMacro::HomeMacro(Armothy * arm)  :AbstractMacro(arm) {
	_state = STANDING_BY;
	_switch_off_time = 0;
}

HomeMacro::~HomeMacro() {
	// TODO Auto-generated destructor stub
}

void HomeMacro::init() {
	_armothy->getZAxisMotor().setSpeedControlActivated(true);
	if(_armothy->getZAxisMotor().isLimitReached()) {
		_state = GOING_DOWN_WITH_SWITCH;
		_armothy->getZAxisMotor().setSpeedSetPoint(-2*SPEED);
	} else {
		_state = GOING_UP;
		_armothy->getZAxisMotor().setSpeedSetPoint(SPEED);
	}

}

int HomeMacro::doIt() {

	switch (_state) {
		case GOING_DOWN_WITH_SWITCH:
			if(!_armothy->getZAxisMotor().isLimitReached()) {
				_switch_off_time = millis();
				_state = GOING_DOWN_WITHOUT_SWITCH;
			}
			break;
		case GOING_DOWN_WITHOUT_SWITCH:
			if(millis() - _switch_off_time > 500) {
				_armothy->getZAxisMotor().setSpeedSetPoint(SPEED);
				_state = GOING_UP;
			}
			break;
		case GOING_UP:
			if(_armothy->getZAxisMotor().isLimitReached()) {
				_armothy->getZAxisMotor().setHome();
				_armothy->getZAxisMotor().stop();
				_armothy->getZAxisMotor().setSpeedControlActivated(false);
				return MACRO_STATUS_FINISHED;
			}
			break;
		default:
			break;
	}

	return MACRO_STATUS_RUNNING;
}

void HomeMacro::leave() {
}

} /* namespace armothy */
