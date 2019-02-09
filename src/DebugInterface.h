/*
 * DebugInterface.h
 *
 *  Created on: 18 déc. 2018
 *      Author: guilhem
 */

#ifndef DEBUGINTERFACE_H_
#define DEBUGINTERFACE_H_


namespace armothy {

class Armothy;

class DebugInterface {
protected:
	enum eState{
		NOT_INIT,
		MENU_TO_DISPLAY,
		MENU_DISPLAYED,
		COMMANDING_DOF_1,
		COMMANDING_DOF_2,
		COMMANDING_DOF_3,
		ACODE_MODE,
		DISPLAYING_POSITION
	};
	eState _state;
	enum eCmd{
		CMD_DOF_1 = 1,
		CMD_DOF_2 = 2,
		CMD_DOF_3 = 3,
		HOME = 4,
		TOGGLE_PUMP = 5,
		TOGGLE_VALVE = 6,
		ACODE = 7,
		MONITOR = 8
	};

	struct sAcodeResult{
		char filledValues;  // & 0x01: zTranslationCmd, & 0x02: zRotationCmd, & 0x04: yRotationCmd, & 0x08: valveCmd, & 0x10: pumpCmd
		float zTranslationCmd;
		float zRotationCmd;
		float yRotationCmd;
		bool valveCmd;
		bool pumpCmd;
	};

	void displayMenu();
	int parseCmd();
	void processCmd(eCmd cmd);
	int parseFloatValue(float* value);
	int parseACode(sAcodeResult* res);
	int readACode(sAcodeResult* res);
	/**
	 * Tries to read a DoF command (float) from the Serial.
	 * @return: 1 if q has been entered, 0 if a value is in the `value parameter`, -1 otherwise.
	 */
	int readDoFCommand(float* value);

	char _input[1025];
	unsigned int _inputLength;
	Armothy* _armothy;
public:
	DebugInterface();
	virtual ~DebugInterface();

	void setup(Armothy* arm);
	void loop();
};

} /* namespace armothy */

#endif /* DEBUGINTERFACE_H_ */
