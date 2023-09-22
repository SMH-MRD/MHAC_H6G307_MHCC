#pragma once

#pragma region �m�b�`PB

//RADIO NOTCH ID  OTE_ID_RADIO + 100xMOTION_ID + 10 + NOTCH(-5�`+5�j
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
HWND h_rdio_ote_notch[MOTION_ID_MAX][OTE_N_NOTCH * 2 + 1];		//�m�b�`PB�n���h��
HWND h_static_ote_notch[MOTION_ID_MAX][OTE_N_NOTCH * 2 + 1];	//�m�b�`�����v�n���h��
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



HWND h_static_msg_cnt;
HWND h_pb_ote[OTE_N_PB_CHK_RADIO];					//PB�n���h��

POINT ote_pb_chk_radio_loc[OTE_N_PB_CHK_RADIO] = {
	{0,540},{160,540}, {320,540}, {480,540}, {640,540},		//�T�u���
	{ 90,5 }, { 145,5 }, { 200,5 },							//IF CHECK
	{700,40} , {85,5 }, {140,5 }, {195,5 },					//CHECK BOX
	{650,65} 	//PB
};

SIZE ote_pb_chk_radio_size[OTE_N_PB_CHK_RADIO] = {
	{ 160,20 }, { 160,20 }, { 160,20 }, { 160,20 }, { 160,20 }, //�T�u���
	{ 50,20 }, { 50,20 }, { 50,20 },							//IF CHECK
	{ 50,20 },{ 50,25 },{ 50,25 },{ 50,25 },					//CHECK BOX
	{ 50,20 }													//PB
};

HWND h_static_ote[OTE_N_STATIC];					//�X�^�e�B�b�N�n���h��
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
L"�ڑ�",
L"����",
L"���[�h",
L"�̏�",
L"���[�����g",

L"UNI",
L"PCM",
L"TEM",

L"���",
L"���u",
L"�ڑ�",
L"MON",
L"�劲"
};

