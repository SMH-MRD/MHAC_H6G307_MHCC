#pragma once

#include <winsock.h>
#include <time.h>

#define ID_OTE_EVENT_CODE_CONST             0
#define ID_OTE_EVENT_CODE_STAT_REPORT       1
#define ID_OTE_EVENT_CODE_CONNECTED         1

#define ID_TE_CONNECT_STATUS_OFF_LINE       0
#define ID_TE_CONNECT_STATUS_STANDBY        3
#define ID_TE_CONNECT_STATUS_ACTIVE         1
#define ID_TE_CONNECT_STATUS_WAITING        2

#define ID_PC_CONNECT_CODE_ENABLE           1
#define ID_PC_CONNECT_CODE_DISABLE          0

/******* ����[��IF ���ʃ��b�Z�[�W�w�b�_��                 ***********/
typedef struct OteComHead { 
    INT32       myid;
    INT32       code;
    SOCKADDR_IN addr;
    INT32       status;
    INT32       tgid;
}ST_OTE_HEAD, * LPST_OTE_HEAD;

typedef struct MOteSndMsg {
    ST_OTE_HEAD         head;
}ST_MOTE_SND_MSG, * LPST_MOTE_SND_MSG;

/******* ����[��IF �}���`�L���X�g�ʐM��M���b�Z�[�W�\���� ***********/
#define N_CRANE_PC_MAX      32
typedef struct MOteRcvBody {
    UCHAR       pc_enable[N_CRANE_PC_MAX];	//�ڑ��\�[���t���O
    INT32	    n_remote_wait;  //�ڑ��҂����u�����䐔
    INT32	    onbord_seqno;   //�@���ڑ��V�[�P���X�ԍ�
    INT32	    remote_seqno;   //���u��ڑ��V�[�P���X�ԍ�
    INT32	    my_seqno;       //���g�̐ڑ��V�[�P���X�ԍ�
}ST_MOTE_RCV_BODY, * LPST_MOTE_RCV_BODY;

typedef struct MOteRcvMsg {
    ST_OTE_HEAD         head;
    ST_MOTE_RCV_BODY    body;
}ST_MOTE_RCV_MSG, * LPST_MOTE_RCV_MSG;

#define PLC_IO_MONT_WORD_NUM		112
/******* ����[��IF ���j�`�L���X�g�ʐM���M���b�Z�[�W�\���� ***********/

typedef struct UOteSndBody {
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
}ST_UOTE_SND_BODY, * LPST_OTE_SND_BODY;

typedef struct UOteSndMsg {
    ST_OTE_HEAD         head;
    ST_UOTE_SND_BODY    body;
}ST_UOTE_SND_MSG, * LPST_UOTE_SND_MSG;

/******* ����[��IF ���j�`�L���X�g�ʐM��M���b�Z�[�W�\���� ***********/
typedef struct UOteRcvBody {
    INT32     tg_pos1[4];         //�ڕW�ʒu���W1
    INT32     tg_dist1[4];        //�ڕW�܂ł̋���1
    INT32     tg_pos2[4];         //�ڕW�ʒu���W2
    INT32     tg_dist2[4];        //�ڕW�܂ł̋���2
    INT16     pb[64];             //�����v�\��
    INT16     notch_pos[8];       //�m�b�`���͈ʒu
    INT16     cam_inf[8];         //����[�J�������
}ST_UOTE_RCV_BODY, * LPST_UOTE_RCV_BODY;

typedef struct UOteRcvdMsg {
    ST_OTE_HEAD         head;
    ST_UOTE_RCV_BODY    body;
}ST_UOTE_RCV_MSG, * LPST_UOTE_RCV_MSG;


