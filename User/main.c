#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Serial.h"
#include "Delay.h"
#include "Key.h"
#include "AD.h"

uint8_t KeyNum;
extern uint8_t func_index;
extern uint8_t last_index;

int main(void)
{
	Key_Init();
	OLED_Init();
	Serial_Init();
	AD_Init();
	
	void (*current_operation_index)();
	void (*enter_operation_index)();
	void (*up_operation_index)();
	void (*down_operation_index)();
	void (*left_operation_index)();
	void (*right_operation_index)();

	fun_0();
	
	while(1)
	{
		KeyNum = Get_Key1num();
		if(KeyNum==1)//up
		{
			func_index = table[func_index].up;
			if((table[func_index].up_operation != NULL))
			{
				up_operation_index = table[func_index].up_operation;
				(*up_operation_index)();
				up_operation_index = NULL;
			}
		}
		if(KeyNum==3)//down
		{
			func_index = table[func_index].down;
			if((table[func_index].down_operation != NULL))
			{
				down_operation_index = table[func_index].down_operation;
				(*down_operation_index)();
				down_operation_index = NULL;
			}
		}
		if(KeyNum==2)//enter
		{
			func_index = table[func_index].enter;
			if((table[func_index].enter_operation != NULL) && (func_index == last_index))
			{
				enter_operation_index = table[func_index].enter_operation;
				(*enter_operation_index)();
			}
		}
		if(KeyNum==4)//left
		{
			if((table[func_index].left_operation != NULL))
			{
				left_operation_index = table[func_index].left_operation;
				(*left_operation_index)();
				left_operation_index = NULL;
			}
		}
		if(KeyNum==5)//right
		{
			if((table[func_index].right_operation != NULL))
			{
				right_operation_index = table[func_index].right_operation;
				(*right_operation_index)();
				right_operation_index = NULL;
			}
		}

		if (func_index != last_index)
		{
			current_operation_index = table[func_index].current_operation;
			
			(*current_operation_index)();

			last_index = func_index;
		}
	}
}
