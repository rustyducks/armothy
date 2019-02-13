/*
 * MacroManager.h
 *
 *  Created on: 10 févr. 2019
 *      Author: Fabien-B
 */

#ifndef SRC_MACROS_MACROMANAGER_H_
#define SRC_MACROS_MACROMANAGER_H_

#include "AbstractMacro.h"
#include "Communication.h"

namespace armothy {

class Armothy;

class MacroManager {
public:

	enum MacrosNumber {
		CATCH_MACRO,
		TAKE_AND_STORE_MACRO,
		HOME_MACRO
	};

	enum MacroState {
		IDLE,
		RUNNING,
		FINISHED
	};



	MacroManager();
	virtual ~MacroManager();

	void setup(Armothy * arm);
	void loop();
	void stop();

	void setMacro(MacrosNumber macroNb, Communication::uArg *args);

	uint8_t getStatusByte() {
		return _statusByte;
	}

	void setStatusByte(uint8_t statusByte) {
		_statusByte = statusByte;
	}

private:
	Armothy * _armothy;
	AbstractMacro* currentMacro;
	MacroState macroState;

	uint8_t _statusByte;
};

} /* namespace armothy */

#endif /* SRC_MACROS_MACROMANAGER_H_ */
