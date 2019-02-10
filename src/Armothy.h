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

	void setup();
	void loop();



protected:
	Communication communication;
	DynamixelSerial _dynamixels;
	DCMotor _zAxisMotor;
	DebugInterface _debugInterface;
	SuccionCup _succionCup;
	MacroManager _macroManager;

	Metro communicationMetro, debugMetro, dcMotorMetro, succionMetro, degubLedMetro, macroManagerMetro;

	bool _debugLedState;
};
}

#endif /* ARMOTHY_H_ */
