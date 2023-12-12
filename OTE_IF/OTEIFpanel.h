#pragma once

#include <windows.h>

//ワークウィンドウオブジェクトID
#define BASE_ID_OTEIF_STATIC				10630
#define ID_OTEIF_INF_CNT_IP0				0
#define ID_OTEIF_INF_IP_SND_FROM			1
#define ID_OTEIF_INF_HEADS					2
#define ID_OTEIF_INF_HEADR					3
#define ID_OTEIF_INF_BODYS					4
#define ID_OTEIF_INF_BODYR					5
			
#define BASE_ID_OTEIF_PB					10650
#define ID_OTEIF_RADIO_UNI					0
#define ID_OTEIF_RADIO_PCM					1
#define ID_OTEIF_RADIO_TEM					2
#define ID_OTEIF_CHK_HOLD					3


#define N_OTEIF_CTRL_TYPE					2
#define ID_OTEIF_CTRL_STATIC				0
#define ID_OTEIF_CTRL_PB					1

#define N_OTEIF_PNL_ITEMS					16
#define N_OTEIF_PNL_STATIC					16
#define N_OTEIF_PNL_PB						16
#define N_OTEIF_PEN							8
#define N_OTEIF_BRUSH						8


//操作端末ウィンドウ構造体
typedef struct _stOTEifWorkWnd {
	BOOL is_panel_disp	= false;		//ウィンドウを表示
	BOOL is_disp_msg	= false;	//メッセージを表示
	BOOL is_hold_disp	= false;	//表示更新ホールド

	UINT32 id_disp_item;

	int area_x = 0, area_y = 0, area_w = 0, area_h = 0;     //メインウィンドウ上の表示エリア
	int bmp_w = 0, bmp_h = 0;                               //グラフィックビットマップサイズ

	HBITMAP hBmap_mem0 = NULL, hBmap_inf = NULL;            //ビットマップハンドル
	HDC		hdc_mem0 = NULL,hdc_mem_inf = NULL;				//メモリデバイスコンテキスト
	HPEN	hpen[N_OTEIF_PEN]			= { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };
	HBRUSH	hbrush[N_OTEIF_BRUSH]	= { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };
	HWND	hctrl[N_OTEIF_CTRL_TYPE][N_OTEIF_PNL_STATIC];
	HFONT	hfont_inftext = NULL;				                //テキスト用フォント
	BLENDFUNCTION bf = { 0,0,0,0 };					        //半透過設定構造体
	
	POINT pt_ctrl[N_OTEIF_CTRL_TYPE][N_OTEIF_PNL_ITEMS] = {
	//#STATIC
		5,30,	//ID_OTEIF_INF_CNT_IP0	
		5,55,	//ID_OTEIF_INF_IP_SND_FROM		
		5,100,	//ID_OTEIF_INF_HEADS	 
		5,145,	//ID_OTEIF_INF_HEADR		
		5,190,	//ID_OTEIF_INF_BODYS	 
		5,275,	//ID_OTEIF_INF_BODYR
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	//#PB
		5 ,5, 65,5,125,5,450,5,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};

	SIZE size_ctrl[N_OTEIF_CTRL_TYPE][N_OTEIF_PNL_STATIC] = {
		//STATIC
		520,20, //ID_OTEIF_INF_CNT_IP0	
		520,40,	//ID_OTEIF_INF_IP_SND_FROM
		520,40, //ID_OTEIF_INF_HEADS 
		520,40, //ID_OTEIF_INF_HEADR 
		520,80, //ID_OTEIF_INF_BODYS 
		520,80,	//ID_OTEIF_INF_BODYR
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		//PB
		50,20, 50,20,50,20, 60,20,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};

	WCHAR ctrl_text[N_OTEIF_CTRL_TYPE][N_OTEIF_PNL_STATIC][128] = {
	//STATIC
	L"CNT IP0",
	L"IPS,IPFROM",
	L"HEAD(S):",
	L"HEAD(R):",
	L"BODY(S)",
	L"BODY(R)",
	L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",
	//PB
	L"UNI",L"PCM",L"TEM",L"HOLD",L"",L"",L"",L"",
	L"",L"",L"",L"",L"",L"",L"",L""
	};

}ST_OTEIF_WORK_WND, * LPST_OTEIF_WORK_WND;
