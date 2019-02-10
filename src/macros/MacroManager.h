/*
 * MacroManager.h
 *
 *  Created on: 10 févr. 2019
 *      Author: Fabien-B
 */

#ifndef SRC_MACROS_MACROMANAGER_H_
#define SRC_MACROS_MACROMANAGER_H_

#include "AbstractMacro.h"


namespace armothy {

class Armothy;

class MacroManager {
public:

	enum MacroState {
		IDLE,
		RUNNING,
		FINISHED
	};

	MacroManager();
	virtual ~MacroManager();

	void setup(Armothy * arm);
	void loop();

	void setMacro(AbstractMacro* macro);

private:
	Armothy * _armothy;
	AbstractMacro* currentMacro;
	MacroState macroState;
};

} /* namespace armothy */

#endif /* SRC_MACROS_MACROMANAGER_H_ */
