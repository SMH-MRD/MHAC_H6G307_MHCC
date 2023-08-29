#pragma once

#include "CTaskObj.h"
#include "Spec.h"
#include "CSharedMem.h"

#define PTN_ORDINARY                0x00000000  //�ʏ�
#define PTN_NON_FBSWAY_FULL         0x00000001
#define PTN_FBSWAY_FULL             0x00000002
#define PTN_NON_FBSWAY_2INCH        0x00000004
#define PTN_FBSWAY_AS               0x00000008

#define PTN_FBSWAY_AS               0x00000008

#define POLICY_PTN_OK               1
#define POLICY_PTN_NG               0

#define N_AUTO_PARAM                8

#define SPD_FB_DELAY_TIME           0.3             //���x�w��-FB�x�ꎞ��
#define FINE_POS_TIMELIMIT          50.0             //�t�@�C���|�W�V���j���O��������
#define POL_TM_OVER_CHECK_COUNTms   120000


typedef struct stPolicyWork {
    double T;	                                //�U�����
    double w;	                                //�U��p���g��
    double w2;	                                //�U��p���g��2��
    double pos[MOTION_ID_MAX];	                //���݈ʒu
    double v[MOTION_ID_MAX];	                //���[�^�̑��x
    double a_abs[MOTION_ID_MAX];	            //���[�^�̉����x�@��Βl
    double a_hp_abs[MOTION_ID_MAX];	            //�ݓ_�̉����x�@��Βl
    double vmax_abs[MOTION_ID_MAX];             //���[�^�̍ő呬�x
    double acc_time2Vmax[MOTION_ID_MAX];        //�ő��������
    double dist_for_target[MOTION_ID_MAX];      //�ڕW�܂ł̋�����������
    double dist_for_target_abs[MOTION_ID_MAX];      //�ڕW�܂ł̋�����������
    double pp_th0[MOTION_ID_MAX][ACCDEC_MAX];   //�ʑ����ʂ̉�]���S
    ST_POS_TARGETS target;                      //�ڕW�ʒu
    int motion_dir[MOTION_ID_MAX];              //�ړ�����
     unsigned int agent_scan_ms;                 //AGENT�^�X�N�̃X�L�����^�C��
     double agent_scan;                         //AGENT�^�X�N�̃X�L�����^�C�� �b
}ST_POLICY_WORK, * LPST_POLICY_WORK;


class CPolicy :public CTaskObj
{
public:
   CPolicy();
   ~CPolicy();

   LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

   void init_task(void* pobj);
   void routine_work(void* param);
 
  //AGENT
   LPST_COMMAND_SET req_command(LPST_JOB_SET pjob_set);                      //Agent����̗v���ɉ����Ď��s�R�}���h���Z�b�g���ĕԂ�
   int update_command_status(LPST_COMMAND_SET pcom, int code);  //Agent����̃R�}���h���s�󋵕񍐂���t,���̃R�}���h����Ƃ��͂����Ԃ�
 
private:

    LPST_POLICY_INFO    pPolicyInf;
    LPST_PLC_IO         pPLC_IO;
    LPST_CRANE_STATUS   pCraneStat;
    LPST_OTE_IO         pOTE_IO;
    LPST_AGENT_INFO     pAgentInf;
    LPST_SWAY_IO        pSway_IO;
    LPST_CS_INFO        pCSInf;
    LPST_JOB_IO         pJob_IO;

    void input();               //�O���f�[�^��荞��
    void main_proc();           //�������e
    void output();              //�o�̓f�[�^�X�V

    LPST_COMMAND_SET setup_job_command(LPST_COM_RECIPE pcom_recipe, int type);  //���s���锼�����̃R�}���h���Z�b�g����
  
 
    int set_recipe_semiauto_bh(int jobtype, LPST_MOTION_RECIPE precipe, bool is_fbtype, LPST_POLICY_WORK pwork);
    int set_recipe_semiauto_slw(int jobtype, LPST_MOTION_RECIPE precipe, bool is_fbtype, LPST_POLICY_WORK pwork);
    int set_recipe_semiauto_mh(int jobtype, LPST_MOTION_RECIPE precipe, bool is_fbtype, LPST_POLICY_WORK pwork);

    LPST_POLICY_WORK set_com_workbuf(ST_POS_TARGETS trget);
    ST_POLICY_INFO   PolicyInf_workbuf;
    ST_POLICY_WORK   st_com_work;
    int command_id = 0;

                                                         
   //�^�u�p�l����Static�e�L�X�g��ݒ�
   void set_panel_tip_txt();
   //�^�u�p�l����Function�{�^����Static�e�L�X�g��ݒ�
   void set_panel_pb_txt();


    
   const double param_auto[NUM_OF_AS_AXIS][N_AUTO_PARAM] =
   { 
     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
     {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
   };

};

