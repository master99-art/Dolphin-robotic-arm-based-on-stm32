#include "stm32f10x.h"                  // Device header

/**
  * 函    数：LED初始化
  * 参    数：无
  * 返 回 值：无
  */
void LED_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);						//将PA1和PA2引脚初始化为推挽输出
	
	/*设置GPIO初始化后的默认电平*/
	GPIO_SetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5);				//设置PA1和PA2引脚为高电平
}
/********************
A1 mode 1
A4 mode 2
A5 normol
*********************/
void Mode_show(u8 Mode)
{
	if(Mode==1)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	}
	else if(Mode==2)
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
		GPIO_SetBits(GPIOA, GPIO_Pin_4);		
	}
}

void LED_ON(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
}
void LED_OFF(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
}

