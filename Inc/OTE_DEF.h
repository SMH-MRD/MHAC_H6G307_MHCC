#pragma once

#include <winsock.h>
#include <time.h>

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

//�d���t�H�[�}�b�g
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
	UCHAR       pc_enable[N_CRANE_PC_MAX];	//�ڑ��\�[���t���O
	INT32	    n_remote_wait;  //�ڑ��҂����u�����䐔
	INT32	    onbord_seqno;   //�@���ڑ��V�[�P���X�ԍ�
	INT32	    remote_seqno;   //���u��ڑ��V�[�P���X�ԍ�
	INT32	    my_seqno;       //���g�̐ڑ��V�[�P���X�ԍ�
}ST_OTE_M_BODY, * LPST_OTE_M_BODY;
typedef struct OteMMsg {
	ST_OTE_HEAD         head;
	ST_OTE_M_BODY    body;
}ST_OTE_M_MSG, * LPST_OTE_M_MSG;
/******* PC���j�L���X�g���b�Z�[�W�\���� ***********/
#define PLC_IO_MONT_WORD_NUM		112
typedef struct PcUBody {
	INT32      pos[8];                         //�ʒuFB
	INT32      v_fb[8];                        //���xFB
	INT32      v_ref[8];                       //���x�w��
	INT32      hp_pos[4];                      //�ݓ_�ʒuFB
	INT32      ld_pos[4];                      //�݉׈ʒuFB
	INT32      ld_v_fb[4];                     //�݉ב��xFB
	INT32      tg_pos[4];                      //�ڕW�ʒu���W1
	INT32      tg_pos_semi[6][4];              //�������ڕW�ʒu���WS1-L3
	INT16      lamp[64];                       //�����v�\��
	INT16      notch_pos[8];                   //�m�b�`�����v�\��
	INT16      cam_inf[8];                     //���u����J�������
	INT16	   plc_data[PLC_IO_MONT_WORD_NUM]; //PLC���j�^�����O�f�[�^
}ST_PC_U_BODY, * LPST_PC_U_BODY;
typedef struct PcUMsg {
	ST_OTE_HEAD         head;
	ST_PC_U_BODY    body;
}ST_PC_U_MSG, * LPST_PC_U_MSG;
/******* OTE���j�L���X�g�ʐM���b�Z�[�W�\���� ***********/
typedef struct OteUBody {
	INT32     tg_pos1[4];         //�ڕW�ʒu���W1
	INT32     tg_dist1[4];        //�ڕW�܂ł̋���1
	INT32     tg_pos2[4];         //�ڕW�ʒu���W2
	INT32     tg_dist2[4];        //�ڕW�܂ł̋���2
	INT16     pb[64];             //�����v�\��
	INT16     notch_pos[8];       //�m�b�`���͈ʒu
	INT16     cam_inf[8];         //����[�J�������
}ST_OTE_U_BODY, * LPST_OTE_U_BODY;
typedef struct OteUMsg {
	ST_OTE_HEAD         head;
	ST_OTE_U_BODY    body;
}ST_OTE_U_MSG, * LPST_OTE_U_MSG;