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
		uArg arg = _waitingCommands.commands[_waitingCommands.readIndex].arg;
		_waitingCommands.readIndex = (_waitingCommands.readIndex + 1) % COMMAND_BUFFER_SIZE;
		if (cmd == START_CALIBRATION_CMD){
			_armothy->home();
		}else if (cmd == DIRECT_AXIS_1_CMD){
			_armothy->sendActuatorCommand(Armothy::PRISMATIC_Z_AXIS, arg.f);
		}else if (cmd == DIRECT_AXIS_2_CMD){
			_armothy->sendActuatorCommand(Armothy::REVOLUTE_Z_AXIS, arg.f);
		}else if (cmd == DIRECT_AXIS_3_CMD){
			_armothy->sendActuatorCommand(Armothy::REVOLUTE_Y_AXIS, arg.f);
		}else if (cmd == STOP_PUMP_CMD){
			_armothy->stopPump();
		}else if (cmd == START_PUMP_CMD){
			_armothy->startPump();
		}else if (cmd == CLOSE_VALVE_CMD){
			_armothy->closeValve();
		}else if (cmd == OPEN_VALVE_CMD){
			_armothy->openValve();
		} else if(cmd == MACRO_CMD) {
			_macroManager->setMacro((MacroManager::MacrosNumber)arg.i);
		}else if (cmd == EMERGENCY_STOP_CMD){
			_armothy->emergencyStop();
		}
	}
}

void Communication::onReceive(int receivedSize){
	uint8_t r = Wire.read();
	if ((_waitingCommands.writeIndex + 1) % COMMAND_BUFFER_SIZE == _waitingCommands.readIndex){
		// Command buffer full...
		while (Wire.available()) Wire.read();
	}else{
		if (r <= PRESSURE_RQST){
			if (r <= EMERGENCY_STOP_CMD){
				eCommandByte cmd = (eCommandByte)r;
				_waitingCommands.commands[_waitingCommands.writeIndex].cmd = cmd;
				if (cmd == DIRECT_AXIS_1_CMD || cmd == DIRECT_AXIS_2_CMD || cmd == DIRECT_AXIS_3_CMD){
					if (receivedSize == 5){
						for (int i = 0; i < 4; i++){
							_waitingCommands.commands[_waitingCommands.writeIndex].arg.data[i] = Wire.read();
						}
					}else{
						// This should not happen... (it means we received a direct command without arguments)
					}
				}
				else if(cmd == MACRO_CMD) {
					if (receivedSize == 2){
						_waitingCommands.commands[_waitingCommands.writeIndex].arg.data[0] = Wire.read();
					} else {
						// This should not happen... (it means we received a macro command without argument)
					}
				}
				_waitingCommands.writeIndex = (_waitingCommands.writeIndex + 1) % COMMAND_BUFFER_SIZE;
			}else{
				_waitingRequest = (eCommandByte)r;
			}
		}else{
			// The command sent is not a valid command. Maybe a desynchronisation problem (so empty the buffer) ?
			while (Wire.available()) Wire.read();
		}
	}
}

void Communication::onRequest(){
	if (_waitingRequest == CALIBRATION_ENDED_RQST){
		Wire.write(_armothy->isMoving());
	}else if (_waitingRequest == AXIS_1_POSITION_RQST){
		sendFloat(_armothy->getDoF(Armothy::PRISMATIC_Z_AXIS));
	}else if (_waitingRequest == AXIS_2_POSITION_RQST){
		sendFloat(_armothy->getDoF(Armothy::REVOLUTE_Z_AXIS));
	}else if(_waitingRequest == AXIS_3_POSITION_RQST){
		sendFloat(_armothy->getDoF(Armothy::REVOLUTE_Y_AXIS));
	}else if (_waitingRequest == PUMP_STATE_RQST){
		Wire.write(_armothy->getPumpState());
	}else if (_waitingRequest == VALVE_STATE_RQST){
		Wire.write(_armothy->getValveState());
	}else if (_waitingRequest == PRESSURE_RQST){
		sendFloat(_armothy->getPressure());
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
