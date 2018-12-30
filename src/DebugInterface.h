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
		DISPLAYING_POSITION
	};
	eState _state;
	enum eCmd{
		HOME = 1,
		CMD_DOF_1 = 2,
		CMD_DOF_2 = 3,
		CMD_DOF_3 = 4,
		TOGGLE_PUMP = 5,
		TOGGLE_VALVE = 6,
		MONITOR = 7
	};

	void displayMenu();
	int parseCmd();
	void processCmd(eCmd cmd);
	int parseFloatValue(float* value);
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
