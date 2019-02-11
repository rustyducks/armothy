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
	static float stackHeight;		//TODO remove static, and take it as argument
public:

	TakeAndStoreMacro(Armothy * arm);
	virtual ~TakeAndStoreMacro();

	void init();

	// return: True if ended
	bool  doIt();

	void leave();

private:

	enum State {
		INITIAL_DESCENT,
		RAISING,
		ROTATION,
		STORE_DESCENT,
		STORE
	};

	float atomHeight;
	float safeHeight;
	unsigned long rotation_time;
	State state;
};

} /* namespace armothy */

#endif /* SRC_MACROS_TAKEANDSTOREMACRO_H_ */
