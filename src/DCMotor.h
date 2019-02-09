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
	void isr_limit_switch();

	float get_position();
	void go_to(double position);
	void home();

	void setup();
	void loop();

	void stop();

	bool isMoving();

private:
	static constexpr float INC_PER_MM = 50;
	static constexpr float KP = 5;
	static constexpr float KI = 0;
	static constexpr float KD = 0;
	static constexpr int MIN_INTEGRAL = -255;
	static constexpr int MAX_INTEGRAL = 255;
	bool _isHoming;
	volatile long _inc;
	int _goal_inc;

	int _prevError;
	int _integralError;
};

void ISR_INC1();
void ISR_INC2();
void ISR_STOP();

#endif /* DCMOTOR_H_ */
