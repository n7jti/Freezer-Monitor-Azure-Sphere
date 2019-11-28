// (c) Alan Ludwig. All Rights Reserved
// Licensed under the MIT license

#pragma once

// door.h
#include "trigger.h"

class Door : public Trigger {
public:
  Door(int pin);
  bool begin();
  bool isTriggered();
  int getStatus(char* buffer, int length);
private:
	int _pin; 
	int _fd; 
};

