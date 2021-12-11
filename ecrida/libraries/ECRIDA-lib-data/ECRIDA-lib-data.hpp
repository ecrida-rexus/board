#ifndef _ECRIDA_LIB_DATA_H_
#define _ECRIDA_LIB_DATA_H_

#include <Adafruit_ADXL343.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <Artekit_TMP116.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>

extern Adafruit_ADXL343 accel;
extern Artekit_TMP116 pcb_temperature;

void ECRIDA_DATA_setup_gpio();

#endif