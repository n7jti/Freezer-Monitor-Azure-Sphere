// ledbuzzer.cpp

#include <unistd.h>
#include <applibs/gpio.h>
#include "ledbuzzer.h"
#include "utils.h"

LedBuzzer::LedBuzzer(Monitor& monitor, int redpin, int greenpin, PWM_ControllerId buzzer_controller, PWM_ChannelId buzzer_channel)
	: Notification(monitor)
	, _redpin(redpin)
	, _redfd(-1)
	, _greenpin(greenpin)
	, _greenfd(-1)
	, _buzzercontroller(buzzer_controller)
	, _buzzerchannel(buzzer_channel)
{

}

static void safe_close_fd(int& fd) {
	if (fd >= 0) {
		close(fd);
		fd = -1;
	}
}

LedBuzzer::~LedBuzzer()
{
	safe_close_fd(_redfd);
	safe_close_fd(_greenfd);
	safe_close_fd(_buzzerfd);
}

bool LedBuzzer::begin()
{
	_redfd = GPIO_OpenAsOutput(_redpin, GPIO_OutputMode_PushPull, GPIO_Value_High);
	_greenfd = GPIO_OpenAsOutput(_greenpin, GPIO_OutputMode_PushPull, GPIO_Value_High);
	_buzzerfd = PWM_Open(_buzzercontroller);
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
	Tone(true);
}

void LedBuzzer::onYellow(bool isNew)
{
	GPIO_SetValue(_redfd, GPIO_Value_High);
	GPIO_SetValue(_greenfd, GPIO_Value_Low);
	Tone(false);
}

void LedBuzzer::onGreen(bool isNew)
{
	GPIO_SetValue(_redfd, GPIO_Value_Low);
	GPIO_SetValue(_greenfd, GPIO_Value_High);
	Tone(false);
}

void LedBuzzer::Tone(bool enabled)
{
	constexpr unsigned int ns_per_sec = 1000000000U;
	constexpr unsigned int hz = 200;
	PwmState pwmState;
	pwmState.period_nsec = ns_per_sec / hz;  //(period = 1/fq so, period in ns = 1e9/hz)
	pwmState.dutyCycle_nsec = pwmState.period_nsec / 2;
	pwmState.polarity = PWM_Polarity_Normal;
	pwmState.enabled = enabled;

	PWM_Apply(_buzzerfd, _buzzerchannel, &pwmState); 
}
