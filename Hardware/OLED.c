#include "stm32f10x.h"
#include "OLED.h"
#include "OLED_Font.h"
#include "OLED_Chinese.h"
#include "OLED_BMP.h"
//#include "OLED_BMP_Spaceman.h"
#include "Delay.h"
#include "Serial.h"
#include "AD.h"
#include <stddef.h>

/*引脚配置*/
#define OLED_W_SCL(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_0, (BitAction)(x))
#define OLED_W_SDA(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_1, (BitAction)(x))

uint8_t BlueTooth_Data = 0x00;

uint8_t func_index;
uint8_t last_index;
//Keyboard
int8_t Keyboard_Num = 0;
uint8_t Select_Count = 0;
uint8_t SelectNum_High;
uint8_t SelectNum_Low;

/*引脚初始化*/
void OLED_I2C_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(Byte & (0x80 >> i));
		OLED_W_SCL(1);
		OLED_W_SCL(0);
	}
	OLED_W_SCL(1);	//额外的一个时钟，不处理应答信号
	OLED_W_SCL(0);
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x00);		//写命令
	OLED_I2C_SendByte(Command); 
	OLED_I2C_Stop();
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x40);		//写数据
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
	
//	OLED_ShowString(1,1,"                ");
//	OLED_ShowString(2,1,"                ");
//	OLED_ShowString(3,1,"                ");
//	OLED_ShowString(4,1,"                ");
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

void OLED_ShowChinese(uint8_t Line, uint8_t Column, uint16_t Num)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 16; i++)
	{
		OLED_WriteData(OLED_Chinese[Num*2][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 16; i++)
	{
		OLED_WriteData(OLED_Chinese[Num*2+1][i]);		//显示下半部分内容
	}
}

void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[])
{
	uint16_t i;
	uint8_t x,y;

	for(y=y0; y<y1; y++)
	{
		OLED_SetCursor(y, x0);
		for(x=x0; x<x1; x++)
		{
			OLED_WriteData(BMP[i++]);
		}
	}
}

void OLED_DrawBMP_Spaceman(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t count)
{
	//uint16_t i;
	uint8_t x,y;
	
	for(y=y0; y<y1; y++)
	{
		OLED_SetCursor(y, x0);
		for(x=x0; x<x1; x++)
		{
			//OLED_WriteData(Spaceman[count][i++]);
		}
	}
}

void OLED_DrawBMP_Keyboard(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t count)
{
	uint16_t i;
	uint8_t x,y;
	
	for(y=y0; y<y1; y++)
	{
		OLED_SetCursor(y, x0);
		for(x=x0; x<x1; x++)
		{
			OLED_WriteData(Keyboard[count][i++]);
		}
	}
}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			//上电延时
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			//端口初始化
	
	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	//设置显示开始行
	
	OLED_WriteCommand(0xA0);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommand(0xC0);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示
		
	OLED_Clear();				//OLED清屏
}

void fun_0(void)
{
	OLED_Clear();
//	uint8_t i,j,k;
//	for(j=0; j<4; j++)
//	{
//		for(i=0; i<28; i++)
//		{
//			OLED_DrawBMP_Spaceman(33,0,97,8,i);
//			Delay_ms(30);
//		}
//	}
//	for(i=28,k=32; i>0; i--,k--)
//	{
//		for(j=0 ;j<8; j++)
//		{
//			OLED_SetCursor(j, k+64);
//			OLED_WriteData(0x00);
//		}
//		OLED_DrawBMP_Spaceman(i,0,i+64,8,28-i);
//		Delay_ms(30);
//	}
	OLED_ShowString(2,11,"Enter");
	
}

void fun_a1(void)
{
	OLED_Clear();
	OLED_ShowString(1,1,"> Light");
	OLED_ShowString(2,1,"  SendData");
	OLED_ShowString(3,1,"  Battery");
	OLED_ShowString(4,1,"  Time");
}

void fun_b1(void)
{
	//OLED_Clear();
	OLED_ShowString(1,1,"  Light");
	OLED_ShowString(2,1,"> SendData");
	OLED_ShowString(3,1,"  Battery");
	OLED_ShowString(4,1,"  Time");
}

void fun_c1(void)
{
	//OLED_Clear();
	OLED_ShowString(1,1,"  Light");
	OLED_ShowString(2,1,"  SendData");
	OLED_ShowString(3,1,"> Battery");
	OLED_ShowString(4,1,"  Time");
}

void fun_d1(void)
{
	//OLED_Clear();
	OLED_ShowString(1,1,"  Light");
	OLED_ShowString(2,1,"  SendData");
	OLED_ShowString(3,1,"  Battery");
	OLED_ShowString(4,1,"> Time");
}

void fun_a21(void)
{
	OLED_Clear();
	OLED_ShowString(1,1,"> Light_On");
	OLED_ShowString(2,1,"  Light_Off");
	OLED_ShowString(3,1,"  return");
}

void fun_a22(void)
{
	//OLED_Clear();
	OLED_ShowString(1,1,"  Light_On");
	OLED_ShowString(2,1,"> Light_Off");
	OLED_ShowString(3,1,"  return");
}

void fun_a23(void)
{
	//OLED_Clear();
	OLED_ShowString(1,1,"  Light_On");
	OLED_ShowString(2,1,"  Light_Off");
	OLED_ShowString(3,1,"> return");
}

void fun_b21(void)
{
	OLED_Init();
	OLED_Clear();
	OLED_ShowString(1,1,"SendData:");
	Keyboard_Num = 0;
	//OLED_DrawBMP_Keyboard(0,2,128,8,Keyboard_Num);
}

void fun_b22(void)
{
	OLED_ShowString(2,1,"SendData:");
	OLED_ShowHexNum(2,12,BlueTooth_Data,2);
	OLED_ShowString(4,1,"Send OK");
}

void fun_b23(void)
{
	BlueTooth_Data--;
	OLED_ShowString(2,1,"SendData:");
	OLED_ShowHexNum(2,12,BlueTooth_Data,2);
}

void fun_b24(void)
{
	OLED_ShowString(2,1,"SendData:");
	OLED_ShowHexNum(2,12,BlueTooth_Data,2);
	OLED_ShowString(4,1,"Send OK");
}

void fun_c21(void)
{
	OLED_Clear();
	uint16_t AD_Value;
	float Voltage,Voltage_Percent;
	uint8_t i,Voltage_Percent_int;
	AD_Value = AD_GetValue();
	Voltage = (float)AD_Value/4095*3.3;
	if(Voltage < 2.50)
	{
		Voltage_Percent_int = 0;
	}
	else
	{
		Voltage_Percent = (Voltage*100-370)/(420-370);
		Voltage_Percent_int = (uint8_t)(Voltage_Percent*100);
	}
	OLED_ShowString(2,1,"Power:      %");
	OLED_ShowString(1,1,"Voltage:  .  V");
	OLED_ShowNum(1, 10, Voltage, 1);
	OLED_ShowNum(1, 12, (uint16_t)(Voltage*100)%100, 2);
	if(Voltage_Percent_int == 100)
	{
		OLED_ShowNum(2, 10, Voltage_Percent_int, 3);
	}
	else
	{
		OLED_ShowNum(2, 11, Voltage_Percent_int, 2);
	}
	OLED_SetCursor(5,11);
	for(i=0;i<106;i++)
	{
		if((i<2) || (i>103))
		{
			OLED_WriteData(0xE0);
		}
		else OLED_WriteData(0x60);
	}
	OLED_SetCursor(7,11);
	for(i=0;i<106;i++)
	{
		if((i<2) || (i>103))
		{
			OLED_WriteData(0x07);
		}
		else OLED_WriteData(0x06);
	}
	OLED_SetCursor(6,11);
		OLED_WriteData(0xFF);
		OLED_WriteData(0xFF);
	OLED_SetCursor(6,115);
		OLED_WriteData(0xFF);
		OLED_WriteData(0xFF);
	OLED_SetCursor(6,14);
	for(i=0; i<Voltage_Percent_int; i++)
	{
		//OLED_SetCursor(7,0);
		OLED_WriteData(0xFF);
		Delay_ms(5);
	}
}

void fun_d21(void)
{
	OLED_Clear();
	OLED_ShowString(1,1,"2023.6.30");
}

key_table table[15]=
{
	//第0层
	{0,0,0,1,(*fun_0),NULL},   //0
	
    //第1层
	{1,4,2,5,(*fun_a1),NULL,NULL,NULL,NULL,NULL}, //1
	{2,1,3,8,(*fun_b1),NULL,NULL,NULL,NULL,NULL}, //2
	{3,2,4,9,(*fun_c1),NULL,NULL,NULL,NULL,NULL}, //3	
	{4,3,1,10,(*fun_d1),NULL,NULL,NULL,NULL,NULL}, //4
	
    //第2层
	{5,7,6,5,(*fun_a21),(*Light_ON),NULL,NULL,NULL,NULL}, //5				
	{6,5,7,6,(*fun_a22),(*Light_OFF),NULL,NULL,NULL,NULL}, //6
	{7,6,5,1,(*fun_a23),NULL,NULL,NULL,NULL,NULL}, //7
	
	{8,8,8,8,(*fun_b21),(*DataSelect),(*DataMinus),(*DataPlus),
				(*Manu_Backto_1),(*DataSendOk)},  //8
	
	{9,9,9,9,(*fun_c21),NULL,NULL,NULL,(*Manu_Backto_1),NULL},  //9
	
	{10,10,10,10,(*fun_d21),NULL,NULL,NULL,(*Manu_Backto_1),NULL}  //10
};

void Light_ON(void)
{
	Serial_SendByte(0x11);
}

void Light_OFF(void)
{
	Serial_SendByte(0x22);
}

void DataSelect(void)
{
	if(Select_Count == 0)
	{
		SelectNum_High = Keyboard_Num;
		OLED_ShowChar(1,10,' ');
		OLED_ShowHexNum(1,11,SelectNum_High,1);
		Select_Count++;
	}
	else if (Select_Count == 1)
	{
		SelectNum_Low = Keyboard_Num;
		OLED_ShowHexNum(1,10,SelectNum_High,1);
		OLED_ShowHexNum(1,11,SelectNum_Low,1);
//		SelectNum_High = 0;
//		SelectNum_Low = 0;
		Select_Count = 0;
	}	
}

void DataPlus(void)
{
	Keyboard_Num++;
	if(Keyboard_Num > 15)
	{
		Keyboard_Num = 0;
	}
	OLED_ShowHexNum(2,11,Keyboard_Num,1);
	//OLED_DrawBMP_Keyboard(0,2,128,8,Keyboard_Num);
}

void DataMinus(void)
{
	Keyboard_Num--;
	if(Keyboard_Num <0 )
	{
		Keyboard_Num = 15;
	}
	//OLED_DrawBMP_Keyboard(0,2,128,8,Keyboard_Num);
	OLED_ShowHexNum(2,11,Keyboard_Num,1);
}

void Manu_Backto_1(void)
{
	func_index = 1;
}

void DataSendOk(void)
{
	if(Select_Count == 1)
	{
		Serial_SendByte(SelectNum_High);
	}
	else if(Select_Count == 0)
	{
		Serial_SendByte((SelectNum_High<<4) | SelectNum_Low);
	}
	OLED_ShowString(1,15,"OK");
	Delay_s(1);
	OLED_ShowString(1,15,"  ");
}
