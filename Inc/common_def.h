#pragma once
#include <windows.h>

#define _DVELOPMENT_MODE                //�J������L���ɂ���

///#  ���ʃt���O
#define     L_ON                        0x01  // ON
#define     L_OFF                       0x00  // OFF

///# ���ʃ}�N��
#ifndef MIN
#  define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif

/*** �����萔�A�W�� ***/
#define GA				9.80665     //�d�͉����x

#define PI360           6.2832      //2��
#define PI330           5.7596   
#define PI315           5.4978
#define PI180           3.1416      //��
#define PI165           2.8798
#define PI150           2.6180
#define PI135           2.3562
#define PI120           2.0944
#define PI90            1.5708
#define PI60            1.0472
#define PI45            0.7854
#define PI41            0.72273
#define PI30            0.5236
#define PI15            0.2618
#define PI1DEG          0.017453
#define PI5             0.0873

#define RAD2DEG         57.29578
#define DEG2RAD         0.0174533
#define RAD2DEG2        3282.6
#define DEG2RAD2        0.00030462

/*** �}�N�� ***/
#ifndef MIN
#  define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif

#ifndef iABS
#  define iABS(a)  (a < 0 ? -a : a)
#endif

#ifndef fABS
#  define fABS(a)  (a < 0.0 ? -a : a)
#endif

#ifndef dABS
#  define dABS(a)  (a < 0.0 ? -a : a)
#endif

/*** �z��Q�Ɨp�@����C���f�b�N�X ***/
#define MOTION_ID_MAX   8  //���䎲�ő吔

#define ID_HOIST        0   //�� �@       ID
#define ID_GANTRY       1   //���s        ID
#define ID_TROLLY       2   //���s        ID
#define ID_BOOM_H       3   //����        ID
#define ID_SLEW         4   //����        ID
#define ID_OP_ROOM      5   //�^�]���ړ��@ID
#define ID_H_ASSY       6   //�݋�        ID
#define ID_COMMON       7   //����        ID

#define BIT_SEL_HST         0x00000001
#define BIT_SEL_GNT         0x00000002
#define BIT_SEL_TRY         0x00000004
#define BIT_SEL_BH          0x00000008
#define BIT_SEL_SLW         0x00000010
#define BIT_SEL_OPR         0x00000020
#define BIT_SEL_ASS         0x00000040
#define BIT_SEL_COM         0x00000080
#define BIT_SEL_ALL_0NOTCH  0x10000000
#define BIT_SEL_MOTION      0x0000FFFF
#define BIT_SEL_STATUS      0xFFFF0000


/*** �z��Q�Ɨp�@�����C���f�b�N�X ***/
#define ID_UP           0   //����
#define ID_DOWN         1   //�E��

#define ID_FWD          1   //�O�i
#define ID_REV          -1   //��i
#define ID_STOP         0   //��~
#define ID_LEFT         0   //����
#define ID_RIGHT        1   //�E��
#define ID_SELECT       2   //�I��

#define ID_ACC          0   //����
#define ID_DEC          1   //����

#define SID_X           0   // X����
#define SID_Y           1   // Y����
#define SID_R			2   // ���a����
#define SID_T           3   // �ڐ�����

/*** MODE ***/
//�V�~�����[�V����
#define IO_PRODUCTION                   0x0000//���@
#define USE_CRANE_SIM                   0x1000//�N���[�������V�~�����[�^�̏o�͂�FB�l�ɓK�p����
#define USE_PLC_SIM_COMMAND				0x0100//�@�㑀����͂�PLC�V�~�����[�^�̏o�͒l���g��
#define USE_REMOTE_SIM_COMMAND          0x0010//���u������͂Ƀ����[�g�V�~�����[�^�̏o�͒l���g��
#define USE_SWAY_CRANE_SIM		        0x0001//�U��Z���T�̐M�����N���[�������V�~�����[�^�̏o�͂��琶������

/*** ���� ***/
#define ID_ACCEPTED      1        //��t����
#define ID_REJECTED      -1       //��t�s��
#define ID_NO_REQUEST    0        //�v������
#define ID_EXIST         1       //�L��
#define ID_NA            0       //���ɂȂ�
#define ID_OK            1       //�L��
#define ID_NG            -1       //���ɂȂ�

class CBasicControl //��{����N���X
{
public:
    LPVOID poutput = NULL;      //���ʏo�̓�����
    DWORD out_size = 0;        //�o�̓o�b�t�@�̃T�C�Y
    DWORD  mode;                //���ʏo�̓��[�h
    DWORD  source_counter;      //���C���v���Z�X�̃w���V�[�J�E���^
    DWORD  my_helthy_counter=0; //���X���b�h�̃w���V�[�J�E���^

    CBasicControl() { mode = source_counter = 0; }
    ~CBasicControl() {}
    virtual int set_outbuf(LPVOID)=0;           //�o�̓o�b�t�@�Z�b�g
    virtual int init_proc() = 0;                //����������
    virtual int input() = 0;                    //���͏���
    virtual int parse() = 0;                     //���C������
    virtual int output() = 0;                   //�o�͏���
};

typedef struct DeviceCode {
    char    order[2];       //����
    char    system[2];  //�@�B�A�V�X�e���R�[�h    :�N���[���ԍ���
    char    type[2];    //�f�o�C�X��ʁ@          :����PC,�[����
    INT16   no;         //�V���A���ԍ�
}ST_DEVICE_CODE, * LPST_DEVICE_CODE;

