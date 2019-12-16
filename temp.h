// (c) Alan Ludwig. All Rights Reserved
// Licensed under the MIT license

#pragma once

// temp.h
#include "mcp9600.h"

#define I2C_STRUCTS_VERSION 0
#include <applibs/i2c.h>

class Temp {
public:
	Temp(I2C_InterfaceId id, I2C_DeviceAddress address);
	bool begin(float set_point = 20, float swing = 2.0);
	bool isTriggered();
	bool run();
	float getLastTemp();
private:
	CMcp9600 _temp;
	float _last_temp; 
	float _set_point;
	float _swing;
	bool _triggered; 
};


