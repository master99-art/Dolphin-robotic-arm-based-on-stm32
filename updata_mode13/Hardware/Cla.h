#ifndef __CLA_H
#define __CLA_H
#include "stm32f10x.h"

void ABC_Trun(float* abc_fenlei);
void xyz_to_jiaodu(float* xyz_fenlei,float* abc_fenlei);
void Cla_Data(u8* Data,float* xyz_fenlei);
void pwm_Move(float* target_xyz);
void jiaodu_to_pwm(float* abc,u16* compare);
void pwm_Move_target_grab(float* target_xyz);
void pwm_Move_to_release(float* target_xyz);



u8 Blue_Mode(u8 *Data, float *blue);
void Blue_MOVE(float* blue);

#endif
