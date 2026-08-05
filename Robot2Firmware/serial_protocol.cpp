#include "serial_protocol.h"
#include "encoder_driver.h"
#include "motor_driver.h"
#include "pid_controller.h"
#include "watchdog.h"

String inputString = "";
bool commandReady = false;

void serialBegin()
{
    Serial.begin(SERIAL_BAUD);
    inputString.reserve(40);
}

void serialUpdate()
{
    while (Serial.available())
    {
        char c = Serial.read();

        if (c == '\r' || c == '\n')
        {
            if (inputString.length() > 0)
            {
                commandReady = true;
            }
        }
        else
        {
            inputString += c;
        }

        if (commandReady)
        {
            // Reset watchdog timer on every received command
            watchdogReset();

            char command = inputString.charAt(0);
            long a1 = 0;
            long a2 = 0;
            long a3 = 0;

            int firstSpace = inputString.indexOf(' ');

            if (firstSpace != -1)
            {
                String args = inputString.substring(firstSpace + 1);
                sscanf(args.c_str(), "%ld %ld %ld", &a1, &a2, &a3);
            }

            processCommand(command, a1, a2, a3);

            inputString = "";
            commandReady = false;
        }
    }
}

void processCommand(char command, long arg1, long arg2, long arg3)
{
    switch (command)
    {
        case 'e':
            Serial.print(encoders.getLeftTicks());
            Serial.print(" ");
            Serial.println(encoders.getRightTicks());
            break;

        case 'r':
            encoders.reset();
            Serial.println("OK");
            break;

        case 'o':
            motorSetPWM((int)arg1, (int)arg2);
            Serial.println("OK");
            break;

        case 'm':
            leftPID.setTarget((float)arg1);
            rightPID.setTarget((float)arg2);
            Serial.println("OK");
            break;

        case 'p':
            leftPID.setTunings((float)arg1, (float)arg2, (float)arg3);
            rightPID.setTunings((float)arg1, (float)arg2, (float)arg3);
            Serial.println("OK");
            break;

        case 's':
            motorSetPWM(0, 0);
            leftPID.reset();
            rightPID.reset();
            Serial.println("OK");
            break;

        default:
            Serial.println("UNKNOWN");
            break;
    }
}
