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
            
            // Crucial: Use floats for values! Josh's driver sends speeds 
            // that parse much cleaner as floating-point target variables.
            float a1 = 0.0;
            float a2 = 0.0;
            float a3 = 0.0;

            int firstSpace = inputString.indexOf(' ');

            if (firstSpace != -1)
            {
                String args = inputString.substring(firstSpace + 1);
                // Parse arguments as floats to prevent truncating small speed increments
                sscanf(args.c_str(), "%f %f %f", &a1, &a2, &a3);
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
            // NO "OK" text is permitted here.
            Serial.print(encoders.getLeftTicks());
            Serial.print(" ");
            Serial.println(encoders.getRightTicks());
            break;

        case 'm':
            // ROS sets target velocities. We pass them directly to the PID controllers.
            // Crucial Fix: Removed the Serial.println("OK") statement!
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
            // Serial.println("OK");
            break;

        case 's':
            motorSetPWM(0, 0);
            leftPID.reset();
            rightPID.reset();
            //Serial.println("OK");
            break;

        default:
            // Do nothing to avoid polluting the buffer if trash data is received
            break;
    }
}
