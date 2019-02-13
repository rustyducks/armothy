/*
 * Armothy.h
 *
 *  Created on: 3 déc. 2018
 *      Author: guilhem
 */

#ifndef ARMOTHY_H_
#define ARMOTHY_H_

#include <DynamixelSerial.h>

#include "Communication.h"
#include "DCMotor.h"
#include "DebugInterface.h"
#include "SuccionCup.h"
#include "MacroManager.h"
#include "Metro.h"

namespace armothy{
class Armothy {
public:
	enum ePumpState{
		PUMP_ON,
		PUMP_OFF
	};
	enum eValveState{
		VALVE_OPENED,
		VALVE_CLOSED
	};
	enum eJoint{
		PRISMATIC_Z_AXIS = 0,
		REVOLUTE_Z_AXIS = 1,
		REVOLUTE_Y_AXIS = 2
	};

	enum Error {
		Z_MAX_REACHED = 1,
		Z_MIN_REACHED = 2,
		Z_MOTOR_OVERCURRENT = 4,

	};

	Armothy();
	virtual ~Armothy(){};

	void home();
	void emergencyStop();
	bool isMoving();

	void startPump();
	void stopPump();
	ePumpState getPumpState();

	void openValve();
	void closeValve();
	eValveState getValveState();

	void sendActuatorCommand(eJoint joint, float command);
	float getDoF(eJoint joint);

	float getPressure();

	uint8_t getErrorByte() {
		return _errorByte;
	}

	void setErrorByte(uint8_t errorByte) {
		_errorByte = errorByte;
	}

	void setup();
	void loop();

	DCMotor& getZAxisMotor() {
		return _zAxisMotor;
	}

protected:
	Communication communication;
	DynamixelSerial _dynamixels;
	DCMotor _zAxisMotor;
	DebugInterface _debugInterface;
	SuccionCup _succionCup;
	MacroManager _macroManager;

	Metro communicationMetro, debugMetro, dcMotorMetro, succionMetro, degubLedMetro, macroManagerMetro;

	uint8_t _errorByte;

	bool _debugLedState;
};
}

#endif /* ARMOTHY_H_ */
