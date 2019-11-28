// (c) Alan Ludwig. All Rights Reserved.
// Licensed under the MIT license

#pragma once

#include <time.h>
#include <sys/epoll.h>
#include <unistd.h>

class EventData;

// Callback Event Handler
typedef void (*EventHandler)(EventData* eventData);

class EventData {
public:
	EventHandler eventHandler; 
	int fd; 
};

class Timer {
public:
	Timer();
	~Timer();

	int Open(const struct timespec* period);
	void Close(); 
	int Get();
	int SetToPeriod(const struct timespec* period);
	int SetToSingleExpiry(const struct timespec* expiry);
	int ConsumeEvent();

private:
	int _timerFd;

};

class Epoll {
public:
	Epoll();
	~Epoll();

	int Open();
	void Close();
	int RegisterEventHandler(int eventFd, EventData* persistentEventData, const uint32_t epollEventMask);
	int UnregisterEventHandler(int eventFd);
	int WaitForEventAndCallHandler();

private:
	int _epollFd; 
};



