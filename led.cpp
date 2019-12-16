
#include <unistd.h>
#include <applibs/gpio.h>
#include "led.h"
#include "utils.h"

Led::Led(int pin)
	: _fd(-1)
	, _pin(pin)
{

}
Led::~Led() 
{
	safe_close_fd(_fd);
}

bool Led::begin()
{
	_fd = GPIO_OpenAsOutput(_pin, GPIO_OutputMode_PushPull, GPIO_Value_Low);
	return _fd >= 0;
}

int Led::setState(LED_STATE state)
{
	int ret; 
	_state = state; 

	switch (_state)
	{
	case LED_OFF:
		ret = GPIO_SetValue(_fd, GPIO_Value_Low);
		break;
	case LED_ON:
	case LED_BLINK:
		ret = GPIO_SetValue(_fd, GPIO_Value_High);
		break;
	}
	return ret; 
}

Led::LED_STATE Led::run()
{
	if (_state == LED_BLINK)
	{
		unsigned int ms = millis();
		if ((ms % 1000) < 500)
		{
			(void)GPIO_SetValue(_fd, GPIO_Value_High);
		}
		else
		{
			(void)GPIO_SetValue(_fd, GPIO_Value_Low);
		}
	}

	return _state; 
}

