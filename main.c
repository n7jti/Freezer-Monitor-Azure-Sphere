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

#include "freezerMonitor.h"

static int button = -1;
const struct timespec sleepTime = { 1, 0 };

void buzzerBeep(int buzzerId)
{
	GPIO_SetValue(buzzerId, GPIO_Value_High);
	nanosleep(&sleepTime, NULL);
	GPIO_SetValue(buzzerId, GPIO_Value_Low);
}

int main(void)
{
	Log_Debug("Starting CMake Hello World application...\n");

	setup();

	// button pin assignment
	button = GPIO_OpenAsInput(34);

	// buzzer
	int buzzer = GPIO_OpenAsOutput(4, GPIO_OutputMode_PushPull, GPIO_Value_Low);

	// LED pin assignments
	int redLED = GPIO_OpenAsOutput(31, GPIO_OutputMode_PushPull, GPIO_Value_Low);
	int greenLED = GPIO_OpenAsOutput(35, GPIO_OutputMode_PushPull, GPIO_Value_Low);

	/*
	if (fd < 0) {
		Log_Debug(
			"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
			strerror(errno), errno);
		return -1;
	}
	*/

	GPIO_Value_Type buttonState;
	GPIO_Value_Type prevButtonState;

	while (true) {
		loop(); 
		/*
		GPIO_SetValue(fd, GPIO_Value_Low);
		nanosleep(&sleepTime, NULL);
		GPIO_SetValue(fd, GPIO_Value_High);
		nanosleep(&sleepTime, NULL);
		*/

		GPIO_GetValue(button, &buttonState);
		if (!(buttonState == prevButtonState))
		{
			GPIO_SetValue(redLED, buttonState);
			GPIO_SetValue(greenLED, !buttonState);
			prevButtonState = buttonState;

			buzzerBeep(buzzer);
		}
	}
}

