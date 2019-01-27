/*
 * DebugInterface.cpp
 *
 *  Created on: 18 déc. 2018
 *      Author: guilhem
 */

#include <DebugInterface.h>
#include "params.h"
#include "Armothy.h"

namespace armothy {

DebugInterface::DebugInterface(): _state(MENU_TO_DISPLAY), _inputLength(0), _armothy(nullptr){

}

DebugInterface::~DebugInterface() {
	// TODO Auto-generated destructor stub
}

void DebugInterface::displayMenu(){
	Serial.print('\n');
	Serial.print("Armothy v");Serial.print(MAJOR_VERSION);Serial.print(".");Serial.print(MINOR_VERSION);
	Serial.println(". Debug interface menu (enter 1-7 and press enter) :");
	Serial.println("1. Go to home position");
	Serial.println("2. Command DoF 1 (z translation)");
	Serial.println("3. Command DoF 2 (z rotation)");
	Serial.println("4. Command DoF 3 (y rotation)");
	Serial.println("5. Toggle Vacuum Pump");
	Serial.println("6. Toggle Valve");
	Serial.println("7. Monitor DoF Values (press a key to quit)");
}

void DebugInterface::setup(Armothy* arm){
	_armothy = arm;
	_inputLength = 0;
	Serial.begin(115200);
	Serial.println("I have been constructed !");
}

void DebugInterface::loop(){
	float value;
	int ret;
	switch (_state){
	case NOT_INIT:
		break;
	case MENU_TO_DISPLAY:
		while(Serial.available()){
			Serial.read();
		}
		displayMenu();
		_state = MENU_DISPLAYED;
		break;
	case MENU_DISPLAYED:
		if (Serial.available() > 0){
			int toRead = Serial.available();
			for (int i = 0; i < toRead; i++){
				int inChar = Serial.read();
				if (isDigit(inChar)){
					_input[_inputLength] = (char)inChar;
					if (_inputLength < 1024){
						_inputLength++;
					}else{
						_inputLength = 0;
					}
				}else if (inChar == '\r'){
					_input[_inputLength] = '\0';
					int cmd = parseCmd();
					_inputLength = 0;
					if (cmd < 1 || cmd > 7){
						Serial.println(cmd);
						Serial.println("You need to enter a value between 1 and 7...");
						_state = MENU_TO_DISPLAY;
					}else{
						processCmd((eCmd)cmd);
					}

				}
			}
		}
		break;

	case COMMANDING_DOF_1:
		ret = readDoFCommand(&value);
		if (ret == 0){
			_armothy->sendActuatorCommand(Armothy::PRISMATIC_Z_AXIS, value);
		}else if (ret == 1){
			_state = MENU_TO_DISPLAY;
		}else if (ret == -1){
			Serial.println("Please enter a valid float (or q to quit).");
		}
		break;
	case COMMANDING_DOF_2:
		ret = readDoFCommand(&value);
		if (ret == 0){
			_armothy->sendActuatorCommand(Armothy::REVOLUTE_Z_AXIS, value);
		}else if (ret == 1){
			_state = MENU_TO_DISPLAY;
		}else if (ret == -1){
			Serial.println("Please enter a valid float (or q to quit).");
		}
		break;
	case COMMANDING_DOF_3:
		ret = readDoFCommand(&value);
		if (ret == 0){
			_armothy->sendActuatorCommand(Armothy::REVOLUTE_Y_AXIS, value);
		}else if (ret == 1){
			_state = MENU_TO_DISPLAY;
		}else if (ret == -1){
			Serial.println("Please enter a valid float (or q to quit).");
		}
		break;
	case DISPLAYING_POSITION:
		if (Serial.available() > 0){
			_state = MENU_TO_DISPLAY;
		}
		Serial.print(_armothy->getDoF(Armothy::PRISMATIC_Z_AXIS));
		Serial.print('\t');
		Serial.print(_armothy->getDoF(Armothy::REVOLUTE_Z_AXIS));
		Serial.print('\t');
		Serial.print(_armothy->getDoF(Armothy::REVOLUTE_Y_AXIS));
		Serial.println();
		break;
	}
}

int DebugInterface::parseCmd(){
	int cmd = 0;
	if (sscanf(_input, "%d", &cmd) != 1){
		return -1;
	}else{
		return cmd;
	}
}

int DebugInterface::parseFloatValue(float* value){
	if (sscanf(_input, "%f", value) != 1){
		return -1;
	}else{
		return 0;
	}
}

int DebugInterface::readDoFCommand(float* value){
	if (Serial.available() > 0){
		int toRead = Serial.available();
		for (int i = 0; i < toRead; i++){
			int inChar = Serial.read();
			if (isDigit(inChar)||inChar == '-'||inChar == '.'){
				_input[_inputLength] = (char)inChar;
				_inputLength++;
				return -3; // Read one char... Waiting for \r
			}else if (inChar == '\r'){
				_input[_inputLength] = '\0';
				int parsed = parseFloatValue(value);
				_inputLength = 0;
				if (parsed){
					Serial.println("You need to enter a valid double...");
					return -1;
				}else{
					return 0;
				}
			}else if (inChar == (int)'q'){
				return 1;
			}
		}
	}else{
		return -2; // Nothing to read
	}
	return -1;
}

void DebugInterface::processCmd(eCmd cmd){
	switch (cmd){
	case HOME:
		Serial.println("Homing...");
		_armothy->home();
		_state = MENU_TO_DISPLAY;
		break;
	case CMD_DOF_1:
		Serial.println("Controlling DoF 1. Enter values and press enter. Enter q to quit.");
		_state = COMMANDING_DOF_1;
		break;
	case CMD_DOF_2:
		Serial.println("Controlling DoF 2. Enter values and press enter. Enter q to quit.");
		_state = COMMANDING_DOF_2;
		break;
	case CMD_DOF_3:
		Serial.println("Controlling DoF 3. Enter values and press enter. Enter q to quit.");
		_state = COMMANDING_DOF_3;
		break;
	case TOGGLE_PUMP:
		if (_armothy->getPumpState() == Armothy::PUMP_OFF){
			Serial.println("Starting pump.");
			_armothy->startPump();
		}else{
			Serial.println("Stopping pump.");
			_armothy->stopPump();
		}
		_state = MENU_TO_DISPLAY;
		break;
	case TOGGLE_VALVE:
		if (_armothy->getValveState() == Armothy::VALVE_CLOSED){
			Serial.println("Opening valve.");
			_armothy->openValve();
		}else{
			Serial.println("Closing valve.");
			_armothy->closeValve();
		}
		_state = MENU_TO_DISPLAY;
		break;
	case MONITOR:
		Serial.println("Monitoring DoF values. Press enter to quit.");
		_state = DISPLAYING_POSITION;
		break;
	}
}

} /* namespace armothy */
