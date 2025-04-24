#include "stm32f10x.h"
#include "math.h"
#include "stdio.h"
#include "Cla.h"
#include "pwm.h"
#include "Delay.h"

#define PI   3.14159265

union float_u8{
float d;
u8 data[4];
}x,y,z,chance;

/*************
���ã���е�۵������㣬�ѻ�е�۵ļ�������ת�����˶�ѧ�ؽڽǶ�
��һ�����������Ŀ�������xyz����
�ڶ�����������������ת�Ƕȣ��ؽڣ���������ȡ�Ĳ��֣�
**********/
void xyz_to_jiaodu(float* xyz_fenlei,float* abc)
{
	/*******************
		��xyz�������е��abc,mm,��
	*******************/
	float derx  = 0							   ;//BC����ת���ĺ�ԭ���ˮƽ����
	float L1    = 135						   ;//�ϲ�������ת���ĵľ��룿����
	float L2    = 145						   ;//�����������
	float L3    = 52.52						 ;//��ȡ�Ļ�еװ�õ�����ĩ�˵�ˮƽ����
	float L4    = 4								 ;//��ȡװ�õ�����ĩ�˵Ĵ�ֱ����
	float h     = 66							 ;//bcת��������ԭ��Ĵ�ֱ����
	float beita = 69 * PI / 180 	 ;//beitaΪb�ǵļ��޽Ƕ�
	float gama  = 15.53 * PI / 180 ; //c��Ĳ��15��
	float aerfa = 30 * PI / 180		 ;//c��ң�˺���λ����ˮƽ�ļн�

	float x = xyz_fenlei[0];//��x����Ϣ���ݸ�x
	float y = xyz_fenlei[1];//��y����Ϣ���ݸ�y
	float z = xyz_fenlei[2];//��z����Ϣ���ݸ�z

	//xz�����֮���xƲ����
	float xpie = 0;
	float ac = 0;
	
	
	///////////////////////////////////////
	/*��yС��hʱ��Ϊһ��ģ�ͣ���y����hʱ��Ϊ����һ��ģ��*/
	///////////////////////////////////////
	if (y <= h)
	{
		xpie = (float)sqrt(x * x + z * z)		;
		xpie -= (derx + L3)									;
		y = h - L4 - y											;
		ac = sqrt((xpie * xpie) + (y * y))	;
	}
	else if (y > h)
	{
		xpie = (float)sqrt(x * x + z * z)   ;
		xpie -= (derx + L3)									;
		y    = y + L4 - h										;
		ac   = sqrt((xpie * xpie) + (y * y));

	}



	//��B
	float B1 = 0; 
	float B2 = 0;
	if (y <= h)
	{
		B1 = ((L1 * L1) + (ac * ac) - (L2 * L2)) / (2 * L1 * ac);
		B1 = acos(B1)																						;
		B2 = acos((h-L4-y)/ac)																	;
		abc[1] = 2*PI - beita - B1 - B2 - PI/2									;

	}
	else if (y > h)
	{
		B1 		 = ((L1 * L1) + (ac * ac) - (L2 * L2)) / (2 * L1 * ac);
		B2 		 = xpie / ac																				  ;
		B1 		 = acos(B1)																					  ;
		B2 		 = acos(B2) 																				  ;
		abc[1] = PI - B1 - B2 - beita																;
	}



	//��A 
	abc[0] = (float)atan((double)(x / z));

	//��C
	//�����Ƕ�
	abc[2] = ((L1 * L1) + (L2 * L2) - (ac * ac)) / (2 * L1 * L2);
	abc[2] = (float)(acos((double)abc[2]))											;
	abc[2] = abc[2] + gama - beita - abc[1] - aerfa + PI				;


	abc[0] *= (18/7);	

}

/****************

���ã��ѽǶ�������pwm��ϵ����
����������������ĵ�abc���ȣ����Ӧ�������pwmֵ
*****************/


void jiaodu_to_pwm(float* abc,u16* compare)
{
	float a=abc[0]						 ;
	float b=abc[1]						 ;
	float c=abc[2]						 ;
	u16 a_pwm=0,b_pwm=0,c_pwm=0;

	//ת�Ƕ�
	a = a * 180 / PI ; 
	b = b * 180 / PI ;
	c = c * 180 / PI ;
	
	//���Ƽ��޽Ƕ�
	if		 (c>=180) c = 180 ;
	else if(c<=100) c = 100 ;
	if		 (a<-90)  a = -90 ;
	else if(a>90)   a = 90  ;
	if	   (b>84)   b = 84  ;
	else if(b<0)    b = 0   ;
	
	
	//������������Լ�����ĽǶȺ�ʵ���϶������ڶ���Լ���0��֮�����ϵ
	float startA=90, startB=0, startC=0;	

	
  //��Χ PWM�ģ�500-0~2500-180
	a_pwm = (u16)((a +startA) / 180 * 2000 + 500);
	b_pwm = (u16)(b / 0.09 +500 )+ startB				 ;//startB����Ϊ������Ƕ�Ϊ0ʱ�������pwm
	c_pwm = (u16)(c / 0.09 +500 )+ startC				 ;//
	
	compare[0] = a_pwm ;
	compare[1] = b_pwm ;
	compare[2] = c_pwm ;
}

/************************
���ݽ��
һ��floatռ�ĸ�

���ĸ�float
��Ҫʮ�����ֽ�

************************/
void Cla_Data(u8* Data,float* xyz_fenlei)
{
	for(u8 i=0;i<4;i++)
	{
		x.data[i]			 =	Data[3-i]	 ;
		y.data[i]			 =	Data[7-i]	 ;
		z.data[i]			 =	Data[11-i] ;
		chance.data[i] =	Data[15-i] ;
	}
	xyz_fenlei[0]	= x.d			;
	xyz_fenlei[1]	= y.d     ;
	xyz_fenlei[2]	= z.d     ;
	xyz_fenlei[3]	= chance.d;
}

/****************
����ƶ�
�������������xyz��Ŀ��xyz

****************/
void pwm_Move_target_grab(float* target_xyz)
{
	float target_abc[3]={0};
	u16 target_pwm[3]={0};
	//ת��Ϊ�˻�����//����������λ�õ�����ת�����˶���Ķ������Ļ����ƽǶ�
	xyz_to_jiaodu(target_xyz,target_abc);//ͬ�� 
	
	
	//����Ļ���ת��Ϊʵ�ʶ�������ƣ�Ȼ��ת��Ϊ�Ƕȣ���ת���pwm
	
	jiaodu_to_pwm(target_abc,target_pwm);
	
	///////////////////////////////////////////CBA/////////////////////////////////

	PWM1(target_pwm[0]);
	Delay_ms(300);
	PWM2(target_pwm[1]);
	Delay_ms(300);
	PWM3(target_pwm[2]);
	Delay_ms(1000);
}
void pwm_Move_to_release(float* target_xyz)
{
	
	float target_abc[3]={0};
	u16 target_pwm[3]={0};
	//ת��Ϊ�˻�����//����������λ�õ�����ת�����˶���Ķ������Ļ����ƽǶ�
	xyz_to_jiaodu(target_xyz,target_abc);//ͬ�� 
	
	
	//����Ļ���ת��Ϊʵ�ʶ�������ƣ�Ȼ��ת��Ϊ�Ƕȣ���ת���pwm
	
	jiaodu_to_pwm(target_abc,target_pwm);
	
	///////////////////////////////////////////CBA/////////////////////////////////

	PWM3(target_pwm[2]);
	Delay_ms(300);
	PWM2(target_pwm[1]);
	Delay_ms(300);
	PWM1(target_pwm[0]);
	Delay_ms(300);
	
	
}

u8 Blue_Mode(u8 *Data, float *blue)
{
	for(u8 i = 0 ; i < 4 ; i++ )
	{
		x.data[i]			 =	Data[3-i]	 ;
		y.data[i]			 =	Data[7-i]	 ;
		z.data[i]			 =	Data[11-i] ;
		chance.data[i] =	Data[15-i] ;
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
		Servo_SetAngle4(100);
	}
	else if (blue[3] !=0 )
	{
		Servo_SetAngle4(60);
	}

}
