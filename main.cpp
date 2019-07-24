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

#include <applibs/log.h>
#include <applibs/gpio.h>

#include "monitor.h"
#include "door.h"
#include "ledbuzzer.h"
#include "mcp9600.h"

constexpr int DOOR_PIN = 8;
constexpr int RED_PIN = 5;
constexpr int GREEN_PIN = 6;
constexpr int BUZZER_PIN = 4;

constexpr int MS_PER_MIN = 60000;
constexpr int DOOR_TIMEOUT_MS = MS_PER_MIN / 6;

constexpr struct timespec sleepTime = { 0, 250000000 }; //250ms

int main(void)
{
	Log_Debug("Starting CMake Hello World application...\n");
	bool quit = false; 

	Door door(DOOR_PIN);

	Monitor monitor(&door, DOOR_TIMEOUT_MS);
	if (!monitor.begin()) {
		Log_Debug("Monitor Failed to start!\n");
		quit = true; 
	}

	LedBuzzer ledBuzzer(monitor, RED_PIN, GREEN_PIN, BUZZER_PIN);
	if (!ledBuzzer.begin()) {
		Log_Debug("Monitor Failed to start!\n");
		quit = true; 
	}

	CMcp9600 tempSensor(0, MCP9600_ADDR);
	if (!tempSensor.mcp9600_begin()) {
		Log_Debug("Temp Sensor Failed to start!\n");
		quit = true;
	}

	while (!quit) {
		monitor.run(); 
		ledBuzzer.run();
		nanosleep(&sleepTime, NULL); // sleep for 250ms

		if (tempSensor.testTermocouple() == -1)
		{
			quit = true;
		}
	}

	return 0; 
}

extern "C" void __cxa_pure_virtual()
{
	while (1);
}

