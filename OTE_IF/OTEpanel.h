#pragma once


#define OTE_ID_RADIO_NOTCH                      21000

//RADIO NOTCH ID  OTE_ID_RADIO + 100xMOTION_ID + 10 + NOTCH(-5Å`+5Åj
#define OTE_ID_RADIO_HST_NOTCH                  21010
#define OTE_ID_RADIO_GNT_NOTCH                  21110
#define OTE_ID_RADIO_TRY_NOTCH                  21210
#define OTE_ID_RADIO_BH_NOTCH                   21310
#define OTE_ID_RADIO_SLW_NOTCH                  21410
#define OTE_ID_RADIO_AH_NOTCH                   21510
#define OTE_ID_STATIC_HST_NOTCH                 21020
#define OTE_ID_STATIC_GNT_NOTCH                 21120
#define OTE_ID_STATIC_TRY_NOTCH                 21220
#define OTE_ID_STATIC_BH_NOTCH                  21320
#define OTE_ID_STATIC_SLW_NOTCH                 21420
#define OTE_ID_STATIC_AH_NOTCH                  21520

SIZE ote_notch_radio_pb_size = { 20,20 };

POINT ote_notch_pb_loc[MOTION_ID_MAX] = {
   {715,400},
   {400,250},
   {0,0},
   {5,400},
   {175,275},
   {750,350},
};

#define OTE_N_NOTCH							5

HWND h_rdio_ote_notch[MOTION_ID_MAX][OTE_N_NOTCH * 2 + 1];
HWND h_static_ote_notch[MOTION_ID_MAX][OTE_N_NOTCH * 2 + 1];

