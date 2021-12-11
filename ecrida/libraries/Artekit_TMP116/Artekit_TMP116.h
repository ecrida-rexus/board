/***************************************************************************
 * TMP116 - Library for Arduino
 * 
 * Artekit Labs AK-TMP116N - Digital Temperature Sensor Breakout
 * https://www.artekit.eu/products/breakout-boards/ak-tmp116n/
 *
 * Copyright (c) 2019 Artekit Labs
 * https://www.artekit.eu

### Artekit_TMP116.h

#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.

***************************************************************************/

#ifndef __Artekit_TMP116_H__
#define __Artekit_TMP116_H__

#include <Arduino.h>
#include <Wire.h>

#define TMP116_ADDRESS				0x48
#define TMP116_REG_TEMPERATURE		0x00
#define TMP116_REG_CONFIGURATION	0x01
#define TMP116_REG_HIGH_LIMIT		0x02
#define TMP116_REG_LOW_LIMIT		0x03
#define TMP116_REG_EEPROM_UNLOCK	0x04
#define TMP116_REG_EEPROM1			0x05
#define TMP116_REG_EEPROM2			0x06
#define TMP116_REG_EEPROM3			0x07
#define TMP116_REG_EEPROM4			0x08
#define TMP116_REG_DEVICE_ID		0x0F

typedef enum
{
	TMP116_NoAlert,
	TMP116_AlertLow,
	TMP116_AlertHigh,
	TMP116_AlertLowHigh,
} TMP116_Alert;

class Artekit_TMP116
{
public:
	Artekit_TMP116(uint8_t address = TMP116_ADDRESS) : dev_address(address) {}

	bool begin();
	void end();

	float readTemperature();
	bool readTemperature(float* temperature);

	bool setLowLimit(float low);
	bool setHighLimit(float high);
	TMP116_Alert getAlertType();
	bool setLowHighLimit(float low, float high);
	void clearAlert();

	uint16_t readRegister(uint8_t address);
	bool readRegister(uint8_t address, uint8_t* valueL, uint8_t* valueH);
	bool readRegister(uint8_t address, uint16_t* value);
	bool writeRegister(uint8_t address, uint16_t value);
	bool writeRegister(uint8_t address, uint8_t valueL, uint8_t valueH);

	bool dataReady();

private:
	uint8_t dev_address;
};

#endif /* __Artekit_TMP116_H__ */
