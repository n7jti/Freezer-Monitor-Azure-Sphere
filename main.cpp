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
#include "ledbuzzer.h"
#include "mcp9600.h"
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
		: _door(DOOR_PIN)
		, _monitor(&_door, DOOR_TIMEOUT_MS)
		, _ledBuzzer(_monitor, RED_PIN, GREEN_PIN, BUZZER_CONTROLLER, BUZZER_CHANNEL)
	{
		_eventData.eventHandler = CallbackWrapper;
		_eventData.context = this;
	}

	int RegisterEventHandler(Epoll& epoll)
	{
		return epoll.RegisterEventHandler(_timer.Get(), &_eventData, EPOLLIN);
	}

	bool begin() {
		if (!_monitor.begin()) {
			Log_Debug("Monitor Failed to start!\n");
			return false;
		}

		if (!_ledBuzzer.begin()) {
			Log_Debug("LedBuzzer Failed to start!\n");
			return false;
		}

		struct timespec mainTimerPeriod = { 0, 500 * 1000 * 1000 };  // 0 seconds, 250 miliseconds
		int ret = _timer.Open(&mainTimerPeriod);
		if ( ret < 0){
			Log_Debug("Failed to open timer! %d", ret);
			return false; 
		}

		return true;
	}

	void run() {
		_monitor.run();
		_ledBuzzer.run(); 
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
	Monitor _monitor;
	LedBuzzer _ledBuzzer;
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

	CMcp9600 tempSensor(0, MCP9600_ADDR);
	if (!tempSensor.mcp9600_begin()) {
		Log_Debug("Temp Sensor Failed to start!\n");
		return 0; 
	}

	tempSensor.setAdcResolution(MCP9600_ADC_RES_14);
	tempSensor.setFilterCoefficients(1);

	Adafruit_7segment sevenSegment(0);
	sevenSegment.begin(0x70);

	while (!quit) {
		ret = epoll.WaitForEventAndCallHandler(); 
		if (ret < 0)
		{
			break; 
		}

		float temperature = tempSensor.getTemprature();
		Log_Debug("Temperature: %f C  %f F\n", temperature, toFarenheit(temperature));
		sevenSegment.print(toFarenheit(temperature), 1);
		sevenSegment.writeDisplay();
	}

	return 0; 
}

extern "C" void __cxa_pure_virtual()
{
	while (1);
}

