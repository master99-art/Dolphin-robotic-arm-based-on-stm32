#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "pwm.h"
#include "Cla.h"
#include "LED.h"
#include "Key.h"
//以mm为单位

/*******
xyz_fenlei:
前三个是目标物体的xyz参数
第四个是物体的分类
*******/
float xyz_fenlei[4]   ={90    ,0   ,100   ,1};

//定义初始位置
//中间位置
float begin_xyz[3]		= {0    ,176 ,135   };
float balance_xyz[3]	= {0    ,90  ,150   };
float end_red_xyz[3]	= {180  ,20  ,60    };
float end_blue_xyz[3] = {-180 ,20  ,60    };
float limit_low[3]		= {0    ,7   ,116.23};
float limit_high[3]		= {0    ,7   ,224.78};
float temp_xyz[4]			= {6.59 ,0   ,162,1 };
float cut_error[3]		= {0    ,7   ,130   };

//蓝牙模式
float Blue[4] 	 = {0} ;
u8 		Blue_Flag  =  0  ;
u8 		Flag       =  0  ;
u8 		color      =  0  ;
u8 		Mode       =  0  ;
/***********************
类似与状态机

Flag=0时，正在寻找物体（此时机械臂归位）
Flag=1时，找到了物体，开始计算角度，移动到指定角度
Flag=2时，移动到了指定角度，开始夹取
Flag=3时，机械臂移动到固定位置
Flag=4时，移动机械臂到目标位置（lei=0，blue坐标；lei=1，red坐标），松手
Flag=5时，归为，重置Flag=0

状态机的标志位，分类标志位
***********************/



int main(void)
{

	//设置中断优先级分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	/*模块初始化*/
	OLED_Init();						//OLED初始化
	Serial_1_Init();						//上位机串口初始化
	Serial_2_Init();						//串口初始化	
	PWM_Init();									//舵机初始化

	short i = 0 ;
	u8    j = 0 ;
	while (1)
	{
		Mode = Key_GetNum();
		Mode_show(Mode);
		//模式一 自动
		if(Mode==1)
		{
			Serial_1_SendByte(0XF1);
			switch(Flag) 
		  {
				case 0 ://在搜索图片
				Serial_1_SendByte(0xFF);
				break;
				
				case 1://开始移动
				//接收到了数据，获取到了物体的xyz坐标
				if(xyz_fenlei[3]==1)		 color = 1;//red
				else if(xyz_fenlei[3]==2)color = 2;//blue
				Serial_1_SendByte(0XFE);
				//开始移动 移动到物体坐标
				xyz_fenlei[0]-=7;
				xyz_fenlei[1]= 17;
				xyz_fenlei[2]-=60;
				cut_error[0]=xyz_fenlei[0]*0.8;
				cut_error[1]=30;
				cut_error[2]=xyz_fenlei[2]*0.8;
				pwm_Move_to_release(cut_error);
				Delay_ms(2000);
				pwm_Move_target_grab(xyz_fenlei);
				Delay_ms(500);
				//移动完毕后标志位置2
				Flag=2;
				//获取到了达到目标所需要的舵机角度哦
				break;
				
				case 2://抓取？
				//舵机四
				Servo_SetAngle4(70);
				Delay_ms(500);
				Flag=3;
				break;
				
				case 3 ://移动至恰当balancexyz轴
				pwm_Move_to_release(balance_xyz);
				Delay_ms(500);
				Flag=4;
				break;
				
				case 4 ://移动至放置位置
				Delay_ms(500);
				if(color == 1)//red  左？
					pwm_Move_to_release(end_red_xyz);
				else if(color == 2)//blue 右？
					pwm_Move_to_release(end_blue_xyz);
				Flag=5;
				break;
				
				case 5 ://松手
				Delay_ms(1000);
				Servo_SetAngle4(110);
				Delay_ms(500);
				Flag=6;
				break;		
				
				case 6 ://归中
				pwm_Move_to_release(balance_xyz);	
				Delay_ms(500);
				Flag=7;
				break;	
				
				case 7 ://从中间位置复位
				
				pwm_Move_to_release(begin_xyz);
				Delay_ms(500);
				Flag=8;
				break;
				
				case 8 ://灯亮
				Flag=0;
				break;

		  }		
		}
		else if(Mode==2)
		{
			Serial_1_SendByte(0XF0);
			Blue_MOVE(Blue);
			i++;
			if(i==5000)
			{
				if(j==0)LED_ON();
				else if(j !=0 )
				{
					LED_OFF();
					j=0;
				}
				i=0;
			}
		}
	}
}


void USART1_IRQHandler(void)
{

	static u8 i=0;
	static u8 RxState=0;
	static u8 Data[16]={0};
	//状态机标志位

	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)		//判断是否是USART1的接收事件触发的中断
	{
		

		
		if(Flag==0)//标志位为0，才可以读取数据
		{
			if(RxState==0)
			{
				if(USART_ReceiveData(USART1) == 0xFF)//包头
				{
					RxState=1;
				}
			}
			else if(RxState ==1)
			{
				Data[i] = USART_ReceiveData(USART1);		//读取数据寄存器，存放在接收的数据变量
				i++;
				if(i>=16)
				{
					RxState=2;
				}
				if(Data[i-1]==0XFE)
				{
					for(u8 i=0;i<16;i++)
					{
						Data[i]=0;
					}
					i=0;
					Cla_Data(Data,xyz_fenlei);
					RxState=0;
					Flag=0;					
				}
			}
			else if(RxState == 2)//直接到包尾
			{
				u8 temp=USART_ReceiveData(USART1);
				if(temp==0xFE)
				{
					RxState=0;//状态机复位
					i=0;			//循环复位，正确的话这里应该是16
					
					//16字节转换成了xyz，颜色的坐标
    			Cla_Data(Data,xyz_fenlei);
					Flag = 1;//标志位置一机械臂开始工作
				}
				else if(temp	!= 0xFE)
				{
					//出错了
					for(u8 i=0;i<16;i++)
					{
						Data[i]=0;
					}
					i=0;
					Cla_Data(Data,xyz_fenlei);
					RxState=0;
					Flag=0;
				}
			}					
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);			

	}
}

void USART2_IRQHandler(void)
{
	static u8 i = 0 ;
	static u8 Data[16]= { 0 };
	static u8 RxState=0;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)	
	{
		if(Flag==0)
		{
			if(RxState==0)
			{
				if(USART_ReceiveData(USART2) == 0xA5)//包头
				{
					RxState=1;
				}
			}
			else if(RxState==1)
			{
				Data[i] = USART_ReceiveData(USART2);		//读取数据寄存器，存放在接收的数据变量
				i++;		
				if(i>=16)
				{
					RxState=2;
				}
			}
			else if(RxState==2)
			{
				u8 temp=USART_ReceiveData(USART2);
				if(temp==0x5A)
				{
					
					Blue_Mode(Data,Blue);
					Blue_Flag=1;

					RxState=0;
					i=0;
					for(u8 i=0;i<16;i++)
					{
						Data[i]=0;
					}
				}
			}
			USART_ClearITPendingBit(USART2, USART_IT_RXNE);		
		}
	}
}

















