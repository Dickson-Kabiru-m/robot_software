#include <Arduino.h>
#include "config.h"
#include "motor_driver.h"
#include "encoder_driver.h"
#include "pid_controller.h"
#include "serial_protocol.h"
#include "watchdog.h"

/*
================================================
 PID LOOP
================================================
*/
unsigned long lastPIDUpdate = 0;

/*
================================================
 SETUP
================================================
*/
void setup()
{
    /*
      Start serial communication
    */
    serialBegin();

    /*
      Initialize L298 drivers
    */
    motorBegin();

    /*
      Initialize encoders
    */
    encoders.begin();

    /*
      Initial PID tuning
      These are starting values.
      They will be tuned later.
    */
    leftPID.setTunings(2.0, 0.0, 0.2);
    rightPID.setTunings(2.0, 0.0, 0.2);

    /*
      Start watchdog timer
    */
    watchdogReset();

    // REMOVED: Serial.println("Robot2 Firmware Ready"); 
    // This keeps the serial buffer 100% clean of text strings for ROS 2.
}

/*
================================================
 MAIN LOOP
================================================
*/
void loop()
{
    /*
      Always listen for commands from Serial
    */
    serialUpdate();

    /*
      PID update loop (20Hz)
    */
    if (millis() - lastPIDUpdate >= PID_PERIOD)
    {
        lastPIDUpdate = millis();

        /*
          Update encoder differences
          Gives ticks travelled since last PID cycle
        */
        encoders.update();

        long leftTicks = encoders.getLeftDelta();
        long rightTicks = encoders.getRightDelta();

        /*
          Compute PID output
        */
        float leftPWM = leftPID.update((float)leftTicks);
        float rightPWM = rightPID.update((float)rightTicks);

        /*
          Send PWM to motors
        */
        motorSetPWM((int)leftPWM, (int)rightPWM);
    }

    /*
      Safety watchdog
      If Raspberry Pi stops sending velocity commands, stop robot.
    */
    if (watchdogExpired())
    {
        motorSetPWM(0, 0);
        leftPID.reset();
        rightPID.reset();
    }
}
