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

        // Check for carriage return or newline to terminate the instruction
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
            // Reset watchdog timer on every received command from ROS 2
            watchdogReset();

            char command = inputString.charAt(0);
            
            float a1 = 0.0;
            float a2 = 0.0;
            float a3 = 0.0;

            int firstSpace = inputString.indexOf(' ');

            if (firstSpace != -1)
            {
                // Get the string containing only the arguments
                String argsStr = inputString.substring(firstSpace + 1);
                
                // Convert to a standard C-string character pointer for sequential parsing
                char* pEnd;
                char* startPtr = (char*)argsStr.c_str();

                // Extract each argument sequentially using strtod (String to Double/Float)
                // This completely bypasses the broken Arduino Uno %f sscanf constraint.
                a1 = strtod(startPtr, &pEnd);
                if (startPtr != pEnd) {
                    a2 = strtod(pEnd, &pEnd);
                    if (pEnd != NULL) {
                        a3 = strtod(pEnd, NULL);
                    }
                }
            }

            processCommand(command, a1, a2, a3);

            inputString = "";
            commandReady = false;
        }
    }
}

void processCommand(char command, float arg1, float arg2, float arg3)
{
    switch (command)
    {
        case 'e':
            // ROS asks for encoders. We return cumulative ticks separated by a space.
            // NO text confirmation is permitted here.
            Serial.print(encoders.getLeftTicks());
            Serial.print(" ");
            Serial.println(encoders.getRightTicks());
            break;

        case 'm':
            // ROS sets target velocities. We pass them directly to the PID controllers.
            leftPID.setTarget(arg1);
            rightPID.setTarget(arg2);
            break;

        case 'r':
            encoders.reset();
            break;

        case 'o':
            motorSetPWM((int)arg1, (int)arg2);
            break;

        case 'p':
            leftPID.setTunings(arg1, arg2, arg3);
            rightPID.setTunings(arg1, arg2, arg3);
            break;

        case 's':
            motorSetPWM(0, 0);
            leftPID.reset();
            rightPID.reset();
            break;

        default:
            // Do nothing to avoid polluting the buffer if trash data is received
            break;
    }
}
