#ifndef __OLED_H
#define __OLED_H

typedef struct
{
	uint8_t current;
	uint8_t up;//向上翻索引号
	uint8_t down;//向下翻索引号
	uint8_t enter;//确认索引号
	void (*current_operation)();
	void (*enter_operation)();
	void (*up_operation)();
	void (*down_operation)();
	void (*left_operation)();
	void (*right_operation)();
}key_table;

extern key_table table[15];

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowChinese(uint8_t Line, uint8_t Column, uint16_t Num);
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[]);
void OLED_DrawBMP_Spaceman(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t count);

void fun_0(void);
void fun_a1(void);
void fun_b1(void);
void fun_c1(void);

void fun_a21(void);
void fun_a22(void);
void fun_a23(void);

void fun_b21(void);
void fun_c21(void);
void fun_d21(void);

void Light_ON(void);
void Light_OFF(void);
void DataSelect(void);
void DataPlus(void);
void DataMinus(void);
void Manu_Backto_1(void);
void DataSendOk(void);

#endif
