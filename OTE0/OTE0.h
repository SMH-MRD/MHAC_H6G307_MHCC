#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "resource.h"
#include <windows.h>
//#include <ws2tcpip.h>
#include "COMMON_DEF.H"
#include <commctrl.h>
#include <time.h>
#include <string>
#include "CSockAlpha.h"
#include "OTE_DEF.h"
#include "OTE0panel.H"

#include "CPsaMain.H"

//カメラ
#define OTE_CAMERA_PTZ0_IP "192.168.1.91"
#define OTE_CAMERA_FISH0_IP "192.168.1.92"
#define OTE_CAMERA_HOOK0_IP "192.168.1.93"
#define OTE_CAMERA_DOOM0_IP "192.168.1.94"

#define OTE_CAMERA_USER "SHI"
#define OTE_CAMERA_PASS "Shimh001"
#define OTE_CAMERA_FORMAT_JPEG 0
#define OTE_CAMERA_FORMAT_H264 3


#define OTE0_N_IP_CAMERA					8//接続するIPカメラの台数
#define OTE_CAMERA_ID_PTZ0					0//運転室PTZカメラ
#define OTE_CAMERA_ID_FISH0					1//運転室全方位カメラ
#define OTE_CAMERA_ID_DOOM0					2//接続するIPカメラの台数
#define OTE_CAMERA_ID_HOOK0					3//接続するIPカメラの台数
#define OTE_CAMERA_ID_NA					-1//接続するIPカメラの台数

#define OTE_CAMERA_WND_ID_BASE				0//カメラ表示BASEウィンドウ
#define OTE_CAMERA_WND_ID_OPT1				1//カメラ表示オプション１ウィンドウ
#define OTE_CAMERA_WND_ID_OPT2				2//カメラ表示オプション１ウィンドウ
#define OTE_CAMERA_WND_ID_OPT3				3//カメラ表示オプション１ウィンドウ
#define OTE_CAMERA_WND_ID_OPT4				4//カメラ表示オプション１ウィンドウ
#define OTE_CAMERA_WND_ID_OPT5				5//カメラ表示オプション１ウィンドウ
#define OTE_CAMERA_WND_ID_OPT6				6//カメラ表示オプション１ウィンドウ
#define OTE_CAMERA_WND_ID_OPT7				7//カメラ表示オプション１ウィンドウ

//タイマー
#define ID_OTE_MULTICAST_TIMER				199
#define ID_OTE_UNICAST_TIMER				198
#define ID_OTE_CAMERA_TIMER					197
#define OTE_MULTICAST_SCAN_MS				1000	    // マルチキャスト IF送信周期
#define OTE_UNICAST_SCAN_MS					100			// ユニキャスト IF送信周期　UI更新周期
#define OTE_CAMERA_SCAN_MS					100			// カメラ指令更新用タイマ

//ソケットイベントID
#define ID_SOCK_EVENT_PC_UNI_OTE 		    10653		//OTE受信ソケットイベント　PC UNICASTメッセージ
#define ID_SOCK_EVENT_PC_MULTI_OTE 		    10654		//OTE受信ソケットイベント　PC MULTICASTメッセージ
#define ID_SOCK_EVENT_OTE_MULTI_OTE 		10655		//OTE受信ソケットイベント　PC MULTICASTメッセージ

//イベントID
#define ID_OTE0_STATIC_UPDATE				10656

#define OTE0_WND_X				0			//OTEメインパネル表示位置X
#define OTE0_WND_Y				560			//OTEメインパネル表示位置Y0
#define OTE0_WND_W				850		//OTEメインパネルWINDOW幅0
#define OTE0_WND_H				600			//OTEメインパネルWINDOW高さ0

#define OTE0_SUB_WND_X			5			//SUBパネル表示位置X
#define OTE0_SUB_WND_Y			5			//SUBパネル表示位置Y
#define OTE0_SUB_WND_W			260			//SUBパネルWINDOW幅
#define OTE0_SUB_WND_H			240			//SUBパネルWINDOW高さ

#define OTE0_SWY_WND_X			270			//振れパネル表示位置X
#define OTE0_SWY_WND_Y			5			//振れパネル表示位置Y
#define OTE0_SWY_WND_W			280			//振れパネルWINDOW幅
#define OTE0_SWY_WND_H			240			//振れパネルWINDOW高さ

#define OTE0_CAM_WND_X			850			//IP CAMERA 表示位置X
#define OTE0_CAM_WND_Y			600			//IP CAMERA 表示位置Y
#define OTE0_CAM_WND_W			700			//IP CAMERA WINDOW幅
#define OTE0_CAM_WND_H			500			//IP CAMERA WINDOW高さ

#define OTE0_CAM2_WND_X			OTE0_CAM_WND_X + OTE0_CAM_WND_W + 10	//IP CAMERA2 表示位置X
#define OTE0_CAM2_WND_Y			OTE0_CAM_WND_Y							//IP CAMERA 表示位置Y
#define OTE0_CAM2_WND_W			OTE0_SWY_WND_W							//IP CAMERA WINDOW幅
#define OTE0_CAM2_WND_H			OTE0_SWY_WND_H							//IP CAMERA WINDOW高さ

#define OTE0_CAM_WND_TG_X		300			//画面切り取りX位置
#define OTE0_CAM_WND_TG_Y		300			//画面切り取りY位置

#define OTE0_MSG_SWICH_CAMERA   WM_USER + 1000

#define OTE0_N_SUB_WND			6
#define ID_OTE0_SUB_WND_CONNECT	0
#define ID_OTE0_SUB_WND_MODE	1
#define ID_OTE0_SUB_WND_AUTO	2
#define ID_OTE0_SUB_WND_FAULT	3
#define ID_OTE0_SUB_WND_MOMENT	4
#define ID_OTE0_SWY_WND	        5

#define OTE_CODE_FLICKER_FREQ		0x0040
#define OTE_CODE_SUB_STATIC_FREQ	0x0001

//メインウィンドウ管理構造体
typedef struct stOte0DataTag {
	double pos[MOTION_ID_MAX];
	double load[MOTION_ID_MAX];
	double v_fb[MOTION_ID_MAX];
	double v_ref[MOTION_ID_MAX];
	double deg_sl;//旋回角°
	double deg_bh;//起伏角°
}ST_OTE0_DATA, * LPST_OTE0_DATA;

//カメラ映像処理用構造体
typedef struct stIPCamSet {
	HWND hwnd = NULL; 
	int icam = OTE_CAMERA_ID_NA;
	CPsaMain* pPSA = NULL;	//PSApi処理用オブジェクト
}ST_IPCAM_SET, * LPST_IPCAM_SET;

class COte
{
public:

	COte(HWND hWnd);
	~COte();

	int init_proc(LPST_OTE_WORK_WND pst);            //初期化処理

	HWND hWnd_parent;
	HWND hwnd_current_subwnd;		//表示中のサブウィンドハンドル

	LPST_OTE_WORK_WND	pst_work_wnd;	//パネルウィンドウ処理用構造体

	WORD helthy_cnt = 0;

	ST_OTE0_DATA data;

	CSockAlpha* pSockPcUniCastOte;		//PC->OTEユニキャスト受信用ソケット
	CSockAlpha* pSockPcMultiCastOte;	//PC->OTEマルチキャスト受信用ソケット
	CSockAlpha* pSockOteMultiCastOte;	//OTE→OTEマルチキャスト受信用ソケット

	SOCKADDR_IN addrin_pc_u_ote;		//PCユニキャスト受信アドレス（OTE用)
	SOCKADDR_IN addrin_pc_m_ote;		//PCマルチキャスト受信アドレス（OTE用)
	SOCKADDR_IN addrin_ote_m_ote;		//OTEマルチキャスト受信アドレス（OTE用)

	SOCKADDR_IN addrin_ote_m_ote_snd;	//OTEマルチキャスト送信アドレス（OTE用)
	SOCKADDR_IN addrin_ote_m_pc_snd;	//OTEマルチキャスト送信アドレス（PC用)
	SOCKADDR_IN addrin_ote_u_snd;		//OTEユニキャスト送信先アドレス（OTE用)

	SOCKADDR_IN addrin_pc_u_from;	//OTEユニキャスト送信元アドレス（OTE用)
	SOCKADDR_IN addrin_pc_m_from;	//PCマルチキャスト送信元アドレス（OTE用)
	SOCKADDR_IN addrin_ote_m_from;	//OTEマルチキャスト送信元アドレス（OTE用)

	SOCKADDR_IN addrin_pc_m_rcv;	//OTEマルチキャスト受信アドレス
	SOCKADDR_IN addrin_ote_m_rcv;	//PCマルチキャスト受信アドレス

	//送受信カウント
	LONG cnt_snd_ote_u, cnt_snd_ote_m_ote, cnt_snd_ote_m_pc;
	LONG cnt_rcv_pc_u, cnt_rcv_pc_m, cnt_rcv_ote_m;

	ST_OTE_U_MSG st_msg_ote_u_snd;
	ST_OTE_M_MSG st_msg_ote_m_snd;

	ST_PC_U_MSG st_msg_pc_u_rcv;
	ST_PC_M_MSG st_msg_pc_m_ote_rcv;
	ST_OTE_M_MSG st_msg_ote_m_ote_rcv;

	HRESULT close();
	int parse();//受信データ展開処理

	LPST_OTE_U_MSG set_msg_ote_u();
	LPST_OTE_M_MSG set_msg_ote_m();

	HRESULT snd_ote_u_pc(LPST_OTE_U_MSG pbuf, SOCKADDR_IN* p_addrin_to);	//OTE→PC　 ユニキャスト送信処理
	HRESULT snd_ote_m_pc(LPST_OTE_M_MSG pbuf);								//OTE->PC	マルチキャスト送信処理
	HRESULT snd_ote_m_ote(LPST_OTE_M_MSG pbuf);								//OTE->OTE	マルチキャスト送信処理

	//受信処理用
	HRESULT rcv_pc_u_ote(LPST_PC_U_MSG pbuf);								//PC→OTE　	ユニキャスト受信処理
	HRESULT rcv_ote_m_ote(LPST_OTE_M_MSG pbuf);								//OTE->OTE	マルチキャスト受信処理
	HRESULT rcv_pc_m_ote(LPST_PC_M_MSG pbuf);								//PC->OTE	マルチキャスト受信処理

	void wstr_out_inf(const std::wstring& srcw);
	void set_sock_addr(SOCKADDR_IN* paddr, PCSTR ip, USHORT port) {				//ソケットアドレスセット関数
		paddr->sin_family = AF_INET;
		paddr->sin_port = htons(port);
		inet_pton(AF_INET, ip, &(paddr->sin_addr.s_addr));
	};

	std::wstring msg_ws;
	std::wostringstream msg_wos;

	long PlayStatus;
};

