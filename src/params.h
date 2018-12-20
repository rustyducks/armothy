/*
 * params.h
 *
 *  Created on: 4 déc. 2018
 *      Author: guilhem
 */

#ifndef PARAMS_H_
#define PARAMS_H_

#include "Arduino.h"

//######## Versioning ########
constexpr char MAJOR_VERSION = 0;
constexpr char MINOR_VERSION = 1;

//######## Network stuff #######
constexpr char I2C_ADDRESS = 0x74;

//######## Pin Layout ########
#error "Needs to be changed !"
#warning "Needs to be changed !"
const unsigned int VERTICAL_MOTOR_PWM = 0; // PWM pin
const unsigned int VERTICAL_MOTOR_DIR = 1;
const unsigned int VERTICAL_ENCODER_A = 2; // Interrupt pin
const unsigned int VERTICAL_ENCODER_B = 3;
const unsigned int VERTICAL_LIMIT_SWITCH = 4; // Interrupt pin

const unsigned int DYNAMIXEL_PIN_RX = 5;
const unsigned int DYNAMIXEL_PIN_TX = 6;

const unsigned int PUMP_PIN = 7; // PWM pin
const unsigned int VALVE_PIN = 8;
const unsigned int SUCCION_SENSOR_PIN = A0; // Analog pin

//####### Dynamixel Calibration #######
#error "Needs to be changed"
const unsigned int Z_AXIS_DYNAMIXEL_ID = 0;
const unsigned int Y_AXIS_DYNAMIXEL_ID = 1;
const unsigned int DYNAMIXEL_TO_0_1 = 90;
const unsigned int DYNAMIXEL_TO_0_2 = 0;


#endif /* PARAMS_H_ */
