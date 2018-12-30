/*
 * SuccionCup.h
 *
 *  Created on: 14 déc. 2018
 *      Author: guilhem
 */

#ifndef SUCCIONCUP_H_
#define SUCCIONCUP_H_

namespace armothy {

class SuccionCup {
public:
	enum ePumpState{
		PUMP_ON,
		PUMP_OFF
	};
	enum eValveState{
		VALVE_OPENED,
		VALVE_CLOSED
	};

	SuccionCup();
	virtual ~SuccionCup();

	void setup();
	void loop();

	void startPump();
	void stopPump();
	ePumpState getPumpState();

	void openValve();
	void closeValve();
	eValveState getValveState();

	float getPressure();

protected:
	eValveState _valveState;
	ePumpState _pumpState;
};

} /* namespace armothy */

#endif /* SUCCIONCUP_H_ */
