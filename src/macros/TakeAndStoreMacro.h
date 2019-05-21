/*
 * TakeAndStore.h
 *
 *  Created on: 10 févr. 2019
 *      Author: robot
 */

#ifndef SRC_MACROS_TAKEANDSTOREMACRO_H_
#define SRC_MACROS_TAKEANDSTOREMACRO_H_

#include <AbstractMacro.h>

namespace armothy {

class TakeAndStoreMacro: public AbstractMacro {
public:

	TakeAndStoreMacro(Armothy * arm, float stack_height, int stack);
	virtual ~TakeAndStoreMacro();

	void init();


	int  doIt();

	void leave();

private:

	enum Stack {
		LEFT,
		RIGHT
	};

	enum State {
		INITIAL_DESCENT,
		RAISING,
		ROTATION,
		STORE_DESCENT,
		STORE,
		WAIT_ATOM_DROP,
		RAISING_BACK,
		ROTATION_BACK,
		RAISING_ERROR	// only in case of error (e.g. no atom catched)
	};

	Stack _stack;
	float stackHeight;
	float atomHeight;
	float safeHeight;
	unsigned long rotation_time;
	unsigned long pressure_time;
	State state;
};

} /* namespace armothy */

#endif /* SRC_MACROS_TAKEANDSTOREMACRO_H_ */
