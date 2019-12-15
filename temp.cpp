// (c) Alan Ludwig. All Rights Reserved
// Licensed under the MIT license

#include "temp.h"
#include <stdio.h>

Temp::Temp(I2C_InterfaceId id, I2C_DeviceAddress address)
	: _temp(id, address)
	, _last_temp(0.0f)
	, _set_point(20.0f)
	, _swing(2.0f)
	, _triggered(false)
{
}

bool Temp::begin(float set_point, float swing)
{
	_set_point = set_point;
	_swing = swing; 
	return _temp.mcp9600_begin();
}

static float toFarenheit(float c) {
	float f;
	f = c * 1.8f + 32;
	return f;
}

bool Temp::isTriggered()
{
	float check_value; 

	if (_triggered) {
		check_value = _set_point - _swing / 2.0f;
	}
	else {
		check_value = _set_point + _swing / 2.0f;
	}

	float f = toFarenheit(_temp.getTemprature());
	if (f > check_value) {
		_triggered = true; 
	}
	else {
		_triggered = false; 
	}

	_last_temp = f;
	return _triggered; 
}

bool Temp::run()
{
	return isTriggered(); 
}

float Temp::getLastTemp()
{
	return _last_temp;
}