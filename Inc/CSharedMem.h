#pragma once

#include <string>
#include "common_def.h"
#include "spec.h"
#include "CVector3.h"
#include "SwySensor_DEF.h"
#include "OTE_DEF.h"
#include "PLC_DEF.h"


#define SMEM_CRANE_STATUS_NAME			L"CRANE_STATUS"
#define SMEM_SWAY_STATUS_NAME			L"SWAY_STATUS"
#define SMEM_OPERATION_STATUS_NAME		L"OPERATION_STATUS"
#define SMEM_FAULT_STATUS_NAME			L"FAULT_STATUS"
#define SMEM_SIMULATION_STATUS_NAME		L"SIMULATION_STATUS"
#define SMEM_PLC_IO_NAME				L"PLC_IO"
#define SMEM_SWAY_IO_NAME				L"SWAY_IO"
#define SMEM_REMOTE_IO_NAME				L"REMOTE_IO"
#define SMEM_CS_INFO_NAME				L"CS_INFO"
#define SMEM_POLICY_INFO_NAME			L"POLICY_INFO"
#define SMEM_AGENT_INFO_NAME			L"AGENT_INFO"
#define SMEM_OTE_IO_NAME				L"OTE_IO"
#define SMEM_CLIENT_IO_NAME				L"CLIENT_IO"
#define SMEM_JOB_IO_NAME				L"JOB_IO"

#define MUTEX_CRANE_STATUS_NAME			L"MU_CRANE_STATUS"
#define MUTEX_SWAY_STATUS_NAME			L"MU_SWAY_STATUS"
#define MUTEX_OPERATION_STATUS_NAME		L"MU_OPERATION_STATUS"
#define MUTEX_FAULT_STATUS_NAME			L"MU_FAULT_STATUS"
#define MUTEX_SIMULATION_STATUS_NAME	L"MU_SIMULATION_STATUS"
#define MUTEX_PLC_IO_NAME				L"MU_PLC_IO"
#define MUTEX_SWAY_IO_NAME				L"MU_SWAY_IO"
#define MUTEX_OTE_IO_NAME				L"MU_OTE_IO"
#define MUTEX_CS_INFO_NAME				L"MU_CS_INFO"
#define MUTEX_POLICY_INFO_NAME			L"MU_POLICY_INFO"
#define MUTEX_AGENT_INFO_NAME			L"MU_AGENT_INFO"
#define MUTEX_CLIENT_IO_NAME			L"MU_CLIENT_IO"
#define MUTEX_JOB_IO_NAME				L"MU_JOB_IO"

#define SMEM_OBJ_ID_CRANE_STATUS		0
#define SMEM_OBJ_ID_SWAY_STATUS			1
#define SMEM_OBJ_ID_OPERATION_STATUS	2
#define SMEM_OBJ_ID_FAULT_STATUS		3
#define SMEM_OBJ_ID_SIMULATION_STATUS	4
#define SMEM_OBJ_ID_PLC_IO				5
#define SMEM_OBJ_ID_SWAY_IO				6
#define SMEM_OBJ_ID_REMOTE_IO			7
#define SMEM_OBJ_ID_CS_INFO				8
#define SMEM_OBJ_ID_POLICY_INFO			9
#define SMEM_OBJ_ID_AGENT_INFO			10
#define SMEM_OBJ_ID_CLIENT_IO			11
#define SMEM_OBJ_ID_JOB_IO				12

//  ���L�������X�e�[�^�X
#define	OK_SHMEM						0	// ���L������ ����/�j������
#define	ERR_SHMEM_CREATE				-1	// ���L������ Create�ُ�
#define	ERR_SHMEM_VIEW					-2	// ���L������ View�ُ�
#define	ERR_SHMEM_NOT_AVAILABLE			-3	// ���L������ View�ُ�
#define	ERR_SHMEM_MUTEX					-4	// ���L������ View�ُ�

#define SMEM_DATA_SIZE_MAX				1000000	//���L���������蓖�čő�T�C�Y�@1Mbyte	

using namespace std;

typedef struct StMoveSet {
	double p;
	double v;
	double a;
}ST_MOVE_SET, * LPST_MOVE_SET;

/****************************************************************************/
/*   PLC IO��`�\����                                                     �@*/
/* �@PLC_IF PROC���Z�b�g���鋤�L��������̏��@�@�@�@�@�@�@�@�@�@�@�@�@�@  */
#pragma region PLC IO
#define PLC_IF_REMOTE_OFF_MODE		0x00000000		//�@��^�]���[�h�iPC����̏o�͖���,�[���ɂď�Ԃ̃��j�^�̂݁j
#define PLC_IF_RMT_MODE_0			0x00000001		//���u�^�]���[�h�iPC����̏o�͗L���@�ʏ�^�p���[�h�j
#define PLC_IF_SIM_MODE_1			0x00000003		//����[������̑�����͗L���ʏ�^�p���[�h
#define PLC_IF_SIM_MODE_2			0x00000007		//PLC�؂藣�����[�h�iPLC IF�ŋ@��ւ̎w��i�u���[�L,MC���j���V�~�����[�g�j

#define PLC_DRUM_LAYER_MAX			10				//�h�����w���ő�l

// IO���t���e�́APLC_DEF.h�ɒ�`
// PLC_��ԐM���\���́i�@��Z���T�M��)
#define PLC_IF_BRK_OPEN				true
#define PLC_IF_BRK_CLOSE			false

#define PLC_IF_INV_DIO_FWD			0x0001
#define PLC_IF_INV_DIO_REV			0x0002

#define PLC_IF_LIMIT_COM_FWD_NORM		0x0000001
#define PLC_IF_LIMIT_COM_REV_NORM		0x0000002
#define PLC_IF_LIMIT_COM_FWD_EMR		0x0000004
#define PLC_IF_LIMIT_COM_REV_EMR		0x0000008
#define PLC_IF_LIMIT_COM_FWD_SLOW		0x0000010
#define PLC_IF_LIMIT_COM_REV_SLOW		0x0000020
#define PLC_IF_LIMIT_COM_FWD_SLOW2		0x0000040
#define PLC_IF_LIMIT_COM_REV_SLOW2		0x0000080
#define PLC_IF_LIMIT_COM_FWD_NORM_EX1	0x0000100
#define PLC_IF_LIMIT_COM_REV_NORM_EX1	0x0000200
#define PLC_IF_LIMIT_COM_FWD_EMR_EX1	0x0000400
#define PLC_IF_LIMIT_COM_REV_EMR_EX1	0x0000800
#define PLC_IF_LIMIT_COM_FWD_NORM_EX2	0x0001000
#define PLC_IF_LIMIT_COM_REV_NORM_EX2	0x0002000
#define PLC_IF_LIMIT_COM_FWD_EMR_EX2	0x0004000
#define PLC_IF_LIMIT_COM_REV_EMR_EX2	0x0008000
#define PLC_IF_LIMIT_COM_FWD_NORM_EX3	0x0010000
#define PLC_IF_LIMIT_COM_REV_NORM_EX3	0x0020000
#define PLC_IF_LIMIT_COM_FWD_EMR_EX3	0x0040000
#define PLC_IF_LIMIT_COM_REV_EMR_EX3	0x0080000
										   
typedef struct StPLC_IO {
	INT32 mode;
	INT32 healthy_cnt;
	double v_fb[MOTION_ID_MAX];						//���xFB
	double nv_ref[MOTION_ID_MAX];					//PLC��INV�ւ̑��x�w�ߏo��
	double nv_tg[MOTION_ID_MAX];					//PLC�ڕW���x
	double trq_fb_01per[MOTION_ID_MAX];				//�g���NFB
	double pos[MOTION_ID_MAX];						//�ʒuFB
	double weight;									//�努�׏dFB
	double weight_ah;								//�⊪�׏dFB
	double th_bh;									//�N���p acos(R/Lm)
	double dth_bh;									//�N���p���x
	double lmh;										//�努���[�v��
	double lah;										//�⊪���[�v��

	INT16 brk[MOTION_ID_MAX];						//�u���[�L���FB 0��
	INT32 endlim[MOTION_ID_MAX];					//�Ɍ��Z���T���
	INT16 notch_ref[MOTION_ID_MAX];					//�m�b�`�w�ߓ���FB�iOTE���͊܂ށj
	PLC_READ_BUF		input;						//PLC����̓ǂݎ��M�����l
	PLC_WRITE_BUF		output;						//PLC�ւ̏������ݐM�����l

	double notch_spd[MOTION_ID_MAX][NOTCH_MAX];			//# �m�b�`���x�ݒ茻�ݒl

}ST_PLC_IO, * LPST_PLC_IO;

#pragma endregion PLC IO��`�\����
/****************************************************************************/
/*   ����[����M����`�\����                                  �@         �@*/
/* �@OTE_IF PROC���Z�b�g���鋤�L��������̏��@�@�@�@�@�@�@          �@    */
#pragma region OTE
typedef struct StOTE_IO {
	ST_OTE_U_MSG	ote_umsg_in;		//�[������̎�M�f�[�^�i���j�L���X�g�j
	ST_PC_U_MSG		ote_umsg_out;		//�[���ւ̑��M�f�[�^�i���j�L���X�g�j
	UINT32			id_ote;				//���j�L���X�g�ڑ��[��ID
	UINT32			ote_u_silent_cnt;  	//��M�J�E���g�ω������ŃJ�E���g�A�b�v
}ST_OTE_IO, * LPST_OTE_IO;

#pragma endregion ����[����M����`�\����
/****************************************************************************/
/*   �U��Z���T�M����`�\����                                  �@         �@*/
/* �@SWAY_PC_IF���Z�b�g���鋤�L��������̏��@      �@�@�@�@�@�@           */
#pragma region SWAY SENSOR
#define SENSOR_CAM_MAX				 2//�J������
#define SENSOR_TARGET_MAX            4//���o�^�[�Q�b�g�ő吔
#define SID_TG1                      0//�^�[�Q�b�gID
#define SID_TG2                      1
#define SID_TG3                      2
#define SID_TG4                      3

#define TG_LAMP_NUM_MAX              3//�^�[�Q�b�g���̃����v�ő吔

#define SID_RED                      0
#define SID_GREEN                    1
#define SID_BLUE                     2

#define SWAY_FAULT_ITEM_MAX			 4//�ُ팟�o���ڐ�
#define SID_COMMON_FLT               0

#define SID_CAM_X              ID_SLEW
#define SID_CAM_Y              ID_BOOM_H
#define SID_CAM_XY             ID_TROLLY
#define SID_LOAD_MH            0
#define SID_LOAD_AH            1
#define SID_N_LOAD             2
#define SID_N_AXIS             4

typedef struct StSwayIO {
	DWORD proc_mode;
	DWORD helthy_cnt;

	char sensorID[4];
	WORD mode[SENSOR_TARGET_MAX];							//�^�[�Q�b�g�T���o���[�h
	WORD status[SENSOR_TARGET_MAX];							//�^�[�Q�b�g�T���o���
	DWORD fault[SWAY_FAULT_ITEM_MAX];						//�Z���T�ُ���
	double pix_size[SENSOR_TARGET_MAX][TG_LAMP_NUM_MAX];	//�^�[�Q�b�g���oPIXEL���i�ʐρj
	double tilt[SENSOR_CAM_MAX][SID_N_AXIS];				//�X�Ίp
	double dtilt[SENSOR_CAM_MAX][SID_N_AXIS];				//�X�Ίp
	double th[SID_N_LOAD][MOTION_ID_MAX];					//�݉אU�p			rad
	double dth[SID_N_LOAD][MOTION_ID_MAX];					//�݉אU�p���x		rad/s
	double dthw[SID_N_LOAD][MOTION_ID_MAX];					//�݉אU�p���x/�ց@	rad
	double ph[SID_N_LOAD][MOTION_ID_MAX];					//�݉׈ʑ����ʈʑ�	rad
	double rad_amp2[SID_N_LOAD][MOTION_ID_MAX];				//�݉אU����2��		rad2
	double cam_pix[SID_N_LOAD][MOTION_ID_MAX];				//�U��Z���T�J����PIX�ʒu
}ST_SWAY_IO, * LPST_SWAY_IO;

#pragma endregion �U��Z���T�M����`�\����
/****************************************************************************/
/*   �V�~�����[�V�����M����`�\����                                  �@   �@*/
/* �@SIM PROC���Z�b�g���鋤�L��������̏��@�@�@�@�@�@�@          �@    �@ */
#pragma region SIMULATOR
typedef struct StSimLoad {//����
	double m;	//����
	double wx;	//��
	double dy;	//���s
	double hz;	//����
}ST_SIM_LOAD, * LPST_SIM_LOAD;

#define SIM_ACTIVE_MODE			0x00000100					//�V�~�����[�V�������s���[�h
#define SIM_SWAY_PACKET_MODE	0x00000010					//�U��Z���T�p�P�b�g���M���[�h
#define SIM_N_SENSOR			128							//�Z���T��
typedef struct StSimulationStatus {
	DWORD mode;
	DWORD helthy_cnt;
	PLC_WRITE_BUF plc_w;
	ST_SWAY_IO sway_io;
	Vector3 L, vL;											//۰���޸��(�努�U��j
	Vector3 L2, vL2;										//۰���޸��(�⊪�U��j
	double a_fb[MOTION_ID_MAX];								//�����W�����xfb
	double v_fb[MOTION_ID_MAX];								//�����W���xfb
	double pos[MOTION_ID_MAX];								//�����W�ʒufb
	double mtrq[MOTION_ID_MAX];								//���[�^�g���Nfb
	ST_SIM_LOAD load[MOTION_ID_MAX];						//�����ׁi���͉׏d�j
	double rad_cam_x, rad_cam_y, w_cam_x, w_cam_y;			//�J�������W�U��p,�U��p���x
	double kbh;												//�������a�Ɉˑ����鑬�x�A�����x�␳�W��
	ST_SWAY_RCV_MSG rcv_msg;
	ST_SWAY_SND_MSG snd_msg;
	INT32 sonsor[SIM_N_SENSOR];								//�Z���T���

	ST_MOVE_SET	nd[MOTION_ID_MAX];							//�h������]����
	ST_MOVE_SET	d;									//�|�X�g�]�N���V�[�u�ԏ�ԁi�����E���x�E�����x�j
	ST_MOVE_SET	db;									//�W�u�|�X�g�]�N���V�[�u�ԏ�ԁi�����E���x�E�����x�j
	ST_MOVE_SET	ph;									//��
	ST_MOVE_SET	phb;								//��
	ST_MOVE_SET	th;									//��

	UINT32 i_layer[MOTION_ID_MAX];					//�h�������ݑw��
	double n_layer[MOTION_ID_MAX];					//�h�������ݑw���搔
	double l_drum[MOTION_ID_MAX];					//�h����������

	ST_MOVE_SET	lrm;								//�努���[�v��
	ST_MOVE_SET	lra;								//�⊪���[�v��

	double T;										//�努�U�����
	double Tah;										//�⊪�U�����
	double w;										//�努�U��p���g��
	double wah;										//�⊪�U��p���g��

	double tht_swx_mh, tht_swy_mh, tht_swx_ah, tht_swy_ah;

}ST_SIMULATION_STATUS, * LPST_SIMULATION_STATUS;
#pragma endregion �V�~�����[�V�����M����`�\����
//****************************************************************************
/// <summary>
/// �N���[����Ԓ�`�\���� 
/// Environment�^�X�N���Z�b�g���鋤�L��������̏��@
/// </summary>
#pragma region CRANE STATUS �N���[����Ԓ�`�\����
#define DBG_PLC_IO				0x00000001
#define DBG_SWAY_IO				0x00000100
#define DBG_ROS_IO				0x00010000
#define DBG_SIM_ACT				0X01000000

#define N_PC_FAULT_WORDS		16			//����PC���o�t�H���gbit�Z�b�gWORD��
#define N_PLC_FAULT_WORDS		32			//PLC���o�t�H���gbit�Z�b�gWORD��

#define OPERATION_MODE_OTE_ACTIVE	0x0000001
#define OPERATION_MODE_OTE_ONBOARD	0x0000002
#define OPERATION_MODE_REMOTE		0x0000004
#define OPERATION_MODE_SIMULATOR	0x0100000
#define OPERATION_MODE_PLC_DBGIO	0x0001000

#define N_SWAY_DIR				4

typedef struct stEnvSubproc {

	bool is_plcio_join = false;
	bool is_sim_join = false;
	bool is_sway_join = false;
	bool is_ote_join = false;

} ST_ENV_SUBPROC, LPST_ENV_SUBPROC;

#define MANUAL_MODE				0
#define ANTI_SWAY_MODE			1
#define SEMI_AUTO_ACTIVE		2
#define AUTO_ACTIVE				3
#define BITSEL_SEMIAUTO			0x0001
#define BITSEL_AUTO				0x0002

#define SPD0_CHECK_RETIO		0.1

#define STAT_ACC				0
#define STAT_DEC				1

#define PB_TRIG_COUNT			1

#define DRUM_LAYER_MAX	10	//�h�������w���ő�l

typedef struct StCraneStatus {
//Event Update				:�C�x���g�����ōX�V
	bool is_tasks_standby_ok;							//�^�X�N�̗����オ��m�F
	bool is_crane_status_ok=false;							//�N���[���X�e�[�^�X����������
	ST_SPEC spec;										//�N���[���d�l

//Periodical Update			�F������X�V
	DWORD env_act_count=0;								//�w���V�[�M��
	ST_ENV_SUBPROC subproc_stat;						//�T�u�v���Z�X�̏��
	WORD operation_mode;								//�^�]���[�h�@�@��,�����[�g

	Vector3 rc;											//�N���[���努�ݓ_�̃N���[����_�Ƃ�x,y,z���΍��W
	Vector3 rl;											//�努�ׂ݉̃N���[���ݓ_�Ƃ�x,y,z���΍��W
	Vector3 rcam_m;										//�努�U��Z���T���ox,y,z���W m

	Vector3 rc_a;										//�N���[���⊪�ݓ_�̃N���[����_�Ƃ�x,y,z���΍��W
	Vector3 rl_a;										//�⊪�ׂ݉̃N���[���ݓ_�Ƃ�x,y,z���΍��W
	Vector3 rcam_m_a;									//�⊪�U��Z���T���ox,y,z���W m
	
	double notch_spd_ref[MOTION_ID_MAX];				//�m�b�`���x�w��

	double mh_l;										//���[�v��
	double T;											//�U����		s
	double w;											//�U�p���g��	/s
	double w2;											//�U�p���g����2��

	double ah_l;										//���[�v��
	double T_ah;										//�U����		s
	double w_ah;										//�U�p���g��	/s
	double w2_ah;										//�U�p���g����2��
	
	double R;											//���񔼌a

	WORD faultPC[N_PC_FAULT_WORDS];						//PLC���o�ُ�
	WORD faultPLC[N_PLC_FAULT_WORDS];					//����PC���o�ُ�

	bool is_fwd_endstop[MOTION_ID_MAX];					//���]�Ɍ�����
	bool is_rev_endstop[MOTION_ID_MAX];					//�t�]�Ɍ�����

	INT32 OTE_req_status;								//����[���v�����

	INT32 notch0;										//0�m�b�`���葍��
	INT32 notch0_crane;									//0�m�b�`����PLC


	double Cdr[MOTION_ID_MAX][PLC_DRUM_LAYER_MAX];	//�h����1�w�~��
	double Ldr[MOTION_ID_MAX][PLC_DRUM_LAYER_MAX];	//�h�����w�����
	double Kdr[MOTION_ID_MAX][PLC_DRUM_LAYER_MAX];	//�h�����w�~���{��

	ST_MOVE_SET	axc[MOTION_ID_MAX];					//������
	ST_MOVE_SET	d;									//�努�|�X�g�]�N���V�[�u�ԏ�ԁi�����E���x�E�����x�j
	ST_MOVE_SET	ph;									//��
	ST_MOVE_SET	th;									//��
	ST_MOVE_SET	hm0;								//�努�V�[�u�N������
	ST_MOVE_SET	ha0;								//�⊪�V�[�u�N������
	ST_MOVE_SET	nd[MOTION_ID_MAX];					//�h������]����
	UINT32 i_layer[MOTION_ID_MAX];					//�h�������ݑw��
	double n_layer[MOTION_ID_MAX];					//�h�������ݑw���搔
	double l_drum[MOTION_ID_MAX];					//�h����������
	ST_MOVE_SET	lrm;								//�努���[�v��
	ST_MOVE_SET	lra;								//�⊪���[�v��


}ST_CRANE_STATUS, * LPST_CRANE_STATUS;
#pragma endregion 
/****************************************************************************/
/*   JOB COMMAND���Ǘ��\����                                   �@   �@	*/
/* �@JOB,COMMAND�����A���s�Ɋւ�����@�@�@�@�@�@�@ �@						*/
#pragma region JOB COMMAND
/****************************************************************************/
/*   �^���v�f��`�\����                                                     */
/* �@�����A�葬�A�������̈�A�̓���́A���̗v�f�̑g�ݍ��킹�ō\�����܂��B   */
/****************************************************************************/

//���V�s�@Type
#define CTR_TYPE_WAIT_TIME					0x0000	//�ҋ@�i���Ԍo�ߑ҂��j
#define CTR_TYPE_WAIT_POS_HST				0x0101	//�����B�҂�
#define CTR_TYPE_WAIT_POS_GNT				0x0102	//���s���B�҂�
#define CTR_TYPE_WAIT_POS_TRY				0x0104	//���s���B�҂�
#define CTR_TYPE_WAIT_POS_BH				0x0108	//�������B�҂�
#define CTR_TYPE_WAIT_POS_SLW				0x0110	//���񓞒B�҂�
#define CTR_TYPE_WAIT_LAND					0x0120	//�����҂�
#define CTR_TYPE_WAIT_PH_SINGLE				0x0201	//�ʑ��҂� 1�_
#define CTR_TYPE_WAIT_PH_DOUBLE				0x0202	//�ʑ��҂� 2�_

#define CTR_TYPE_VOUT_TIME					0x1000  //�X�e�b�v���x�@���Ԋ���
#define CTR_TYPE_VOUT_V						0x1001  //�X�e�b�v���x�@���x���B����
#define CTR_TYPE_VOUT_POS					0x1002  //�X�e�b�v���x�@�ʒu���B����
#define CTR_TYPE_VOUT_PHASE     			0x1004  //�X�e�b�v���x�@�ʑ����B����
#define CTR_TYPE_VOUT_LAND					0x1008  //�X�e�b�v���x�@��������

#define CTR_TYPE_AOUT_TIME					0x2000  //�������x�@���Ԋ���
#define CTR_TYPE_AOUT_V						0x2001  //�������x�@���x���B����
#define CTR_TYPE_AOUT_POS					0x2002  //�������x�@�ʒu���B����
#define CTR_TYPE_AOUT_PHASE     			0x2004  //�������x�@�ʑ����B����
#define CTR_TYPE_AOUT_LAND					0x2008  //�������x�@��������

#define CTR_TYPE_FINE_POS					0x8001	//�����ʒu���킹
#define CTR_TYPE_FB_SWAY					0x8002	//FB�U��~��
#define CTR_TYPE_FB_SWAY_POS				0x8004	//FB�U��~�߈ʒu����

#define TIME_LIMIT_CONFIRMATION				0.1		//�p�^�[���o�͒������� �b
#define TIME_LIMIT_FINE_POS					10.0	//�����ʒu���킹�������� �b
#define TIME_LIMIT_ERROR_DETECT				120		//�ُ팟�o����
#define N_STEP_OPTION_MAX					8

#define STEP_OPT_PHASE_SINGLE				0		//�N���ʑ��̔z��C���f�b�N�X�@SINGLE
#define STEP_OPT_PHASE_FWD					0		//�N���ʑ��̔z��C���f�b�N�X�@DOUBLE�@�������p
#define STEP_OPT_PHASE_REV					1		//�N���ʑ��̔z��C���f�b�N�X�@DOUBLE�@�t�����p
#define STEP_OPT_PHASE_CHK_RANGE			2		//�N���ʑ��̔z��C���f�b�N�X�@DOUBLE�@�t�����p

typedef struct stMotionElement {	//�^���v�f
	//recipe
	int type;								//������
	double _a;								//�ڕW�������x
	double _v;								//�ڕW���x
	double _p;								//�ڕW�ʒu
	double _t;								//�\��p������
	int time_count;							//�\��p�����Ԃ̃J�E���^�ϊ��l
	double opt_d[N_STEP_OPTION_MAX];		//�I�v�V����double
	int opt_i[N_STEP_OPTION_MAX];			//�I�v�V����int
	//status
	int act_count;
	int status;
}ST_MOTION_STEP, * LPST_MOTION_STEP;

/****************************************************************************/
/*   ������e��`�\���́i�P���j												*/
/* �@�P���̖ڕW��ԂɈڍs���铮��p�^�[�����^���v�f�̑g�ݍ��킹�Ŏ������܂� */
/****************************************************************************/
#define M_STEP_MAX	32

//Recipe
typedef struct stMotionRecipe {					//�ړ��p�^�[��
	//CS set
	int axis_id;
	int motion_type;
	int option_i;								//�I�v�V��������
	int direction;								//�������(-1,0,+1)
	int n_step;									//����\���v�f��
	int time_limit;								//�^�C���I�[�o�[����l
	ST_MOTION_STEP steps[M_STEP_MAX];			//�����`�v�f�z��

	//Agent set
	int i_hot_step;								//���s���v�f�z��index -1�Ŋ���
	int motion_act_count;						//������s���ԃJ�E���g�J�E���g��
	int fin_code;								//�����R�[�h

}ST_MOTION_RECIPE, * LPST_MOTION_RECIPE;

/********************************************************************************/
/*   ���A���^�]���e(COMMAND)��`�\����                             �@�@�@�@�@�@ */
/* �@�ړI�������������^�]���e��P������̑g�ݍ��킹�Ŏ������܂�               */
/********************************************************************************/

/*** �R�}���h��� ***/
#define COM_TYPE_MASK			0x0F00      
#define COM_TYPE_PICK			0x0100
#define COM_TYPE_GRND			0x0200
#define COM_TYPE_PARK			0x0400
#define COM_TYPE_FROM_TO		0x0800
#define COM_TYPE_NON 			0x0000

/*** �R�}���h��� ***/
#define STAT_MASK               0x00FF      //�X�e�[�^�X���}�X�N
#define STAT_NA					0x0000      //�K�p�s��
#define STAT_STANDBY			0x0001      //�K�p�s��
#define STAT_TRIGED             0x0002      //�N���ώ��s�҂�
#define STAT_ACTIVE             0x0004      //���s����
#define STAT_SUSPENDED          0x0008      //�ꎞ��~��
#define STAT_ABOTED             0x0010      //���f
#define STAT_END				0x0020      //���튮��
#define STAT_REQ_WAIT           0x0080      //�v���҂�
#define STAT_POSITIVE			0x1000		//OK
#define STAT_SUCCEED			0x1000		//����
#define STAT_ACK				0x1001		//�����t
#define STAT_NAK				0x8000		//NG
#define STAT_ABNORMAL			0x8000		//���s
#define STAT_TIME_OVER			0x8010		//�^�C���I�[�o�[
#define STAT_ABNORMAL_END       0x8020      //�ُ튮��
#define STAT_LOGICAL_ERROR		0x8004      //�������ُ�
#define STAT_CODE_ERROR			0x8008      //�K���R�[�h����

typedef struct StPosTargets {
	double pos[MOTION_ID_MAX];
	bool is_held[MOTION_ID_MAX];				//�ڕW�ʒu�z�[���h���t���O
}ST_POS_TARGETS, * LPST_POS_TARGETS;

typedef struct StCommandCode {
	int i_list;
	int i_job;
	int i_recipe;
}ST_COM_CODE, * LPST_COM_CODE;

typedef struct stCommandSet {
	//POLICY SET
	ST_COM_CODE com_code;
	int n_motion;									//�R�}���h�œ��삷�鎲�̐�
	int active_motion[MOTION_ID_MAX];				//�R�}���h���삷�鎲��ID�z��iMOTION�@RECIPE�̑Ώ۔z��j
	ST_MOTION_RECIPE recipe[MOTION_ID_MAX];
	ST_POS_TARGETS target;							//�ڕW�ʒu	

	//AGENT SET
	int motion_status[MOTION_ID_MAX];
	int recipe_counter[MOTION_ID_MAX];
	int com_status;

}ST_COMMAND_SET, * LPST_COMMAND_SET;

#define JOB_COMMAND_MAX				10			//�@JOB���\������R�}���h�ő吔

//# Policy �^�X�N�Z�b�g�̈�

#define MODE_PC_CTRL		0x00000001
#define MODE_ANTISWAY		0x00010000
#define MODE_RMOTE_PANEL	0x00000100

/************************************************************************************/
/*   ��Ɠ��e�iJOB)��`�\����                                 �@     �@�@�@�@�@�@	*/
/* �@ClientService �^�X�N���Z�b�g���鋤�L��������̏��								*/
/* �@JOB	:From-To�̔����R�}���h													*/
/*   COMMAND:1��JOB���A�����̃R�}���h�ō\��	PICK GRAND PARK						*/
/* �@JOB	:From-To�̔������													*/
/************************************************************************************/
#define JOB_REGIST_MAX				10			//�@JOB�o�^�ő吔
#define JOB_N_STEP_SEMIAUTO			1
#define COM_RECIPE_OPTION_N			8

typedef struct stComRecipe {
	//CS SET
	int id;
	int type;									//JOB��ʁiPICK,GRND,PARK...�j
	int time_limit;								//JOB�\���R�}���h��
	ST_POS_TARGETS target;						//�e�R�}���h�̖ڕW�ʒu	

	int option_i[COM_RECIPE_OPTION_N];
	double option_d[COM_RECIPE_OPTION_N];

	ST_COMMAND_SET comset;						//���V�s��W�J�����R�}���h�Z�b�g
	int status;

	SYSTEMTIME time_start;
	SYSTEMTIME time_end;
}ST_COM_RECIPE, * LPST_COM_RECIPE;

typedef struct stJobSet {
	int list_id;								//�o�^����Ă���JOB list��id
	int id;										//�o�^����Ă���JOB list���ł�id
	int status;									//���ݎ��s�Ώۂ�JOB�̏��
	int n_com;									//JOB�\���R�}���h��
	int type;									//JOB��ʁiJOB,������,OPERATION�j�j
	ST_COM_RECIPE recipe[JOB_COMMAND_MAX];	//�e�R�}���h�̃��V�s
	int i_hot_com;

	SYSTEMTIME time_start;
	SYSTEMTIME time_end;
}ST_JOB_SET, * LPST_JOB_SET;

//JOB LIST
typedef struct _stJobList {
	int id;
	int type;									//JOB��ʁiJOB,�������j
	int n_hold_job;								//����Job��
	int i_job_hot;								//�������҂�Job(���s��or�ҋ@���j	  id
	ST_JOB_SET job[JOB_REGIST_MAX];				//�o�^job
}ST_JOB_LIST, * LPST_JOB_LIST;

#define N_JOB_LIST						2				//JOB LIST�o�^��
#define ID_JOBTYPE_JOB					0				//JOB Type index�ԍ�
#define ID_JOBTYPE_SEMI					1				//SEMIAUTO Type index�ԍ�
#define ID_JOBTYPE_ANTISWAY				2				//FB ANTISWAY Type index�ԍ�
#define JOB_HOLD_MAX					10				//�ێ��\JOB�ő吔

typedef struct stJobIO {
	ST_JOB_LIST	job_list[N_JOB_LIST];
}ST_JOB_IO, * LPST_JOB_IO;

#pragma endregion ��Ɠ��e�iJOB)��`�\���� 
/****************************************************************************/
/*   Client Service	����`�\����                                   �@   �@*/
/* �@Client Service�^�X�N���Z�b�g���鋤�L��������̏��@�@�@�@�@�@�@ �@    */
#pragma region CLIENT

#define CS_SEMIAUTO_TG_SEL_DEFAULT      0
#define CS_SEMIAUTO_TG_SEL_CLEAR		0
#define CS_SEMIAUTO_TG_SEL_ACTIVE       1
#define CS_SEMIAUTO_TG_SEL_FIXED        2
#define CS_SEMIAUTO_TG_MAX				16

#define CS_ID_SEMIAUTO_TOUCH_PT			8

#define CS_ID_NOTCH_POS_HOLD			0
#define CS_ID_NOTCH_POS_TRIG			1

#define CS_CODE_OTE_REMOTE_ENABLE		0x1

typedef struct stCSInfo {
	//UI�֘A
	ST_OTE_LAMP_COM ote_pb_lamp[N_OTE_PNL_PB];							//�[�������v�\���w��
	ST_OTE_LAMP_COM ote_notch_lamp[N_OTE_PNL_NOTCH];					//�[�������v�\���w��
	int	semi_auto_selected;												//�I�𒆂̔�����ID
	int ote_remote_status;												//����[���̏�ԁ@0bit�F�[���M���L��
	INT16	notch_pos[2][MOTION_ID_MAX];								//�m�b�`�w��l

	ST_POS_TARGETS semi_auto_setting_target[CS_SEMIAUTO_TG_MAX];		//�������ݒ�ڕW�ʒu
	ST_POS_TARGETS semi_auto_selected_target;							//�������I��ڕW�ʒu
	INT32 semi_auto_selected_target_for_view[MOTION_ID_MAX];			//�������I��ڕW�ʒu(�J�������W�j
	INT32 hunging_point_for_view[MOTION_ID_MAX];						//�������I��ڕW�ʒu(�J�������W�j	

	int command_type;													//PARK,PICK,GRND
	int tg_sel_trigger_z = L_OFF, tg_sel_trigger_xy = L_OFF;			//�ڕW�ʒu�̐ݒ���́i������PB�A���j�^�^�b�`�j�����������ǂ����̔���l
	int target_set_z = CS_SEMIAUTO_TG_SEL_FIXED, target_set_xy = CS_SEMIAUTO_TG_SEL_FIXED;		//Z���W�ڕW�ʒu�m��
	LPST_JOB_SET p_active_job;
	int job_set_event;

	//����,���u�ݒ�i���[�h�j
	int auto_mode;														//�������[�h
	int antisway_mode;													//�U��~�߃��[�h
	int estop_active;													//����~���쒆
	int ote_notch_dist_mode;											//�^�u���b�g�ڕW���́@�ړ������w��

	double ote_camera_height_m;											//����[��VIEW�̃J�����ݒu����

}ST_CS_INFO, * LPST_CS_INFO;

#define N_JOB_TARGET_MAX	10
#define N_JOB_OPTION_MAX	10

typedef struct stClientRcvMSG {
	int req_code;
	int JOB_ID;
	double target[N_JOB_TARGET_MAX][MOTION_ID_MAX];
	int option[N_JOB_OPTION_MAX];
}ST_CLIENT_RCV_MSG, * LPST_CLIENT_RCV_MSG;

typedef struct stClientSndMSG {
	int fb_code;
	int JOB_ID;
	int option[N_JOB_OPTION_MAX];
}ST_CLIENT_SND_MSG, * LPST_CLIENT_SND_MSG;

#define N_CLIENT_MSG_HOLD_MAX	10

typedef struct stClientIO {
	int i_rcv_hot;
	int i_snd_hot;
	ST_CLIENT_RCV_MSG rmsg[N_CLIENT_MSG_HOLD_MAX];
	ST_CLIENT_SND_MSG smsg[N_CLIENT_MSG_HOLD_MAX];
}ST_CLIENT_IO, * LPST_CLIENT_IO;

#pragma endregion Client Service����`�\����
/****************************************************************************/
/*   Policy	����`�\����                                   �@			  �@*/
/* �@Policy	�^�X�N���Z�b�g���鋤�L��������̏��@�@�@�@�@�@�@		 �@		*/
#pragma region POLICY
#define FAULT_MAP_W_SIZE	64	//�t�H���g�}�b�v�T�C�Y
typedef struct stPolicyInfo {

	WORD fault_map[FAULT_MAP_W_SIZE];

}ST_POLICY_INFO, * LPST_POLICY_INFO;
#pragma endregion Policy����`�\����
/****************************************************************************/
/*   Agent	����`�\����                                   �@   �@		*/
/* �@Agent	�^�X�N���Z�b�g���鋤�L��������̏��@�@�@�@�@�@�@ �@			*/
#pragma region AGENT

/*** �W���u,�R�}���h�X�e�[�^�X ***/
//auto_ongoing�p
#define AUTO_TYPE_JOB_MASK       0x7000      //JOB��ʕ��}�X�N
#define AUTO_TYPE_SEL_MASK       0xf000      //������ʕ��}�X�N
#define AUTO_TYPE_MANUAL		 0x0000
#define AUTO_TYPE_JOB            0x1000      //���JOB
#define AUTO_TYPE_SEMIAUTO       0x2000      //��ʔ�����
#define AUTO_TYPE_OPERATION      0x4000      //��ʃN���[������
#define AUTO_TYPE_FB_ANTI_SWAY	 0x8000

//antisway_on_going�p
#define ANTISWAY_ALL_MANUAL		 0x00000000      //�U��~�ߖ���
#define ANTISWAY_BH_ACTIVE		 0x00000001      //�����U�~���s��
#define ANTISWAY_BH_PAUSED		 0x00000002      //�����U�~�ꎞ��~
#define ANTISWAY_BH_COMPLETE	 0x00000008      //�����U�~����
#define ANTISWAY_SLEW_ACTIVE	 0x00000010      //����U�~���s
#define ANTISWAY_SLEW_PAUSED	 0x00000020      //����U�~�ꎞ��~
#define ANTISWAY_SLEW_COMPLETE	 0x00000080      //����U�~���� 

//axis_status�p
#define AG_AXIS_STAT_FB0				0x000000001	 //��~��
#define AG_AXIS_STAT_PC_ENABLE			0x800000000	 //PC�w�߉�
#define AG_AXIS_STAT_AUTO_ENABLE		0x100000000	 //������
#define AG_AXIS_STAT_ANTISWAY_ENABLE	0x200000000	 //�U��~�߉�

typedef struct stAgentInfo {

	ST_COMMAND_SET st_as_comset;					//�U��~�ߗp�R�}���h�Z�b�g
	ST_POS_TARGETS auto_pos_target;					//�����ڕW�ʒu
	double dist_for_target[MOTION_ID_MAX];			//�ڕW�܂ł̋���
	int auto_on_going;								//���s���̎������
	int antisway_on_going;							//���s���U��~��
	int pc_ctrl_mode;								//PC����̎w�߂œ��삳���鎲�̎w��
	int auto_active[MOTION_ID_MAX];					//�������s���t���O(����)
	int axis_status[MOTION_ID_MAX];					//�e���̏�ԁifb0,�ُ퓙�j

	double v_ref[MOTION_ID_MAX];					//���x�w�ߏo�͒l
	ST_COMMAND_SET st_active_com_inf;				//���s���R�}���h�Z�b�g���

	int as_count[MOTION_ID_MAX];					//�U��~�߃��V�s�쐬�Ăяo����
	int command_count;								//�R�}���h���V�s�쐬�Ăяo����


}ST_AGENT_INFO, * LPST_AGENT_INFO;

#pragma endregion AGENT����`�\����
/****************************************************************************/
/*���L�������N���X��`														*/
static char smem_dummy_buf[SMEM_DATA_SIZE_MAX];
class CSharedMem
{
public:
	CSharedMem();
	~CSharedMem();

	int smem_available;			//���L�������L��
	int data_size;				//�f�[�^�T�C�Y

	int create_smem(LPCTSTR szName, DWORD dwSize, LPCTSTR szMuName);
	int delete_smem();
	int clear_smem();

	wstring wstr_smem_name;

	HANDLE get_hmutex() { return hMutex; }
	LPVOID get_pMap() { return pMapTop; }

protected:
	HANDLE hMapFile;
	LPVOID pMapTop;
	DWORD  dwExist;

	HANDLE hMutex;
};


