#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNum(uint32_t Num, uint16_t Length);
uint8_t Serial_GetFlag(void);
uint16_t Serial_GetData(void);

#endif
