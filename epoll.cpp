// (c) Alan Ludwig. All Rights Reserved.
// Licensed under the MIT license

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <applibs/log.h>
#include "epoll.h"


EPollTimer::EPollTimer()
	: _timerFd(-1)
{

}

EPollTimer::~EPollTimer()
{
	Close(); 
}

int EPollTimer::Open(const struct timespec* period)
{
	Close(); 

	// Create the timerfd and arm it by setting the interval to period
	int timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
	if (timerFd < 0) {
		Log_Debug("ERROR: Could not create timerfd: %s (%d).\n", strerror(errno), errno);
		return -1;
	}

	_timerFd = timerFd;

	return SetToPeriod(period); 	 
}

void EPollTimer::Close()
{
	if (_timerFd >= 0)
	{
		close(_timerFd);
		_timerFd = -1;
	}
}

int EPollTimer::Get()
{ 
	return _timerFd; 
}

int EPollTimer::SetToPeriod(const struct timespec* period)
{
	struct itimerspec newValue = { .it_interval = *period, .it_value = *period };

	if (timerfd_settime(_timerFd, 0, &newValue, NULL) < 0) {
		Log_Debug("ERROR: Could not set Timer period: %s (%d).\n", strerror(errno), errno);
		return -1;
	}

	return 0;
}

int EPollTimer::SetToSingleExpiry(const struct timespec* expiry)
{
	struct itimerspec newValue = { .it_interval = {}, .it_value = *expiry };

	if (timerfd_settime(_timerFd, 0, &newValue, NULL) < 0) {
		Log_Debug("ERROR: Could not set timerfd interval: %s (%d).\n", strerror(errno), errno);
		return -1;
	}

	return 0;

}
int EPollTimer::ConsumeEvent()
{
	uint64_t timerData = 0;

	if (read(_timerFd, &timerData, sizeof(timerData)) == -1) {
		Log_Debug("ERROR: Could not read timerfd %s (%d).\n", strerror(errno), errno);
		return -1;
	}

	return 0;
}

Epoll::Epoll()
	:_epollFd(-1)
{

}

Epoll::~Epoll()
{
	Close(); 
}

int Epoll::Open()
{
	int epollFd = -1;

	Close();
	epollFd = epoll_create1(0);
	if (epollFd == -1) {
		Log_Debug("ERROR: Could not create epoll instance: %s (%d).\n", strerror(errno), errno);
		return -1;
	}
	_epollFd = epollFd;
	return _epollFd;
}
void Epoll::Close()
{
	if (_epollFd >= 0)
	{
		close(_epollFd);
		_epollFd = -1; 
	}
}

int Epoll::RegisterEventHandler(int eventFd, EventData* persistentEventData, const uint32_t epollEventMask)
{
	struct epoll_event eventToAddOrModify;
	eventToAddOrModify.events = epollEventMask;
	eventToAddOrModify.data.ptr = persistentEventData;

	persistentEventData->fd = eventFd;
	
	// Register the eventFd on the epoll instance referred by epollFd
	// and register the eventHandler handler for events in epollEventMask.
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, eventFd, &eventToAddOrModify) == -1) {
		// If the Add fails, retry with the Modify as the file descriptor has already been
		// added to the epoll set after it was removed by the kernel upon its closure.
		if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, eventFd, &eventToAddOrModify) == -1) {
			Log_Debug("ERROR: Could not register event to epoll instance: %s (%d).\n",
				strerror(errno), errno);
			return -1;
		}
	}

	return 0;
}

int Epoll::UnregisterEventHandler(int eventFd)
{
	int res = 0;
	// Unregister the eventFd on the epoll instance referred by epollFd.
	if ((res = epoll_ctl(_epollFd, EPOLL_CTL_DEL, eventFd, NULL)) == -1) {
		if (res == -1 && errno != EBADF) { // Ignore EBADF errors
			Log_Debug("ERROR: Could not remove event from epoll instance: %s (%d).\n",
				strerror(errno), errno);
			return -1;
		}
	}

	return 0;
}

int Epoll::WaitForEventAndCallHandler()
{
	struct epoll_event event;
	int numEventsOccurred = epoll_wait(_epollFd, &event, 1, -1);

	if (numEventsOccurred == -1) {
		if (errno == EINTR) {
			// interrupted by signal, e.g. due to breakpoint being set; ignore
			return 0;
		}
		Log_Debug("ERROR: Failed waiting on events: %s (%d).\n", strerror(errno), errno);
		return -1;
	}

	if (numEventsOccurred == 1 && event.data.ptr != NULL) {
		EventData* eventData = reinterpret_cast<EventData*>(event.data.ptr);
		eventData->eventHandler(eventData);
	}

	return 0;
}