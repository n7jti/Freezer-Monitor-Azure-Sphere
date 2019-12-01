// (c) Alan Ludwig. All Rights Reserved
// Licensed under the MIT license

#include "notification.h"

Notification::Notification(Monitor &monitor)
    : _monitor(monitor)
{

}

bool Notification::begin()
{
    _lastState = _monitor.getState();
	return _lastState;
}

void Notification::run()
{
    MONITOR_STATE monitorState = _monitor.getState(); 

    switch(monitorState){
    case MONITOR_GREEN:
        onGreen(_lastState != MONITOR_GREEN);
        break;
    case MONITOR_YELLOW:
        onYellow(_lastState != MONITOR_YELLOW);
        break;
    case MONITOR_RED:
        onRed(_lastState != MONITOR_RED);
        break;
    }


    _lastState = _monitor.getState();
}
