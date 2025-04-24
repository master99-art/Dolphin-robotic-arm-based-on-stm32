#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>

//ион╩╩З 
void Serial_1_Init(void);
void Serial_1_SendByte(uint8_t Byte);
uint8_t Serial_1_GetRxData(void);
uint8_t Serial_1_GetRxFlag(void);

void Serial_2_Init(void);
void Serial_2_SendByte(uint8_t Byte);
//void Serial_2_SendString(char *String);
//void Serial_2_Printf(char *format, ...);

#endif

