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

using namespace armothy;

Communication * Communication::_instance = nullptr;

Communication::Communication(): _waitingCommand(START_CALIBRATION_CMD), _isCommandWaiting(false), _waitingRequest(CALIBRATION_ENDED_RQST), _armothy(nullptr){
}

void Communication::setup(Armothy * arm){
	_armothy = arm;
	setInstance(this);
	_cmdArg.f = 0.0;
	Wire.begin(I2C_ADDRESS);
	Wire.onReceive(Communication::SOnReceive);
	Wire.onRequest(Communication::SOnRequest);
}

void Communication::loop(){
	if (_isCommandWaiting){
		_isCommandWaiting = false;
		if (_waitingCommand == START_CALIBRATION_CMD){
			_armothy->home();
		}else if (_waitingCommand == DIRECT_AXIS_1_CMD){
			_armothy->sendActuatorCommand(Armothy::PRISMATIC_Z_AXIS, _cmdArg.f);
		}else if (_waitingCommand == DIRECT_AXIS_2_CMD){
			_armothy->sendActuatorCommand(Armothy::REVOLUTE_Z_AXIS, _cmdArg.f);
		}else if (_waitingCommand == DIRECT_AXIS_3_CMD){
			_armothy->sendActuatorCommand(Armothy::REVOLUTE_Y_AXIS, _cmdArg.f);
		}else if (_waitingCommand == STOP_PUMP_CMD){
			_armothy->stopPump();
		}else if (_waitingCommand == START_PUMP_CMD){
			_armothy->startPump();
		}else if (_waitingCommand == CLOSE_VALVE_CMD){
			_armothy->closeValve();
		}else if (_waitingCommand == OPEN_VALVE_CMD){
			_armothy->openValve();
		}else if (_waitingCommand == EMERGENCY_STOP_CMD){
			_armothy->emergencyStop();
		}
	}
}

void Communication::onReceive(int receivedSize){
	uint8_t r = Wire.read();
	if (r <= PRESSURE_RQST){
		if (r <= EMERGENCY_STOP_CMD){
			_isCommandWaiting = true;
			_waitingCommand = (eCommandByte)r;
			if (_waitingCommand == DIRECT_AXIS_1_CMD || _waitingCommand == DIRECT_AXIS_2_CMD ||
					_waitingCommand == DIRECT_AXIS_3_CMD){
				if (receivedSize == 5){
					for (int i = 0; i < 4; i++){
						_cmdArg.data[i] = Wire.read();
					}
				}else{
					// This should not happen... (it means we received a direct command without arguments)
				}
			}
		}else{
			_waitingRequest = (eCommandByte)r;
		}

	}else{
		// The command sent is not a valid command. Maybe a desynchronisation problem (so empty the buffer) ?
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
	uFloat f;
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
