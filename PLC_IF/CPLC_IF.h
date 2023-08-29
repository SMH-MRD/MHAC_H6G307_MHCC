#pragma once
#include "COMMON_DEF.H"
#include "CSharedMem.h"	    //# ���L�������N���X
#include "CPushButton.h"
#include "PLC_IO_DEF.h"

#define PLC_IF_PLC_IO_MEM_NG        0x8000
#define PLC_IF_CRANE_MEM_NG         0x4000
#define PLC_IF_SIM_MEM_NG           0x2000
#define PLC_IF_AGENT_MEM_NG         0x1000
#define PLC_IF_CS_MEM_NG            0x0800

typedef struct st_PLCreadB_tag {                //���񖢎g�p
    INT16 spare[PLC_IF_SPARE_B_BUFSIZE];
    INT16 PB[PLC_IF_PB_B_BUFSIZE];
    INT16 NA[PLC_IF_NA_B_BUFSIZE];
}ST_PLC_READ_B, * LPST_PLC_READ_B;

typedef struct st_PLCreadW_tag{
    short main_x_buf[PLC_IF_MAIN_X_BUFSIZE];    //MAINPLC X
    short main_y_buf[PLC_IF_MAIN_Y_BUFSIZE];    //MAINPLC Y
    short gnt_x_buf[PLC_IF_GNT_X_BUFSIZE];      //���sPLC X
    short gnt_y_buf[PLC_IF_GNT_Y_BUFSIZE];      //���sPLC Y
    short ope_x_buf[PLC_IF_OPE_X_BUFSIZE];      //�^�]��PLC X
    short ope_y_buf[PLC_IF_OPE_Y_BUFSIZE];      //�^�]��PLC Y
    short cc_x_buf[PLC_IF_CC_X_BUFSIZE];        //CC�@LINK X
    short cc_y_buf[PLC_IF_CC_Y_BUFSIZE];        //CC�@LINK Y
    short padding_buf[2];                       //�\��(DI,AI���E
    short cc_w_buf[PLC_IF_CC_W_BUFSIZE];        //CC�@LINK W
    short abso_dw_buf[PLC_IF_ABS_DW_BUFSIZE];   //�A�u�\�R�[�_data
    short sensor_buf[PLC_IF_SENS_W_BUFSIZE];    //���̑��A�i���O�M��
}ST_PLC_READ_W, * LPST_PLC_READ_W;

typedef struct st_PLCwriteB_tag {
    INT16 pc_com_buf[PLC_IF_PC_B_WRITE_COMSIZE];
    INT16 pc_sim_buf[PLC_IF_PC_B_WRITE_SIMSIZE];
}ST_PLC_WRITE_B, * LPST_PLC_WRITE_B;

typedef struct st_PLCwriteW_tag {
    INT16 pc_com_buf[PLC_IF_PC_W_WRITE_COMSIZE];
    INT16 pc_sim_buf[PLC_IF_PC_W_WRITE_SIMSIZE];
}ST_PLC_WRITE_W, * LPST_PLC_WRITE_W;

#define MELSEC_NET_CH               51      //MELSECNET/H�{�[�h�̃`���l��No.
#define MELSEC_NET_NW_NO            2       //MELSECNET/H�l�b�g���[�N�ԍ�
#define MELSEC_NET_MY_NW_NO         0       //MELSECNET/H�{�[�h  ��NW�w�� 0�i�{�[�h�ݒ�l�Ƃ͈قȂ�j
#define MELSEC_NET_MY_STATION       255     //MELSECNET/H�{�[�h�v���ǔ� 255�i�{�[�h�ݒ�l�Ƃ͈قȂ�j
#define MELSEC_NET_SOURCE_STATION   1       //PLC�ǔ�
#define MELSEC_NET_B_WRITE_START    0x0600  //�������݊J�n�A�h���X�iPC�{�[�h��LB�̃A�h���X�w��j
#define MELSEC_NET_W_WRITE_START    0x0600  //�������݊J�n�A�h���X�iPC�{�[�h��LW�̃A�h���X�w��j
#define MELSEC_NET_B_READ_START     0x0900  //�ǂݍ��݊J�n�A�h���X�iPLC MAP����B�̃A�h���X�w��j
#define MELSEC_NET_W_READ_START     0x089C  //�ǂݍ��݊J�n�A�h���X�iPLC MAP����W�̃A�h���X�w��j

#define MELSEC_NET_OK               1
#define MELSEC_NET_SEND_ERR         -1
#define MELSEC_NET_RECEIVE_ERR      -2
#define MELSEC_NET_CLOSE            0

#define MELSEC_NET_RETRY_CNT        100 //�G���[��Retry�J�E���g����

#define MELSEC_NET_CODE_LW          24  //�f�o�C�X�R�[�h
#define MELSEC_NET_CODE_LB          23  //�f�o�C�X�R�[�h
#define MELSEC_NET_CODE_SM          5   //�f�o�C�X�R�[�h
#define MELSEC_NET_CODE_SB          5   //�f�o�C�X�R�[�h
#define MELSEC_NET_CODE_SD          14  //�f�o�C�X�R�[�h
#define MELSEC_NET_CODE_SW          14  //�f�o�C�X�R�[�h

#define N_PLC_W_OUT_WORD            128   //PLC LINK PLC�o��WORD��
#define N_PLC_B_OUT_WORD            16    //PLC LINK PLC�o��WORD��
#define N_PC_W_OUT_WORD             80   //PLC LINK PLC�o��WORD��
#define N_PC_B_OUT_WORD             16    //PLC LINK PLC�o��WORD��

#define MEL_FORCE_PLC_B             0    //IO�����Z�b�gID
#define MEL_FORCE_PLC_W             1    //IO�����Z�b�gID
#define MEL_FORCE_PC_B              2    //IO�����Z�b�gID
#define MEL_FORCE_PC_W              3    //IO�����Z�b�gID
#define MEL_FORCE_RESET             4    //IO�����Z�b�gID

typedef struct st_MelsecNet_tag {
    short chan=0;                         //�ʐM����̃`���l��No.
    short mode=0;                         //�_�~�[
    long  path;                         //�I�[�v�����ꂽ����̃p�X�@����N���[�Y���ɕK�v
    long err;                           //�G���[�R�[�h
    short status;                       //����̏�ԁ@0:������m���@0����F����@0��艺�F�ُ�
    short retry_cnt;                    //����I�[�v�����g���C�J�E���g �}���`���f�B�A�^�C�}�����̔{�������ԊԊu

    long write_size_w;                  //PC LW�������݃T�C�Y
    long write_size_b;                  //PC LB�������݃T�C�Y
    INT16 pc_w_out[sizeof(ST_PLC_READ_W)];    //PC�����o�b�t�@W
    INT16 pc_b_out[sizeof(ST_PLC_READ_B)];    //PC�����o�b�t�@B

    long read_size_w;                   //PLC LW�������݃T�C�Y
    long read_size_b;                   //PLC LB�������݃T�C�Y
    INT16 plc_w_out[sizeof(ST_PLC_WRITE_W)];  //PLC�����o�b�t�@W
    INT16 plc_b_out[sizeof(ST_PLC_WRITE_B)];  //PLC�����o�b�t�@B
  
    ST_PLC_OUT_BMAP plc_b_map;          //PLC LB�������݃o�b�t�@MAP���
    ST_PLC_OUT_WMAP plc_w_map;          //PLC LW�������݃o�b�t�@MAP���
    ST_PC_OUT_BMAP  pc_b_map;           //PC LB�������݃o�b�t�@MAP���
    ST_PC_OUT_WMAP  pc_w_map;           //PC LW�������݃o�b�t�@MAP���

    //BI,WI,BO,WO�����Z�b�g
    bool is_force_set_active[4];        //IO�����Z�b�g�t���O
    WORD forced_dat[4];                 //�����Z�b�g�l
    int forced_index[4];                 //�����Z�b�g�l

    //PC CTRL, PLC emulate�����Z�b�g

    BOOL is_forced_pc_ctrl;						//�����o�͗L���t���O
    BOOL is_forced_emulate;						//�����o�͗L���t���O

}ST_MELSEC_NET, * LPST_MELSEC_NET;

class CPLC_IF :    public CBasicControl
{
public:
    CPLC_IF();
    ~CPLC_IF();
 
    WORD helthy_cnt=0;

    //�I�[�o�[���C�h
    int set_outbuf(LPVOID); //�o�̓o�b�t�@�Z�b�g
    int init_proc();        //����������
    int input();            //���͏���
    int parse();            //���C������
    int output();           //�o�͏���

    //�ǉ����\�b�h
    int set_debug_status(); //�f�o�b�O���[�h���Ƀf�o�b�O�p�l���E�B���h�E����̓��͂ŏo�͓��e���㏑��
    int set_sim_status();   //�f�o�b�O���[�h����Simulator����̓��͂ŏo�͓��e���㏑��
    int closeIF();
 
    ST_MELSEC_NET   melnet;

    LPST_MELSEC_NET get_melnet() { return &melnet; }

    void set_debug_mode(int id) {
        if (id) mode |= PLC_IF_PC_DBG_MODE;
        else    mode &= ~PLC_IF_PC_DBG_MODE;
        return;
    }

    void set_pc_ctrl_forced(bool b) {
        if (b) melnet.is_forced_pc_ctrl = true;
        else melnet.is_forced_pc_ctrl = false;
        return;
    }
    void set_plc_emu_forced(bool b) {
        if (b) melnet.is_forced_emulate = true;
        else melnet.is_forced_emulate = false;
        return;
    }

    int is_debug_mode() { return(mode & PLC_IF_PC_DBG_MODE); }
    int mel_set_force(int id, bool bset, int index, WORD value);

private:
    //# �o�͗p���L�������I�u�W�F�N�g�|�C���^:
    CSharedMem* pPLCioObj;
    //# ���͗p���L�������I�u�W�F�N�g�|�C���^:
    CSharedMem* pCraneStatusObj;
    CSharedMem* pSimulationStatusObj;
    CSharedMem* pAgentInfObj;
    CSharedMem* pCSInfObj;

 
    ST_PLC_IO plc_io_workbuf;   //���L�������ւ̏o�̓Z�b�g��Ɨp�o�b�t�@

    LPST_SIMULATION_STATUS pSim;    //�V�~�����[�^�X�e�[�^�X
    LPST_CRANE_STATUS pCrane;
    LPST_AGENT_INFO pAgentInf;
    LPST_CS_INFO pCSInf;

    int parse_notch_com();
    int parce_brk_status();
    int parse_ope_com();
    int parse_sensor_fb();
    int set_notch_ref();
    int set_bit_coms();
    int set_ao_coms();
 };
