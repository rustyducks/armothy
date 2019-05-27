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
class MacroManager;

class Communication {
public:
	Communication();
	virtual ~Communication(){};

	void setup(Armothy* arm, MacroManager* macroManager);
	void loop();
	static void SOnReceive(int receivedSize);
	static void SOnRequest();

	union uArg{
		uint32_t ui;
		int32_t i;
		float f;
		uint8_t data[4];
	};

protected:
	static constexpr uint8_t COMMAND_BUFFER_SIZE = 16;
	static constexpr uint8_t MAX_ARGS_NUMBER = 5;
	enum eCommandByte{
	START_CALIBRATION_CMD,
	DIRECT_AXIS_1_CMD, // Expects a 4 bytes float with this command
	DIRECT_AXIS_2_CMD, // Expects a 4 bytes float with this command
	DIRECT_AXIS_3_CMD, // Expects a 4 bytes float with this command
	STOP_PUMP_CMD,
	START_PUMP_CMD,
	CLOSE_VALVE_CMD,
	OPEN_VALVE_CMD,
	MACRO_CMD,			//Expects a 1 byte integer with this command

	EMERGENCY_STOP_CMD,

	// Request commands (answer length specified in comment)
	CALIBRATION_ENDED_RQST, // 1 byte (0 : ended; 1 : running)
	AXIS_1_POSITION_RQST, // 4 bytes (float)
	AXIS_2_POSITION_RQST, // 4 bytes (float)
	AXIS_3_POSITION_RQST, // 4 bytes (float)
	PUMP_STATE_RQST, // 1 byte (0: stopped; 1: started)
	VALVE_STATE_RQST, // 1 byte (0: closed; 1: opened)
	ERROR_BYTE_RQST,	//1 byte
	MACRO_STATE_RSQT,	//1 byte
	PRESSURE_RQST // 4 bytes (float) /!\ Should be the last one in this enum !
	};


	struct sCommand{
		eCommandByte cmd;
		uArg args[MAX_ARGS_NUMBER];
	};
	struct sCommandBuffer{
		uint8_t readIndex;
		uint8_t writeIndex;
		sCommand commands[COMMAND_BUFFER_SIZE];
	};

	sCommandBuffer _waitingCommands;
	eCommandByte _waitingRequest;

	Armothy * _armothy;
	MacroManager* _macroManager;

	static Communication * _instance;
	static void setInstance(Communication * instance);
	void onReceive(int receivedSize);
	void onRequest();

	void sendFloat(float toSend);


};
}

#endif /* COMMUNICATION_H_ */
