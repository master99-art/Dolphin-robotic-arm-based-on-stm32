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
���ã���е�۵������㣬�ѻ�е�۵ļ�������ת�����˶�ѧ�ؽڽǶ�
��һ�����������Ŀ�������xyz����
�ڶ�����������������ת�Ƕȣ��ؽڣ���������ȡ�Ĳ��֣�
**********/
void xyz_to_jiaodu(float* xyz_fenlei,float* abc)
{
	/*******************
		��xyz�������е��abc,mm,��
	*******************/
	float derx = 0;//BC����ת���ĺ�ԭ���ˮƽ����
	float L1 = 135;//�ϲ�������ת���ĵľ��룿����
	float L2 = 145;//�����������
	float L3 = 52.52;//��ȡ�Ļ�еװ�õ�����ĩ�˵�ˮƽ����
	float L4 = 4;//��ȡװ�õ�����ĩ�˵Ĵ�ֱ����
	float h = 66;//bcת��������ԭ��Ĵ�ֱ����
	float beita = 69 * PI / 180;//beitaΪb�ǵļ��޽Ƕ�
	//float Axianzhi=70*PI/180;//a����ת�����Ƕ�70�㣬û�иı䴫����
	float gama = 15.53 * PI / 180; //c��Ĳ��15��
	float aerfa = 30 * PI / 180;//c��ң�˺���λ����ˮƽ�ļн�

	float x = -xyz_fenlei[0];//��x����Ϣ���ݸ�x
	float y = xyz_fenlei[1];//��y����Ϣ���ݸ�y
	float z = xyz_fenlei[2];//��z����Ϣ���ݸ�z

	//xz�����֮���xƲ����
	float xpie = 0;
	float ac = 0;
	
	
	///////////////////////////////////////
	/*��yС��hʱ��Ϊһ��ģ�ͣ���y����hʱ��Ϊ����һ��ģ��*/
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



	//��B
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



	//��A ??????????
	abc[0] = (float)atan((double)(x / z));

	//��C
	//�����Ƕ�
	abc[2] = ((L1 * L1) + (L2 * L2) - (ac * ac)) / (2 * L1 * L2);
	abc[2] = (float)(acos((double)abc[2]));
	abc[2] = abc[2] + gama - beita - abc[1] - aerfa + PI;
	abc[2] *= 1.05;

	//��ʱ������˻�е�ۿɲ����ĽǶȣ�Ȼ�����ͨ������֮��Ĳ�����ʵ����ʵ�ʶ���Ƕȶ�Ӧ
	ABC_Trun(abc);
	

}

/**********************
���ã��õ�����Ƕȵ���ʵֵ
*********************/
void ABC_Trun(float* abc)
{
	//���ں���ʽ��е�ۣ�����Ҫ֪��һЩ����
	/****************
	�������ǰѴ����ȸ���������˰�
	A�ᴫ���� 7��18 ���ת
	
	
	
	b��û�нǶ�û�б仯��Ȼ�����ļ���λ�þ�����������õ�0�㣬������Ҫ�������ú�
	����˵������b�Ἣ��λ��ʱ�������pwm���ǹٷ���Ӧ��0�㣬��Ӧ����x����ô���ǾͰ�x��Ϊ���׼��
	*****************/	
	abc[0]*=(18/7);
	
	
	
	
	
}

/****************

���ã��ѽǶ�������pwm��ϵ����
����������������ĵ�abc���ȣ����Ӧ�������pwmֵ
*****************/


void jiaodu_to_pwm(float* abc,u16* compare)
{
	float a=abc[0];
	float b=abc[1];
	float c=abc[2];
	u16 a_pwm=0,b_pwm=0,c_pwm=0;

	//ת�Ƕ�
	a = a*180/PI; 
	b = b*180/PI;
	c = c*180/PI;
	
	//���Ƽ��޽Ƕ�
	if(c >= 180) c = 180;
	else if(c <= 60) c = 60;
	if(a < -90) a = -90;
	else if(a > 90) a = 90;
	if(b > 100) b = 100;
	else if(b < 0) b = 0;
	
	
	//������������Լ�����ĽǶȺ�ʵ���϶������ڶ���Լ���0��֮�����ϵ
	float startA=90, startB=0, startC=0;	
	//��Χ PWM�ģ�500-0~2500-180
	
	/*************************************************************************
	����ʹ����һ��ǿ��ת���������̫��
	*************************************************************************/
	a_pwm = (u16)((a + startA) / 180 * 2000 + 500);
	b_pwm = (u16)((b / 180 * 2000 + 500) + startB);//startB����Ϊ������Ƕ�Ϊ0ʱ�������pwm
	c_pwm = (u16)((c / 180 * 2000 + 500) + startC);//
	
	compare[0]=a_pwm;
	compare[1]=b_pwm;
	compare[2]=c_pwm;
}

/************************
���ݽ��
һ��floatռ�ĸ�

���ĸ�float
��Ҫʮ�����ֽ�

************************/


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
			//��¼֮ǰ��

	
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
	//ת��Ϊ�˻�����//����������λ�õ�����ת�����˶���Ķ������Ļ����ƽǶ�
	xyz_to_jiaodu(target_xyz,target_abc);//ͬ�� 
	
	
	//����Ļ���ת��Ϊʵ�ʶ�������ƣ�Ȼ��ת��Ϊ�Ƕȣ���ת���pwm
	//��¼֮ǰ��

	
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
