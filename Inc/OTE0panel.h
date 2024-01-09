#pragma once

#include "common_def.h"

#define N_OTE_PEN				8
#define N_OTE_BRUSH				8

#define OTE0_GLAY               0
#define OTE0_WHITE              1
#define OTE0_RED                2
#define OTE0_BLUE               3
#define OTE0_GREEN              4
#define OTE0_YELLOW             5
#define OTE0_MAZENDA            6 
#define OTE0_ORANGE				7

//オブジェクトID
#define BASE_ID_OTE_STATIC			10700
//STATIC ID 0-31は、PBランプ用予約領域
#define ID_OTE_LABEL_MH				32
#define ID_OTE_LABEL_AH				33
#define ID_OTE_LABEL_BH				34
#define ID_OTE_LABEL_SL				35
#define ID_OTE_LABEL_GT				36
#define ID_OTE_INF_MH0				37
#define ID_OTE_INF_AH0				38
#define ID_OTE_INF_BH0				39
#define ID_OTE_INF_SL0				40
#define ID_OTE_INF_GT0				41
#define ID_OTE_INF_SWAY_MH			42
#define ID_OTE_INF_SWAY_AH			43
#define ID_OTE_SUB_CONNECT_ADDR		44
#define ID_OTE_SUB_CONNECT_HEADS	45
#define ID_OTE_SUB_CONNECT_HEADR	46
#define ID_OTE_SUB_FAULT_LIST		47
#define ID_OTE_SUB_STAT_MH			48
#define ID_OTE_SUB_STAT_AH			49
#define ID_OTE_SUB_STAT_BH			50
#define ID_OTE_SUB_STAT_SL			51
#define ID_OTE_SUB_STAT_GT			52


#define BASE_ID_OTE_PB		10800
#define ID_OTE_PB_TEISHI		0
#define ID_OTE_PB_KIDOU			1
#define ID_OTE_PB_SYUKAN		2	//主幹PB
#define ID_OTE_PB_HIJYOU		3	//非常停止PB
#define ID_OTE_PB_AUTO			4
#define ID_OTE_PB_FUREDOME		5
#define ID_OTE_CHK_S1			6
#define ID_OTE_CHK_S2			7
#define ID_OTE_CHK_S3			8
#define ID_OTE_CHK_N1			9
#define ID_OTE_CHK_N2			10
#define ID_OTE_CHK_N3			11
#define ID_OTE_RADIO_WIDE		12
#define ID_OTE_RADIO_ZOOM		13
#define ID_OTE_RADIO_HOOK		14
#define ID_OTE_RADIO_OPE1		15
#define ID_OTE_RADIO_OPE2		16
#define ID_OTE_RADIO_COM		17
#define ID_OTE_RADIO_MODE		18
#define ID_OTE_RADIO_FAULT		19
#define ID_OTE_RADIO_STAT		20
#define ID_OTE_CHK_TOUCH		21
#define ID_OTE_RADIO_SOCK_PU	22
#define ID_OTE_RADIO_SOCK_PM	23
#define ID_OTE_RADIO_SOCK_OM	24
#define ID_OTE_RADIO_MHSPD_7	25
#define ID_OTE_RADIO_MHSPD_14	26
#define ID_OTE_RADIO_MHSPD_21	27
#define ID_OTE_RADIO_AHSPD_14	28
#define ID_OTE_RADIO_AHSPD_24	29
#define ID_OTE_RADIO_BH_57		30
#define ID_OTE_RADIO_BH_62		31
#define ID_OTE_RADIO_BH_REST	32
#define ID_OTE_PB_GT_SHOCK		33
#define ID_OTE_RADIO_JIB_NARROW	34
#define ID_OTE_RADIO_JIB_WIDE	35
#define ID_OTE_PB_LOAD_SWY		36
#define ID_OTE_RADIO_FLT_ALL	37
#define ID_OTE_RADIO_FLT_HEAVY	38
#define ID_OTE_RADIO_FLT_LIGHT	39
#define ID_OTE_RADIO_FLT_WARN	40
#define ID_OTE_PB_FLT_RESET		41
#define ID_OTE_CHK_IL_BYPASS	42
#define ID_OTE_PB_ARESET_ALL	43
#define ID_OTE_CHK_ASET_MH		44
#define ID_OTE_CHK_ASET_AH		45
#define ID_OTE_CHK_ASET_BH		46
#define ID_OTE_CHK_ASET_SL		47
#define ID_OTE_CHK_CAMERA_WND	48		
#define ID_OTE_CHK_CAMERA_LIVE	49		//IP　CAMERA　WINDOW
#define ID_OTE_CHK_CAMERA_TILU	50		//IP　CAMERA　WINDOW
#define ID_OTE_CHK_CAMERA_TILD	51		//IP　CAMERA　WINDOW
#define ID_OTE_CHK_CAMERA_PANL	52		//IP　CAMERA　WINDOW
#define ID_OTE_CHK_CAMERA_PANR	53		//IP　CAMERA　WINDOW
#define ID_OTE_CHK_CAMERA_ZOMW	54		//IP　CAMERA　WINDOW
#define ID_OTE_CHK_CAMERA_ZOMN	55		//IP　CAMERA　WINDOW
#define ID_OTE_PB_CAMERA_STOP	56		//IP　CAMERA　WINDOW



//ノッチID　BASE + 10*MOTION_ID + NOTCH + ID_OTE_0NOTCH_POS
#define BASE_ID_OTE_NOTCH		10900
#define ID_OTE_NOTCH_MH_MIN		10900
#define ID_OTE_NOTCH_MH_MAX		10909
#define ID_OTE_NOTCH_GT_MIN		10910
#define ID_OTE_NOTCH_GT_MAX		10919
#define ID_OTE_NOTCH_GR_MIN		10920
#define ID_OTE_NOTCH_GR_MAX		10929
#define ID_OTE_NOTCH_BH_MIN		10930
#define ID_OTE_NOTCH_BH_MAX		10939
#define ID_OTE_NOTCH_SL_MIN		10940
#define ID_OTE_NOTCH_SL_MAX		10949
#define ID_OTE_NOTCH_AH_MIN		10950
#define ID_OTE_NOTCH_AH_MAX		10959


#define ID_OTE_GRIP_SWITCH		2			
#define ID_OTE_GRIP_ESTOP_POS	0x00000001
#define ID_OTE_GRIP_NOTCH_POS	0x00000002
#define ID_OTE_GRIP_RMT_POS		0x00000004

#define N_OTE_NOTCH_ARRAY		10			//ノッチ割り当て配列数
#define N_OTE_NOTCH_MAX			4
#define ID_OTE_0NOTCH_POS		4

//グリップノッチ
#define ID_OTE_GRIP_ESTOP		20			//グリップ停止スイッチ
#define ID_OTE_GRIP_NOTCH		21			//グリップスイッチ
#define ID_OTE_GRIP_RMT			22			//リモート有効
#define ID_OTE_GRIP_PP			23			//カメラパン＋
#define ID_OTE_GRIP_PN			24			//カメラパン−
#define ID_OTE_GRIP_TP			25			//カメラチルト＋
#define ID_OTE_GRIP_TN			26			//カメラチルト−
#define ID_OTE_GRIP_ZP			27			//カメラズーム＋
#define ID_OTE_GRIP_ZN			28			//カメラズーム−

#define N_OTE_CTRL_TYPE			3
#define ID_OTE_CTRL_STATIC		0
#define ID_OTE_CTRL_PB			1
#define ID_OTE_CTRL_NOTCH		2



#define N_OTE_FONT				8
#define ID_OTE_FONT8			0
#define ID_OTE_FONT6			1
#define ID_OTE_FONT12			2
#define ID_OTE_FONT10			3
#define ID_OTE_FONT20			4

#define N_OTE_HBMAP				8
#define ID_OTE_HBMAP_MEM0		0
#define ID_OTE_HBMAP_MEM_IF		1
#define ID_OTE_HBMAP_MEM_GR		2
#define ID_OTE_HBMAP_SWY_MEM0	3
#define ID_OTE_HBMAP_SWY_MEM_IF	4
#define ID_OTE_HBMAP_SWY_MEM_GR	5

#define N_OTE_HDC				8
#define ID_OTE_HDC_MEM0			0
#define ID_OTE_HDC_MEM_IF		1
#define ID_OTE_HDC_MEM_GR		2
#define ID_OTE_HDC_SWY_MEM0		3
#define ID_OTE_HDC_SWY_MEM_IF	4
#define ID_OTE_HDC_SWY_MEM_GR	5
#define ID_OTE_HDC_CAMERA_VIEW	6

#define PRM_OTE_DEF_PB_W		50
#define PRM_OTE_DEF_PB_W2		30
#define PRM_OTE_DEF_PB_H		20
#define PRM_OTE_DEF_LAMP_W		15
#define PRM_OTE_DEF_LAMP_H		20


#define PRM_POINT_X_LABEL_BH	20
#define PRM_POINT_X_LABEL_MH	760
#define PRM_POINT_Y_LABEL_SL	250
#define PRM_POINT_Y_LABEL_BH	290
#define PRM_POINT_Y_LABEL_GT	540
#define PRM_POINT_Y_NOTCH_SL	250

#define OTE0_PB_OFF_DELAY_COUNT	10

#define OTE0_GR_AREA_X			120
#define OTE0_GR_AREA_Y			280
#define OTE0_GR_AREA_W			320
#define OTE0_GR_AREA_H			250

#define OTE0_GR_AREA2_X			460
#define OTE0_GR_AREA2_Y			280
#define OTE0_GR_AREA2_W			320
#define OTE0_GR_AREA2_H			250

#define OTE0_IF_AREA_X			560
#define OTE0_IF_AREA_Y			180
#define OTE0_IF_AREA_W			270
#define OTE0_IF_AREA_H			60


//操作端末ウィンドウ構造体
typedef struct _stOTEWorkWnd {
	int area_x = 0, area_y = 0, area_w = 0, area_h = 0; //メインウィンドウ上の表示エリア(デバイスコンテキスト用）
	int bmp_w, bmp_h;									//グラフィックビットマップサイズ
	ST_OTE_LAMP_COM pb_lamp[N_OTE_PNL_PB];		//ランプ状態　　
	ST_OTE_LAMP_COM notch_lamp[N_OTE_PNL_PB];	//ランプ状態　
	UINT16 pb_stat[N_OTE_PNL_PB];						//PB状態 OFFディレイカウント
	UINT16 notch_pb_stat[N_OTE_PNL_PB];					//PB状態 OFFディレイカウント
	INT16  notch_pos[3][MOTION_ID_MAX] = { { 0,0,0,0,0,0,0,0 }, { 0,0,0,0,0,0,0,0 } ,{ 0,0,0,0,0,0,0,0 } };//HOLD,TRIG,COUNT

	UINT32 connect_wnd_item = ID_OTE_RADIO_SOCK_PU;
	UINT32 mh_spd_mode = ID_OTE_RADIO_MHSPD_7;
	UINT32 ah_spd_mode = ID_OTE_RADIO_AHSPD_14;
	UINT32 bh_work_mode = ID_OTE_RADIO_BH_57;
	UINT32 jib_chk_mode = ID_OTE_RADIO_JIB_NARROW;
	UINT32 flt_disp_mode = ID_OTE_RADIO_FLT_ALL;
	INT32  camera_sel = ID_OTE_RADIO_WIDE;
	INT32  subpanel_sel = ID_OTE_RADIO_COM;

	HBITMAP hBmap[N_OTE_HBMAP];								//ビットマップハンドル
	HDC		hdc[N_OTE_HDC];			//メモリデバイスコンテキスト
	HPEN	hpen[N_OTE_PEN] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };
	HBRUSH	hbrush[N_OTE_BRUSH] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };
	HWND	hctrl[N_OTE_CTRL_TYPE][N_OTE_PNL_STATIC];
	HFONT	hfont[N_OTE_FONT];				            //テキスト用フォント
	BLENDFUNCTION bf = { 0,0,0,0 };					        //半透過設定構造体

	POINT pt_ctrl[N_OTE_CTRL_TYPE][N_OTE_PNL_ITEMS] = {
		//#STATIC
		//LAMP
		  //メインパネル
		  0,0,0,0,0,0,0,0,
		  0,0,0,0,0,0,0,0,
		  0,0,0,0,0,0,0,0,
		  0,0,0,0,0,0,0,0,
		  0,0,0,0,0,0,0,0,
		  0,0,0,0,0,0,0,0,
		  0,0,0,0,0,0,0,0,
		  0,0,0,0,0,0,0,0,//0-31
		//LABEL
		//ID_OTE_LABEL_MH	         ID_OTE_LABEL_AH	         ID_OTE_LABEL_BH	     ID_OTE_LABEL_SL            ID_OTE_LABEL_GT
		  5,PRM_POINT_Y_LABEL_BH,  65,PRM_POINT_Y_LABEL_BH , 790,PRM_POINT_Y_LABEL_BH,  265,PRM_POINT_Y_LABEL_SL,  265,PRM_POINT_Y_LABEL_GT,
		 //INF
		 //ID_OTE_INF_MH0  ID_OTE_INF_AH0 ID_OTE_INF_BH0 ID_OTE_INF_SL0	 ID_OTE_INF_GT0	 ID_OTE_INF_SWAY_MH	 ID_OTE_INF_SWAY_AH	
		  0,0,				0,0,			0,0,          0,0,				0,0,            0,0,                0,0,
		
		//ID_OTE_SUB_CONNECT_ADDR	ID_OTE_SUB_CONNECT_HEADS	 ID_OTE_SUB_CONNECT_HEADR	 
		  5,30,                        5,75,						5,140,
		 //ID_OTE_SUB_FAULT_LIST
		  0,30,
		 //ID_OTE_SUB_STAT_MH	ID_OTE_SUB_STAT_AH		ID_OTE_SUB_STAT_BH		ID_OTE_SUB_STAT_SL		ID_OTE_SUB_STAT_GT		
		  45,0,				120,0,					45,120,					120,120,					195,120,
		  
		  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//16

		  //#PB
		  //メインパネル
		  573,5,	642,5,	712,5,	780,5,
		  573,30,	642,30,	712,30,	780,30,
		  573,55,	642,55,	712,55,	780,55,
		  //CAMERA SEL
		  570,80,	620,80,	670,80,	720,80,	770,80,
		  //SUB PANEL
		  15,250,70,250,125,250,180,250,
		  //TOUCH PB
		  220,5,
		  //CONNECTパネル　
		  //ID_OTE_RADIO_SOCK_PU	ID_OTE_RADIO_SOCK_PM ID_OTE_RADIO_SOCK_OM
		  5,5,						60,5,				115,5,
		  //MODEパネル
	      // ID_OTE_RADIO_MHSPD_7	 ID_OTE_RADIO_MHSPD_14	 ID_OTE_RADIO_MHSPD_21	
	      25,60,					25,85,						25,110,
	      // ID_OTE_RADIO_AHSPD_14 ID_OTE_RADIO_AHSPD_24	
	      90,60,					90,85,
	      // ID_OTE_RADIO_BH_57 ID_OTE_RADIO_BH_62 ID_OTE_RADIO_BH_REST	
	      155,60,155,85,155,115,
	      // ID_OTE_PB_GT_SHOCK ID_OTE_RADIO_JIB_NARROW ID_OTE_RADIO_JIB_WIDE
	      25,180,				90,180,					90,205,
		  // ID_OTE_PB_LOAD_SWY
		  155,180,
	      // ID_OTE_RADIO_FLT_ALL	ID_OTE_RADIO_FLT_HEAVY	ID_OTE_RADIO_FLT_LIGHT	ID_OTE_RADIO_FLT_WARN	
	      10,5,60,5,110,5,160,5,
	      // ID_OTE_PB_FLT_RESET	 ID_OTE_CHK_IL_BYPASS	
		  120,212,					190,212,

		  //ID_OTE_PB_ARESET_ALL	//ID_OTE_CHK_ASET_MH	//ID_OTE_CHK_ASET_AH	//ID_OTE_CHK_ASET_BH  //ID_OTE_CHK_ASET_SL	
		  555,130,						615,130,					675,130,			730,130,				785,130,
		  //ID_OTE_CHK_CAMERA_WND
		  720,535,
		  //CAMERA　WINDOW
		  //ID_OTE_CHK_CAMERA_LIVE	  ID_OTE_CHK_CAMERA_TILU ID_OTE_CHK_CAMERA_TILD  ID_OTE_CHK_CAMERA_PANL	  ID_OTE_CHK_CAMERA_PANR  ID_OTE_CHK_CAMERA_ZOMW  ID_OTE_CHK_CAMERA_ZOMN
		  10,480,						645,10,					645,30,					645,55,					645,80,					645,105,					645,125,
		  //ID_OTE_CHK_CAMERA_STOP
		  645,150,
		  
		  0,0,0,0,0,0,0,0,0,0,0,0,0,0,//16

		  //#NOTCH
		  //MH
		  15,510,15,485,15,460,15,435,15,410,15,385,15,360,15,335,15,310,0,0,
		  //GT
		  310,PRM_POINT_Y_LABEL_GT,350,PRM_POINT_Y_LABEL_GT,390,PRM_POINT_Y_LABEL_GT,430,PRM_POINT_Y_LABEL_GT,470,PRM_POINT_Y_LABEL_GT,510,PRM_POINT_Y_LABEL_GT,550,PRM_POINT_Y_LABEL_GT,590,PRM_POINT_Y_LABEL_GT,630,PRM_POINT_Y_LABEL_GT,0,0,//GT
		  //GRIP
		  //ESTP   NOTCH  
		  725,240,780,240,670,240,570,105,605,105,650,105,685,105,730,105,765,105,0,0,//GRIPスイッチ
		  //BH
		  800,510,800,485,800,460,800,435,800,410,800,385,800,360,800,335,800,310,0,0,

		  //SL
		  310,PRM_POINT_Y_NOTCH_SL,350,PRM_POINT_Y_NOTCH_SL,390,PRM_POINT_Y_NOTCH_SL,430,PRM_POINT_Y_NOTCH_SL,470,PRM_POINT_Y_NOTCH_SL,510,PRM_POINT_Y_NOTCH_SL,550,PRM_POINT_Y_NOTCH_SL,590,PRM_POINT_Y_NOTCH_SL,630,PRM_POINT_Y_NOTCH_SL,0,0,//SL
		  //AH
		  70,510,70,485,70,460,70,435,70,410,70,385,70,360,70,335,70,310,0,0,
		  0,0,0,0,0,0,0,0
	};
	SIZE size_ctrl[N_OTE_CTRL_TYPE][N_OTE_PNL_STATIC] = {
		//#STATIC
		//LAMP
		   //メインパネル
		   PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,
		   PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,
		   PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,
		   PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,
		   PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,
		   //CONNECTパネル　ID_OTE_RADIO_SOU ID_OTE_RADIO_RPU                  ID_OTE_RADIO_SOM                ID_OTE_RADIO_ROM                        ID_OTE_RADIO_RPM
		   PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,PRM_OTE_DEF_LAMP_W,PRM_OTE_DEF_LAMP_H,
		   //ID_OTE_CHK_CAMERA_WND
		   0,0,
		   0,0,0,0,0,0,0,0,0,0,0,0,				//16
		//LABEL
		//ID_OTE_LABEL_MH	ID_OTE_LABEL_AH	  ID_OTE_LABEL_BH	ID_OTE_LABEL_SL   ID_OTE_LABEL_GT
		  40,20,            40,20,            40,20,            40,20,             40,20,
		 //INF
		 //ID_OTE_INF_MH0  ID_OTE_INF_AH0 ID_OTE_INF_BH0 ID_OTE_INF_SL0	 ID_OTE_INF_GT0	 ID_OTE_INF_SWAY_MH	 ID_OTE_INF_SWAY_AH	
		  100,150,			100,150,       100,150,       100,150,        150,50,            100,150,         100,150,

		//ID_OTE_SUB_CONNECT_ADDR	ID_OTE_SUB_CONNECT_HEADS	 ID_OTE_SUB_CONNECT_HEADR	 
		  280,40,                      280,60,						280,60,
		//ID_OTE_SUB_FAULT_LIST
		  280,180,
		//ID_OTE_SUB_STAT_MH	ID_OTE_SUB_STAT_AH		ID_OTE_SUB_STAT_BH		ID_OTE_SUB_STAT_SL		ID_OTE_SUB_STAT_GT		
		 70,115,					70,115,					70,115,					70,115,					70,115,

		 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,


		   //#PB
		   //メインパネル
		   PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		   PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		   PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		   PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		   PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		   //TOUCH
		   PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		  //ID_OTE_RADIO_SOCK_PU				ID_OTE_RADIO_SOCK_PM				ID_OTE_RADIO_SOCK_OM  
		  PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,	PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,	PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		  // ID_OTE_RADIO_MHSPD_7				ID_OTE_RADIO_MHSPD_14				ID_OTE_RADIO_MHSPD_21	
		  PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,	PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,	PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		  // ID_OTE_RADIO_AHSPD_14 ID_OTE_RADIO_AHSPD_24	
		  PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,	PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		  // ID_OTE_RADIO_BH_57					ID_OTE_RADIO_BH_62					ID_OTE_RADIO_BH_REST	
		  PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,	PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,	PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		  // ID_OTE_PB_GT_SHOCK					ID_OTE_RADIO_JIB_NARROW				ID_OTE_RADIO_JIB_WIDE
		  PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,	PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,	PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		  // ID_OTE_PB_LOAD_SWY
		  PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		  // ID_OTE_RADIO_FLT_ALL				ID_OTE_RADIO_FLT_HEAVY				ID_OTE_RADIO_FLT_LIGHT				ID_OTE_RADIO_FLT_WARN	
		  PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,	PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,	PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,	PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		  // ID_OTE_PB_FLT_RESET	 ID_OTE_CHK_IL_BYPASS	
		  PRM_OTE_DEF_PB_W +15,PRM_OTE_DEF_PB_H+5,	PRM_OTE_DEF_PB_W+10,PRM_OTE_DEF_PB_H+5,
		  //ID_OTE_PB_ARESET_ALL	//ID_OTE_CHK_ASET_MH	//ID_OTE_CHK_ASET_AH	//ID_OTE_CHK_ASET_BH  //ID_OTE_CHK_ASET_SL	
		  PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,//16
		  //ID_OTE_CHK_CAMERA_WND
		   PRM_OTE_DEF_PB_W+20,PRM_OTE_DEF_PB_H,
		   //CAMERA　WINDOW
			//ID_OTE_CHK_CAMERA_LIVE		ID_OTE_CHK_CAMERA_TILU				ID_OTE_CHK_CAMERA_TILD  ID_OTE_CHK_CAMERA_PANL							ID_OTE_CHK_CAMERA_PANR				ID_OTE_CHK_CAMERA_ZOMW				ID_OTE_CHK_CAMERA_ZOMN
		   PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,	PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,	PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,	PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,	PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,	PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		   //ID_OTE_CHK_CAMERA_STOP
		   PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,
		   
		   0,0,0,0,0,0,0,0,0,0,0,0,0,0,//9

		   //#NOTCH
		   PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,0,0,//HOIST
		   PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,0,0,//GT
		   PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,0,0,//GRIP
		   PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,0,0,//BH
		   PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,0,0,//SL
		   PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,PRM_OTE_DEF_PB_W2,PRM_OTE_DEF_PB_H,0,0,//AH
		   0,0,0,0,0,0,0,0
	};
	RECT notch_rect[MOTION_ID_MAX][N_OTE_NOTCH_ARRAY];	//ノッチランプグラフィックエリア
	RECT pb_rect[N_OTE_PNL_ITEMS];						//PBランプグラフィックエリア
	WCHAR ctrl_text[N_OTE_CTRL_TYPE][N_OTE_PNL_STATIC][128] = {
		//STATIC
		L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",//0-15
		L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",//16-32
		//MAIN PANEL
		L"MH",L"AH",L"BH",L"SL",L"GT",L"",L"",L"",L"",L"",L"",L"",
		//CONNECT
		L"ADDR -",L"HEADS -",L"HEADR -",
		//FAULT
		L"FAULT",
		//STAT
		L"MH",L"AH",L"BH",L"SL",L"GT",
		
		L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",L"",

		//PB
		L"停止",L"起動",L"主幹",L"非常",
		L"自動",L"振止",L"S1",L"S2",
		L"S3",L"N1",L"N2",L"N3",
		L"WIDE",L"ZOOM",L"HOOK",L"OPE1",L"OPE2",
		L"通信",L"MODE",L"故障",L"状態",L"ﾀｯﾁ",
		L"PU",L"PM",L"OM",
		// ID_OTE_RADIO_MHSPD_7	 ID_OTE_RADIO_MHSPD_14	 ID_OTE_RADIO_MHSPD_21	
		L"7",L"14",L"21",
		// ID_OTE_RADIO_AHSPD_14 ID_OTE_RADIO_AHSPD_24	
		L"14",L"24",
		// ID_OTE_RADIO_BH_57 ID_OTE_RADIO_BH_62 ID_OTE_RADIO_BH_REST	
		L"57",L"62",L"rest",
		// ID_OTE_PB_GT_SHOCK ID_OTE_RADIO_JIB_NARROW ID_OTE_RADIO_JIB_WIDE
		L"",L"狭",L"広",

		L"",
		// ID_OTE_RADIO_FLT_ALL	ID_OTE_RADIO_FLT_HEAVY	ID_OTE_RADIO_FLT_LIGHT	ID_OTE_RADIO_FLT_WARN	
		L"ALL",L"重",L"軽",L"IL",
		// ID_OTE_PB_FLT_RESET	 ID_OTE_CHK_IL_BYPASS	
		L"故障RST",L"ﾊﾞｲﾊﾟｽ",
		//ID_OTE_PB_ARESET_ALL	//ID_OTE_CHK_ASET_MH	//ID_OTE_CHK_ASET_AH	//ID_OTE_CHK_ASET_BH  //ID_OTE_CHK_ASET_SL	
		L"解除",L"主巻",L"補巻",L"引込",L"旋回",
		L"CAMERA",
		//CAMERA　WINDOW
		//ID_OTE_CHK_CAMERA_LIVE ID_OTE_CHK_CAMERA_TILU	ID_OTE_CHK_CAMERA_TILD  ID_OTE_CHK_CAMERA_PANL ID_OTE_CHK_CAMERA_PANR	ID_OTE_CHK_CAMERA_ZOMW	ID_OTE_CHK_CAMERA_ZOMN
		L"LIVE",L"TIL U",L"TIL D",L"PAN L",L"PAN R",L"ZM W",L"ZM N",
		//ID_OTE_PB_CAMERA_STOP
		L"STOP",
		
		L"",L"",L"",L"",L"",L"",L"",

		//NOTCH
		L"-4",L"-3",L"-2",L"-1",L"0",L"1",L"2",L"3",L"4",L"",//MH
		L"-4",L"-3",L"-2",L"-1",L"0",L"1",L"2",L"3",L"4",L"",//GT
		L"GSTP",L"GACT",L"RMT",L"P+",L"P-",L"T+",L"T-",L"Z+",L"Z-",L"", //GRIP CAMERA リモート
		L"-4",L"-3",L"-2",L"-1",L"0",L"1",L"2",L"3",L"4",L"",//BH
		L"-4",L"-3",L"-2",L"-1",L"0",L"1",L"2",L"3",L"4",L"",//SL
		L"-4",L"-3",L"-2",L"-1",L"0",L"1",L"2",L"3",L"4",L"",//AH
		L"",L"",L"",L""
	};

}ST_OTE_WORK_WND, * LPST_OTE_WORK_WND;



