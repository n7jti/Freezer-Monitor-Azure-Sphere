// (c) Alan Ludwig. All Rights Reserved
// Licensed under the MIT license

#pragma once

// ledbuzzer.h
#include <applibs/pwm.h>
#include "notification.h"

class LedBuzzer : public Notification
{
public:
    LedBuzzer(Monitor &monitor, int redpin, int greenpin, PWM_ControllerId buzzer_controller, PWM_ChannelId buzzer_channel);
	~LedBuzzer();
    virtual bool begin();
    void run();
protected:
    virtual void onRed(bool isNew);
    virtual void onYellow(bool isNew);
    virtual void onGreen(bool isNew); 
private:
	void Tone(bool enabled);

    int _redpin; 
	int _redfd; 
    int _greenpin;
	int _greenfd; 

    PWM_ControllerId _buzzercontroller; 
	PWM_ChannelId _buzzerchannel;
	int _buzzerfd; 

    

};