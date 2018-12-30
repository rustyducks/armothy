/*
 * Communication.h
 *
 *  Created on: 3 déc. 2018
 *      Author: guilhem
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <Arduino.h>

namespace armothy{

class Armothy;

class Communication {
public:
	Communication();
	virtual ~Communication(){};

	void setup(Armothy* arm);
	void loop();
	static void SOnReceive(int receivedSize);
	static void SOnRequest();

protected:
	enum eCommandByte{
	START_CALIBRATION_CMD,
	DIRECT_AXIS_1_CMD, // Expects a 4 bytes float with this command
	DIRECT_AXIS_2_CMD, // Expects a 4 bytes float with this command
	DIRECT_AXIS_3_CMD, // Expects a 4 bytes float with this command
	STOP_PUMP_CMD,
	START_PUMP_CMD,
	CLOSE_VALVE_CMD,
	OPEN_VALVE_CMD,
	EMERGENCY_STOP_CMD,

	// Request commands (answer length specified in comment)
	CALIBRATION_ENDED_RQST, // 1 byte (0 : ended; 1 : running)
	AXIS_1_POSITION_RQST, // 4 bytes (float)
	AXIS_2_POSITION_RQST, // 4 bytes (float)
	AXIS_3_POSITION_RQST, // 4 bytes (float)
	PUMP_STATE_RQST, // 1 byte (0: stopped; 1: started)
	VALVE_STATE_RQST, // 1 byte (0: closed; 1: opened)
	PRESSURE_RQST // 4 bytes (float) /!\ Should be the last one in this enum !
	};

	union uFloat{
		float f;
		uint8_t data[4];
	};

	//Possible improvement : Make a command buffer instead of just one command possible at time
	eCommandByte _waitingCommand;
	bool _isCommandWaiting;
	eCommandByte _waitingRequest;
	uFloat _cmdArg;

	Armothy * _armothy;

	static Communication * _instance;
	static void setInstance(Communication * instance);
	void onReceive(int receivedSize);
	void onRequest();

	void sendFloat(float toSend);


};
}

#endif /* COMMUNICATION_H_ */
