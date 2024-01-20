#pragma once
#include "COMMON_DEF.H"
#include "CSharedMem.h"	    //# ���L�������N���X
#include "PLC_DEF.h"

#define PLC_IF_PLC_IO_MEM_NG        0x8000
#define PLC_IF_CRANE_MEM_NG         0x4000
#define PLC_IF_SIM_MEM_NG           0x2000
#define PLC_IF_AGENT_MEM_NG         0x1000
#define PLC_IF_CS_MEM_NG            0x0800
#define PLC_IF_OTE_MEM_NG           0x0400

//-IF Window�̔z�u�ݒ�
#define IF_WND_INIT_SIZE_W			530		//-IF Window�̏����T�C�Y�@W
#define IF_WND_INIT_SIZE_H			600		//-IF Window�̏����T�C�Y�@H
#define IF_WND_INIT_POS_X			858	//-IF Window�̏����ʒu�ݒ�@X
#define IF_WND_INIT_POS_Y			0		//-IF Window�̏����ʒu�ݒ�@Y

#define PLC_IF_SIM_HCNT_PLS1N       2048        //�����J�E���^���[�^1��]�p���X
#define PLC_IF_SIM_HCNT_DEF         100000000   //�f�t�H���g�J�E���g�l

typedef struct st_PanelSim {//�p�l���V�~�����[�g�p�\����
    double nscan1sec = 1000.0 / 25.0;//1�b�Ԃ̃X�L������
    INT32 hcnt[MOTION_ID_MAX] = {//�����J�E���^�J�E���g�l
        PLC_IF_SIM_HCNT_DEF,PLC_IF_SIM_HCNT_DEF,PLC_IF_SIM_HCNT_DEF,PLC_IF_SIM_HCNT_DEF,PLC_IF_SIM_HCNT_DEF,PLC_IF_SIM_HCNT_DEF
    };
    double vcnt100[MOTION_ID_MAX] = {//100�����x�ł̃J�E���g���x
        25600.0,0.0,0.0,5973.3,5973.3,34133.33
    };
    double vcnt1invscan[MOTION_ID_MAX] = {//�C���o�[�^�P�w�ߓ���̃X�L�������J�E���g���x
        0.0096,0.0,0.0,0.074667,0.074667,0.085333
    };
    double ndrm[MOTION_ID_MAX] = {//�h������]��
        25600,0.0,0.0,64.0,0.0,65.526
    };
}ST_PANEL_SIM, * LPST_PANEL_SIM;

class CPLC_IF :    public CBasicControl
{
public:
    CPLC_IF(HWND hWnd_parent);
    ~CPLC_IF();
 
    WORD helthy_cnt=0;
    HWND hWnd_parent;       //�e�E�B���h�E�̃n���h��
    HWND hWnd_plcif;

    ST_PANEL_SIM st_pnl_sim;

    //�I�[�o�[���C�h
    int set_outbuf(LPVOID); //�o�̓o�b�t�@�Z�b�g
    int init_proc();        //����������
    int input();            //���͏���
    int parse();            //���C������
    int output();           //�o�͏���

    //�ǉ����\�b�h
    BOOL show_if_wnd();
    BOOL hide_if_wnd();


    LPST_PLC_WRITE lp_PLCwrite; //PLC�������݃f�[�^�o�b�t�@�A�h���X
    LPST_PLC_READ  lp_PLCread;  //PLC�ǂݍ��݃f�[�^�o�b�t�@�A�h���X

    SOCKADDR_IN addrinc, addrins, addrfrom;         //MC�v���g�R���p�\�P�b�g�A�h���X

    void set_mode(DWORD id) {                       //0bit:����w�ߗL���@1bit�F�Z���T��Ԏw�ߗL�� �@2bit�FFULL SIMULATION
        mode &= 0x00000000;
        mode |= id;
        return;
    }
 
private:
    //# �o�͗p���L�������I�u�W�F�N�g�|�C���^:
    CSharedMem* pPLCioObj;
    //# ���͗p���L�������I�u�W�F�N�g�|�C���^:
    CSharedMem* pCraneStatusObj;
    CSharedMem* pSimulationStatusObj;
    CSharedMem* pAgentInfObj;
    CSharedMem* pCSInfObj;
    CSharedMem* pOTEioObj;
     
    ST_PLC_IO plc_if_workbuf;   //���L�������ւ̏o�̓Z�b�g��Ɨp�o�b�t�@

    LPST_SIMULATION_STATUS pSim;    //�V�~�����[�^�X�e�[�^�X
    LPST_CRANE_STATUS pCrane;
    LPST_AGENT_INFO pAgentInf;
    LPST_CS_INFO pCSInf;
    LPST_OTE_IO pOTEio;

    int clear_plc_write();

    int parse_data_in();
    int parse_data_out();
    int parse_ote_com();
    int parse_auto_com();
    int parse_sim_status();

    int parce_brk_status();
    int parse_ope_com();
    int parse_sensor_fb();
    int set_notch_ref();
    int set_ao_coms();

    UINT16 get_notch_code(INT16 notch);
    INT16 get_notch_pos(UINT16 code);
 };
