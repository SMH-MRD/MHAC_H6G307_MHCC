#pragma once
#include "COMMON_DEF.H"
#include "CSharedMem.h"	    //# ���L�������N���X
#include "CPushButton.h"

#include "PLC_IO_DEF_MELNET.h"
#include "PLC_IO_DEF_MC.h"

#define PLC_IF_PLC_IO_MEM_NG        0x8000
#define PLC_IF_CRANE_MEM_NG         0x4000
#define PLC_IF_SIM_MEM_NG           0x2000
#define PLC_IF_AGENT_MEM_NG         0x1000
#define PLC_IF_CS_MEM_NG            0x0800



class CPLC_IF :    public CBasicControl
{
public:
    CPLC_IF(HWND hWnd_parent);
    ~CPLC_IF();
 
    WORD helthy_cnt=0;
    HWND hWnd_parent;       //�e�E�B���h�E�̃n���h��

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
#ifdef _TYPE_MELSECNET
    ST_MELSEC_NET   melnet;
    LPST_MELSEC_NET get_melnet() { return &melnet; }
#endif
    LPST_PLC_WRITE lp_PLCwrite;
    LPST_PLC_READ  lp_PLCread;

    SOCKADDR_IN addrinc, addrins, addrfrom;         //MC�v���g�R���p�\�P�b�g�A�h���X

    void set_debug_mode(int id) {
        if (id) mode |= PLC_IF_PC_DBG_MODE;
        else    mode &= ~PLC_IF_PC_DBG_MODE;
        return;
    }
#ifdef _TYPE_MELSECNET
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
#endif
    void set_pc_ctrl_forced(bool b) {
        return;
    }
    void set_plc_emu_forced(bool b) {
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
