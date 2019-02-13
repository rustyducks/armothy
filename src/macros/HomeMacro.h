/*
 * HomeMacro.h
 *
 *  Created on: 12 févr. 2019
 *      Author: robot
 */

#ifndef SRC_MACROS_HOMEMACRO_H_
#define SRC_MACROS_HOMEMACRO_H_

#include <AbstractMacro.h>

namespace armothy {

class HomeMacro: public AbstractMacro {
public:
	HomeMacro(Armothy * arm);
	virtual ~HomeMacro();

	void init();
	int  doIt();
	void leave();

private:

	static constexpr float SPEED = -10.0;
	unsigned long _switch_off_time;
	enum State{
		STANDING_BY,
		GOING_DOWN_WITH_SWITCH,
		GOING_DOWN_WITHOUT_SWITCH,
		GOING_UP
	};

	State _state;
};

} /* namespace armothy */

#endif /* SRC_MACROS_HOMEMACRO_H_ */
