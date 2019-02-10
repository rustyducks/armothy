/*
 * CatchMacro.h
 *
 *  Created on: 10 févr. 2019
 *      Author: robot
 */

#ifndef SRC_MACROS_CATCHMACRO_H_
#define SRC_MACROS_CATCHMACRO_H_

#include <AbstractMacro.h>

namespace armothy {

class Armothy;

class CatchMacro: public AbstractMacro {
public:
	CatchMacro(Armothy * arm);
	virtual ~CatchMacro();

	void init();

	// return: True if ended
	bool  doIt();

	void leave();
};

} /* namespace armothy */

#endif /* SRC_MACROS_CATCHMACRO_H_ */
