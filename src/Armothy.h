/*
 * Armothy.h
 *
 *  Created on: 3 déc. 2018
 *      Author: guilhem
 */

#ifndef ARMOTHY_H_
#define ARMOTHY_H_

#include <DynamixelSerial5.h>

#include "Communication.h"
#include "DCMotor.h"

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

	void startPump();
	void stopPump();
	ePumpState getPumpState();

	void openValve();
	void closeValve();
	eValveState getValveState();

	void sendActuatorCommand(eJoint joint, float command);
	float getDoF(eJoint joint);

protected:
	Communication communication;
	DynamixelClass& _dynamixels;
	DCMotor _zAxisMotor;
};

extern Armothy m_armothy;

#endif /* ARMOTHY_H_ */
