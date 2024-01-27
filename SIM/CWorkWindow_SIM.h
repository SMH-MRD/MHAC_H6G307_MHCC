#pragma once

#include <Windows.h>
#include <commctrl.h>
#include <time.h>
#include <string>
#include "spec.h"
#include "CSharedMem.h"	    //# 共有メモリクラス


#define WORK_WND_X							858		//MAP表示位置X
#define WORK_WND_Y							600			//MAP表示位置Y
#define WORK_WND_W							530		    //MAP WINDOW幅
#define WORK_WND_H							550			//MAP WINDOW高さ

#define WORK_SCAN_TIME						100			// SWAY IF送信周期msec


//コントロールID
#define ID_WORK_WND_BASE                    10600
#define ID_WORK_WND_CLOSE_PB				10601
#define ID_STATIC_SWAY_IF_LABEL_RCV         10602
#define ID_STATIC_SWAY_IF_LABEL_SND         10603
#define ID_STATIC_SWAY_IF_VIEW_RCV          10604
#define ID_STATIC_SWAY_IF_VIEW_SND          10605

#define ID_UDP_EVENT				        10606

#define ID_CHK_PACKET_SND					10607
#define ID_STATIC_SIM_INF					10608


//起動タイマーID
#define ID_WORK_WND_TIMER					100

#define NUM_OF_CTR_OBJECT					128

class CWorkWindow
{
public:
	CWorkWindow();
	~CWorkWindow();

	std::wstring wstr;
	HINSTANCE hInst;
	static HWND hWorkWnd;

	virtual HWND open_WorkWnd(HWND hwnd_parent);
	static LRESULT CALLBACK WorkWndProc(HWND, UINT, WPARAM, LPARAM);
	static int close_WorkWnd();
	static int init_sock(HWND hwnd);
	static HWND hwndSTATMSG;
	static HWND hwndRCVMSG;
	static HWND hwndSNDMSG;
	static HWND hwndStaticInf;
	static HWND hwndChkBoxSwAct;
	static HWND hwndStaticSimInf;
	static LPST_SIMULATION_STATUS pSimStat;
	static bool is_sway_packet_send;							//振れセンサのパケットを送信する

	void set_sim_stat(LPST_SIMULATION_STATUS _p) { pSimStat = _p;  return; }

private:
	static void tweet2statusMSG(const std::wstring& srcw);
	static void tweet2rcvMSG(const std::wstring& srcw);
	static void tweet2sndMSG(const std::wstring& srcw);

	static void update_siminf();
};


