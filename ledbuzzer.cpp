// ledbuzzer.cpp
#include <applibs/gpio.h>
#include "ledbuzzer.h"
#include "utils.h"

LedBuzzer::LedBuzzer(Monitor &monitor, int redpin, int greenpin, int buzzerpin) 
    : Notification(monitor)
    , _redpin(redpin)
    , _greenpin(greenpin)
    , _buzzerpin(buzzerpin)
{

}

bool LedBuzzer::begin()
{
	_redfd = GPIO_OpenAsOutput(_redpin, GPIO_OutputMode_PushPull, GPIO_Value_High);
	_greenfd = GPIO_OpenAsOutput(_greenpin, GPIO_OutputMode_PushPull, GPIO_Value_High);
	_buzzerfd = GPIO_OpenAsOutput(_buzzerpin, GPIO_OutputMode_PushPull, GPIO_Value_High);
	return _redfd >= 0 && _greenfd >= 0 && _buzzerfd >= 0;
}

void LedBuzzer::run()
{
    Notification::run();
}

void LedBuzzer::onRed(bool isNew)
{
    unsigned int ms = millis();
    if ((ms % 1000) < 500)
    {
		GPIO_SetValue(_redfd, GPIO_Value_High); 
    }
    else
    {
		GPIO_SetValue(_redfd, GPIO_Value_Low);
    }
	GPIO_SetValue(_greenfd, GPIO_Value_Low);
	GPIO_SetValue(_buzzerfd, GPIO_Value_High);
}

void LedBuzzer::onYellow(bool isNew)
{
	GPIO_SetValue(_redfd, GPIO_Value_High);
	GPIO_SetValue(_greenfd, GPIO_Value_Low);
	GPIO_SetValue(_buzzerfd, GPIO_Value_Low);
}

void LedBuzzer::onGreen(bool isNew)
{
	GPIO_SetValue(_redfd, GPIO_Value_Low);
	GPIO_SetValue(_greenfd, GPIO_Value_High);
	GPIO_SetValue(_buzzerfd, GPIO_Value_Low);
}
