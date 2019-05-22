/*
 * AbstractMacro.h
 *
 *  Created on: 10 févr. 2019
 *      Author: Fabien-B
 */

#ifndef SRC_MACROS_ABSTRACTMACRO_H_
#define SRC_MACROS_ABSTRACTMACRO_H_

namespace armothy{
class Armothy;

class AbstractMacro {
public:

	enum MacroStatus {
		MACRO_STATUS_FINISHED = 1,
		MACRO_STATUS_RUNNING = 2,
		MACRO_STATUS_ERROR = 4,
		MACRO_STATUS_RUNNING_SAFE = 8
	};

	AbstractMacro(Armothy * arm);
	virtual ~AbstractMacro();

	void setup(Armothy * arm);

	virtual void init() = 0;

	// return: FINISHED if ended, ERROR or RUNNING
	virtual int  doIt() = 0;

	virtual void leave() = 0;

protected:
	Armothy* _armothy;
};
}

#endif /* SRC_MACROS_ABSTRACTMACRO_H_ */
