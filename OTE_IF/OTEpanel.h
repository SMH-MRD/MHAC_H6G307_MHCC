#pragma once

#pragma region ノッチPB

//RADIO NOTCH ID  OTE_ID_RADIO + 100xMOTION_ID + 10 + NOTCH(-5～+5）
#define OTE_ID_RADIO_NOTCH                      21000
#define OTE_ID_RADIO_HST_NOTCH                  21010
#define OTE_ID_RADIO_GNT_NOTCH                  21110
#define OTE_ID_RADIO_TRY_NOTCH                  21210
#define OTE_ID_RADIO_BH_NOTCH                   21310
#define OTE_ID_RADIO_SLW_NOTCH                  21410
#define OTE_ID_RADIO_AH_NOTCH                   21510
#define OTE_ID_STATIC_HST_NOTCH                 21030
#define OTE_ID_STATIC_GNT_NOTCH                 21130
#define OTE_ID_STATIC_TRY_NOTCH                 21230
#define OTE_ID_STATIC_BH_NOTCH                  21330
#define OTE_ID_STATIC_SLW_NOTCH                 21430
#define OTE_ID_STATIC_AH_NOTCH                  21530

#define OTE_N_NOTCH							5
HWND h_rdio_ote_notch[MOTION_ID_MAX][OTE_N_NOTCH * 2 + 1];		//ノッチPBハンドル
HWND h_static_ote_notch[MOTION_ID_MAX][OTE_N_NOTCH * 2 + 1];	//ノッチランプハンドル
SIZE ote_notch_radio_pb_size = { 20,20 };
POINT ote_notch_pb_loc[MOTION_ID_MAX] = {
   {715,400},
   {400,250},
   {0,0},
   {5,400},
   {175,275},
   {750,350},
};
#pragma endregion

#pragma region PB
//RADIO NOTCH ID  OTE_ID_RADIO + 100xMOTION_ID + 10 + NOTCH(-5～+5）
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
#define OTE_INDEX_PB_START				12
#define OTE_INDEX_PB_CTR_SOURCE			12
#define OTE_INDEX_PB_END				12

#define OTE_ID_STATIC                   21700
#define OTE_INDEX_STATIC_MSG_CNT			0
#define OTE_INDEX_STATIC_COMMON_WND			1
#define OTE_INDEX_STATIC_CONNECT_CNT		2
#define OTE_INDEX_STATIC_OTE_IP_INF			3
#define OTE_INDEX_STATIC_IFCHK_MSG_HEAD_S	4
#define OTE_INDEX_STATIC_IFCHK_MSG_BODY_S	5
#define OTE_INDEX_STATIC_IFCHK_MSG_HEAD_R	6
#define OTE_INDEX_STATIC_IFCHK_MSG_BODY_R	7

HWND h_static_msg_cnt;
HWND h_pb_ote[OTE_N_PB_CHK_RADIO];					//PBハンドル

POINT ote_pb_chk_radio_loc[OTE_N_PB_CHK_RADIO] = {
	{0,540},{160,540}, {320,540}, {480,540}, {640,540},		//サブ画面
	{ 90,5 }, { 145,5 }, { 200,5 },							//IF CHECK
	{700,40} , {85,5 }, {140,5 }, {195,5 },					//CHECK BOX
	{650,65} 	//PB
};

SIZE ote_pb_chk_radio_size[OTE_N_PB_CHK_RADIO] = {
	{ 160,20 }, { 160,20 }, { 160,20 }, { 160,20 }, { 160,20 }, //サブ画面
	{ 50,20 }, { 50,20 }, { 50,20 },							//IF CHECK
	{ 50,20 },{ 50,25 },{ 50,25 },{ 50,25 },					//CHECK BOX
	{ 50,20 }													//PB
};

HWND h_static_ote[OTE_N_STATIC];					//スタティックハンドル
POINT ote_static_loc[OTE_N_STATIC] = { 
	{ 255, 0 },{ 0, 0 },{ 0, 45 },{ 0, 90 }, 
	{ 5, 30 },{ 5, 55 },{ 5, 140 },{ 5, 165 }		//IF CHECK MSG
};
SIZE ote_static_size[OTE_N_STATIC] = { 
	{ 545, 20 },{ 50, 20 },{ 250, 40 },{ 250, 110 }, 
	{ 750, 20 },{ 750, 80 },{ 750, 20 },{ 750, 80 }		//IF CHECK MSG
};
INT32 ote_static_stat[OTE_N_STATIC];

WCHAR pb_text[OTE_N_PB_CHK_RADIO][128] = {
L"接続",
L"自動",
L"モード",
L"故障",
L"モーメント",

L"UNI",
L"PCM",
L"TEM",

L"非停",
L"遠隔",
L"接続",
L"MON",
L"主幹"
};

#pragma endregion
