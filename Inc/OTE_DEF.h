#pragma once

//#include <winsock.h>

#include "PLC_DEF.h"

#define ID_OTE0					0xFF
#define ID_PC0					0x1FF

//IP�A�h���X
#define OTE_IF_UNICAST_IP_PC				"192.168.1.100" 		//PC��OTE UNICAST�@IF�pIP	�i��M,���M�j
#define OTE_IF_UNICAST_IP_OTE0				OTE_IF_UNICAST_IP_PC	//�f�t�H���gOTE��UNICAST�pIP�i��M,���M�j

//#define IP_ADDR_PC_UNI_CAST_OTE		"192.168.1.100"
//#define IP_ADDR_OTE_UNI_CAST_OTE		"127.0.0.1"

#define OTE_IF_MULTICAST_IP_PC				"239.1.0.2"				//PC�}���`�L���X�g�pIP
#define OTE_IF_MULTICAST_IP_OTE				"239.1.0.1"				//OTE�}���`�L���X�g�pIP

#define OTE_IF_UNICAST_PORT_PC				10050					//���j�L���X�gPC��M�|�[�g
#define OTE_IF_UNICAST_PORT_OTE				10051					//���j�L���X�g�[����M�|�[�g

#define OTE_IF_MULTICAST_PORT_PC2OTE		20080					//PC���M�}���`�L���X�gOTE��M�|�[�g
#define OTE_IF_MULTICAST_PORT_OTE2PC		20081					//OTE���M�}���`�L���X�gPC��M�|�[�g
#define OTE_IF_MULTICAST_PORT_PC2PC			20082					//PC���M�}���`�L���X�gPC��M�|�[�g
#define OTE_IF_MULTICAST_PORT_OTE2OTE		20083					//OTE���M�}���`�L���X�gOTE��M�|�[�g

#define ID_OTE_MODE_REMOTE_OPE				0x00000001
#define ID_OTE_MODE_MONITOR					0x00000002
#define ID_OTE_MODE_REMOTE_ACTIVE			0x00000010

//����[���̃{�^���A�����v��
#define N_OTE_PNL_ITEMS			128
#define N_OTE_PNL_STATIC		128
#define N_OTE_PNL_NOTCH			128
#define N_OTE_PNL_PB			128

//**************************************************************************************
//�ʐM�d���t�H�[�}�b�g
//**************************************************************************************
#define CODE_ITE_RES_ACK		1
#define CODE_ITE_RES_NAK		1


/******* ���ʃ��b�Z�[�W�w�b�_�� ***********/
typedef struct OteComHead {
	INT32       myid;		//���uID
	INT32       code;		//�v�����e�R�[�h
	SOCKADDR_IN addr;		//���M����M�A�h���X
	INT32       status;		//���
	INT32       tgid;		//�ڑ����@��ID
}ST_OTE_HEAD, * LPST_OTE_HEAD;
/******* PC�}���`�L���X�g���b�Z�[�W ***********/
typedef struct PcMBody {
	INT32 pos[MOTION_ID_MAX];
}ST_PC_M_BODY, * LPST_PC_M_BODY;

typedef struct PcMMsg {
	ST_OTE_HEAD         head;
	ST_PC_M_BODY		body;
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
	ST_OTE_HEAD      head;
	ST_OTE_M_BODY    body;
}ST_OTE_M_MSG, * LPST_OTE_M_MSG;
/******* PC���j�L���X�g���b�Z�[�W�\���� ***********/
#define PLC_IO_MONT_WORD_NUM		100

#define ID_PUC_BODY_LD_MHX				0
#define ID_PUC_BODY_LD_MHY				1
#define ID_PUC_BODY_LD_MHZ				2
#define ID_PUC_BODY_LD_MHTH				3
#define ID_PUC_BODY_LD_AHX				0
#define ID_PUC_BODY_LD_AHY				1
#define ID_PUC_BODY_LD_AHZ				2
#define ID_PUC_BODY_LD_AHTH				3

#define ID_PUC_BODY_TG_MAIN				0
#define ID_PUC_BODY_TG_S1				1
#define ID_PUC_BODY_TG_S2				2
#define ID_PUC_BODY_TG_S3				3
#define ID_PUC_BODY_TG_L1				4
#define ID_PUC_BODY_TG_L2				5
#define ID_PUC_BODY_TG_L3				6

#define OTE_LAMP_COM_OFF				0
#define OTE_LAMP_COM_ON					1
#define OTE_LAMP_COM_FLICK				2

#define OTE_LAMP_FLICK_COUNT			0x0008
#define OTE_STATIC_UPDATE_COUNT			0x0004


typedef struct OteLampCom {
	UINT8 com;
	UINT8 color;
}ST_OTE_LAMP_COM, * LPST_OTE_LAMP_COM;

typedef struct PcUBody {
	ST_OTE_LAMP_COM		pb_lamp[N_OTE_PNL_PB];
	ST_OTE_LAMP_COM		notch_lamp[N_OTE_PNL_NOTCH];
	//INT16				plc_in[PLC_IO_MONT_WORD_NUM];//PLC IF�f�[�^
	ST_PLC_READ			plc_in;//PLC IF�f�[�^
	INT32				pos[MOTION_ID_MAX];			//�ʒuFB
	INT32				v_fb[MOTION_ID_MAX];		//���xFB
	INT32				v_ref[MOTION_ID_MAX];		//���x�w��
	INT32				ld_pos[8];					//�݉׈ʒuFB
	INT32				ld_spd[8];					//�݉׈ʒuFB
	INT16				brk[MOTION_ID_MAX];			//�u���[�L���
	INT16				tg_pos[8][MOTION_ID_MAX];	//�ڕW�ʒu���W
}ST_PC_U_BODY, * LPST_PC_U_BODY;
typedef struct PcUMsg {
	ST_OTE_HEAD     head;
	ST_PC_U_BODY    body;
}ST_PC_U_MSG, * LPST_PC_U_MSG;
/******* OTE���j�L���X�g�ʐM���b�Z�[�W�\���� ***********/

#define ID_OTE_NOTCH_POS_HOLD	0
#define ID_OTE_NOTCH_POS_TRIG	1
#define ID_OTE_NOTCH_POS_CNT	2

typedef struct OteUBody {
	UINT16		pb_ope[128];					//�����PB����
	UINT16		pb_notch[128];				//�����m�b�`����
	INT16		notch_pos[2][MOTION_ID_MAX];	//�m�b�`���͈ʒu
	INT16		tg_pos[8];					//�ݒ�ڕW�ʒu
}ST_OTE_U_BODY, * LPST_OTE_U_BODY;
typedef struct OteUMsg {
	ST_OTE_HEAD         head;
	ST_OTE_U_BODY    body;
}ST_OTE_U_MSG, * LPST_OTE_U_MSG;




