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

#define AUTO_TG_ADJUST_100mm      0.1 //目標位置補正距離0.1m
#define AUTO_TG_ADJUST_1000mm     1.0 //目標位置補正距離01m

#define CS_NORMAL_OPERATION_MODE 0

#define CS_JOBSET_STATUS_CLEAR               0x0000
#define CS_JOBSET_STATUS_DISABLE             0x0000
#define CS_JOBSET_STATUS_IDLE                0x0001
#define CS_JOBSET_STATUS_STANDBY             0x0002
#define CS_JOBSET_STATUS_ACTIVE              0x0004


#define CS_N_MSG_HOLD                       10

#define CS_OTE_U_MSG_TIMEOUT                10

class CClientService :public CTaskObj
{
public:
    CClientService(); 
    ~CClientService();
  
    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    void init_task(void* pobj);
    void routine_work(void* param);
 
   
    //AGENTからのアクセス関数
    LPST_JOB_SET get_next_job();                                    //次のJob問い合わせ
    int get_gurip_status() { return grip_status; }                  //グリップ状態問い合わせ

    //POLICYからのアクセス関数
    int update_job_status(LPST_JOB_SET pjobset, int fb_code);       //Jobの実行状況アンサバック

    //CLIENTへの報告関数
    int job_report2client(LPST_JOB_SET pjobset, int fb_code);       //Jobの実行状況報告

    void init_job();

private:

    int grip_status;

    int can_ote_activate();         //操作端末有効判定
    int ote_handle_proc();          //操作端末処理
 
     //クライアントからのメッセージ解析
    int perce_client_message(LPST_CLIENT_COM_RCV_MSG pmsg);
    
    //レシピセット
    LPST_JOB_SET set_job_seq(LPST_JOB_SET pjob_set);
    LPST_JOB_SET set_semi_seq(LPST_JOB_SET pjob_set);
 
   
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

   void input();               //外部データ取り込み
   void main_proc();           //処理内容
   void output();              //出力データ更新
                                  
   //タブパネルのStaticテキストを設定
   void set_panel_tip_txt();
   //タブパネルのFunctionボタンのStaticテキストを設定
   void set_panel_pb_txt();

};


