#pragma once

#include "common_def.h"

#define WND_OTE0_X						10
#define WND_OTE0_Y						600
#define WND_OTE0_W						800
#define WND_OTE0_H						600

//オブジェクトID
#define BASE_ID_OTEIF_STATIC			10700
#define ID_OTE_LABEL_MH					0
#define ID_OTE_LABEL_AH					1
#define ID_OTE_LABEL_BH					2
#define ID_OTE_LABEL_SL					3
#define ID_OTE_LABEL_GT					4
#define ID_OTE_INF_MH0					5
#define ID_OTE_INF_AH0					6
#define ID_OTE_INF_BH0					7
#define ID_OTE_INF_SL0					8
#define ID_OTE_INF_GT0					9
#define ID_OTE_INF_SWAY_MH				10
#define ID_OTE_INF_SWAY_AH				11

#define BASE_ID_OTEIF_PB				10800
#define ID_OTEIF_PB_HIJYOU				0	//非常停止PB
#define ID_OTEIF_PB_SYUKAN				1	//主幹PB
#define ID_OTEIF_PB_KIDOU				2
#define ID_OTEIF_PB_TEISHI				3
#define ID_OTEIF_PB_AUTO				4
#define ID_OTEIF_PB_FUREDOME			5
#define ID_OTEIF_PB_S1					6
#define ID_OTEIF_PB_S2					7
#define ID_OTEIF_PB_S3					8
#define ID_OTEIF_PB_N1					9
#define ID_OTEIF_PB_N2					10
#define ID_OTEIF_PB_N3					11
#define ID_OTEIF_CHK_WIDE				12
#define ID_OTEIF_CHK_ZOOM				13
#define ID_OTEIF_CHK_HOOK				14
#define ID_OTEIF_CHK_OPE				15
#define ID_OTEIF_RADIO_COM				16
#define ID_OTEIF_RADIO_MODE				17
#define ID_OTEIF_RADIO_FAULT			18
#define ID_OTEIF_RADIO_STAT				19

//ノッチID　BASE + 10*MOTION_ID + NOTCH
#define BASE_ID_OTE_NOTCH				11010
#define ID_OTE_NOTCH_MH0				11010
#define ID_OTE_NOTCH_AH0				11510		//表示更新停止
#define ID_OTE_NOTCH_BH0				11310
#define ID_OTE_NOTCH_GT0				11110
#define ID_OTE_NOTCH_SL0				11410

#define N_OTE_CTRL_TYPE					3
#define ID_OTE_CTRL_STATIC				0
#define ID_OTE_CTRL_PB					1
#define ID_OTE_CTRL_NOTCH				2

#define N_OTE_PNL_ITEMS					64
#define N_OTE_PNL_STATIC				64
#define N_OTE_PNL_NOTCH					64
#define N_OTE_PNL_PB					64

#define N_OTE_PEN						8
#define N_OTE_BRUSH						8

#define N_OTE_HBMAP						8
#define ID_OTE_HBMAP_MEM0				0
#define ID_OTE_HBMAP_MEM_IF				1

#define ID_OTE_LAMP_WHITE				0
#define ID_OTE_LAMP_GLAY				1
#define ID_OTE_LAMP_RED					2
#define ID_OTE_LAMP_GREEN				3

#define PRM_OTE_DEF_PB_W				60
#define PRM_OTE_DEF_PB_H				20

#define PRM_LAMP_CHK_CNT				20
#define PRM_LAMP_FLICK_CNT				10	//ランプON判定カウント　counter%PRM_LAMP_CHK_CNTがこの値より大でOFF

#define PRM_POINT_X_LABEL_BH			20
#define PRM_POINT_X_LABEL_MH			760
#define PRM_POINT_Y_LABEL_SL			250
#define PRM_POINT_Y_LABEL_GT			560

//操作端末ウィンドウ構造体
typedef struct _stOTEWorkWnd {
	int area_x = 0, area_y = 0, area_w = 0, area_h = 0;     //メインウィンドウ上の表示エリア
	int bmp_w = WND_OTE0_W, bmp_h = WND_OTE0_H;             //グラフィックビットマップサイズ
		int lamp_status[N_OTE_PNL_PB];						//ランプ状態

	HIMAGELIST hImgLamp0;									//釦ランプのイメージリストハンドル

	HBITMAP hBmap[N_OTE_HBMAP];								//ビットマップハンドル
	HDC		hdc_mem0 = NULL, hdc_mem_inf = NULL;			//メモリデバイスコンテキスト
	HPEN	hpen[N_OTE_PEN] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };
	HBRUSH	hbrush[N_OTE_BRUSH] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };
	HWND	hctrl[N_OTE_CTRL_TYPE][N_OTE_PNL_STATIC];
	HFONT	hfont_inftext = NULL;				                //テキスト用フォント
	BLENDFUNCTION bf = { 0,0,0,0 };					        //半透過設定構造体

	POINT pt_ctrl[N_OTE_CTRL_TYPE][N_OTE_PNL_ITEMS] = {
		//#STATIC
		//LABEL
		//ID_OTE_LABEL_MH	         ID_OTE_LABEL_AH	         ID_OTE_LABEL_BH	     ID_OTE_LABEL_SL            ID_OTE_LABEL_GT
		  750,PRM_POINT_Y_LABEL_SL,  690,PRM_POINT_Y_LABEL_SL , 20,PRM_POINT_Y_LABEL_SL,  80,PRM_POINT_Y_LABEL_SL,  80,PRM_POINT_Y_LABEL_GT,
		//INF
		//ID_OTE_INF_MH0  ID_OTE_INF_AH0 ID_OTE_INF_BH0	 ID_OTE_INF_SL0	 ID_OTE_INF_GT0	 ID_OTE_INF_SWAY_MH	 ID_OTE_INF_SWAY_AH	
		  400,300,         600,300,       100,300,          250,300,      0,0,            400,20,                550,20,                
		0,0,0,0,0,0,0,0,												//16
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//16
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//16
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//16

		//#PB
		720,0,660,0,600,0,540,0,
		720,25,660,25,600,25,540,25,
		720,50,660,50,600,50,540,50,
		720,75,660,75,600,75,540,75,
		720,100,660,100,600,100,540,100,
		720,125,660,125,600,125,540,125,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,				//16
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//16
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//16

		//#NOTCH
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//HOIST
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//GT
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//-
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//BH
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//SL
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//AH
		0,0,0,0,0,0,0,0
	};

	SIZE size_ctrl[N_OTE_CTRL_TYPE][N_OTE_PNL_STATIC] = {
		//#STATIC
		//LABEL
		//ID_OTE_LABEL_MH	ID_OTE_LABEL_AH	  ID_OTE_LABEL_BH	ID_OTE_LABEL_SL   ID_OTE_LABEL_GT
		  50,20,            50,20,            50,20,            50,20,             50,20,
		 //INF
		 //ID_OTE_INF_MH0  ID_OTE_INF_AH0 ID_OTE_INF_BH0 ID_OTE_INF_SL0	 ID_OTE_INF_GT0	 ID_OTE_INF_SWAY_MH	 ID_OTE_INF_SWAY_AH	
			100,150,         100,150,       100,150,       100,150,        100,150,            100,150,         100,150,
		  0,0,0,0,0,0,0,0,												//16
		  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//16
		  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//16
		  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//16

		  //#PB
		  PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		  PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		  PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		  PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		  PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,				//16
		  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//16
		  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//16

		  //#NOTCH
		  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//HOIST
		  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//GT
		  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//-
		  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//BH
		  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//SL
		  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//AH
		  0,0,0,0,0,0,0,0
	};

	WCHAR ctrl_text[N_OTE_CTRL_TYPE][N_OTE_PNL_STATIC][128] = {
		//STATIC
		L"MH",L"AH",L"BH",L"SL",L"GT",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
		L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
		L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
		L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
	
		//PB
		L"非常",L"主幹",L"起動",L"停止",L"自動",L"振止",L"S1",L"S2",L"S3",L"N1",L"N2",L"N3",L"WIDE",L"ZOOM",L"HOOK",L"OPE",
		L"通信",L"MODE",L"故障",L"状態",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
		L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
		L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",

		//NOTCH
		L"",L"-4",L"-3",L"-2",L"-1",L"0",L"1",L"2",L"3",L"4",//MH
		L"",L"-4",L"-3",L"-2",L"-1",L"0",L"1",L"2",L"3",L"4",//GT
		L"",L"-4",L"-3",L"-2",L"-1",L"0",L"1",L"2",L"3",L"4",
		L"",L"-4",L"-3",L"-2",L"-1",L"0",L"1",L"2",L"3",L"4",//BH
		L"",L"-4",L"-3",L"-2",L"-1",L"0",L"1",L"2",L"3",L"4",//SL
		L"",L"-4",L"-3",L"-2",L"-1",L"0",L"1",L"2",L"3",L"4",//AH
		L"",L"",L"",L""
	};

}ST_OTE_WORK_WND, * LPST_OTE_WORK_WND;


