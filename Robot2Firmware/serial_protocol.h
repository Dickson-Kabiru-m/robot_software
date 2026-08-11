#ifndef SERIAL_PROTOCOL_H
#define SERIAL_PROTOCOL_H

#include <Arduino.h>

void serialBegin();
void serialUpdate();
void processCommand(char command, float arg1, float arg2, float arg3);

#endif
