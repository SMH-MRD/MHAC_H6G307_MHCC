#pragma once

#include "CTaskObj.h"
#include "Spec.h"
#include "CSharedMem.h"




#define PHASE_CHECK_RANGE           0.02  // ���V�s�o�́@�ʑ����B����͈́@rad :1deg = 0.017 rad

#define AGENT_AUTO_TRIG_ACK_COUNT                       10
#define AGENT_CHECK_LARGE_SWAY_m2                       1.0     //�N�����ɏ����U���Ƃ݂Ȃ��U���m��2��
#define AGENT_CHECK_HST_POS_CLEAR_RANGE                 2.0     //�������㎞�Ɉ����A����J�n�\�Ȋ��㓞�B����
#define AGENT_CHECK_BH_POS_CLEAR_HST_DOWN_RANGE         2.0     //���������\�Ȉ������B����m
#define AGENT_CHECK_SLW_POS_CLEAR_HST_DOWN_RANGE_rad    0.17    //���������\�Ȑ��񓞒B����m
#define AGENT_CHECK_BH_POS_CLEAR_SLW_RANGE              3.0     //����J�n�\�Ȉ������B����m
#define AGENT_CHECK_SLW_POS_CLEAR_BH_RANGE_rad          0.5    //�����J�n�\�Ȑ��񓞒B����rad

#define AGENT_AS_PTN_PAUSE                              0        //�ۗ�
#define AGENT_AS_PTN_FINE_POS                           1        //�����ʒu����
#define AGENT_AS_PTN_POS_IN                             2        //�U��~��
#define AGENT_AS_PTN_POS_OUT                            3        //�U��o��

#define AGENT_AS_PH_CHECK_RANGE                         0.05   //1��= 0.0175rad

typedef struct stAgentWork {
    double T;	                                //�U�����
    double w;	                                //�U��p���g��
    double w2;	                                //�U��p���g��2��
    double pos[MOTION_ID_MAX];	                //���݈ʒu
    double v[MOTION_ID_MAX];	                //���[�^�̑��x
    double a_abs[MOTION_ID_MAX];	                //���[�^�̉����x
    double a_hp_abs[MOTION_ID_MAX];	                //���[�^�̉����x
    double vmax[MOTION_ID_MAX];                 //�ݓ_�̉����x
    double acc_time2Vmax[MOTION_ID_MAX];        //�ő��������
 
    double pp_th0[MOTION_ID_MAX][ACCDEC_MAX];   //�ʑ����ʂ̉�]���S
    double dist_for_target[MOTION_ID_MAX];      //�ڕW�܂ł̋���
    double dist_for_target_abs[MOTION_ID_MAX];  //�ڕW�܂ł̋���
    int    motion_dir[MOTION_ID_MAX];           //�ړ�����

    unsigned int agent_scan_ms;                 //AGENT�^�X�N�̃X�L�����^�C�� int ms
    double agent_scan;                          //AGENT�^�X�N�̃X�L�����^�C�� double s

    int as_ptn_type[MOTION_ID_MAX];             //�U��~�߃p�^�[��
    double as_gain_ta[MOTION_ID_MAX];           //�U��~�߃Q�C���i�������ԁj
    double as_ph[MOTION_ID_MAX];                //�U��~�߃Q�C���i�ʑ��j
 }ST_AGENT_WORK, * LPST_AGENT_WORK;


class CAgent:public CTaskObj
{
  public:
    CAgent();
    ~CAgent();

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    void init_task(void* pobj);
    void routine_work(void* param);

      
  private:
    
    LPST_POLICY_INFO    pPolicyInf;
    LPST_CS_INFO        pCSInf;
    LPST_AGENT_INFO     pAgentInf;
    LPST_CRANE_STATUS   pCraneStat;
    LPST_PLC_IO         pPLC_IO;
    LPST_SWAY_IO        pSway_IO;
    LPST_JOB_IO         pJob_IO;

    ST_AGENT_INFO       AgentInf_workbuf;
    ST_AGENT_WORK       st_as_work;                         //�U��~�߃p�^�[���쐬�p
    
    LPST_JOB_SET        pjob_active;                        //���s��JOB
    LPST_COMMAND_SET    pCom_hot;                           //���s���R�}���h
 


    void input();                                           //�O���f�[�^��荞��
    void main_proc();                                       //�������e
    void output();                                          //�o�̓f�[�^�X�V

    int init_comset(LPST_COMMAND_SET pcom);               //�R�}���h������

    LPST_COMMAND_SET    pCom_as;                            //�U��~�ߗp�R�}���h�Z�b�g�|�C���^�i���Ԃ͋��L��������ցj
    void set_as_workbuf(int motion);                        //�U��~�߃p�^�[���쐬�p�f�[�^��荞��
    int cal_as_recipe(int motion);                          //�U��~�߃p�^�[���v�Z�Z�b�g

  
    double cal_step(LPST_COMMAND_SET pCom, int motion);     //�����w�ߏo�͒l�̌v�Z

    bool is_command_completed(LPST_COMMAND_SET pCom);
        
    int set_ref_mh();                                       //�����x�w�ߒl�o��
    int set_ref_gt();                                       //���s���x�w�ߒl�o��
    int set_ref_slew();                                     //���񑬓x�w�ߒl�o��
    int set_ref_bh();                                       //�������x�w�ߒl�o��
    void update_pb_lamp_com();                              //�����v�\���o��
     
    int dbg_mont[8];//�f�o�b�O�p
                                                         
    //�^�u�p�l����Static�e�L�X�g��ݒ�
    void set_panel_tip_txt();
    //�^�u�p�l����Function�{�^����Static�e�L�X�g��ݒ�
    void set_panel_pb_txt();

};

