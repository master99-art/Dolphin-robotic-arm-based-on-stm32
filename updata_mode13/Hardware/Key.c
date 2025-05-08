#include "stm32f10x.h"                  // Device header
#include "Delay.h"

/**
  * 函    数：按键初始化
  * 参    数：无
  * 返 回 值：无
  */
void Key_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);						//将PB1和PB11引脚初始化为上拉输入
	
}

uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 1;		
	
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)			//读PB1输入寄存器的状态，如果为0，则代表按键1按下
	{
		Delay_ms(20);											//延时消抖
		//while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1);	//等待按键松手
		//Delay_ms(20);											//延时消抖
		KeyNum = 2;												//置键码为2
	}

	return KeyNum;			//返回键码值，如果没有按键按下，所有if都不成立，则键码为默认值0
}
