#pragma once

#include "resource.h"

//#define _TYPE_MELSECNET
#define _TYPE_MCPROTOCOL

///# ベース設定

//-タスク設定
#define TARGET_RESOLUTION			1		//マルチメディアタイマー分解能 msec
#define SYSTEM_TICK_ms				25		//メインスレッド周期 msec

//-Main Windowの配置設定
#define MAIN_WND_INIT_SIZE_W		220		//-Main Windowの初期サイズ　W
#define MAIN_WND_INIT_SIZE_H		180		//-Main Windowの初期サイズ　H
#define MAIN_WND_INIT_POS_X			1150	//-Main Windowの初期位置設定　X
#define MAIN_WND_INIT_POS_Y			20		//-Main Windowの初期位置設定　Y

//-ID定義 Mainスレッド用　2000 + 200 +α
#define ID_STATUS					2200
#define IDC_STATIC_0				2201
#define IDC_PB_EXIT					2202
#define IDC_PB_DEBUG				2203
#define IDC_CHK_IFCHK				2204
#define IDC_RADIO_SIM_M0			2205	//モニタのみ　PLC書き込み無し
#define IDC_RADIO_SIM_M1			2206	//遠隔モード　運転室操作関連のみ書き込み
#define IDC_RADIO_SIM_M2			2207	//シミュレータモード　PLC書き込みフルモード

//メインウィンドウ管理構造体
typedef struct stMainWndTag {
	HWND hWnd_status_bar = NULL;	//ステータスバーのハンドル
	HWND h_static0 = NULL;			//スタティックオブジェクトのハンドル
	HWND h_pb_exit = NULL;			//ボタンオブジェクトのハンドル

	HWND h_redio_m2 = NULL;		//ラジオボタンオブジェクトのハンドル
	HWND h_redio_m0 = NULL;		//ラジオボタンオブジェクトのハンドル
	HWND h_redio_m1 = NULL;		//ラジオボタンオブジェクトのハンドル
	HWND h_chk_if = NULL;			//チェックボックスブジェクトのハンドル

	HWND hWorkWnd = NULL;			//ワークウィンドウのハンドル
	HWND hIOWnd = NULL;				//IO Checkウィンドウのハンドル
}ST_MAIN_WND, * LPST_MAIN_WND;

//マルチメディアタイマー管理構造体
#define TARGET_RESOLUTION			1		//マルチメディアタイマー分解能 msec
#define SYSTEM_TICK_ms				25		//メインスレッド周期 msec
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