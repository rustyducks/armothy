/*
 * PutDownMacro.h
 *
 *  Created on: 17 mai 2019
 *      Author: fabien
 */

#ifndef SRC_MACROS_PUTDOWNMACRO_H_
#define SRC_MACROS_PUTDOWNMACRO_H_

#include <AbstractMacro.h>

namespace armothy {

class PutDownMacro: public AbstractMacro {
public:

	PutDownMacro(Armothy * arm, float stack_height, int stack, int rotation_drop, float drop_height = 300);
	virtual ~PutDownMacro();

	void init();


	int  doIt();

	void leave();

private:

	enum Stack {
		LEFT,
		RIGHT
	};

	enum State {
		INIT_RAISING,
		INIT_ROTATION,
		STACK_DESCENT,
		RAISING,
		ROTATION,
		PUT_DESCENT,
		DROPING,
		WAIT_ATOM_DROP,
		RAISING_BACK,
		ROTATION_BACK
	};

	Stack _stack;
	float stackHeight;
	float dropHeight;
	int rotationDrop;


	float safeHeight;
	unsigned long rotation_time;
	unsigned long pressure_time;
	State state;
};

} /* namespace armothy */

#endif /* SRC_MACROS_PUTDOWNMACRO_H_ */
