/*
 * Communication.cpp
 *
 *  Created on: 3 déc. 2018
 *      Author: guilhem
 */

#include "Communication.h"
#include <Wire.h>
#include "params.h"
#include "Armothy.h"
#include "MacroManager.h"

using namespace armothy;

Communication * Communication::_instance = nullptr;

Communication::Communication(): _waitingRequest(CALIBRATION_ENDED_RQST), _armothy(nullptr), _macroManager(nullptr){
	_waitingCommands.readIndex = 0;
	_waitingCommands.writeIndex = 0;
}

void Communication::setup(Armothy * arm, MacroManager* macroManager){
	_armothy = arm;
	_macroManager = macroManager;
	setInstance(this);
	Wire.begin(I2C_ADDRESS);
	Wire.onReceive(Communication::SOnReceive);
	Wire.onRequest(Communication::SOnRequest);
}

void Communication::loop(){
	while (_waitingCommands.readIndex != _waitingCommands.writeIndex){
		eCommandByte cmd = _waitingCommands.commands[_waitingCommands.readIndex].cmd;
		uArg* args = _waitingCommands.commands[_waitingCommands.readIndex].args;
		_waitingCommands.readIndex = (_waitingCommands.readIndex + 1) % COMMAND_BUFFER_SIZE;
		if (cmd == START_CALIBRATION_CMD){
			_armothy->home();
		}else if (cmd == DIRECT_AXIS_1_CMD){
			_armothy->sendActuatorCommand(Armothy::PRISMATIC_Z_AXIS, args[0].f);
		}else if (cmd == DIRECT_AXIS_2_CMD){
			_armothy->sendActuatorCommand(Armothy::REVOLUTE_Z_AXIS, args[0].f);
		}else if (cmd == DIRECT_AXIS_3_CMD){
			_armothy->sendActuatorCommand(Armothy::REVOLUTE_Y_AXIS, args[0].f);
		}else if (cmd == STOP_PUMP_CMD){
			_armothy->stopPump();
		}else if (cmd == START_PUMP_CMD){
			_armothy->startPump();
		}else if (cmd == CLOSE_VALVE_CMD){
			_armothy->closeValve();
		}else if (cmd == OPEN_VALVE_CMD){
			_armothy->openValve();
		} else if(cmd == MACRO_CMD) {
			_macroManager->setMacro((MacroManager::MacrosNumber)args[0].ui, &args[1]);
		}else if (cmd == EMERGENCY_STOP_CMD){
			_armothy->emergencyStop();
		}
	}
}

void Communication::onReceive(int receivedSize){
	if ((_waitingCommands.writeIndex + 1) % COMMAND_BUFFER_SIZE == _waitingCommands.readIndex){
		// Command buffer full...
		while (Wire.available()) Wire.read();
		return;
	}

	uint8_t r = Wire.read();

	eCommandByte cmd = (eCommandByte)r;

	if (cmd > PRESSURE_RQST){
		// The command sent is not a valid command. Maybe a desynchronisation problem (so empty the buffer) ?
		while (Wire.available()) Wire.read();
		return;
	}

	if(cmd > EMERGENCY_STOP_CMD) {
		if(receivedSize == 1) {
			_waitingRequest = cmd;
		} else {
			//too many bytes. Is there a problem ?
			while (Wire.available()) Wire.read();
		}
		return;
	}

	_waitingCommands.commands[_waitingCommands.writeIndex].cmd = cmd;
	uint8_t nbArgs = Wire.read();										//number of arguments to be read
	if (receivedSize == 2+nbArgs*4 && nbArgs <= MAX_ARGS_NUMBER){		//check the number of bytes received
		for(int i=0; i<nbArgs; i++) {
			for(int j=0; j<4; j++) {
				_waitingCommands.commands[_waitingCommands.writeIndex].args[i].data[j] = Wire.read();
			}
			//Serial.println(_waitingCommands.commands[_waitingCommands.writeIndex].args[i].ui, HEX);
		}

	} else {
		//Serial.println("Wrong byte number");
		// This should not happen... (it means we received a macro command without argument)
		while (Wire.available()) Wire.read();
		return;
	}

	_waitingCommands.writeIndex = (_waitingCommands.writeIndex + 1) % COMMAND_BUFFER_SIZE;

}

void Communication::onRequest(){
	Serial.println(_waitingRequest);
	switch (_waitingRequest) {
		case CALIBRATION_ENDED_RQST:
			Wire.write(_armothy->isMoving());
			break;
		case AXIS_1_POSITION_RQST:
			sendFloat(_armothy->getDoF(Armothy::PRISMATIC_Z_AXIS));
			break;
		case AXIS_2_POSITION_RQST:
			sendFloat(_armothy->getDoF(Armothy::REVOLUTE_Z_AXIS));
			break;
		case AXIS_3_POSITION_RQST:
			sendFloat(_armothy->getDoF(Armothy::REVOLUTE_Y_AXIS));
			break;
		case PUMP_STATE_RQST:
			Wire.write(_armothy->getPumpState());
			break;
		case VALVE_STATE_RQST:
			Wire.write(_armothy->getValveState());
			break;
		case PRESSURE_RQST:
			sendFloat(_armothy->getPressure());
			break;
		case ERROR_BYTE_RQST:
			Wire.write(_armothy-> getErrorByte());
			break;
		case MACRO_STATE_RSQT:
			Wire.write(_macroManager->getStatusByte());
			break;
		default:
			break;
	}

}

void Communication::sendFloat(float toSend){
	uArg f;
	f.f = toSend;
	Wire.write(f.data, 4);
}

void Communication::SOnReceive(int receivedSize){
	_instance->onReceive(receivedSize);
}

void Communication::SOnRequest(){
	_instance->onRequest();
}

void Communication::setInstance(Communication * instance){
	_instance = instance;
}
