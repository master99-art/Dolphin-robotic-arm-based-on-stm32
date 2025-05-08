#include "stm32f10x.h"
#include "math.h"
#include "stdio.h"
#include "Cla.h"
#include "pwm.h"
#include "Delay.h"



union float_u8{
float d;
u8 data[4];
}x,y,z,chance;

#define PI   3.14159265
#define X_MAX	500
#define X_MIN	0


/*************
作用：机械臂的逆运算，把机械臂的夹手坐标转换成运动学关节角度
第一个传入参数：目标物体的xyz坐标
第二个传入参数：舵机旋转角度（关节，不包含夹取的部分）
**********/
void xyz_to_jiaodu(float* xyz_fenlei,float* abc)
{
	/*******************
		从xyz计算出机械的abc,mm,°
	*******************/
	float derx = 0;//BC轴旋转中心和原点的水平距离
	float L1 = 135;//上部两个旋转中心的距离？？？
	float L2 = 145;//最上面的连杆
	float L3 = 52.52;//夹取的机械装置到连杆末端的水平距离
	float L4 = 4;//夹取装置到连杆末端的垂直距离
	float h = 66;//bc转轴与坐标原点的垂直距离
	float beita = 69 * PI / 180;//beita为b角的极限角度
	//float Axianzhi=70*PI/180;//a轴旋转的最大角度70°，没有改变传动比
	float gama = 15.53 * PI / 180; //c轴的差角15°
	float aerfa = 30 * PI / 180;//c轴遥杆后极限位置与水平的夹角

	float x = -xyz_fenlei[0];//把x轴信息传递给x
	float y = xyz_fenlei[1];//把y轴信息传递给y
	float z = xyz_fenlei[2];//把z轴信息传递给z

	//xz轴合体之后的x撇长度
	float xpie = 0;
	float ac = 0;
	
	
	///////////////////////////////////////
	/*当y小于h时，为一个模型；当y大于h时，为另外一个模型*/
	///////////////////////////////////////
	float temp_y = y;
	if (y <= h)
	{
		
		xpie = (float)sqrt(x * x + z * z);
		xpie -= (derx + L3);
		temp_y = h - L4 - temp_y;
		ac = sqrt((xpie * xpie) + (temp_y * temp_y));

	}
	else if (y > h)
	{
		xpie = (float)sqrt(x * x + z * z);
		xpie -= (derx + L3);

		temp_y = temp_y + L4 - h;

		ac = sqrt((xpie * xpie) + (temp_y * temp_y));

	}



	//角B
	float B1 = 0;
	float B2 = 0;
	if (y <= h)
	{
		float tamp_y1 = y;
		B1 = ((L1 * L1) + (ac * ac) - (L2 * L2)) / (2 * L1 * ac);
		B1 = acos(B1);
		B2 = acos((h - L4 - tamp_y1) / ac);
		abc[1] = 2 * PI - beita - B1 - B2 - PI / 2;
		abc[1] *= 1.1;
	}
	else if (y > h)
	{
		B1= ((L1 * L1) + (ac * ac) - (L2 * L2)) / (2 * L1 * ac);
		B2 = xpie / ac;
		B1 =  acos(B1);
		B2 = acos(B2);
	
		abc[1] = PI - B1 - B2 - beita;
		abc[1] *= 1.1;
	}



	//角A ??????????
	abc[0] = (float)atan((double)(x / z));

	//角C
	//弯曲角度
	abc[2] = ((L1 * L1) + (L2 * L2) - (ac * ac)) / (2 * L1 * L2);
	abc[2] = (float)(acos((double)abc[2]));
	abc[2] = abc[2] + gama - beita - abc[1] - aerfa + PI;
	abc[2] *= 1.05;

	//此时计算出了机械臂可测量的角度，然后计算通过齿轮之类的操作，实现与实际舵机角度对应
	ABC_Trun(abc);
	

}

/**********************
作用：得到舵机角度的真实值
*********************/
void ABC_Trun(float* abc)
{
	//对于海豚式机械臂，我们要知道一些参数
	/****************
	这里我们把传动比搞出来就行了吧
	A轴传动比 7：18 舵机转
	
	
	
	b轴没有角度没有变化，然后他的极限位置就是他舵机对用的0°，我们需要把它配置好
	比如说我们在b轴极限位置时，输出的pwm不是官方对应的0°，对应的是x，那么我们就把x作为零基准，
	*****************/	
	abc[0]*=(18/7);
	
	
	
	
	
}

/****************

作用：把角度与舵机的pwm联系起来
传入参数：舵机定义的的abc弧度，舵机应该输出的pwm值
*****************/


void jiaodu_to_pwm(float* abc,u16* compare)
{
	float a=abc[0];
	float b=abc[1];
	float c=abc[2];
	u16 a_pwm=0,b_pwm=0,c_pwm=0;

	//转角度
	a = a*180/PI; 
	b = b*180/PI;
	c = c*180/PI;
	
	//限制极限角度
	if(c >= 180) c = 180;
	else if(c <= 60) c = 60;
	if(a < -90) a = -90;
	else if(a > 90) a = 90;
	if(b > 100) b = 100;
	else if(b < 0) b = 0;
	
	
	//这里就是我们自己定义的角度和实际上舵机相对于舵机自己的0°之间的联系
	float startA=90, startB=0, startC=0;	
	//范围 PWM的：500-0~2500-180
	
	/*************************************************************************
	这里使用了一个强制转换，这个不太好
	*************************************************************************/
	a_pwm = (u16)((a + startA) / 180 * 2000 + 500);
	b_pwm = (u16)((b / 180 * 2000 + 500) + startB);//startB参数为当定义角度为0时，舵机的pwm
	c_pwm = (u16)((c / 180 * 2000 + 500) + startC);//
	
	compare[0]=a_pwm;
	compare[1]=b_pwm;
	compare[2]=c_pwm;
}

/************************
数据结合
一个float占四个

有四个float
需要十六个字节

************************/


/****************
舵机移动
传入参数：现在xyz，目标xyz

****************/

void pwm_Move_target_grab(float* target_xyz)
{
	float target_abc[3]={0};
	u16 target_pwm[3]={0};
	//转换为了弧度制//把现在所处位置的坐标转换成了定义的舵机定义的弧度制角度
	xyz_to_jiaodu(target_xyz,target_abc);//同上 
	
	
	//定义的弧度转换为实际舵机弧度制，然后转换为角度，再转误差pwm
			//记录之前的

	
	jiaodu_to_pwm(target_abc,target_pwm);
	
	///////////////////////////////////////////CBA/////////////////////////////////


		PWM1(target_pwm[0]);	
	PWM2(target_pwm[1]);	
	PWM3(target_pwm[2]);
	

}
void pwm_Move_to_release(float* target_xyz)
{
	
	float target_abc[3]={0};
	u16 target_pwm[3]={0};
	//转换为了弧度制//把现在所处位置的坐标转换成了定义的舵机定义的弧度制角度
	xyz_to_jiaodu(target_xyz,target_abc);//同上 
	
	
	//定义的弧度转换为实际舵机弧度制，然后转换为角度，再转误差pwm
	//记录之前的

	
	///////////////////////////////////////////CBA/////////////////////////////////
	

	PWM3(target_pwm[2]);
	PWM2(target_pwm[1]);
	PWM1(target_pwm[0]);

	
}




void Cla_Data(u8* Data,float* xyz_fenlei)
{
	for(u8 i=0;i<4;i++)
	{
		x.data[i]			 =	Data[0+i]	 ;
		y.data[i]			 =	Data[4+i]	 ;
		z.data[i]			 =	Data[8+i] ;
		chance.data[i] =	Data[12+i] ;
	}
	xyz_fenlei[0]	= x.d			;
	xyz_fenlei[1]	= y.d     ;
	xyz_fenlei[2]	= z.d     ;
	xyz_fenlei[3]	= chance.d;
}

u8 Blue_Mode(u8 *Data, float *blue)
{
	for(u8 i = 0 ; i < 4 ; i++ )
	{
		x.data[i]			 =	Data[0+i]	 ;
		y.data[i]			 =	Data[4+i]	 ;
		z.data[i]			 =	Data[8+i] ;
		chance.data[i] =	Data[12+i] ;
	}
	blue[0]	= x.d			 ;
	blue[1]	= y.d      ;
	blue[2]	= z.d      ;
	blue[3]	= chance.d ;
	return 1;
}


void Blue_MOVE(float* blue)
{
	
	if(blue[0]>=0 && blue[0]<=180)
	{
		Servo_SetAngle1(blue[0]);
	}
	else if(blue[0]<0)
	{
		blue[0]=0;
		Servo_SetAngle1(0);
	}
	else if (blue[0]>180)
	{
		blue[0] = 180;
		Servo_SetAngle1(180);
	}
	
	if(blue[1]>=0 && blue[1]<=84)
	{
		Servo_SetAngle2(blue[1]);
	}
	else if(blue[1]<0)
	{
		blue[1] = 0;
		Servo_SetAngle2(0);
	}
	else if(blue[1]>84)
	{
		blue[1] = 84;
		Servo_SetAngle2(84);
	}
	
	if(blue[2]>=60 && blue[2]<=180)
	{
		Servo_SetAngle3(blue[2]);
	}
	else if(blue[2]<60)
	{
		blue[2] = 120;
		Servo_SetAngle3(blue[2]);
	}
	else if(blue[2]>180)
	{
		blue[2] = 180;
		Servo_SetAngle3(blue[2]);
	}
	
	if(blue[3] == 0)
	{
		Servo_SetAngle4(130);
	}
	else if (blue[3] !=0 )
	{
		Servo_SetAngle4(70);
	}

}
