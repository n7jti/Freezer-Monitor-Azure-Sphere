// (c) Alan Ludwig. All Rights Reserved
// Licensed under the MIT license

#include "door.h"
#include <string.h>
#include <applibs/gpio.h>
#include <applibs/log.h>
#include "utils.h"

Door::Door(int pin, long timeout)
	:_pin(pin)
	, _fd(-1)
	, _timeout(timeout)
	, _state(DOOR_GREEN)
	, _stateStartMs(millis()) 
{
  
}

bool Door::begin()
{
  _fd = GPIO_OpenAsInput(_pin);
  return _fd >= 0; 
}

bool Door::isOpen()
{
	GPIO_Value_Type val = GPIO_Value_Low; 
  // The magnetic switch connects the pin to ground.
  // The magnetic switch is normally open, but is pulled closed by the magnet.
  // The pin is pulled-up and reads high when the switch is open 
  // We want our state variables to be HIGH when the door is open and LOW when closed. 
  // The pin reads high when the door is open. 
  if (GPIO_GetValue(_fd, &val) < 0) {
	Log_Debug("Failed to get GPIO value\n");
  }
  return val == GPIO_Value_High;
}


Door::DOOR_STATE Door::getState()
{
	return _state;
}

Door::DOOR_STATE Door::run()
{
	long now = millis();
	long elapsed = now - _stateStartMs;
	bool door_open = isOpen();

	switch (_state)
	{
	case DOOR_RED:
		if (door_open == false)
		{
			_stateStartMs = now;
			_state = DOOR_GREEN;
		}
		break;
	case DOOR_YELLOW:
		if (door_open == false)
		{
			_stateStartMs = now;
			_state = DOOR_GREEN;
		}
		else if (elapsed > _timeout)
		{
			_state = DOOR_RED;
		}
		break;
	case DOOR_GREEN:
		if (door_open == true)
		{
			_stateStartMs = now;
			_state = DOOR_YELLOW;
		}
		break;
	}

	return _state;
}
