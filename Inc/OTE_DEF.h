#pragma once

//#include <winsock.h>

//IP�A�h���X
#define OTE_IF_UNICAST_IP_PC				"192.168.1.100" 		//PC��OTE UNICAST�@IF�pIP	�i��M,���M�j
#define OTE_IF_UNICAST_IP_OTE0				OTE_IF_UNICAST_IP_PC	//�f�t�H���gOTE��UNICAST�pIP�i��M,���M�j

//#define IP_ADDR_PC_UNI_CAST_OTE		"192.168.1.100"
//#define IP_ADDR_OTE_UNI_CAST_OTE		"127.0.0.1"

#define OTE_IF_MULTICAST_IP_PC				"239.1.0.2"					//PC�}���`�L���X�g�pIP
#define OTE_IF_MULTICAST_IP_OTE				"239.1.0.1"					//OTE�}���`�L���X�g�pIP

#define OTE_IF_UNICAST_PORT_PC				10050					//PC���j�L���X�gIF��M�|�[�g
#define OTE_IF_UNICAST_PORT_OTE				10051					//�[�����j�L���X�gIF��M�|�[�g

#define OTE_IF_MULTICAST_PORT_PC2OTE		20080					//PC���M�}���`�L���X�g��M�|�[�g
#define OTE_IF_MULTICAST_PORT_OTE2PC		20081					//OTE���M�}���`�L���X�g��M�|�[�g
#define OTE_IF_MULTICAST_PORT_PC2PC			20082					//PC���M�}���`�L���X�gOTE��M�|�[�g
#define OTE_IF_MULTICAST_PORT_OTE2OTE		20083					//OTE���M�}���`�L���X�gOTE��M�|�[�g

#define ID_OTE_MODE_REMOTE_OPE				0x00000001
#define ID_OTE_MODE_MONITOR					0x00000002
#define ID_OTE_MODE_REMOTE_ACTIVE			0x00000010


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


typedef struct StOTE_IO {
	ST_OTE_U_BODY	ote_in;
	ST_PC_U_BODY	ote_out;
	INT32 OTEstatus;
	INT32 OTEactive;				//�ڑ����̒[��ID�@�ڑ��f�̎�0
}ST_OTE_IO, * LPST_OTE_IO;

//OTE PB INDEX pb_ope
#define ID_OTE_PB_CTRL_SOURCE			0
#define ID_OTE_PB_ESTOP					1