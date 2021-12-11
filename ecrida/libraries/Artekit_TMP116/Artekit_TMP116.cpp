/***************************************************************************
 * TMP116 - Library for Arduino
 * 
 * Artekit Labs AK-TMP116N - Digital Temperature Sensor Breakout
 * https://www.artekit.eu/products/breakout-boards/ak-tmp116n/
 *
 * Copyright (c) 2019 Artekit Labs
 * https://www.artekit.eu

### Artekit_TMP116.cpp

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

#include "Artekit_TMP116.h"

bool Artekit_TMP116::begin()
{
	// Initialize I2C
	Wire.begin();

	// Check for ID
	uint16_t id = readRegister(TMP116_REG_DEVICE_ID);
	if (id & 0xFFF != 0x116)
		return false;

	// Set continuous conversion at 500ms and 8 averages
	return writeRegister(TMP116_REG_CONFIGURATION, 0x01, 0xA0);
}

float Artekit_TMP116::readTemperature()
{
	// Read value
	uint16_t temp = readRegister(TMP116_REG_TEMPERATURE);

	// Convert and return
	return (float) temp * 0.0078125f;
}

bool Artekit_TMP116::readTemperature(float* temperature)
{
	if (!temperature)
		return false;

	// Check for data ready
	if (!dataReady())
		return false;

	// Read value
	uint16_t temp = readRegister(TMP116_REG_TEMPERATURE);

	// Convert and return
	*temperature = (float) temp * 0.0078125f;
	return true;
}

bool Artekit_TMP116::dataReady()
{
	uint16_t value = 0;

	if (!readRegister(TMP116_REG_CONFIGURATION, &value))
		return false;

	return ((value >> 13) & 0x01) == 1;
}

bool Artekit_TMP116::setLowLimit(float low)
{
	// Set the low threshold
	uint16_t temp = (uint16_t) (low / 0.0078125f);
	return writeRegister(TMP116_REG_LOW_LIMIT, temp);
}

bool Artekit_TMP116::setHighLimit(float high)
{
	// Set the low threshold
	uint16_t temp = (uint16_t) (high / 0.0078125f);
	return writeRegister(TMP116_REG_HIGH_LIMIT, temp);
}

bool Artekit_TMP116::setLowHighLimit(float low, float high)
{
	return (setLowLimit(low) && setHighLimit(high));
}

void Artekit_TMP116::clearAlert()
{
	readRegister(TMP116_REG_CONFIGURATION);
}

TMP116_Alert Artekit_TMP116::getAlertType()
{
	uint16_t reg = readRegister(TMP116_REG_CONFIGURATION);
	
	// No alerts?
	if ((reg & 0xC000) == 0x00)
		return TMP116_NoAlert;

	// Both alerts?
	if ((reg & 0xC000) == 0xC000)
		return TMP116_AlertLowHigh;

	// High or low alert?
	if ((reg & 0xC000) == 0x8000)
		return TMP116_AlertHigh;
	else
		return TMP116_AlertLow;
}

void Artekit_TMP116::end()
{
	// Set sensor in shutdown mode
	writeRegister(TMP116_REG_CONFIGURATION, 0x800);

	delay(25);

	// Disable Wire
	Wire.end();
}

uint16_t Artekit_TMP116::readRegister(uint8_t address)
{
	uint16_t value = 0;
	readRegister(address, &value);
	return value;
}

bool Artekit_TMP116::readRegister(uint8_t address, uint8_t* valueL, uint8_t* valueH)
{
	if (!valueL || !valueH)
		return false;

	Wire.beginTransmission(dev_address);
	Wire.write(address);
	Wire.endTransmission();
	Wire.requestFrom((int) dev_address, (int) 2);

	while (Wire.available() < 2);

	*valueH = Wire.read();
	*valueL = Wire.read();
	return true;
}

bool Artekit_TMP116::readRegister(uint8_t address, uint16_t* value)
{
	if (!value)
		return false;

	uint8_t valueL = 0, valueH = 0;
	if (!readRegister(address, &valueL, &valueH))
		return false;

	uint8_t* data = (uint8_t*) value;

	data[0] = valueL;
	data[1] = valueH;
	
	/*
	Serial.print("Register ");
	Serial.print(address);
	Serial.print(" = ");
	Serial.print(data[0]);
	Serial.print(" ");
	Serial.println(data[1]);
	*/
	return true;
}

bool Artekit_TMP116::writeRegister(uint8_t address, uint16_t value)
{
	uint8_t* data = (uint8_t*) &value;
	return writeRegister(address, data[0], data[1]);
}

bool Artekit_TMP116::writeRegister(uint8_t address, uint8_t valueL, uint8_t valueH)
{
	Wire.beginTransmission(dev_address);
	Wire.write(address);
	Wire.write(valueH);
	Wire.write(valueL);
	Wire.endTransmission();
	return true;
}
