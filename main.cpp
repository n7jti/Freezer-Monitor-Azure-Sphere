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

constexpr int DOOR_PIN = 8;
constexpr int RED_PIN = 5;
constexpr int GREEN_PIN = 6;
constexpr int BUZZER_PIN = 4;

constexpr int MS_PER_MIN = 60000;
constexpr int DOOR_TIMEOUT_MS = MS_PER_MIN;


int main(void)
{
	Log_Debug("Starting CMake Hello World application...\n");

	Door door(DOOR_PIN);
	if (!door.begin()) {
		Log_Debug("Door Failed to start!\n");
	}

	Monitor monitor(&door, DOOR_TIMEOUT_MS);
	if(!monitor.begin()) {
		Log_Debug("Monitor Failed to start!\n");
	}


	// buzzer
	// int buzzer = GPIO_OpenAsOutput(4, GPIO_OutputMode_PushPull, GPIO_Value_Low);

	// LED pin assignments
	// int redLED = GPIO_OpenAsOutput(5, GPIO_OutputMode_PushPull, GPIO_Value_Low);
	// int greenLED = GPIO_OpenAsOutput(6, GPIO_OutputMode_PushPull, GPIO_Value_Low);

	while (true) {
		monitor.run(); 
		
	}
}

extern "C" void __cxa_pure_virtual()
{
	while (1);
}

