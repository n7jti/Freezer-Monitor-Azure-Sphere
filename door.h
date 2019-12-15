// (c) Alan Ludwig. All Rights Reserved
// Licensed under the MIT license

#pragma once

// door.h
#include "trigger.h"

class Door {
public:
	enum DOOR_STATE{
		DOOR_RED,
		DOOR_YELLOW,
		DOOR_GREEN
	};

	Door(int pin, long timeout);
	bool begin();
	bool isOpen();
	DOOR_STATE getState();
	DOOR_STATE run();


private:
	int _pin; 
	int _fd; 
	long _timeout;
	DOOR_STATE _state;
	long _stateStartMs;

};

