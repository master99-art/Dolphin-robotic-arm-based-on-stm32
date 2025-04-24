#include "stm32f10x.h"                  // Device header
#include "pwm.h"

/**
  * 函    数：PWM初始化
  * 参    数：无
  * 返 回 值：无
  */
void PWM_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);			//开启TIM3的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);			//开启GPIOA and B的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);							//将PA1引脚初始化为复用推挽输出	
																	//受外设控制的引脚，均需要配置为复用模式
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure1;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure1);							//将PA1引脚初始化为复用推挽输出	
																	//受外设控制的引脚，均需要配置为复用模式	
	
	TIM_InternalClockConfig(TIM3);

	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;   
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;				//计数
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;				//预分
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;          
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);           
	
//	/*输出比较初始化*/ 
	TIM_OCInitTypeDef TIM_OCInitStructure;							//定义结构
//	TIM_OCStructInit(&TIM_OCInitStructure);                       
	                                                              
	                                                              
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = 0;								//初始的CCR值
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);                      

	TIM_OCInitTypeDef TIM_OCInitStructure2;	
	TIM_OCInitStructure2.TIM_OCMode = TIM_OCMode_PWM1;            
	TIM_OCInitStructure2.TIM_OCPolarity = TIM_OCPolarity_High;    
	TIM_OCInitStructure2.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure2.TIM_Pulse = 0;								//初始的CCR值
	TIM_OC2Init(TIM3, &TIM_OCInitStructure2);                     


	TIM_OCInitTypeDef TIM_OCInitStructure3;	
	TIM_OCInitStructure3.TIM_OCMode = TIM_OCMode_PWM1;            
	TIM_OCInitStructure3.TIM_OCPolarity = TIM_OCPolarity_High;    
	TIM_OCInitStructure3.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure3.TIM_Pulse = 0;								//初始的CCR值
	TIM_OC3Init(TIM3, &TIM_OCInitStructure3);                     
	
	TIM_OCInitTypeDef TIM_OCInitStructure4;	
	TIM_OCInitStructure4.TIM_OCMode = TIM_OCMode_PWM1;            
	TIM_OCInitStructure4.TIM_OCPolarity = TIM_OCPolarity_High;    
	TIM_OCInitStructure4.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure4.TIM_Pulse = 0;								//初始的CCR值
	TIM_OC4Init(TIM3, &TIM_OCInitStructure4);                     
	
	/*TIM使能*/
	TIM_Cmd(TIM3, ENABLE);			//使能TIM3，定时器开始运行
}

void PWM1(u16 compare1)
{
	TIM_SetCompare1(TIM3,compare1);
}


void PWM2(u16 compare2)
{
	TIM_SetCompare2(TIM3,compare2);
}

void PWM3(u16 compare3)
{
	TIM_SetCompare3(TIM3,compare3);
}

void PWM4(u16 compare4)
{
	TIM_SetCompare4(TIM3,compare4);
}

void Servo_SetAngle1(float Angle)
{
	PWM1(Angle / 180 * 2000 + 500);	//设置占空比
												//将角度线性变换，对应到舵机要求的占空比范围上
}

void Servo_SetAngle2(float Angle)
{
	PWM2(Angle / 180 * 2000 + 500);	//设置占空比
												//将角度线性变换，对应到舵机要求的占空比范围上
}

void Servo_SetAngle3(float Angle)
{
	PWM3(Angle / 180 * 2000 + 500);	//设置占空比
												//将角度线性变换，对应到舵机要求的占空比范围上
}

void Servo_SetAngle4(float Angle)
{
	PWM4(Angle / 180 * 2000 + 500);	//设置占空比
												//将角度线性变换，对应到舵机要求的占空比范围上
}





