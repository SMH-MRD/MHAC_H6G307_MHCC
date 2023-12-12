#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>
#include "COMMON_DEF.H"
#include "CSharedMem.h"	    //# 共有メモリクラス
#include "Spec.h"
#include <commctrl.h>
#include <time.h>
#include <string>
#include "OTE_DEF.h"
#include "OTEIFpanel.h"

//タイマー
#define ID_PC_MULTICAST_TIMER				10600	    // マルチキャスト IF送信周期
#define PC_MULTICAST_SCAN_MS				1000	    // マルチキャスト IF送信周期

//共有メモリ異常フラグ
#define OTE_IF_OTE_IO_MEM_NG				0x8000
#define OTE_IF_CRANE_MEM_NG					0x4000
#define OTE_IF_SIM_MEM_NG					0x2000
#define OTE_IF_PLC_MEM_NG					0x1000

//ソケットイベントID
#define ID_SOCK_EVENT_PC_MULTI_PC 		    10603		//PC受信ソケットイベント　PC MULTICASTメッセージ
#define ID_SOCK_EVENT_OTE_UNI_PC	        10604		//PC受信ソケットイベント　OTE UNICASTメッセージ
#define ID_SOCK_EVENT_OTE_MULTI_PC	        10605		//PC受信ソケットイベント　PC UNICASTメッセージ

#define OTEIF_WORK_WND_X					1380				//WORK WINDOW 表示位置X
#define OTEIF_WORK_WND_Y					0					//WORK WINDOW 表示位置Y
#define OTEIF_WORK_WND_W0					550					//WORK WINDOW WINDOW幅(MSG無）
#define OTEIF_WORK_WND_H0					400					//WORK WINDOW WINDOW高さ(MSG有）
#define OTEIF_WORK_WND_W1					1200				//WORK WINDOW WINDOW幅(MSG無）
#define OTEIF_WORK_WND_H1					400					//WORK WINDOW WINDOW高さ(MSG有）

class COteIF :  public CBasicControl
{
public:

    COteIF(HWND hWnd);
   ~COteIF();

   static ST_OTE_IO ote_io_workbuf;
   static HWND hWnd_parent;		//親ウィンドウのハンドル
   static HWND hWnd_work;
   static ST_OTEIF_WORK_WND st_work_wnd;

   WORD helthy_cnt = 0;

   //# 出力用共有メモリオブジェクトポインタ:
   CSharedMem* pOteIOObj;
   //# 入力用共有メモリオブジェクトポインタ:
   CSharedMem* pCraneStatusObj;
   CSharedMem* pSimulationStatusObj;
   CSharedMem* pPLCioObj;
   CSharedMem* pCSInfObj;
   CSharedMem* pAgentInfObj;
   CSharedMem* pSwayIO_Obj;

   static HWND open_work_Wnd(HWND hwnd);
   static HRESULT close();

   //オーバーライド
   int set_outbuf(LPVOID);     //出力バッファセット
   int init_proc();            //初期化処理
   int input();                //入力処理
   int parse();                //メイン処理
   int output();               //出力処理

   void set_if_disp_hold(bool flg) {
       if (flg) {
           st_work_wnd.is_hold_disp = true; 
           SendMessage(st_work_wnd.hctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_HOLD], BM_SETCHECK, BST_CHECKED, 0L);
        }
       else {
            st_work_wnd.is_hold_disp = false; 
            SendMessage(st_work_wnd.hctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_HOLD], BM_SETCHECK, BST_UNCHECKED, 0L);
       }
   }

       //追加メソッド
   BOOL show_if_wnd();
   BOOL hide_if_wnd();

   static LPST_OTE_IO		pOTEio;
   static LPST_CRANE_STATUS	pCraneStat;
   static LPST_PLC_IO		pPLCio;
   static LPST_CS_INFO		pCSInf;
   static LPST_AGENT_INFO	pAgentInf;
   static LPST_SWAY_IO		pSway_IO;

   static std::wstring			msg_ws;
   static std::wostringstream	msg_wos;

   static CSockAlpha* pSockOteUniCastPc;		//OTE→PCユニキャスト受信用ソケット
   static CSockAlpha* pSockPcMultiCastPc;		//PC->PCマルチキャスト受信用ソケット
   static CSockAlpha* pSockOteMultiCastPc;		//OTE→PCマルチキャスト受信用ソケット
    
   static SOCKADDR_IN addrin_ote_u_pc;			//OTE→PC PCユニキャスト受信アドレス（PC用)
   static SOCKADDR_IN addrin_pc_m_pc;			//PC->PC PCマルチキャスト受信アドレス（PC用)
   static SOCKADDR_IN addrin_ote_m_pc;			//OTE→PC OTEマルチキャスト受信アドレス（PC用)
   
   static SOCKADDR_IN addrin_pc_u_snd;		    //PC->OTE PCユニチキャスト送信先アドレス（PC受信用)
   static SOCKADDR_IN addrin_pc_m_pc_snd;		//PC->PC PCマルチキャスト送信先アドレス（PC受信用)
   static SOCKADDR_IN addrin_pc_m_ote_snd;		//PC->OTE PCマルチキャスト送信先アドレス（OTE受信用)

   static SOCKADDR_IN addrin_ote_u_from;		//OTEユニキャスト送信元アドレス（PC用)
   static SOCKADDR_IN addrin_pc_m_from;		    //PCマルチキャスト送信元アドレス（PC用)
   static SOCKADDR_IN addrin_ote_m_from;		//OTEマルチキャスト送信元アドレス（PC用)
      
   static SOCKADDR_IN addr_active_ote;			//操作信号が有効な端末のアドレス


   static LONG cnt_snd_pc_u, cnt_snd_pc_m_ote, cnt_snd_pc_m_pc;
   static LONG cnt_rcv_ote_u, cnt_rcv_ote_m, cnt_rcv_pc_m;

   static ST_PC_U_MSG st_msg_pc_u_snd;
   static ST_PC_M_MSG st_msg_pc_m_snd;
   
   static ST_PC_M_MSG st_msg_pc_m_pc_rcv;
   static ST_PC_M_MSG st_msg_pc_m_ote_rcv;
  
   static ST_OTE_M_MSG st_msg_ote_m_pc_rcv;
   static ST_OTE_M_MSG st_msg_ote_m_ote_rcv;
   static ST_OTE_U_MSG st_msg_ote_u_rcv;
   
   static ST_OTE_U_MSG st_ote_active_msg;	//操作信号が有効な現メッセージ

   static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
     
   static void wstr_out_inf(const std::wstring& srcw);

   static LPST_PC_U_MSG set_msg_pc_u();
   static LPST_PC_M_MSG set_msg_pc_m();
 
   static HRESULT snd_pc_m_pc(LPST_PC_M_MSG pbuf);								//PC→PC　	マルチキャスト送信処理
   static HRESULT snd_pc_u_ote(LPST_PC_U_MSG pbuf, SOCKADDR_IN* p_addrin_to);	//PC→OTE　	ユニキャスト送信処理
   static HRESULT snd_pc_m_ote(LPST_PC_M_MSG pbuf);								//PC->OTE	マルチキャスト送信処理

   //受信処理用
   static HRESULT rcv_ote_u_pc(LPST_OTE_U_MSG pbuf);							//OTE→PC　 ユニキャスト受信処理
   static HRESULT rcv_pc_m_pc(LPST_PC_M_MSG pbuf);								//PC→PC　	マルチキャスト受信処理
   static HRESULT rcv_ote_m_pc(LPST_OTE_M_MSG pbuf);							//OTE->PC	マルチキャスト受信処理
   
   static void if_disp_update();
  
   static void disp_msg_cnt();
   
   static void set_OTEIF_panel_objects(HWND hWnd);
   static void set_sock_addr(SOCKADDR_IN *paddr, PCSTR ip,USHORT port){
	   paddr->sin_family = AF_INET;
	   paddr->sin_port = htons(port);
	   inet_pton(AF_INET, ip, &(paddr->sin_addr.s_addr));
   }
 };
