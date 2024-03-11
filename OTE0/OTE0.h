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
#define OTE_CAMERA_PTZ0_IP "192.168.1.202"
#define OTE_CAMERA_FISH0_IP "192.168.1.201"
#define OTE_CAMERA_HOOK0_IP "192.168.1.205"
#define OTE_CAMERA_DOOM0_IP "192.168.1.204"

#define OTE_CAMERA_USER "SHIMH"
#define OTE_CAMERA_PASS "Shimh001"
#define OTE_CAMERA_FORMAT_JPEG 0
#define OTE_CAMERA_FORMAT_H264 3
#define OTE_CAMERA_FORMAT_H265 6


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
#define OTE0_WND_W				960			//OTEメインパネルWINDOW幅0
#define OTE0_WND_H				600			//OTEメインパネルWINDOW高さ0

#define OTE0_SUB_WND_X			5			//SUBパネル表示位置X
#define OTE0_SUB_WND_Y			5			//SUBパネル表示位置Y
#define OTE0_SUB_WND_W			280			//SUBパネルWINDOW幅
#define OTE0_SUB_WND_H			260			//SUBパネルWINDOW高さ

#define OTE0_SWY_WND_X			5			//振れパネル表示位置X
#define OTE0_SWY_WND_Y			5			//振れパネル表示位置Y
#define OTE0_SWY_WND_W			280			//振れパネルWINDOW幅
#define OTE0_SWY_WND_H			260			//振れパネルWINDOW高さ


#define OTE0_CAM_WND_X			0			//IP CAMERA 表示位置X
#define OTE0_CAM_WND_Y			-860		//IP CAMERA 表示位置Y
#define OTE0_CAM_WND_W			1300		//IP CAMERA WINDOW幅
#define OTE0_CAM_WND_H			750			//IP CAMERA WINDOW高さ

#define OTE0_CAM2_WND_X			OTE0_WND_X + OTE0_SUB_WND_W + 63	//IP CAMERA2 表示位置X
#define OTE0_CAM2_WND_Y			5 									//IP CAMERA 表示位置Y
#define OTE0_CAM2_WND_W			360									//IP CAMERA WINDOW幅
#define OTE0_CAM2_WND_H			270									//IP CAMERA WINDOW高さ

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

#define OTE_N_TARGET		8
#define OTE_ID_HOT_TARGET	0
#define OTE_ID_S1_TARGET	1
#define OTE_ID_S2_TARGET	2
#define OTE_ID_S3_TARGET	3
#define OTE_ID_N1_TARGET	4
#define OTE_ID_N2_TARGET	5
#define OTE_ID_N3_TARGET	6

#define OTE_ID_AUTOSTAT_OFF		0
#define OTE_ID_AUTOSTAT_STANDBY	1
#define OTE_ID_AUTOSTAT_ACTIVE	2

//メインウィンドウ管理構造体
typedef struct stOte0DataTag {
	double	pos[MOTION_ID_MAX];
	double	load[MOTION_ID_MAX];
	double	v_fb[MOTION_ID_MAX];
	double	v_ref[MOTION_ID_MAX];
	double	deg_sl;				//旋回角°
	double	deg_bh;				//起伏角°
	POINT	pt_tgpos[OTE_N_TARGET][OTE0_N_AREA_GR];
	double	d_tgpos[OTE_N_TARGET][MOTION_ID_MAX];
	INT gpad_mode = L_OFF;
	INT auto_mode = OTE_ID_AUTOSTAT_OFF;
	INT anti_sway_mode = OTE_ID_AUTOSTAT_OFF;
	INT auto_sel[MOTION_ID_MAX] = { OTE_ID_AUTOSTAT_OFF ,OTE_ID_AUTOSTAT_OFF ,OTE_ID_AUTOSTAT_OFF ,OTE_ID_AUTOSTAT_OFF ,OTE_ID_AUTOSTAT_OFF ,OTE_ID_AUTOSTAT_OFF ,OTE_ID_AUTOSTAT_OFF ,OTE_ID_AUTOSTAT_OFF };

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

	void parse_auto_status();
	void update_auto_target_touch(int area, int x, int y);
	POINT cal_gr_pos_from_d_pos(int motion, double dpos1,double dpos2);	//軸の位置からグラフィック上のポイントを求める 引込、旋回はdpos1,dpos2が必要、主補巻はdpos1のみ

	std::wstring msg_ws;
	std::wostringstream msg_wos;

	//long PlayStatus;
};

