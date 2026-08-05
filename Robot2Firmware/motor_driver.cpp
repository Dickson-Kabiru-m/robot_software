#include "motor_driver.h"

/*
================================================
 Initialize L298 Drivers
================================================
*/
void motorBegin()
{
    pinMode(LEFT_PWM, OUTPUT);
    pinMode(LEFT_IN1, OUTPUT);
    pinMode(LEFT_IN2, OUTPUT);

    pinMode(RIGHT_PWM, OUTPUT);
    pinMode(RIGHT_IN1, OUTPUT);
    pinMode(RIGHT_IN2, OUTPUT);

    motorSetPWM(0, 0);
}

/*
================================================
 LEFT SIDE CONTROL
 One L298 controls:
 Left Front Motor + Left Rear Motor
================================================
*/
void setLeftMotor(int pwm)
{
    bool reverse = false;

    if (pwm < 0)
    {
        reverse = true;
        pwm = -pwm;
    }

    if (pwm > MOTOR_MAX_PWM)
        pwm = MOTOR_MAX_PWM;

    if (reverse)
    {
        digitalWrite(LEFT_IN1, LOW);
        digitalWrite(LEFT_IN2, HIGH);
    }
    else
    {
        digitalWrite(LEFT_IN1, HIGH);
        digitalWrite(LEFT_IN2, LOW);
    }

    analogWrite(LEFT_PWM, pwm);
}

/*
================================================
 RIGHT SIDE CONTROL
 One L298 controls:
 Right Front Motor + Right Rear Motor
================================================
*/
void setRightMotor(int pwm)
{
    bool reverse = false;

    if (pwm < 0)
    {
        reverse = true;
        pwm = -pwm;
    }

    if (pwm > MOTOR_MAX_PWM)
        pwm = MOTOR_MAX_PWM;

    if (reverse)
    {
        digitalWrite(RIGHT_IN1, LOW);
        digitalWrite(RIGHT_IN2, HIGH);
    }
    else
    {
        digitalWrite(RIGHT_IN1, HIGH);
        digitalWrite(RIGHT_IN2, LOW);
    }

    analogWrite(RIGHT_PWM, pwm);
}

/*
================================================
 Main interface used by PID
================================================
*/
void motorSetPWM(int leftPWM, int rightPWM)
{
    setLeftMotor(leftPWM);
    setRightMotor(rightPWM);
}
