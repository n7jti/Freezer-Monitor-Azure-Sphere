/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

// This minimal Azure Sphere app repeatedly toggles GPIO 8, which is the red channel of RGB
// LED 1 on the MT3620 RDB. Use this app to test that device and SDK installation succeeded
// that you can build, deploy, and debug a CMake app with Visual Studio.
//
// It uses the API for the following Azure Sphere application libraries:
// - gpio (digital input for button)
// - log (messages shown in Visual Studio's Device Output window during debugging)

#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>
// (c) Alan Ludwig. All Rights Reserved
// Licensed under the MIT license

#include <signal.h>

#include <applibs/log.h>

#include "monitor.h"
#include "door.h"
#include "led.h"
#include "buzzer.h"
#include "temp.h"
#include "Adafruit_LEDBackpack.h"
#include "epoll.h"

constexpr int DOOR_PIN = 30;
constexpr int RED_PIN = 34;
constexpr int GREEN_PIN = 32;
constexpr PWM_ControllerId BUZZER_CONTROLLER = 1;
constexpr PWM_ChannelId BUZZER_CHANNEL = 0;

constexpr int MS_PER_MIN = 60000;
constexpr int DOOR_TIMEOUT_MS = MS_PER_MIN / 6;

constexpr struct timespec sleepTime = { 0, 250000000 }; //250ms

// Termination state
static volatile sig_atomic_t quit = false;

float toFarenheit(float c) {
	float f; 
	f = c * 1.8f + 32; 
	return f; 
}

/// <summary>
///     Signal handler for termination requests. This handler must be async-signal-safe.
/// </summary>
static void TerminationHandler(int signalNumber)
{
	// Don't use Log_Debug here, as it is not guaranteed to be async-signal-safe.
	quit = true;
}

void register_termination_handler() {
	// Register a SIGTERM handler for termination requests
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = TerminationHandler;
	sigaction(SIGTERM, &action, NULL);
}


class MainTimer {
public:
	MainTimer()
		: _door(DOOR_PIN, DOOR_TIMEOUT_MS)
		, _temp(0, MCP9600_ADDR)
		, _red_led(RED_PIN)
		, _green_led(GREEN_PIN)
		, _buzzer(BUZZER_CONTROLLER, BUZZER_CHANNEL, 200)
		, _sevenSegment(0)
	{
		_eventData.eventHandler = CallbackWrapper;
		_eventData.context = this;
	}

	int RegisterEventHandler(Epoll& epoll)
	{
		return epoll.RegisterEventHandler(_timer.Get(), &_eventData, EPOLLIN);
	}

	bool begin() {
		if (!_door.begin()) {
			Log_Debug("Door Failed to start!\n");
			return false;
		}

		if (!_temp.begin()) {
			Log_Debug("Temp Failed to start!\n");
			return false;
		}

		if (!_red_led.begin()) {
			Log_Debug("Red LED Failed to start!\n");
			return false;
		}

		if (!_green_led.begin()) {
			Log_Debug("Green LED Failed to start!\n");
			return false;
		}

		if (!_buzzer.begin()) {
			Log_Debug("Buzzer Failed to start!\n");
			return false;
		}

		_sevenSegment.begin();

		struct timespec mainTimerPeriod = { 0, 500 * 1000 * 1000 };  // 0 seconds, 250 miliseconds
		int ret = _timer.Open(&mainTimerPeriod);
		if ( ret < 0){
			Log_Debug("Failed to open timer! %d", ret);
			return false; 
		}

		return true;
	}


	void run() {
		Door::DOOR_STATE door_state = _door.run();
		bool temp_trigger = _temp.run(); 

		switch (door_state) {
		case Door::DOOR_GREEN:
			_red_led.setState(Led::LED_OFF);
			_green_led.setState(Led::LED_ON);
			_buzzer.sound(false);
			break;
		case Door::DOOR_YELLOW:
			_red_led.setState(Led::LED_ON);
			_green_led.setState(Led::LED_OFF);
			_buzzer.sound(false);
			break;
		case Door::DOOR_RED:
			_red_led.setState(Led::LED_BLINK);
			_green_led.setState(Led::LED_OFF);
			_buzzer.sound(true);
			break;
		}

		_red_led.run();
		_green_led.run();
		
		_sevenSegment.print(_temp.getLastTemp() , 1);
		_sevenSegment.writeDisplay();

		(void)temp_trigger; 

	}

	static void CallbackWrapper(EventData* eventData)
	{
		MainTimer* pThis = reinterpret_cast<MainTimer*>(eventData->context);
		pThis->Callback(eventData);
	}

	void Callback(EventData* eventData) {
		run(); 
		if (_timer.ConsumeEvent() != 0)
		{
			quit = true;
		}
		return;
	}


private:
	Door _door;
	Temp _temp; 
	Led _red_led;
	Led _green_led;
	Buzzer _buzzer;
	Adafruit_7segment _sevenSegment;
	EPollTimer _timer; 
	EventData _eventData;
	
};



int main(void)
{
	Log_Debug("Starting Freezer Monitor!...\n");
	register_termination_handler();

	MainTimer mainTimer; 
	
	if (!mainTimer.begin())
	{
		Log_Debug("Main Timer class failed to start!\n");
		return 0;
	}

	Epoll epoll;
	int ret = epoll.Open(); 
	if (ret < 0)
	{
		Log_Debug("Failed to Open Epoll: %d", ret);
		return 0;
	}

	ret = mainTimer.RegisterEventHandler(epoll);
	if (ret < 0) {
		Log_Debug("Failed to register event handler.");
		return 0;
	}

	while (!quit) {
		ret = epoll.WaitForEventAndCallHandler(); 
		if (ret < 0)
		{
			break; 
		}
	}

	return 0; 
}

extern "C" void __cxa_pure_virtual()
{
	while (1);
}

