#pragma once

//int cnt_start_delay[MOTION_ID_MAX];  ���[�^�[�N���x�ꎞ��sec
#define SIM_TLOSS_HOIST        1.0    //�� �@      
#define SIM_TLOSS_GANTRY       0.5   //���s       
#define SIM_TLOSS_TROLLY       0.3   //���s       
#define SIM_TLOSS_BOOM_H       0.3   //����       
#define SIM_TLOSS_SLEW         0.3   //����       
 
//double Tf[MOTION_ID_MAX];               �����x�ꎞ�x��
#define SIM_TF_HOIST           0.2   //�� �@      
#define SIM_TF_GANTRY          0.2   //���s       
#define SIM_TF_TROLLY          0.2   //���s       
#define SIM_TF_BOOM_H          0.2   //����       
#define SIM_TF_SLEW            0.2   //����       