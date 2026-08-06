#ifndef SERIAL_PROTOCOL_H
#define SERIAL_PROTOCOL_H

#include <Arduino.h>

void serialBegin();
void serialUpdate();
void processCommand(char command, long arg1, long arg2, long arg3);

#endif
