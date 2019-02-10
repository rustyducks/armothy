/*
 * TakeAndStore.h
 *
 *  Created on: 10 févr. 2019
 *      Author: robot
 */

#ifndef SRC_MACROS_TAKEANDSTORE_H_
#define SRC_MACROS_TAKEANDSTORE_H_

#include <AbstractMacro.h>

namespace armothy {

class TakeAndStore: public AbstractMacro {
public:

	TakeAndStore();
	virtual ~TakeAndStore();

	void init();

	// return: True if ended
	bool  doIt();

	void leave();

private:
	enum State {
		INITIAL_DESCENT,
		RAISING,
		STORE_DESCENT
	};
};

} /* namespace armothy */

#endif /* SRC_MACROS_TAKEANDSTORE_H_ */
