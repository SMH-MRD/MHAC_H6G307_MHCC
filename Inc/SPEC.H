#pragma once

#include "common_def.h"

#ifndef MOTION_ID_MAX

#define MOTION_ID_MAX   8  //���䎲�ő吔

#endif//



/*** �d�l��`�\���� ***/

#define DIRECTION_MAX 2
#define ACCDEC_MAX 2
#define FWD 0
#define REV 1
#define ACC 0
#define DEC 1

#define DEMENSION_MAX 64
#define ID_BOOM_HIGHT  0

#define NUM_OF_AS_AXIS			5	//�����̐��䎲���@���s�A���s�A����A�X�L���[
#define NUM_OF_SWAY_LEVEL		3	//����,�g���K,����
#define NUM_OF_POSITION_LEVEL	3	//����,�g���K,����

typedef struct StSpec {

	/*** �f�o�C�X�R�[�h�\���� ***/
	ST_DEVICE_CODE device_code = {
		'P','1',//���Ԏ��ʃR�[�h
		'L','C',//�@�B�R�[�h
		'C','R',//�^�C�v
		1		//ID�ԍ�
	};

#define NOTCH_MAX 6
#define NOTCH_0	  0
#define NOTCH_1	  1
#define NOTCH_2	  2
#define NOTCH_3	  3
#define NOTCH_4	  4
#define NOTCH_5	  5

	double notch_spd_f[MOTION_ID_MAX][NOTCH_MAX] = {		//# �m�b�`�w�ߑ��x�i�@��j
	{ 0.0,	0.083,	0.25,	0.417,	0.583,	1.666 },		//[ID_HOIST]	m/s
	{ 0.0,	0.04,	0.125,	0.25,	0.416,	0.416 },		//[ID_GANTRY]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },			//[ID_TROLLY]	m/s
	{ 0.0,	0.1,	0.3,	0.5,	0.7,	1.0 },			//[ID_BOOM_H]	m/s
	{ 0.0,	0.0157,	0.05,	0.0785,	0.11,	0.157 },		//[ID_SLEW]		rad/s;
	{ 0.0,	0.25,	0.25,	0.25,	0.25,	0.25 },			//[ID_OP_ROOM]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },			//[ID_H_ASSY];
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },			//[ID_MOTION1];
	};
	double notch_spd_r[MOTION_ID_MAX][NOTCH_MAX] = {			//# �m�b�`�w�ߑ��x�i�@��j
	{ 0.0,	-0.083,	-0.25,	-0.417,	-0.583,	-1.666 },		//[ID_HOIST]	m/s
	{ 0.0,	-0.04,	-0.125,	-0.25,	-0.416,	-0.416 },		//[ID_GANTRY]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },			//[ID_TROLLY]	m/s
	{ 0.0,	-0.1,	-0.3,	-0.5,	-0.7,	-1.0 },			//[ID_BOOM_H]	m/s
	{ 0.0,	-0.0157,	-0.05,	-0.0785,-0.11,-0.157 },		//[ID_SLEW]		rad/s;
	{ 0.0,	-0.25,	-0.25,	-0.25,	-0.25,	-0.25 },		//[ID_OP_ROOM]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },			//[ID_H_ASSY];
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },			//[ID_MOTION1];
	};

	//[ID_HOIST],[ID_GANTRY],[ID_TROLLY],[ID_BOOM_H],[ID_SLEW],[ID_OP_ROOM],[ID_H_ASSY],[ID_MOTION1]

	double notch_spd_remote_f[MOTION_ID_MAX][NOTCH_MAX] = {	//# �m�b�`�w�ߑ��x�i���u�j
	{ 0.0,	0.083,	0.25,	0.417,	0.583,	1.666 },		//[ID_HOIST]	m/s
	{ 0.0,	0.04,	0.125,	0.25,	0.416,	0.416 },		//[ID_GANTRY]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },			//[ID_TROLLY]	m/s
	{ 0.0,	0.1,	0.3,	0.5,	0.7,	1.0 },			//[ID_BOOM_H]	m/s
	{ 0.0,	0.0157,	0.05,	0.0785,	0.11,	0.157 },		//[ID_SLEW]		rad/s;
	{ 0.0,	0.25,	0.25,	0.25,	0.25,	0.25 },			//[ID_OP_ROOM]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },			//[ID_H_ASSY];
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },			//[ID_MOTION1];
	};
	double notch_spd_remote_r[MOTION_ID_MAX][NOTCH_MAX] = {	//# �m�b�`�w�ߑ��x�i���u�j
	{ 0.0,	-0.083,	-0.25,	-0.417,	-0.583,	-1.666 },		//[ID_HOIST]	m/s
	{ 0.0,	-0.04,	-0.125,	-0.25,	-0.416,	-0.416 },		//[ID_GANTRY]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },			//[ID_TROLLY]	m/s
	{ 0.0,	-0.1,	-0.3,	-0.5,	-0.7,	-1.0 },			//[ID_BOOM_H]	m/s
	{ 0.0,	-0.0157,-0.05,	-0.0785,-0.11,	-0.157 },		//[ID_SLEW]		rad/s;
	{ 0.0,	-0.25,	-0.25, -0.25, -0.25, -0.25 },			//[ID_OP_ROOM]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },			//[ID_H_ASSY];
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },			//[ID_MOTION1];
	};
	double accdec[MOTION_ID_MAX][DIRECTION_MAX][ACCDEC_MAX] = {	//# �e����������x
	{{ 0.387,	-0.387},	{-0.387,		0.387 }},				//[ID_HOIST]	m/s2
	{{ 0.0812,	-0.0812},	{-0.0812,	0.0812 }},				//[ID_GANTRY]	m/s2
	{{ 1.0,		-1.0},		{-1.0,		1.0 }},					//[ID_TROLLY]	m/s2
	{{ 0.333,	-0.333},	{	-0.333,	0.333 }},				//[ID_BOOM_H]	m/s2
	{{ 0.01301,	-0.01301},	{-0.01301,	0.01301 }},			//[ID_SLEW]		rad/s2;
	{{ 0.0125,	-0.0125},	{-0.0125,	0.0125 }},				//[ID_OP_ROOM]	m/s2
	{{ 0.0,	0.0},			{0.0,	0.0 }},						//[ID_H_ASSY];
	{{ 0.0,	0.0},			{0.0,	0.0 }},						//[ID_MOTION1];
	};

#define POS_LIMIT_TYPE_MAX	4
#define END_LIMIT			0
#define SPD_LIMIT1			1
#define SPD_LIMIT2			2
#define SPD_LIMIT3			3
	
	double pos_limit[MOTION_ID_MAX][DIRECTION_MAX][POS_LIMIT_TYPE_MAX] = {//# �Ɍ����@								//# �e�퐡�@
	{{ 14.5,0.0,0.0,0.0},	{ -11.0,0.0,0.0,0.0}},				//[ID_HOIST]
	{{ 80.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_GANTRY]
	{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_TROLLY]
	{{ 27.0,0.0,0.0,0.0},	{ 8.0,0.0,0.0,0.0}},				//[ID_BOOM_H]
	{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_SLEW]
	{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_OP_ROOM]
	{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_H_ASSY]
	{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_MOTION1]
	};

#define PARAM_PHASE_CHK_RANGE_BH						0.05	//�ʑ����B����͈�
#define PARAM_PHASE_CHK_RANGE_SLW						0.05	//�ʑ����B����͈�

#define ID_LV_COMPLE			0	//����
#define ID_LV_TRIGGER			1	//�g���K
#define ID_LV_LIMIT				2	//����
	double as_rad_level[NUM_OF_AS_AXIS][NUM_OF_SWAY_LEVEL] = {	//# �U��~�ߔ���@�U��p���x��(rad)
	{ 0.0,	0.0, 0.0},											//[ID_HOIST]
	{ 0.005, 0.01, 0.020 },									//[ID_GANTRY]
	{ 0.005, 0.01, 0.020 },									//[ID_TROLLY]
	{ 0.005, 0.01, 0.020 },									//[ID_BOOM_H]
	{ 0.005, 0.01, 0.020 },									//[ID_SLEW]	
	};
	double as_rad2_level[NUM_OF_AS_AXIS][NUM_OF_SWAY_LEVEL] = {	//# �U��~�ߔ���@�U��U�����x��(rad^2)
	{ 0.0,	0.0, 0.0},											//[ID_HOIST]
	{ 0.000025, 0.0001, 0.0004 },								//[ID_GANTRY]
	{ 0.000025, 0.0001, 0.0004 },								//[ID_TROLLY]
	{ 0.000025, 0.0001, 0.0004 },								//[ID_BOOM_H]
	{ 0.000025, 0.0001, 0.0004 },								//[ID_SLEW]
	};
	double as_m_level[NUM_OF_AS_AXIS][NUM_OF_SWAY_LEVEL] = {	//# �U��~�ߔ���@�U��p���x��(m)
	{ 0.0,	0.0, 0.0},											//[ID_HOIST]
	{ 0.1, 0.2, 2.00 },										//[ID_GANTRY]
	{ 0.1, 0.2, 2.00 },										//[ID_TROLLY]
	{ 0.1, 0.2, 2.00 },										//[ID_BOOM_H]
	{ 0.1, 0.2, 2.00 },										//[ID_SLEW]	
	};
	double as_m2_level[NUM_OF_AS_AXIS][NUM_OF_SWAY_LEVEL] = {	//# �U��~�ߔ���@�U��U�����x��(m^2)
	{ 0.0,	0.0, 0.0},											//[ID_HOIST]
	{ 0.01, 0.04, 4.0 },										//[ID_GANTRY]
	{ 0.01, 0.04, 4.0 },										//[ID_TROLLY]
	{ 0.01, 0.04, 4.0 },										//[ID_BOOM_H]
	{ 0.01, 0.04, 4.0 },										//[ID_SLEW]
	};
	double as_pos_level[NUM_OF_AS_AXIS][NUM_OF_POSITION_LEVEL] = {	//# �ʒu���ߔ���@�ʒu���ꃌ�x��(m,rad) 
	{ 0.03, 0.06, 0.020 },										//m[ID_HOIST]
	{ 0.03, 0.06, 0.020 },										//m[ID_GANTRY]
	{ 0.03, 0.06, 0.020 },										//m[ID_TROLLY]
	{ 0.03, 0.06, 0.020 },										//m[ID_BOOM_H]
	{ 0.005, 0.01, 0.1 }										//rad[ID_SLEW]
	};

#define ID_LV_DIR_CHECK_MARGIN	0								//�ړ���������}�[�W��
#define ID_LV_COMP_POS_PASSED	1								//�ʉ�
#define ID_LV_COMP_POS_RANGE	2								//�͈�

	double pos_check_limit[MOTION_ID_MAX][NUM_OF_POSITION_LEVEL] = {	//# �ʒu���B����͈�(m,rad) 
	{ 0.1, 0.2, 0.5 },											//m[ID_HOIST]
	{ 0.1, 0.2, 0.5 },											//m[ID_GANTRY]
	{ 0.1, 0.2, 0.5 },											//m[ID_TROLLY]
	{ 0.1, 0.2, 0.5 },											//m[ID_BOOM_H]
	{ 0.005, 0.17, 0.017},										//rad[ID_SLEW]
	{ 0.0, 0.0, 0.0 },											//
	{ 0.0, 0.0, 0.0 },											//
	{ 0.0, 0.0, 0.0 },											//
	};

#define NUM_OF_DELAY_PTN		5								//��������FB���Ԓx��]���p�^�[����
#define ID_DELAY_0START			0								//��~����̉���
#define ID_DELAY_ACC_DEC		1								//�������̌����ؑ֎�
#define ID_DELAY_DEC_ACC		2								//�������̉����ؑ֎�
#define ID_DELAY_CNT_ACC		3								//�葬����̉�����
#define ID_DELAY_CNT_DEC		4								//�葬����̌�����	

	double delay_time[NUM_OF_AS_AXIS][NUM_OF_DELAY_PTN] = {		// ����������FB�ꎞ�x�ꎞ�萔
	{ 0.3,0.3,0.3,0.3,0.3},										//[ID_HOIST]
	{ 0.3,0.3,0.3,0.3,0.3 },									//[ID_GANTRY]
	{ 0.3,0.3,0.3,0.3,0.3 },									//[ID_TROLLY]
	{ 0.3,0.3,0.3,0.3,0.3 },									//[ID_BOOM_H]
	{ 0.3,0.3,0.3,0.3,0.3 },									//[ID_SLEW]
	};
	
	//�\�����@
	double boom_high = 25.0, wheel_span = 20.0, leg_span = 15.0;// �u�[������,�z�C���X�p��,�r�ԃX�p��
	//�Ɍ�
	double hoist_pos_min = -10.0, hoist_pos_max = 20.0, boom_pos_min = 5.0, boom_pos_max = 28.0, gantry_pos_min = 0.0, gantry_pos_max = 300.0;
	//�݋�׏d
	double m_loard0 = 1000;	//�݋��kg
	//�U��Z���T



#define N_SWAY_SENSOR			3		//�U��Z���T�̐�
#define SID_SENSOR1				0		//No.1�U��Z���T
#define SID_SENSOR2				1		//No.2�U��Z���T
#define SID_SIM					2		//�V�~�����[�^

#define SWAY_SENSOR_N_AXIS		2
#define SID_AXIS_X				0		//X��
#define SID_AXIS_Y				1		//y��

#define N_SWAY_SENSOR_CAMERA    2		//�U��Z���T�P�Z�b�g�̃J�����̐�
#define SID_CAMERA1				0		//No.1�U��Z���T
#define SID_CAMERA2				1		//No.2�U��Z���T

#define N_SWAY_SENSOR_TARGET    2		//�U��Z���T�J����1�䂠����̃^�[�Q�b�g��
#define SID_TARGET1				0		//No.1�U��Z���T
#define SID_TARGET2				1		//No.2�U��Z���T

#define SWAY_CAMERA_N_PARAM		6
#define SID_L0					0		//�J������]������
#define SID_PH0					1		//�J������]���p�x
#define SID_l0					2		//�J�������S�ƃn�E�W���O��]���ԋ���
#define	SID_ph0					3		//�J�����̃n�E�W���O�ւ̎��t���p�x
#define	SID_phc					4		//�J�����̃n�E�W���O�ւ̎��t���p�x
#define	SID_PIXlRAD				5		//�J�����̃n�E�W���O�ւ̎��t���p�x


#define CTRL_PC_IP_ADDR_SWAY	"192.168.1.6"
#define SWAY_SENSOR_IP_ADDR		"192.168.1.81"


#define OTE_MULTI_IP_ADDR		"239.1.0.1"
//#define CTRL_PC_IP_ADDR_OTE		"192.168.1.201"
#define CTRL_PC_IP_ADDR_OTE		"192.168.1.24"
#define OTE_DEFAULT_IP_ADDR		"127.0.0.1"
//#define OTE_DEFAULT_IP_ADDR		"192.168.1.200"




#define SWAY_IF_IP_SWAY_PORT_C			10080
#define SWAY_IF_IP_SWAY_PORT_S			10081
#define OTE_IF_IP_UNICAST_PORT_C		10050	//���j�L���X�g�[����M�|�[�g
#define OTE_IF_IP_UNICAST_PORT_S		10051	//���j�L���X�g�N���[����M�|�[�g
#define OTE_IF_IP_MULTICAST_PORT_TE		20081	//�}���`�L���X�g�[����M�|�[�g
#define OTE_IF_IP_MULTICAST_PORT_CR		20080	//�}���`�L���X�g�N���[����M�|�[�g


	double SwayCamParam[N_SWAY_SENSOR][N_SWAY_SENSOR_CAMERA][SWAY_SENSOR_N_AXIS][SWAY_CAMERA_N_PARAM] = {	//�U��Z���T�@�p�����[�^
		{//No.1 �Z���T
		{{1.0,0.0174,0.2,0.01,0.01,2800.0},{1.0,0.0,0.0,0.00,0.00,2800.0}},	//�J����1 x,y���� L0 m,PH rad,l0 m,ph0 rad, phc rad,pix/rad
		{{1.0,0.0174,0.2,0.01,0.01,2800.0},{1.0,0.0174,0.2,0.01,0.01,2800.0}},	//�J����2 x,y���� L0 m,PH rad,l0 m,ph0 rad, phc rad,pix/rad
		},
		{//No.2 �Z���T
		{{1.0,0.0174,0.2,0.01,0.01,2800.0},{1.0,0.0174,0.2,0.01,0.01,2800.0}},	//�J����1 x,y���� L0 m,PH rad,l0 m,ph0 rad, phc rad,pix/rad
		{{1.0,0.0174,0.2,0.01,0.01,2800.0},{1.0,0.0174,0.2,0.01,0.01,2800.0}},	//�J����2 x,y���� L0 m,PH rad,l0 m,ph0 rad, phc rad,pix/rad
		},
		{//No.3 �Z���T
		{{1.0,0.0174,0.2,0.01,0.01,2800.0},{1.0,0.0174,0.2,0.01,0.01,2800.0}},	//�J����1 x,y���� L0 m,PH rad,l0 m,ph0 rad, phc rad,pix/rad
		{{1.0,0.0174,0.2,0.01,0.01,2800.0},{1.0,0.0174,0.2,0.01,0.01,2800.0}},	//�J����2 x,y���� L0 m,PH rad,l0 m,ph0 rad, phc rad,pix/rad
		}
	};

#define SEMI_AUTO_TARGET_MAX	6	// �������ڕW�z�[���h��

	double semi_target[SEMI_AUTO_TARGET_MAX][MOTION_ID_MAX]{//�������f�t�H���g�ڕW�ʒu
	//	HST,	GNT,	TRY,	BH,		SLW,	OPR,	ASSY,	COM
		{10.0,	0.0,	0.0,	8.0,	0.0,	0.0,	0.0,	0.0},
		{5.0,	0.0,	0.0,	25.0,	0.0,	0.0,	0.0,	0.0},
		{18.0,	0.0,	0.0,	25.0,	PI180,	0.0,	0.0,	0.0},
		{10.0,	0.0,	0.0,	25.0,	PI90,	0.0,	0.0,	0.0},
		{10.0,	0.0,	0.0,	5.0,	PI90,	0.0,	0.0,	0.0},
		{15.0,	0.0,	0.0,	18.0,	-PI60,	0.0,	0.0,	0.0},
	};
}ST_SPEC, * LPST_SPEC;



