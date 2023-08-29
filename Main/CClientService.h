#pragma once

#include "CTaskObj.h"
#include "Spec.h"
#include "CSharedMem.h"
#include "ClientIf.h"

#include "CPolicy.h"
#include "CEnvironment.h"


#define SEMI_AUTO_TG_RESET_TIME     100
#define SEMI_AUTO_TG_SELECT_TIME    4
#define AUTO_START_CHECK_TIME       4
#define LAMP_FLICKER_BASE_COUNT     8
#define LAMP_FLICKER_CHANGE_COUNT   5


#define CS_CLEAR_SEMIAUTO      1
#define CS_ADD_SEMIAUTO        2
#define CS_CLEAR_JOB           3
#define CS_ADD_JOB             4

#define AUTO_TG_ADJUST_100mm      0.1 //�ڕW�ʒu�␳����0.1m
#define AUTO_TG_ADJUST_1000mm     1.0 //�ڕW�ʒu�␳����01m

#define CS_NORMAL_OPERATION_MODE 0

#define CS_JOBSET_EVENT_CLEAR               0
#define CS_JOBSET_EVENT_JOB_STANDBY         1
#define CS_JOBSET_EVENT_SEMI_STANDBY        2
#define CS_JOBSET_EVENT_JOB_TRIG            4
#define CS_JOBSET_EVENT_SEMI_TRIG           8
#define CS_JOBSET_EVENT_JOB_OVERFLOW       16
#define CS_JOBSET_EVENT_SEMI_SEL_CLEAR     32

#define CS_N_MSG_HOLD                       10

class CClientService :public CTaskObj
{
public:
    CClientService(); 
    ~CClientService();
  
    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    void init_task(void* pobj);
    void routine_work(void* param);
 
   
    //AGENT����̃A�N�Z�X�֐�
    LPST_JOB_SET get_next_job();                                    //����Job�₢���킹

    //POLICY����̃A�N�Z�X�֐�
    int update_job_status(LPST_JOB_SET pjobset, int fb_code);       //Job�̎��s�󋵃A���T�o�b�N

    //CLIENT�ւ̕񍐊֐�
    int job_report2client(LPST_JOB_SET pjobset, int fb_code);       //Job�̎��s�󋵕�

private:

    int parce_onboard_input(int mode);
    int parce_ote_imput(int mode);
    int can_ote_activate();

    bool chk_trig_ote_touch_pos_target();
    bool chk_trig_ote_touch_dist_target();
    int update_ote_touch_pos_tg(); 

    int set_selected_target_for_view();
    int set_hp_pos_for_view();

    //�N���C�A���g����̃��b�Z�[�W���
    int perce_client_message(LPST_CLIENT_COM_RCV_MSG pmsg);
    
    //���V�s�Z�b�g
    LPST_JOB_SET set_job_recipe(LPST_JOB_SET pjob_set);
    LPST_JOB_SET set_semi_recipe(LPST_JOB_SET pjob_set);
 
   
    LPST_CRANE_STATUS   pCraneStat;
    LPST_PLC_IO         pPLC_IO;
    LPST_CS_INFO        pCSinf;
    LPST_AGENT_INFO     pAgent_Inf;
    LPST_OTE_IO         pOTE_IO;
    LPST_JOB_IO         pJob_IO;

    CPolicy* pPolicy;
    CEnvironment* pEnvironment;

    ST_CS_INFO CS_workbuf;



 
    ST_CLIENT_COM_RCV_MSG client_rcv_msg[CS_N_MSG_HOLD];

   void input();               //�O���f�[�^��荞��
   void main_proc();           //�������e
   void output();              //�o�̓f�[�^�X�V
                                  
   //�^�u�p�l����Static�e�L�X�g��ݒ�
   void set_panel_tip_txt();
   //�^�u�p�l����Function�{�^����Static�e�L�X�g��ݒ�
   void set_panel_pb_txt();

};


