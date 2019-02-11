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
		TAKE_AND_STORE_MACRO
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

private:
	Armothy * _armothy;
	AbstractMacro* currentMacro;
	MacroState macroState;
};

} /* namespace armothy */

#endif /* SRC_MACROS_MACROMANAGER_H_ */
