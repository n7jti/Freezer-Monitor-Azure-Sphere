

#include "utils.h"
#include "buzzer.h"



Buzzer::Buzzer(PWM_ControllerId buzzer_controller, PWM_ChannelId buzzer_channel, unsigned int hz)
	: _fd(-1)
	, _controller(buzzer_controller)
	, _channel(buzzer_channel)
	, _hz(hz)
{

}

Buzzer::~Buzzer()
{
	safe_close_fd(_fd);
}

bool Buzzer::begin()
{
	_fd = PWM_Open(_controller);
	return _fd >= 0; 
}

int Buzzer::sound(bool enabled)
{
	constexpr unsigned int ns_per_sec = 1000000000U;
	PwmState pwmState;
	pwmState.period_nsec = ns_per_sec / _hz;  //(period = 1/fq so, period in ns = 1e9/hz)
	pwmState.dutyCycle_nsec = pwmState.period_nsec / 2;
	pwmState.polarity = PWM_Polarity_Normal;
	pwmState.enabled = enabled;

	return PWM_Apply(_fd, _channel, &pwmState);
}