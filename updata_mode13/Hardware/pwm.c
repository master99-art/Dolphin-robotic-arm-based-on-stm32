#include "stm32f10x.h"                  // Device header
#include "pwm.h"

/**
  * ��    ����PWM��ʼ��
  * ��    ������
  * �� �� ֵ����
  */
void PWM_Init(void)
{
	/*����ʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);			//����TIM3��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);			//����GPIOA and B��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);							//��PA1���ų�ʼ��Ϊ�����������	
																	//��������Ƶ����ţ�����Ҫ����Ϊ����ģʽ
	
	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStructure1;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure1);							//��PA1���ų�ʼ��Ϊ�����������	
																	//��������Ƶ����ţ�����Ҫ����Ϊ����ģʽ	
	
	TIM_InternalClockConfig(TIM3);

	/*ʱ����Ԫ��ʼ��*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//��
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;   
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;				//����
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;				//Ԥ��
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;          
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);           
	
//	/*����Ƚϳ�ʼ��*/ 
	TIM_OCInitTypeDef TIM_OCInitStructure;							//����ṹ
//	TIM_OCStructInit(&TIM_OCInitStructure);                       
	                                                              
	                                                              
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = 0;								//��ʼ��CCRֵ
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);                      

	TIM_OCInitTypeDef TIM_OCInitStructure2;	
	TIM_OCInitStructure2.TIM_OCMode = TIM_OCMode_PWM1;            
	TIM_OCInitStructure2.TIM_OCPolarity = TIM_OCPolarity_High;    
	TIM_OCInitStructure2.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure2.TIM_Pulse = 0;								//��ʼ��CCRֵ
	TIM_OC2Init(TIM3, &TIM_OCInitStructure2);                     


	TIM_OCInitTypeDef TIM_OCInitStructure3;	
	TIM_OCInitStructure3.TIM_OCMode = TIM_OCMode_PWM1;            
	TIM_OCInitStructure3.TIM_OCPolarity = TIM_OCPolarity_High;    
	TIM_OCInitStructure3.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure3.TIM_Pulse = 0;								//��ʼ��CCRֵ
	TIM_OC3Init(TIM3, &TIM_OCInitStructure3);                     
	
	TIM_OCInitTypeDef TIM_OCInitStructure4;	
	TIM_OCInitStructure4.TIM_OCMode = TIM_OCMode_PWM1;            
	TIM_OCInitStructure4.TIM_OCPolarity = TIM_OCPolarity_High;    
	TIM_OCInitStructure4.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure4.TIM_Pulse = 0;								//��ʼ��CCRֵ
	TIM_OC4Init(TIM3, &TIM_OCInitStructure4);                     
	
	/*TIMʹ��*/
	TIM_Cmd(TIM3, ENABLE);			//ʹ��TIM3����ʱ����ʼ����
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
	PWM1(Angle / 180 * 2000 + 500);	//����ռ�ձ�
												//���Ƕ����Ա任����Ӧ�����Ҫ���ռ�ձȷ�Χ��
}

void Servo_SetAngle2(float Angle)
{
	PWM2(Angle / 180 * 2000 + 500);	//����ռ�ձ�
												//���Ƕ����Ա任����Ӧ�����Ҫ���ռ�ձȷ�Χ��
}

void Servo_SetAngle3(float Angle)
{
	PWM3(Angle / 180 * 2000 + 500);	//����ռ�ձ�
												//���Ƕ����Ա任����Ӧ�����Ҫ���ռ�ձȷ�Χ��
}

void Servo_SetAngle4(float Angle)
{
	PWM4(Angle / 180 * 2000 + 500);	//����ռ�ձ�
												//���Ƕ����Ա任����Ӧ�����Ҫ���ռ�ձȷ�Χ��
}





