// (c) Alan Ludwig. All Rights Reserved
// Licensed under the MIT license


#include "utils.h"
#include <applibs/log.h>
#include <time.h>


long millis() {
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
}