// (C) Alan Ludwig 2019 All Rights Reserved
#include <time.h>

long millis() {
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_nsec / 1000000;
}