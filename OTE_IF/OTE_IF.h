#pragma once
#include "resource.h"
#include <windows.h>    // included for Windows Touch
#include <windowsx.h>   // included for point conversion

///# ベース設定

//タイマーID
#define ID_MAIN_WINDOW_UPDATE_TIMER				10702
#define ID_MAIN_WINDOW_UPDATE_TICK_ms			1000

//-Main Windowの配置設定
#define MAIN_WND_INIT_SIZE_W		220		//-Main Windowの初期サイズ　W
#define MAIN_WND_INIT_SIZE_H		180		//-Main Windowの初期サイズ　H
#define MAIN_WND_INIT_POS_X			648	//-Main Windowの初期位置設定　X
#define MAIN_WND_INIT_POS_Y			180		//-Main Windowの初期位置設定　Y

//-ID定義 Mainスレッド用　2000 + 400 +α
#define ID_STATUS					2400
#define IDC_STATIC_0				2401
#define IDC_PB_EXIT					2402
#define IDC_PB_DEBUG				2403
#define IDC_CHK_IFCHK				2404
#define IDC_CHK_OTE					2405
#define IDC_RADIO_LOCAL_OTE			2406	//モニタのみ　PLC書き込み無し
#define IDC_RADIO_REMOTE_OTE		2407	//遠隔モード　運転室操作関連のみ書き込み


#define MSG_SENSOR_SOCK				2410	//WinSockイベントメッセージ


//メインウィンドウ管理構造体
typedef struct stMainWndTag {
	HWND hWnd_status_bar = NULL;	//ステータスバーのハンドル
	HWND h_static0 = NULL;			//スタティックオブジェクトのハンドル
	HWND h_pb_exit = NULL;			//ボタンオブジェクトのハンドル
	HWND h_chk_if = NULL;			//チェックボックスオブジェクトのハンドル
	HWND h_chk_local_ote = NULL;	//チェックボックスオブジェクトのハンドル

	HWND h_static1 = NULL;			//スタティックオブジェクトのハンドル
	HWND h_pb_sel_sensor1 = NULL;	//ボタンオブジェクトのハンドル
	HWND h_pb_sel_sensor2 = NULL;	//ボタンオブジェクトのハンドル
	HWND h_pb_0set_sensor = NULL;	//ボタンオブジェクトのハンドル
	HWND h_pb_0set_tilt = NULL;		//ボタンオブジェクトのハンドル
	HWND h_pb_reset_sensor = NULL;	//ボタンオブジェクトのハンドル
	HWND h_pb_reset_tilt = NULL;	//ボタンオブジェクトのハンドル
	HWND h_pb_pc_reset = NULL;		//ボタンオブジェクトのハンドル
	HWND h_pb_img_save = NULL;		//ボタンオブジェクトのハンドル

	HWND hWorkWnd = NULL;			//ワークウィンドウのハンドル

}ST_MAIN_WND, * LPST_MAIN_WND;

//マルチメディアタイマー管理構造体
#define TARGET_RESOLUTION			1		//マルチメディアタイマー分解能 msec
#define SYSTEM_TICK_ms				100		//メインスレッド周期 msec
#define INITIAL_TASK_STACK_SIZE		16384	//タスクオブジェクトスレッド用スタックサイズ

typedef struct stKnlManageSetTag {
	WORD mmt_resolution = TARGET_RESOLUTION;			//マルチメディアタイマーの分解能
	unsigned int cycle_base = SYSTEM_TICK_ms;			//マルチメディアタイマーの分解能
	WORD KnlTick_TimerID = 0;							//マルチメディアタイマーのID
	unsigned int num_of_task = 0;						//アプリケーションで利用するスレッド数
	unsigned long sys_counter = 0;						//マルチメディア起動タイマカウンタ
	SYSTEMTIME Knl_Time = { 0,0,0,0,0,0,0,0 };			//アプリケーション開始からの経過時間
	unsigned int stackSize = INITIAL_TASK_STACK_SIZE;	//タスクの初期スタックサイズ
}ST_KNL_MANAGE_SET, * LPST_KNL_MANAGE_SET;