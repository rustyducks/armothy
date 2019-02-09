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
constexpr int MAJOR_VERSION = 0;
constexpr int MINOR_VERSION = 3;

//######## Network stuff #######
constexpr int I2C_ADDRESS = 0x74;

//######## Pin Layout ########
//#error "Needs to be changed !"
//#warning "Needs to be changed !"
const unsigned int VERTICAL_MOTOR_PWM = 10; // PWM pin
const unsigned int VERTICAL_MOTOR_DIR_A = 2;
const unsigned int VERTICAL_MOTOR_DIR_B = 9;
const unsigned int VERTICAL_ENCODER_A = 11; // Interrupt pin
const unsigned int VERTICAL_ENCODER_B = 12;
const unsigned int VERTICAL_LIMIT_SWITCH = 16; // Interrupt pin

const unsigned int DYNAMIXEL_PIN_RX = 7;
const unsigned int DYNAMIXEL_PIN_TX = 8;

const unsigned int PUMP_PIN = 6; // PWM pin
const unsigned int VALVE_PIN = 5;
const unsigned int SUCCION_SENSOR_PIN = A0; // Analog pin

const unsigned int DEBUG_LED = 13;

//####### Dynamixel Calibration #######
//#error "Needs to be changed"
const unsigned int Z_AXIS_DYNAMIXEL_ID = 1;
const unsigned int Y_AXIS_DYNAMIXEL_ID = 3;
const unsigned int DYNAMIXEL_TO_0_1 = 512;
const unsigned int DYNAMIXEL_TO_0_2 = 512;

//####### Sensors Calibration ########
const float ANALOG_TO_BAR = 0.5;

//####### Loop periods #######
const unsigned int DEBUG_INTERFACE_PERIOD = 100;
const unsigned int COMMUNICATION_PERIOD = 10;
const unsigned int DC_MOTOR_PERIOD = 10;
const unsigned int SUCCION_CUP_PERIOD = 50;


#endif /* PARAMS_H_ */
