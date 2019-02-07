/*
 * SuccionCup.cpp
 *
 *  Created on: 14 déc. 2018
 *      Author: guilhem
 */

#include <SuccionCup.h>
#include <Arduino.h>
#include "params.h"

namespace armothy {

SuccionCup::SuccionCup(): _valveState(VALVE_CLOSED), _pumpState(PUMP_OFF) {

}

SuccionCup::~SuccionCup() {
	// TODO Auto-generated destructor stub
}

void SuccionCup::setup(){
	pinMode(PUMP_PIN, OUTPUT);
	pinMode(VALVE_PIN, OUTPUT);
	pinMode(SUCCION_SENSOR_PIN, INPUT);
	closeValve();
	stopPump();
}

void SuccionCup::loop(){
}

void SuccionCup::startPump() {
	digitalWrite(PUMP_PIN, LOW);
	_pumpState = PUMP_ON;
}

void SuccionCup::stopPump() {
	digitalWrite(PUMP_PIN, HIGH);
	_pumpState = PUMP_OFF;
}

SuccionCup::ePumpState SuccionCup::getPumpState() {
	return _pumpState;
}

void SuccionCup::openValve() {
	digitalWrite(VALVE_PIN, HIGH);
	_valveState = VALVE_OPENED;
}

void SuccionCup::closeValve() {
	digitalWrite(VALVE_PIN, LOW);
	_valveState = VALVE_CLOSED;
}

SuccionCup::eValveState SuccionCup::getValveState() {
	return _valveState;
}

float SuccionCup::getPressure() {
	return analogRead(SUCCION_SENSOR_PIN) * ANALOG_TO_BAR;
}
} /* namespace armothy */
