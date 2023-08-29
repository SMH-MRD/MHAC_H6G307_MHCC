#pragma once

#include <winsock.h>
#include <time.h>

/*#########################################################################*/
/*      �@�@         CLIENT�@PC�Ƃ̒ʐM���b�Z�[�W�t�H�[�}�b�g�@�@�@�@�@�@�@*/
/*#########################################################################*/

#define CLIENT_CODE_JOB_REQUEST             0x80000001
#define CLIENT_CODE_CONTROL_SOURCE          0x00000001
#define CLIENT_CODE_CONTROL_ON              0x00000003
#define CLIENT_CODE_ANTISWAY_OFF            0x00000004
#define CLIENT_CODE_ANTISWAY_ON             0x0000000C
#define CLIENT_CODE_AUTO_OFF                0x00000010
#define CLIENT_CODE_AUTO_ON                 0x00000030

#define CLIENT_COM_OPT_PB_ON                 0x00000001
#define CLIENT_COM_OPT_PB_OFF                0x00000000
#define CLIENT_COM_OPT_JOB_PICK              0x00000001
#define CLIENT_COM_OPT_JOB_GRND              0x00000002
#define CLIENT_COM_OPT_JOB_PARK              0x00000003


typedef struct ClientComHead {              //�[��IF���b�Z�[�W�w�b�_��
    INT32	        myid;		            //PC ID
    INT32	        code;		            //���M���e�R�[�h
    SOCKADDR_IN     addr_in;                //���j�L���X�g��MIP,�|�[�g
    INT32           option;
    INT32           node_id;                // �ڑ���ID
    SYSTEMTIME      time;                   // Time Stamp
}ST_CLIENT_COM_HEAD, * LPST_CLIENT_COM_HEAD;

typedef struct ClientComRcvBody {              //�[��IF���b�Z�[�W�w�b�_��
    double auto_tg_pos[8];              //�����ڕW�ʒu
}ST_CLIENT_RCV_BODY, * LPST_CLIENT_RCV_BODY;

typedef struct ClientComRcvMsg {      //�U��Z���T��M���b�Z�[�W
    ST_CLIENT_COM_HEAD         head;
    ST_CLIENT_RCV_BODY           body;
}ST_CLIENT_COM_RCV_MSG, * LPST_CLIENT_COM_RCV_MSG;

typedef struct ClientComSndMsg {      //�U��Z���T��M���b�Z�[�W
    ST_CLIENT_COM_HEAD         head;
  }ST_CLIENT_COM_SND_MSG, * LPST_CLIENT_COM_SND_MSG;

#if 0

/*#########################################################################*/
/*      �@�@         ����[���Ƃ̒ʐM���b�Z�[�W�t�H�[�}�b�g�@�@�@�@�@�@�@�@*/
/*#########################################################################*/

typedef struct OteComHead {             //�[��IF���b�Z�[�W�w�b�_��
    INT32	        myid;		        //PC ID
    INT32	        code;		        //���M���e�R�[�h
    SOCKADDR_IN     addr_in;            //���j�L���X�g��MIP,�|�[�g
    INT32           status;
    INT32           node_id;            // �ڑ���ID
}ST_OTE_COM_HEAD, * LPST_OTE_COM_HEAD;

typedef struct OteMultiComRcvBody {     //�U��Z���T��M���b�Z�[�W�{�f�B��
    INT32 n_standby_ote;
    INT32 rmt_connect_seq_no;           //���u�[���ڑ��V�[�P���X�ԍ�
    INT32 site_connect_seq_no;          //�@���[���ڑ��V�[�P���X�ԍ�
    INT32 my_latest_seq_no;             //���[���ڑ��V�[�P���X�ԍ�
}ST_OTE_MULTI_RCV_BODY, * LPST_OTE_MULTI_RCV_BODY;

typedef struct OteUniComRcvBody {       //�[������̎�M���b�Z�[�W�{�f�B��
    char paddintA1[4];
    double auto_tg_pos[8];
    double auto_tg_dist[8];
    char paddintD1[4];
    UCHAR PB[64];
    INT16 notch[8];
}ST_OTE_UNI_RCV_BODY, * LPST_OTE_UNI_RCV_BODY;

typedef struct OteUniComSndBody {       //�[���ւ̑��M���b�Z�[�W�{�f�B��
    char paddintA1[4];
    double pos_fb[8];
    double v_fb[8];
    double v_ref[8];
    double load_pos_fb[8];              //�݉׈ʒu
    double load_v_fb[8];                //�݉ב��x
    double auto_tg_pos[8];              //�����ڕW�ʒu
    double auto_tg_dist[8];             //�����ڕW����
    double auto_semi_set_tg[8][3];      //�������ݒ�ڕW�ʒu
    char paddintD1[4];
    UCHAR PB[64];
    INT16 notch[8];
}ST_OTE_UNI_SND_BODY, * LPST_OTE_UNI_SND_BODY;

typedef struct OteComMultiRcvMsg {      //�U��Z���T��M���b�Z�[�W
    ST_OTE_COM_HEAD         head;
    ST_OTE_MULTI_RCV_BODY   body;
}ST_OTE_MULTI_RCV_MSG, * LPST_OTE_MULTI_RCV_MSG;

typedef struct OteComUniRcvMsg {        //�U��Z���T��M���b�Z�[�W
    ST_OTE_COM_HEAD         head;
    ST_OTE_UNI_RCV_BODY   body;
}ST_OTE_UNI_RCV_MSG, * LPST_OTE_UNI_RCV_MSG;

typedef struct OteComMultiSndMsg {      //�U��Z���T��M���b�Z�[�W
    ST_OTE_COM_HEAD         head;
}ST_OTE_MULTI_SND_MSG, * LPST_OTE_MULTI_SND_MSG;

typedef struct OteComUniSndMsg {        //�U��Z���T��M���b�Z�[�W
    ST_OTE_COM_HEAD         head;
    ST_OTE_UNI_SND_BODY   body;
}ST_OTE_UNI_SND_MSG, * LPST_OTE_UNI_SND_MSG;

#endif