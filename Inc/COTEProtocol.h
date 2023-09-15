#pragma once

#include <Windows.h>
#include <commctrl.h>
#include <time.h>
#include <string>
#include "CSockAlpha.h"
#include <winsock2.h>
#include <ws2tcpip.h>

#include "OTE_DEF.h"

#define DEVICE_TYPE_PC					0		//デバイスタイプ制御PC
#define DEVICE_TYPE_OTE					1		//デバイスタイプ端末

#define OTE_SOCKET_TYPE_UNICAST				0//ソケットタイプ ユニキャスト
#define OTE_SOCKET_TYPE_PC_MULTICAST		1//ソケットタイプ マルチキャスト
#define OTE_SOCKET_TYPE_OTE_MULTICAST		2//ソケットタイプ マルチキャスト

#define IP_ADDR_OTE_SERVER_DEF	"127.0.0.1"
#define IP_ADDR_OTE_CLIENT_DEF	"127.254.254.254"
#define IP_ADDR_MULTI_CAST_DEF	"239.1.0.1"

//WorkWindow用定義
//ソケットイベント受信ID
#define ID_SOCK_UNICAST			OTE_IF_UNICAST_PORT
#define ID_SOCK_OTE_MULTICAST	OTE_IF_OTE_MULTICAST_PORT
#define ID_SOCK_PC_MULTICAST	OTE_IF_PC_MULTICAST_PORT

#define ID_OTE_MULTICAST_UPDATE_TIMER	10900	//TIMER ID
#define OTE_TIMER_PERIOD				3000	//TIMER周期

#define WORK_WND_INIT_SIZE_W			530		//-Main Windowの初期サイズ　W
#define WORK_WND_INIT_SIZE_H			300		//-Main Windowの初期サイズ　H
#define WORK_WND_INIT_POS_X				600		//-Main Windowの初期位置設定　X
#define WORK_WND_INIT_POS_Y				20		//-Main Windowの初期位置設定　Y

#define OTE_PRM_MEM0_POS_Y           150
#define OTE_PRM_MEM0_POS_H           150
#define OTE_PRM_INFMEM_POS_Y         300
#define OTE_PRM_INFMEM_POS_H         300

#define N_CREATE_PEN                8
#define N_CREATE_BRUSH              8
#define CMON_RED_PEN                0
#define CMON_BLUE_PEN               1
#define CMON_GREEN_PEN              2
#define CMON_GLAY_PEN               3
#define CMON_YELLOW_PEN             4
#define CMON_MAZENDA_PEN            5 
#define CMON_MAZENDA_PEN2           6

#define CMON_RED_BRUSH              0
#define CMON_BLUE_BRUSH             1
#define CMON_GREEN_BRUSH            2
#define CMON_BG_BRUSH               3

#define IDC_OTE_STATIC_SND_CNT_U		30100
#define IDC_OTE_STATIC_RCV_CNT_U		30101
#define IDC_OTE_STATIC_RCV_SUB_CNT_U	30102
#define IDC_OTE_STATIC_SND_CNT_M		30103
#define IDC_OTE_STATIC_RCV_CNT_M		30104
#define IDC_OTE_STATIC_SOCK_INF			30105

#define IDC_OTE_STATIC_SND_U			30106
#define IDC_OTE_STATIC_RCV_U			30107
#define IDC_OTE_STATIC_RCV_SUB_U		30108
#define IDC_OTE_STATIC_SND_M			30109
#define IDC_OTE_STATIC_RCV_M			30110

#define IDC_CHK_DISP_SOCK				30120
#define IDC_CHK_IS_SLOW_MODE			30121
#define IDC_CHK_INF						30122
#define IDC_CHK_MSG						30123


//通信用ウィンドウ構造体
typedef struct _stOTEifwnd {
	HWND hWnd_parent;
	HWND hWnd;
	HINSTANCE hInst;

	LONG count_snd_m = 0, count_snd_u = 0, count_rcv_m = 0, count_rcv_u = 0, count_rcv_sub_u = 0;
	UINT32 cnt_res_w = 0, cnt_res_r = 0;

	int disp_item = 0;                                                      //表示項目
	int area_x = 0, area_y = 0, area_w = 0, area_h = 0;                     //メインウィンドウ上の表示エリア
	int inf_area_x = 0, inf_area_y = 0, inf_area_w = 0, inf_area_h = 0;     //メインウィンドウ上の表示エリア
	int bmp_w = 0, bmp_h = 0;                               //グラフィックビットマップサイズ

	HBITMAP hBmap_mem0 = NULL;
	HBITMAP hBmap_inf = NULL;
	HDC hdc_mem0 = NULL;						            //合成画面メモリデバイスコンテキスト
	HDC hdc_mem_inf = NULL;					                //文字画面メモリデバイスコンテキスト

	HFONT hfont_inftext = NULL;				                //テキスト用フォント
	BLENDFUNCTION bf = { 0,0,0,0 };					        //半透過設定構造体

	HPEN hpen[N_CREATE_PEN] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };
	HBRUSH hbrush[N_CREATE_BRUSH] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };

	HWND h_static_snd_msg_u, h_static_snd_msg_m, h_static_rcv_msg_u, h_static_rcv_sub_msg_u, h_static_rcv_msg_m, h_socket_inf;
	HWND h_static_snd_cnt_u, h_static_snd_cnt_m, h_static_rcv_cnt_u, h_static_rcv_sub_cnt_u, h_static_rcv_cnt_m;
	HWND h_static_res_w, h_static_res_r;
	HWND h_pb_read, h_pb_write;;
	HWND h_chkSlow, h_chkSockinf, h_chk_inf, h_chk_msg;

	bool is_slowmode = false, disp_sock_info = false, disp_msg = false, disp_infomation = true;
	wostringstream	wos;
	ostringstream	os;
	wstring			wstr;
	string			str;

}ST_OTE_IF_WND, * LPST_OTE_IF_WND;


class COTEProtocol
{
public:
	COTEProtocol();		
	~COTEProtocol();	
	
	static int device_type;				//PC/OTE種別フラグ
	static int socket_type;				//ソケット種別フラグ

	static SOCKADDR_IN addrin;			//受信用ソケットアドレス
	static SOCKADDR_IN addrfrom;		//送信元ソケットアドレス
	static SOCKADDR_IN addrin_to;		//送信先アドレス

	static SOCKADDR_IN addr_active_ote;				//操作信号が有効な端末のアドレス
	static ST_OTE_U_MSG st_ote_active_msg;			//操作信号が有効な現メッセージ

	static UINT32 ote_com_status;		//OTE操作信号有無効状態フラグ

	static std::wostringstream msg_wos;

	static CSockAlpha* pOTESock;
	static ST_OTE_IF_WND st_work_wnd;
	static HWND open_work_Wnd(HWND hwnd);
	
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static void wstr_out_inf(const std::wstring& srcw);

	HWND Initialize(HWND hwnd, SOCKADDR_IN* paddrin, SOCKADDR_IN* paddrin_to,int sock_type, int device);
	HRESULT close();

	void set_buf_pc_m_snd(LPST_PC_M_MSG pbuf) { pbuf_pc_m_snd = pbuf; return; };
	void set_buf_pc_m_rcv(LPST_PC_M_MSG pbuf) { pbuf_pc_m_rcv = pbuf; return; };
	void set_buf_ote_m_snd(LPST_OTE_M_MSG pbuf) { pbuf_ote_m_snd = pbuf; return; };
	void set_buf_ote_m_rcv(LPST_OTE_M_MSG pbuf) { pbuf_ote_m_rcv = pbuf; return; };

	void set_buf_pc_u_snd(LPST_PC_U_MSG pbuf) { pbuf_pc_u_snd = pbuf; return; };
	void set_buf_pc_u_rcv(LPST_PC_U_MSG pbuf) { pbuf_pc_u_rcv = pbuf; return; };
	void set_buf_pc_u_rcv_sub(LPST_OTE_U_MSG pbuf) { pbuf_ote_u_rcv_sub = pbuf; return; };
	void set_buf_ote_u_snd(LPST_OTE_U_MSG pbuf) { pbuf_ote_u_snd = pbuf; return; };
	void set_buf_ote_u_rcv(LPST_OTE_U_MSG pbuf) { pbuf_ote_u_rcv = pbuf; return; };

private:
	static HRESULT snd_pc_u(LPST_PC_U_MSG pbuf,SOCKADDR_IN *pto_addrinc);	//PCユニキャスト送信処理
	static HRESULT snd_pc_m(LPST_PC_M_MSG pbuf);							//PCマルチキャスト送信処理
	static HRESULT snd_ote_u(LPST_OTE_U_MSG pbuf, SOCKADDR_IN *pto_addrin);	//OTEユニキャスト送信処理
	static HRESULT snd_ote_m(LPST_OTE_M_MSG pbuf);							//OTEマルチキャスト送信処理
	
	//受信処理用
	static HRESULT rcv_pc_m(LPST_PC_M_MSG pbuf);						//PCマルチキャスト電文受信処理
	static HRESULT rcv_pc_u(LPST_PC_U_MSG pbuf);						//PCユニキャスト電文受信処理
	static HRESULT rcv_ote_m(LPST_OTE_M_MSG pbuf);						//OTEマルチキャスト電文受信処理
	static HRESULT rcv_ote_u(LPST_OTE_U_MSG pbuf);						//OTEユニキャスト電文受信処理

	static bool is_umsg_res_completed;
	static LPST_PC_M_MSG	pbuf_pc_m_snd,	pbuf_pc_m_rcv;
	static LPST_OTE_U_MSG	pbuf_ote_u_snd,	pbuf_ote_u_rcv, pbuf_ote_u_rcv_sub;
	static LPST_OTE_M_MSG	pbuf_ote_m_snd,	pbuf_ote_m_rcv;
	static LPST_PC_U_MSG	pbuf_pc_u_snd,	pbuf_pc_u_rcv;

};