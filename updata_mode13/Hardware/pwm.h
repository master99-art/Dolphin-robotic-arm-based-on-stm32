#ifndef __PWM_H
#define __PWM_H

void PWM_Init(void);

void PWM1(u16 compare1);
void PWM2(u16 compare2);
void PWM3(u16 compare3);
void PWM4(u16 compare4);
void Servo_SetAngle1(float Angle);
void Servo_SetAngle2(float Angle);
void Servo_SetAngle3(float Angle);
void Servo_SetAngle4(float Angle);

#endif


