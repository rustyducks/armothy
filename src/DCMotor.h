/*
 * DCMotor.h
 *
 *  Created on: 3 déc. 2018
 *      Author: guilhem
 */

#ifndef DCMOTOR_H_
#define DCMOTOR_H_

class DCMotor {
public:
	DCMotor();
	virtual ~DCMotor(){};

	void isr_inc1();
	void isr_inc2();

	float get_position();
	void go_to(float position);

	void setup();
	void loop();

	void stop();

	bool isMoving();
	void checkCurrent();

	bool isSpeedControlActivated() const {
		return _speedControlActivated;
	}

	void setSpeedControlActivated(bool speedControlActivated);

	void setSpeedSetPoint(float speedSetPoint) {
		_speedSetPoint = speedSetPoint;
	}

	bool isLimitReached() const {
		return _limitReached;
	}

	void setLimitReached(bool limitReached) {
		_limitReached = limitReached;
	}

	void setHome() {
		_inc = 0;
	}

	void setMotorCommand(int command);

private:
	static constexpr float INC_PER_MM = 55.5;
	static constexpr float KP = 15;
	static constexpr float KI = 0.4;
	static constexpr float KD = 0;
	static constexpr float KP_SPEED = 2;
	static constexpr float KI_SPEED = 1;
	static constexpr float MIN_INTEGRAL = -127;
	static constexpr float MAX_INTEGRAL = 127;
	static constexpr float Z_MAX = 130;


	void speedControl();

	void positionControl();

	volatile long _inc;
	long _prev_inc;
	float _goal;
	float _speed;

	float _prevError;
	float _integralError;

	float _speedSetPoint;
	bool _speedControlActivated;
	bool _limitReached;
	float _intSpeedError;

	int _current;
};

void ISR_INC1();
void ISR_INC2();
void ISR_LIMIT_SWITCH();

#endif /* DCMOTOR_H_ */
