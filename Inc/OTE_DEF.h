#pragma once

#include <winsock.h>
#include <time.h>
#include "OTE_DEF.h"

#define OTE_IF_UNICAST_PORT_PC			10050	//�[�����j�L���X�gIF��M�|�[�g
#define OTE_IF_UNICAST_PORT_OTE			10051	//�[�����j�L���X�gIF��M�|�[�g

#define OTE_IF_PC_MULTICAST_PORT_PC		20080	//����P�}���`�L���X�g��M�|�[�g
#define OTE_IF_OTE_MULTICAST_PORT_PC	20081	//����P�}���`�L���X�g��M�|�[�g
#define OTE_IF_PC_MULTICAST_PORT_OTE	20082	//����P�}���`�L���X�g��M�|�[�g
#define OTE_IF_OTE_MULTICAST_PORT_OTE	20083	//����P�}���`�L���X�g��M�|�[�g

#define ID_OTE_EVENT_CODE_CONST             0
#define ID_OTE_EVENT_CODE_STAT_REPORT       1
#define ID_OTE_EVENT_CODE_CONNECTED         1

#define ID_TE_CONNECT_STATUS_OFF_LINE       0
#define ID_TE_CONNECT_STATUS_STANDBY        3
#define ID_TE_CONNECT_STATUS_ACTIVE         1
#define ID_TE_CONNECT_STATUS_WAITING        2

#define ID_PC_CONNECT_CODE_ENABLE           1
#define ID_PC_CONNECT_CODE_DISABLE          0

#define OTE_REQ_CODE_NA						0
#define OTE_REQ_CODE_ONBOAD					1
#define OTE_REQ_CODE_REMOTE					2
#define OTE_REQ_CODE_CONNECTED				4

#define ID_OTE_MODE_REMOTE_OPE				0x00000001
#define ID_OTE_MODE_MONITOR					0x00000002
#define ID_OTE_MODE_REMOTE_ACTIVE			0x00000010

//RADIO NOTCH ID  OTE_ID_RADIO + 100xMOTION_ID + 10 + NOTCH(-5�`+5�j
#define OTE_ID_PB_CHK_RADIO             21600
//���W�I
#define OTE_INDEX_RADIO_CONNECT			0
#define OTE_INDEX_RADIO_AUTO			1
#define OTE_INDEX_RADIO_MODE			2
#define OTE_INDEX_RADIO_FAULT			3
#define OTE_INDEX_RADIO_MOMENT			4

#define OTE_INDEX_RADIO_IFCHK_UNI		5
#define OTE_INDEX_RADIO_IFCHK_MPC		6
#define OTE_INDEX_RADIO_IFCHK_MTE		7

//�`�F�b�N�{�b�N�X
#define OTE_INDEX_CHK_ESTOP				8
#define OTE_INDEX_CHK_REMOTE			9
#define OTE_INDEX_CHK_CONNECT			10
#define OTE_INDEX_CHK_OTE_MON			11
//PB
#define OTE_INDEX_PB_START				12	//�t�I�u�W�F�N�g�̊J�nINDEX
#define OTE_INDEX_PB_CTR_SOURCE			12
#define OTE_INDEX_PB_FAULT_RESET		13
#define OTE_INDEX_PB_END				13	//�t�I�u�W�F�N�g�̏I��INDEX

#define OTE_ID_STATIC                   21700
#define OTE_INDEX_STATIC_MSG_CNT			0
#define OTE_INDEX_STATIC_COMMON_WND			1
#define OTE_INDEX_STATIC_CONNECT_CNT		2
#define OTE_INDEX_STATIC_OTE_IP_INF			3
#define OTE_INDEX_STATIC_IFCHK_MSG_HEAD_S	4
#define OTE_INDEX_STATIC_IFCHK_MSG_BODY_S	5
#define OTE_INDEX_STATIC_IFCHK_MSG_HEAD_R	6
#define OTE_INDEX_STATIC_IFCHK_MSG_BODY_R	7

//**************************************************************************************
//�ʐM�d���t�H�[�}�b�g
//**************************************************************************************
/******* ���ʃ��b�Z�[�W�w�b�_�� ***********/
typedef struct OteComHead {
	INT32       myid;		//���uID
	INT32       code;		//�v�����e�R�[�h
	SOCKADDR_IN addr;		//���M����M�A�h���X
	INT32       status;		//���
	INT32       tgid;		//�ڑ����@��ID
}ST_OTE_HEAD, * LPST_OTE_HEAD;
/******* PC�}���`�L���X�g���b�Z�[�W�i�w�b�_�̂݁j ***********/
typedef struct PcMMsg {
	ST_OTE_HEAD         head;
}ST_PC_M_MSG, * LPST_PC_M_MSG;
/******* OTE�}���`�L���X�g���b�Z�[�W�\���� ***********/
#define N_CRANE_PC_MAX      32
typedef struct OteMBody {
	UCHAR       pc_enable[N_CRANE_PC_MAX];		//�ڑ��\�[���t���O
	INT32	    n_remote_wait;					//�ڑ��҂����u�����䐔
	INT32	    onbord_seqno;					//�@���ڑ��V�[�P���X�ԍ�
	INT32	    remote_seqno;					//���u��ڑ��V�[�P���X�ԍ�
	INT32	    my_seqno;						//���g�̐ڑ��V�[�P���X�ԍ�
}ST_OTE_M_BODY, * LPST_OTE_M_BODY;
typedef struct OteMMsg {
	ST_OTE_HEAD         head;
	ST_OTE_M_BODY    body;
}ST_OTE_M_MSG, * LPST_OTE_M_MSG;
/******* PC���j�L���X�g���b�Z�[�W�\���� ***********/
#define PLC_IO_MONT_WORD_NUM		150
typedef struct PcUBody {
	INT16      plc_in[PLC_IO_MONT_WORD_NUM];	//�ʒuFB
	INT32      pos[8];							//�ʒuFB
	INT32      v_fb[8];							//���xFB
	INT32      v_ref[8];						//���x�w��
	INT32      ld_pos[4];						//�݉׈ʒuFB
	INT32      ld_v_fb[4];						//�݉ב��xFB
	INT32      tg_pos[8];						//�ڕW�ʒu���W1
	INT32      tg_pos_semi[6][8];				//�������ڕW�ʒu���WS1-L3
	INT16      lamp[64];						//�����v�\��
	INT16      notch_pos[8];					//�m�b�`�����v�\��
	INT16      cam_inf[8];						//���u����J�������
}ST_PC_U_BODY, * LPST_PC_U_BODY;
typedef struct PcUMsg {
	ST_OTE_HEAD     head;
	ST_PC_U_BODY    body;
}ST_PC_U_MSG, * LPST_PC_U_MSG;
/******* OTE���j�L���X�g�ʐM���b�Z�[�W�\���� ***********/
typedef struct OteUBody {
	INT16     pb_ope[64];			//����쑊��PB����
	INT16     notch_pos[8];			//�m�b�`���͈ʒu
	INT16     pb_auto[64];			//����PB����
	INT32     tg_pos[8];			//�ݒ�ڕW�ʒu
	INT32     cam_inf[8];			//����[�J�������
	INT32     spare[16];			//�\��
	INT16	  sub_monitor_mode;		//�I�v�V�������j�^�I�����
	INT16	  ote_mode;			//OTE�̑��샂�[�h
}ST_OTE_U_BODY, * LPST_OTE_U_BODY;
typedef struct OteUMsg {
	ST_OTE_HEAD         head;
	ST_OTE_U_BODY    body;
}ST_OTE_U_MSG, * LPST_OTE_U_MSG;


#define OTE_N_PB_CHK_RADIO				64
#define OTE_N_STATIC					64
#define OTE_PB_HOLDTIME_MASK			0x00000007

