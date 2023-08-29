#pragma once

#include "COMMON_DEF.H"
#include "CSharedMem.h"	    //# ���L�������N���X
#include "CMob.h"

#define SWAY_IF_MIN_SCAN_MS         100     //�U��Z���T�̑��M�X�L�����^�C��msec

#define SIM_IF_PLC_IO_MEM_NG        0x8000
#define SIM_IF_CRANE_MEM_NG         0x4000
#define SIM_IF_SIM_MEM_NG           0x2000
#define SIM_IF_AGENT_MEM_NG         0x1000
#define SIM_IF_SWAY_MEM_NG          0x0800

class CSIM :
    public CBasicControl
{

private:

    //# �o�͗p���L�������I�u�W�F�N�g�|�C���^:
    CSharedMem* pSimulationStatusObj;
 
    //# ���͗p���L�������I�u�W�F�N�g�|�C���^:
    CSharedMem* pCraneStatusObj;
    CSharedMem* pPLCioObj;
    CSharedMem* pAgentInfObj;
 
    ST_SIMULATION_STATUS sim_stat_workbuf;   //���L�������ւ̏o�̓Z�b�g��Ɨp�o�b�t�@

    LPST_CRANE_STATUS pCraneStat;
    LPST_PLC_IO pPLC;
    LPST_AGENT_INFO pAgent;

    double dt;//�X�L��������

    CCrane* pCrane; //�N���[���̃��f��
    CLoad* pLoad;   //�ׂ݉̃��f��

  public:
    CSIM();
    ~CSIM();

    //�I�[�o�[���C�h
    int set_outbuf(LPVOID); //�o�̓o�b�t�@�Z�b�g
    int init_proc();        //����������
    int input();            //���͏���
    int parse();            //���C������
    int output();           //�o�͏���
    void set_dt(double _dt) { dt = _dt; return; }   //�N���Ԋu�Z�b�g�iMM Timer�̊֐��ŌĂяo�����ɃZ�b�g�j
    
    LPST_SIMULATION_STATUS pSIM_work;

    //�ǉ����\�b�h
 
    void set_mode(int id) {
        if (id) mode |= SIM_ACTIVE_MODE;
        else    mode &= ~SIM_ACTIVE_MODE;
    }

    int is_sim_active_mode() { return(mode & SIM_ACTIVE_MODE); }

private:
   
    int set_cran_motion();
    int set_sway_io();

};

