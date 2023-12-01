#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <ws2tcpip.h>
#include "COMMON_DEF.H"
#include <commctrl.h>
#include <time.h>
#include <string>
#include "CSockAlpha.h"
#include "OTE_DEF.h"
#include "OTEpanel.H"

//タイマー
#define ID_OTE_MULTICAST_TIMER				199
#define ID_OTE_UNICAST_TIMER				198
#define OTE_MULTICAST_SCAN_MS				1000	    // マルチキャスト IF送信周期
#define OTE_UNICAST_SCAN_MS					100			// ユニキャスト IF送信周期

//ソケットイベントID
#define ID_SOCK_EVENT_PC_UNI_OTE 		    10653		//OTE受信ソケットイベント　PC UNICASTメッセージ
#define ID_SOCK_EVENT_PC_MULTI_OTE 		    10654		//OTE受信ソケットイベント　PC MULTICASTメッセージ
#define ID_SOCK_EVENT_OTE_MULTI_OTE 		10655		//OTE受信ソケットイベント　PC MULTICASTメッセージ


#define OTE_N_PEN		            8
#define OTE_N_BRUSH                 8
#define CMON_RED_PEN                0
#define CMON_BLUE_PEN               1
#define CMON_GREEN_PEN              2
#define CMON_GLAY_PEN               3
#define CMON_YELLOW_PEN             4
#define CMON_MAZENDA_PEN            5 
#define CMON_MAZENDA_PEN2           6

#define OTE_WORK_WND_X						10					//OTEメインパネル表示位置X
#define OTE_WORK_WND_Y						550					//OTEメインパネル表示位置Y
#define OTE_WORK_WND_W						800					//OTEメインパネルWINDOW幅
#define OTE_WORK_WND_H						600					//OTEメインパネルWINDOW高さ

#define OTE_WORK_SUB_WND_X					0					//メンテパネル表示位置X
#define OTE_WORK_SUB_WND_Y					0					//メンテパネル表示位置Y
#define OTE_WORK_SUB_WND_W					250					//メンテパネルWINDOW幅
#define OTE_WORK_SUB_WND_H					270					//メンテパネルWINDOW高さ

#define OTE_N_SUB_WND						5

//RADIO NOTCH ID  OTE_ID_RADIO + 100xMOTION_ID + 10 + NOTCH(-5～+5）
#define OTE_N_ID_PB_CHK_RADIO			32
#define OTE_ID_PB_CHK_RADIO             21600
//ラジオ
#define OTE_INDEX_RADIO_CONNECT			0
#define OTE_INDEX_RADIO_AUTO			1
#define OTE_INDEX_RADIO_MODE			2
#define OTE_INDEX_RADIO_FAULT			3
#define OTE_INDEX_RADIO_MOMENT			4

#define OTE_INDEX_RADIO_IFCHK_UNI		5
#define OTE_INDEX_RADIO_IFCHK_MPC		6
#define OTE_INDEX_RADIO_IFCHK_MTE		7

//チェックボックス
#define OTE_INDEX_CHK_ESTOP				8
#define OTE_INDEX_CHK_REMOTE			9
#define OTE_INDEX_CHK_CONNECT			10
#define OTE_INDEX_CHK_OTE_MON			11
//PB
#define OTE_INDEX_PB_START				12	//釦オブジェクトの開始INDEX
#define OTE_INDEX_PB_CTR_SOURCE			12
#define OTE_INDEX_PB_FAULT_RESET		13
#define OTE_INDEX_PB_END				13	//釦オブジェクトの終了INDEX

//STATIC ID  OTE_ID_STATIC + ITEM ID
#define OTE_N_ID_STATIC					32
#define OTE_ID_STATIC					21700
#define OTE_INDEX_STATIC_COMMON_WND		0
#define OTE_INDEX_STATIC_CONNECT_CNT    1
#define OTE_INDEX_STATIC_OTE_IP_INF     2

#define OTE_N_PB_CHK_RADIO				64
#define OTE_N_STATIC					64
#define OTE_PB_HOLDTIME_MASK			0x00000007

class COte 
{
public:

	COte(HWND hWnd);
	~COte();

	int init_proc();            //初期化処理

	static HWND hWnd_parent;				//親ウィンドウのハンドル
	static HWND hWnd_work;					//操作端末メインウィンドウハンドル
	static HWND hWnd_sub[OTE_N_SUB_WND];	//通信イベント処理用ウィンドウハンドル
	static HWND hwnd_current_subwnd;		//表示中のサブウィンドハンドル

	static ST_OTE_WORK_WND	st_work_wnd;	//パネルウィンドウ処理用構造体

	WORD helthy_cnt = 0;

	static std::wstring msg_ws;
	static std::wostringstream msg_wos;

	static CSockAlpha* pSockPcUniCastOte;		//PC->OTEユニキャスト受信用ソケット
	static CSockAlpha* pSockPcMultiCastOte;		//PC->OTEマルチキャスト受信用ソケット
	static CSockAlpha* pSockOteMultiCastOte;	//OTE→OTEマルチキャスト受信用ソケット

	static SOCKADDR_IN addrin_pc_u_ote;			//PCユニキャスト受信アドレス（OTE用)
	static SOCKADDR_IN addrin_pc_m_ote;			//PCマルチキャスト受信アドレス（OTE用)
	static SOCKADDR_IN addrin_ote_m_ote;		//OTEマルチキャスト受信アドレス（OTE用)

	static SOCKADDR_IN addrin_ote_m_snd;		//OTEマルチキャスト送信アドレス（OTE用)
	static SOCKADDR_IN addrin_ote_u_snd;		//OTEユニキャスト送信先アドレス（OTE用)
		
	//送受信カウント
	static LONG cnt_snd_ote_u, cnt_snd_ote_m;
	static LONG cnt_rcv_pc_u, cnt_rcv_pc_m, cnt_rcv_ote_m;

	static ST_OTE_U_MSG st_msg_ote_u_snd;
	static ST_OTE_M_MSG st_msg_ote_m_snd;

	static ST_PC_U_MSG st_msg_ote_u_rcv;
	static ST_PC_M_MSG st_msg_pc_m_ote_rcv;
	static ST_OTE_M_MSG st_msg_ote_m_ote_rcv;

	static HWND open_work_Wnd(HWND hwnd);
	static HWND open_connect_Wnd(HWND hwnd);
	static HWND open_mode_Wnd(HWND hwnd);
	static HWND open_fault_Wnd(HWND hwnd);
	static HWND open_moment_Wnd(HWND hwnd);
	static HWND open_auto_Wnd(HWND hwnd);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndConnectProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndAutoProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndModeProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndFaultProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndMomentProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static HRESULT close();
	static int parse();//端末表示更新処理

	static LPST_OTE_U_MSG set_msg_ote_u();
	static LPST_OTE_M_MSG set_msg_ote_m();

	static HRESULT snd_ote_u_pc(LPST_OTE_U_MSG pbuf, SOCKADDR_IN* p_addrin_to);	//OTE→PC　 ユニキャスト送信処理
	static HRESULT snd_ote_m_pc(LPST_OTE_M_MSG pbuf);							//OTE->PC	マルチキャスト送信処理
	static HRESULT snd_ote_m_ote(LPST_OTE_M_MSG pbuf);							//OTE->OTE	マルチキャスト送信処理
	
	//受信処理用
	static HRESULT rcv_pc_u_ote(LPST_PC_U_MSG pbuf);							//PC→OTE　	ユニキャスト受信処理
	static HRESULT rcv_ote_m_ote(LPST_OTE_M_MSG pbuf);							//OTE->OTE	マルチキャスト受信処理
	static HRESULT rcv_pc_m_ote(LPST_PC_M_MSG pbuf);							//PC->OTE	マルチキャスト受信処理

	static void wstr_out_inf(const std::wstring& srcw);
	static void disp_msg_cnt();
	static void disp_ip_inf();

	static void set_OTE_panel_objects(HWND hWnd);								//OTEウィンドウ上へコントロール配置
	static void draw_pb();														//オーナドローPB描画

	void activate_ote(bool is_activate_req);									//実行タイマー起動、停止

	void set_sock_addr(SOCKADDR_IN* paddr, PCSTR ip, USHORT port) {				//ソケットアドレスセット関数
		paddr->sin_family = AF_INET;
		paddr->sin_port = htons(port);
		inet_pton(AF_INET, ip, &(paddr->sin_addr.s_addr));
	}
};


