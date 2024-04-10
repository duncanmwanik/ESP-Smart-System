#ifndef DEF_H
#define DEF_H

#include <Arduino.h>

#define ON_OFF_BUTTON_PIN 16

#define LIGHT_SENSOR_PIN 4
#define LIGHT_THRESHOLD 500

// One mat wire connects to GND, the other to a touch pin(13) to sense contact when mat is stepped on
#define PRESSURE_PIN GPIO_NUM_23

// BLE
void sendBLEData(int value);

// On-Off Button
bool isButtonOn();

// Light Sensor
bool isDark();

// Pressure Sensor
int isMatSteppedOn();

// deep sleep
void enterDeepSleep();

#endif
