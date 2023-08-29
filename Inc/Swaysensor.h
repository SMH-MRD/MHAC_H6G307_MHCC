#pragma once

#include <winsock.h>
#include <time.h>

#define SWAY_SENSOR_N_CAM       2
#define SWAY_SENSOR_CAM1        0
#define SWAY_SENSOR_CAM2        1

#define SWAY_SENSOR_N_TARGET    2
#define SWAY_SENSOR_TG1         0
#define SWAY_SENSOR_TG2         1

#define SWAY_SENSOR__MSG_SEND_COM         10600

typedef struct SwayComRcvHead { //�U��Z���T��M���b�Z�[�W�w�b�_��
    char	id[4];			//PC ID
    SYSTEMTIME time;		//�^�C���X�^���v
}ST_SWAY_RCV_HEAD, * LPST_SWAY_RCV_HEAD;

typedef struct SwayCamSpec {
    INT32	pix_x;			//�J������f��x��
    INT32	pix_y;			//�J������f��y��
    INT32	pixlrad_x;	    //�J��������\�@PIX/rad
    INT32	pixlrad_y;	    //�J��������\�@PIX/rad
    INT32	l0_x;			//�J������t�p�����[�^�o
    INT32	l0_y;			//�J������t�p�����[�^�o
    INT32	ph0_x;			//�J������t�p�����[�^x1000000rad
    INT32	ph0_y;			//�J������t�p�����[�^x1000000rad
    INT32	phc_x;			//�J������t�p�����[�^x1000000rad
    INT32	phc_y;			//�J������t�p�����[�^x1000000rad
}ST_SWAY_CAM_SPEC, * LPST_SWAY_CAM_SPEC;

typedef struct SwaySensStatus {
    //�@���ԏ��
    INT32	mode;		    //���[�h
    INT32	error;		    //�G���[�X�e�[�^�X
    INT32	status;		    //���o�X�e�[�^�X
    //�X�Όv���  
    INT32	tilt_x;         //�X��X
    INT32	tilt_y;         //�X��Y
    INT32	tilt_dx;        //�X�Ίp���xX
    INT32	tilt_dy;        //�X�Ίp���xY
} ST_SWAY_SENS_STAT, * LPST_SWAY_SENS_STAT;

typedef struct TargetStatus {
    //�U�ꌟ�o���  
    INT32	th_x;		    //�U�pxPIX
    INT32	th_y;		    //�U�pyPIX
    INT32	dth_x;		    //�U�p���xx�@PIX/s
    INT32	dth_y;			//�U�p���xy�@PIX/s
    INT32	th_x0;			//�U�p0�_xPIX
    INT32	th_y0;			//�U�p0�_yPIX
    INT32	dpx_tgs;		//�^�[�Q�b�g�ԋ���X����
    INT32	dpy_tgs;		//�^�[�Q�b�g�ԋ���Y����
    INT32	tg_size;		//�^�[�Q�b�g�T�C�Y
} ST_TARGET_STAT, * LPST_TARGET_STAT;

typedef struct SwayComRcvBody { //�U��Z���T��M���b�Z�[�W�{�f�B��
    ST_SWAY_CAM_SPEC cam_spec;
    ST_SWAY_SENS_STAT cam_stat;
    ST_TARGET_STAT tg_stat[SWAY_SENSOR_N_TARGET];           //�J�����Q���^�[�Q�b�g�Q
    char info[32];
}ST_SWAY_RCV_BODY, * LPST_SWAY_RCV_BODY;

typedef struct SwayComRcvMsg { //�U��Z���T��M���b�Z�[�W
    ST_SWAY_RCV_HEAD head;
    ST_SWAY_RCV_BODY body[SWAY_SENSOR_N_CAM];
}ST_SWAY_RCV_MSG, * LPST_SWAY_RCV_MSG;


typedef struct SwayComSndHead { //�U��Z���T���M���b�Z�[�W�w�b�_��
    char	id[4];			    //�@��̏��
    sockaddr_in sockaddr;       //���M��IP�A�h���X
}ST_SWAY_SND_HEAD, * LPST_SWAY_SND_HEAD;

typedef struct SwayComSndBody { //�U��Z���T���M���b�Z�[�W�{�f�B��
    INT32 command;
    INT32 mode;
    INT32 freq;                         //�ŏ���M����       
    INT32 d[SWAY_SENSOR_N_CAM];         //�J����-�^�[�Q�b�g�ԋ���
}ST_SWAY_SND_BODY, * LPST_SWAY_SND_BODY;

typedef struct SwayComSndMsg { //�U��Z���T��M���b�Z�[�W
    ST_SWAY_SND_HEAD head;
    ST_SWAY_SND_BODY body;
}ST_SWAY_SND_MSG, * LPST_SWAY_SND_MSG;

//���M�f�[�^�R�[�h
#define SW_SND_COM_ONCE_DATA        0x00000000
#define SW_SND_COM_CONST_DATA       0x00000001
#define SW_SND_COM_STOP_DATA        0x00000008
#define SW_SND_COM_CAMERA1_0SET     0x00000010
#define SW_SND_COM_CAMERA2_0SET     0x00000020
#define SW_SND_COM_TILT1_0SET       0x00000040
#define SW_SND_COM_TILT2_0SET       0x00000080
#define SW_SND_COM_CAMERAR1_RESET   0x00000100
#define SW_SND_COM_CAMERAR2_RESET   0x00000200
#define SW_SND_COM_TILT1_RESET      0x00000400
#define SW_SND_COM_TILT2_RESET      0x00000800
#define SW_SND_COM_SAVE_IMG         0x00001000
#define SW_SND_COM_PC_RESET         0x10000000

#define SW_SND_DEFAULT_SCAN         100         //100msec

#define SW_SND_MODE_NORMAL          0x000000001         //100msec


//��M�f�[�^�R�[�h
//�G���[���
#define SW_RCV_ERR_CODE_CAMERA      0x00000001
#define SW_RCV_ERR_CODE_TILT        0x00000002
#define SW_RCV_ERR_CODE_DETECT_TG1  0x00000010
#define SW_RCV_ERR_CODE_DETECT_TG2  0x00000020

//���o���
#define SW_RCV_STAT_TG1_DETECTED    0x00000001
#define SW_RCV_STAT_TG2_DETECTED    0x00000002
#define SW_RCV_STAT_INIT_SENSOR     0x00000010
#define SW_RCV_STAT_INIT_TILT       0x00000020
#define SW_RCV_STAT_0SET_SENSOR     0x00000010
#define SW_RCV_STAT_0SET_TILT       0x00000020

//���b�Z�[�W�e�L�X�g
//�G���[���
#define SW_TXT_ERR_CODE_CAMERA      L"�J�����ُ�"
#define SW_TXT_ERR_CODE_TILT        L"�X�Όv�ُ�"
#define SW_TXT_ERR_CODE_DETECT_TG1  L"�^�[�Q�b�g�P���o�ُ�"
#define SW_TXT_ERR_CODE_DETECT_TG2  L"�^�[�Q�b�g�Q���o�ُ�"

//���o���
#define SW_TXT_STAT_TG1_DETECTED    L"�^�[�Q�b�g�P���o��"
#define SW_TXT_STAT_TG2_DETECTED    L"�^�[�Q�b�g�Q���o��"
#define SW_TXT_STAT_INIT_SENSOR     L"�Z���T������"
#define SW_TXT_STAT_INIT_TILT       L"�X�Όv������"
#define SW_TXT_STAT_0SET_SENSOR     L"�Z���T�O�Z�b�g��"
#define SW_TXT_STAT_0SET_TILT       L"�X�Όv�O�Z�b�g��"