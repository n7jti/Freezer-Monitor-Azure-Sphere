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
#include <signal.h>

#include <applibs/log.h>

#include "monitor.h"
#include "door.h"
#include "ledbuzzer.h"
#include "mcp9600.h"
#include "Adafruit_LEDBackpack.h"

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

int main(void)
{
	Log_Debug("Starting CMake Hello World application...\n");
	register_termination_handler();

	Door door(DOOR_PIN);

	Monitor monitor(&door, DOOR_TIMEOUT_MS);
	if (!monitor.begin()) {
		Log_Debug("Monitor Failed to start!\n");
		quit = true; 
	}

	LedBuzzer ledBuzzer(monitor, RED_PIN, GREEN_PIN, BUZZER_CONTROLLER, BUZZER_CHANNEL);
	if (!ledBuzzer.begin()) {
		Log_Debug("Monitor Failed to start!\n");
		quit = true; 
	}

	CMcp9600 tempSensor(0, MCP9600_ADDR);
	if (!tempSensor.mcp9600_begin()) {
		Log_Debug("Temp Sensor Failed to start!\n");
		quit = true;
	}

	Adafruit_7segment sevenSegment(0);
	sevenSegment.begin(0x70);

	while (!quit) {
		monitor.run(); 
		ledBuzzer.run();
		nanosleep(&sleepTime, NULL); // sleep for 250ms

		float temperature = tempSensor.getTemprature();
		Log_Debug("Temperature: %f\n", temperature);
		sevenSegment.print(toFarenheit(temperature));
		sevenSegment.writeDisplay();
	}

	return 0; 
}

extern "C" void __cxa_pure_virtual()
{
	while (1);
}

