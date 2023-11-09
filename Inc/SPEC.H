#pragma once

#include "common_def.h"

#ifndef MOTION_ID_MAX

#define MOTION_ID_MAX   8  //制御軸最大数

#endif//



/*** 仕様定義構造体 ***/

#define DIRECTION_MAX 2
#define ACCDEC_MAX 2
#define FWD 0
#define REV 1
#define ACC 0
#define DEC 1

#define DEMENSION_MAX 64
#define ID_BOOM_HIGHT  0

#define NUM_OF_AS_AXIS			5	//自動の制御軸数　走行、横行、旋回、スキュー
#define NUM_OF_SWAY_LEVEL		3	//完了,トリガ,制限
#define NUM_OF_POSITION_LEVEL	3	//完了,トリガ,制限

typedef struct StSpec {

	/*** デバイスコード構造体 ***/
	ST_DEVICE_CODE device_code = {
		'P','1',//製番識別コード
		'L','C',//機械コード
		'C','R',//タイプ
		1		//ID番号
	};

#define NOTCH_MAX 6
#define NOTCH_0	  0
#define NOTCH_1	  1
#define NOTCH_2	  2
#define NOTCH_3	  3
#define NOTCH_4	  4
#define NOTCH_5	  5

	double notch_spd_f[MOTION_ID_MAX][NOTCH_MAX] = {			//# ノッチ指令速度（機上）1倍速時
	{ 0.0,	0.012,	0.35,	0.07,	0.117,	0.117 },			//[ID_HOIST]	m/s
	{ 0.0,	0.04,	0.125,	0.25,	0.416,	0.416 },			//[ID_GANTRY]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_TROLLY]	m/s
	{ 0.0,	0.1,	0.3,	0.5,	0.7,	1.0 },				//[ID_BOOM_H]	m/s
	{ 0.0,	0.003,	0.011,	0.021,	0.035,	0.035 },			//[ID_SLEW]		rad/s;
	{  0.0,	0.02,	0.06,	0.12,	0.2,	0.2 },				//[ID_AHOIST]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_H_ASSY];
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_MOTION1];
	};
	double notch_spd_r[MOTION_ID_MAX][NOTCH_MAX] = {			//# ノッチ指令速度（機上）1倍速時
	{ 0.0,	-0.012,	-0.35,	-0.07,	-0.117,	-0.117 },			//[ID_HOIST]	m/s
	{ 0.0,	-0.04,	-0.125,	-0.25,	-0.416,	-0.416 },			//[ID_GANTRY]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_TROLLY]	m/s
	{ 0.0,	-0.1,	-0.3,	-0.5,	-0.7,	-1.0 },				//[ID_BOOM_H]	m/s
	{  0.0,	-0.003,	-0.011,	-0.021,	-0.035,	-0.035  },			//[ID_SLEW]		rad/s;
	{ 0.0,	-0.02,	-0.06,	-0.12,	-0.2,	-0.2  },			//[ID_AHOIST]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_H_ASSY];
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_MOTION1];
	};

	//[ID_HOIST],[ID_GANTRY],[ID_TROLLY],[ID_BOOM_H],[ID_SLEW],[ID_OP_ROOM],[ID_H_ASSY],[ID_MOTION1]

	double notch_spd_remote_f[MOTION_ID_MAX][NOTCH_MAX] = {		//# ノッチ指令速度（遠隔）1倍速時
	{ 0.0,	0.012,	0.35,	0.07,	0.117,	0.117  },			//[ID_HOIST]	m/s
	{ 0.0,	0.5,	0.5,	0.5,	0.5,	0.5 },				//[ID_GANTRY]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_TROLLY]	m/s
	{ 0.0,	0.1,	0.3,	0.5,	0.7,	1.0 },				//[ID_BOOM_H]	m/s
	{ 0.0,	0.003,	0.011,	0.021,	0.035,	0.035  },			//[ID_SLEW]		rad/s;
	{ 0.0,	0.02,	0.06,	0.12,	0.2,	0.2 },				//[ID_AHOIST]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_H_ASSY];
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_MOTION1];
	};
	double notch_spd_remote_r[MOTION_ID_MAX][NOTCH_MAX] = {		//# ノッチ指令速度（遠隔）1倍速時
	{ 0.0,	-0.012,	-0.35,	-0.07,	-0.117,	-0.117 },			//[ID_HOIST]	m/s
	{ 0.0,	-0.5,	-0.5,	-0.5,	-0.5,	-0.5 },				//[ID_GANTRY]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_TROLLY]	m/s
	{ 0.0,	-0.1,	-0.3,	-0.5,	-0.7,	-1.0 },				//[ID_BOOM_H]	m/s
	{  0.0,	-0.003,	-0.011,	-0.021,	-0.035,	-0.035  },			//[ID_SLEW]		rad/s;
	{ 0.0,	-0.02,	-0.06,	-0.12,	-0.2,	-0.2 },				//[ID_AHOIST]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_H_ASSY];
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_MOTION1];
	};
	double notch_spd_drum_f[MOTION_ID_MAX][NOTCH_MAX] = {		//# ノッチ指令速度（ドラム回転・車輪回転）1倍速時
	{ 0.0,	0.054,	0.0163,	0.0326,	0.544,	0.544 },			//[ID_HOIST]	rps
	{ 0.0,	0.3238,	0.3238,	0.3238,	0.3238,	0.3238 },			//[ID_GANTRY]	rps
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_TROLLY]	rps
	{ 0.0,	0.0128,	0.0385,	0.0770,	0.1283,	0.1283 },			//[ID_BOOM_H]	rps
	{ 0.0,	0.0068,	0.0204,	0.0409,	0.0681,	0.0681},			//[ID_SLEW]		rps
	{ 0.0,	0.0694,	0.2082,	0.4164,	0.694,	0.694 },			//[ID_AHOIST]	rps
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_H_ASSY];
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_MOTION1];
	};
	double notch_spd_drum_r[MOTION_ID_MAX][NOTCH_MAX] = {		//# ノッチ指令速度（ドラム・車輪回転）1倍速時
	{ 0.0,	-0.054,	-0.0163,-0.0326,-0.544,	-0.544 },			//[ID_HOIST]	rps
	{ 0.0,	-0.3238,-0.3238,-0.3238,-0.3238,-0.3238 },			//[ID_GANTRY]	rps
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_TROLLY]	rps
	{ 0.0,	-0.0128,-0.0385,-0.0770,-0.1283,-0.1283 },			//[ID_BOOM_H]	rps
	{ 0.0,	-0.0068,-0.0204,-0.0409,-0.0681,-0.0681 },			//[ID_SLEW]		rps
	{ 0.0,	-0.0694, -0.2082, -0.4164, -0.694, -0.694},			//[ID_AHOIST]	rps
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_H_ASSY];
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_MOTION1];
	};

	double accdec[MOTION_ID_MAX][DIRECTION_MAX][ACCDEC_MAX] = {	//# 各動作加減速度
	{{ 0.078,	-0.078},	{-0.078,	0.078 }},				//[ID_HOIST]	m/s2
	{{ 0.0405,	-0.0405},	{-0.0405,	0.0405 }},				//[ID_GANTRY]	m/s2
	{{ 0.0,		-0.0},		{-0.0,		0.0 }},					//[ID_TROLLY]	m/s2
	{{ 0.187,	-0.187},	{-0.187,	0.187 }},				//[ID_BOOM_H]	m/s2
	{{ 0.00756,	-0.00756},	{-0.00756,	0.00756 }},				//[ID_SLEW]		rad/s2;
	{{ 0.133,	-0.133},	{-0.133,	0.133 }},				//[ID_AHOIST]	m/s2
	{{ 0.0,	0.0},			{0.0,	0.0 }},						//[ID_H_ASSY];
	{{ 0.0,	0.0},			{0.0,	0.0 }},						//[ID_MOTION1];
	};

	double accdec_d[MOTION_ID_MAX][DIRECTION_MAX][ACCDEC_MAX] = {	//# 各動作ドラム回転加減速度
	{{ 0.363,	-0.363},	{-0.363,	0.363 }},				//[ID_HOIST]	n/s2
	{{ 0.0812,	-0.0812},	{-0.0812,	0.0812 }},				//[ID_GANTRY]	n/s2
	{{ 1.0,		-1.0},		{-1.0,		1.0 }},					//[ID_TROLLY]	n/s2
	{{ 0.0855,	-0.0855},	{-0.0855,	0.0855 }},				//[ID_BOOM_H]	n/s2
	{{ 0.01301,	-0.01301},	{-0.01301,	0.01301 }},				//[ID_SLEW]		n/s2;
	{{ 0.463,	-0.463},	{-0.463,	0.463 }},				//[ID_AHOIST]	n/s2
	{{ 0.0,	0.0},			{0.0,	0.0 }},						//[ID_H_ASSY];
	{{ 0.0,	0.0},			{0.0,	0.0 }},						//[ID_MOTION1];
	};

#define POS_LIMIT_TYPE_MAX	4
#define END_LIMIT			0
#define SPD_LIMIT1			1
#define SPD_LIMIT2			2
#define SPD_LIMIT3			3
	
	double pos_limit[MOTION_ID_MAX][DIRECTION_MAX][POS_LIMIT_TYPE_MAX] = {//# 極限寸法								//# 各種寸法
	{{ 14.5,0.0,0.0,0.0},	{ -11.0,0.0,0.0,0.0}},				//[ID_HOIST]
	{{ 80.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_GANTRY]
	{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_TROLLY]
	{{ 27.0,0.0,0.0,0.0},	{ 8.0,0.0,0.0,0.0}},				//[ID_BOOM_H]
	{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_SLEW]
	{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_OP_ROOM]
	{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_H_ASSY]
	{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_MOTION1]
	};

#define PARAM_PHASE_CHK_RANGE_BH						0.05	//位相到達判定範囲
#define PARAM_PHASE_CHK_RANGE_SLW						0.05	//位相到達判定範囲

#define ID_LV_COMPLE			0	//完了
#define ID_LV_TRIGGER			1	//トリガ
#define ID_LV_LIMIT				2	//制限
	double as_rad_level[NUM_OF_AS_AXIS][NUM_OF_SWAY_LEVEL] = {	//# 振れ止め判定　振れ角レベル(rad)
	{ 0.0,	0.0, 0.0},											//[ID_HOIST]
	{ 0.005, 0.01, 0.020 },									//[ID_GANTRY]
	{ 0.005, 0.01, 0.020 },									//[ID_TROLLY]
	{ 0.005, 0.01, 0.020 },									//[ID_BOOM_H]
	{ 0.005, 0.01, 0.020 },									//[ID_SLEW]	
	};
	double as_rad2_level[NUM_OF_AS_AXIS][NUM_OF_SWAY_LEVEL] = {	//# 振れ止め判定　振れ振幅レベル(rad^2)
	{ 0.0,	0.0, 0.0},											//[ID_HOIST]
	{ 0.000025, 0.0001, 0.0004 },								//[ID_GANTRY]
	{ 0.000025, 0.0001, 0.0004 },								//[ID_TROLLY]
	{ 0.000025, 0.0001, 0.0004 },								//[ID_BOOM_H]
	{ 0.000025, 0.0001, 0.0004 },								//[ID_SLEW]
	};
	double as_m_level[NUM_OF_AS_AXIS][NUM_OF_SWAY_LEVEL] = {	//# 振れ止め判定　振れ角レベル(m)
	{ 0.0,	0.0, 0.0},											//[ID_HOIST]
	{ 0.1, 0.2, 2.00 },										//[ID_GANTRY]
	{ 0.1, 0.2, 2.00 },										//[ID_TROLLY]
	{ 0.1, 0.2, 2.00 },										//[ID_BOOM_H]
	{ 0.1, 0.2, 2.00 },										//[ID_SLEW]	
	};
	double as_m2_level[NUM_OF_AS_AXIS][NUM_OF_SWAY_LEVEL] = {	//# 振れ止め判定　振れ振幅レベル(m^2)
	{ 0.0,	0.0, 0.0},											//[ID_HOIST]
	{ 0.01, 0.04, 4.0 },										//[ID_GANTRY]
	{ 0.01, 0.04, 4.0 },										//[ID_TROLLY]
	{ 0.01, 0.04, 4.0 },										//[ID_BOOM_H]
	{ 0.01, 0.04, 4.0 },										//[ID_SLEW]
	};
	double as_pos_level[NUM_OF_AS_AXIS][NUM_OF_POSITION_LEVEL] = {	//# 位置決め判定　位置ずれレベル(m,rad) 
	{ 0.03, 0.06, 0.020 },										//m[ID_HOIST]
	{ 0.03, 0.06, 0.020 },										//m[ID_GANTRY]
	{ 0.03, 0.06, 0.020 },										//m[ID_TROLLY]
	{ 0.03, 0.06, 0.020 },										//m[ID_BOOM_H]
	{ 0.005, 0.01, 0.1 }										//rad[ID_SLEW]
	};

#define ID_LV_DIR_CHECK_MARGIN	0								//移動方向判定マージン
#define ID_LV_COMP_POS_PASSED	1								//通過
#define ID_LV_COMP_POS_RANGE	2								//範囲

	double pos_check_limit[MOTION_ID_MAX][NUM_OF_POSITION_LEVEL] = {	//# 位置到達判定範囲(m,rad) 
	{ 0.1, 0.2, 0.5 },											//m[ID_HOIST]
	{ 0.1, 0.2, 0.5 },											//m[ID_GANTRY]
	{ 0.1, 0.2, 0.5 },											//m[ID_TROLLY]
	{ 0.1, 0.2, 0.5 },											//m[ID_BOOM_H]
	{ 0.005, 0.17, 0.017},										//rad[ID_SLEW]
	{ 0.0, 0.0, 0.0 },											//
	{ 0.0, 0.0, 0.0 },											//
	{ 0.0, 0.0, 0.0 },											//
	};

#define NUM_OF_DELAY_PTN		5								//加減速時FB時間遅れ評価パターン数
#define ID_DELAY_0START			0								//停止からの加速
#define ID_DELAY_ACC_DEC		1								//加速中の減速切替時
#define ID_DELAY_DEC_ACC		2								//減速中の加速切替時
#define ID_DELAY_CNT_ACC		3								//定速からの加速時
#define ID_DELAY_CNT_DEC		4								//定速からの減速時	

	double delay_time[NUM_OF_AS_AXIS][NUM_OF_DELAY_PTN] = {		// 加減速時のFB一時遅れ時定数
	{ 0.3,0.3,0.3,0.3,0.3},										//[ID_HOIST]
	{ 0.3,0.3,0.3,0.3,0.3 },									//[ID_GANTRY]
	{ 0.3,0.3,0.3,0.3,0.3 },									//[ID_TROLLY]
	{ 0.3,0.3,0.3,0.3,0.3 },									//[ID_BOOM_H]
	{ 0.3,0.3,0.3,0.3,0.3 },									//[ID_SLEW]
	};
	
	//構造寸法
	double boom_high = 25.0, wheel_span = 20.0, leg_span = 15.0;// ブーム高さ,ホイルスパン,脚間スパン
	//極限
	double hoist_pos_min = -10.0, hoist_pos_max = 20.0, boom_pos_min = 5.0, boom_pos_max = 28.0, gantry_pos_min = 0.0, gantry_pos_max = 300.0;
	//吊具荷重
	double m_loard0 = 1000;	//吊具質量kg
	//振れセンサ



#define N_SWAY_SENSOR			3		//振れセンサの数
#define SID_SENSOR1				0		//No.1振れセンサ
#define SID_SENSOR2				1		//No.2振れセンサ
#define SID_SIM					2		//シミュレータ

#define SWAY_SENSOR_N_AXIS		2
#define SID_AXIS_X				0		//X軸
#define SID_AXIS_Y				1		//y軸

#define N_SWAY_SENSOR_CAMERA    2		//振れセンサ１セットのカメラの数
#define SID_CAMERA1				0		//No.1振れセンサ
#define SID_CAMERA2				1		//No.2振れセンサ

#define N_SWAY_SENSOR_TARGET    2		//振れセンサカメラ1台あたりのターゲット数
#define SID_TARGET1				0		//No.1振れセンサ
#define SID_TARGET2				1		//No.2振れセンサ

#define SWAY_CAMERA_N_PARAM		6
#define SID_L0					0		//カメラ回転軸距離
#define SID_PH0					1		//カメラ回転軸角度
#define SID_l0					2		//カメラ中心とハウジング回転軸間距離
#define	SID_ph0					3		//カメラのハウジングへの取り付け角度
#define	SID_phc					4		//カメラのハウジングへの取り付け角度
#define	SID_PIXlRAD				5		//カメラのハウジングへの取り付け角度


#define CTRL_PC_IP_ADDR_SWAY	"192.168.1.6"
#define SWAY_SENSOR_IP_ADDR		"192.168.1.81"


#define OTE_MULTI_IP_ADDR		"239.1.0.1"
//#define CTRL_PC_IP_ADDR_OTE		"192.168.1.201"
#define CTRL_PC_IP_ADDR_OTE		"192.168.1.24"
#define OTE_DEFAULT_IP_ADDR		"127.0.0.1"
//#define OTE_DEFAULT_IP_ADDR		"192.168.1.200"


#define SWAY_IF_IP_SWAY_PORT_C			10080
#define SWAY_IF_IP_SWAY_PORT_S			10081
#define OTE_IF_IP_UNICAST_PORT_C		10050	//ユニキャスト端末受信ポート
#define OTE_IF_IP_UNICAST_PORT_S		10051	//ユニキャストクレーン受信ポート
#define OTE_IF_IP_MULTICAST_PORT_TE		20081	//マルチキャスト端末受信ポート
#define OTE_IF_IP_MULTICAST_PORT_CR		20080	//マルチキャストクレーン受信ポート


	double SwayCamParam[N_SWAY_SENSOR][N_SWAY_SENSOR_CAMERA][SWAY_SENSOR_N_AXIS][SWAY_CAMERA_N_PARAM] = {	//振れセンサ　パラメータ
		{//No.1 センサ
		{{1.0,0.0174,0.2,0.01,0.01,2800.0},{1.0,0.0,0.0,0.00,0.00,2800.0}},	//カメラ1 x,y方向 L0 m,PH rad,l0 m,ph0 rad, phc rad,pix/rad
		{{1.0,0.0174,0.2,0.01,0.01,2800.0},{1.0,0.0174,0.2,0.01,0.01,2800.0}},	//カメラ2 x,y方向 L0 m,PH rad,l0 m,ph0 rad, phc rad,pix/rad
		},
		{//No.2 センサ
		{{1.0,0.0174,0.2,0.01,0.01,2800.0},{1.0,0.0174,0.2,0.01,0.01,2800.0}},	//カメラ1 x,y方向 L0 m,PH rad,l0 m,ph0 rad, phc rad,pix/rad
		{{1.0,0.0174,0.2,0.01,0.01,2800.0},{1.0,0.0174,0.2,0.01,0.01,2800.0}},	//カメラ2 x,y方向 L0 m,PH rad,l0 m,ph0 rad, phc rad,pix/rad
		},
		{//No.3 センサ
		{{1.0,0.0174,0.2,0.01,0.01,2800.0},{1.0,0.0174,0.2,0.01,0.01,2800.0}},	//カメラ1 x,y方向 L0 m,PH rad,l0 m,ph0 rad, phc rad,pix/rad
		{{1.0,0.0174,0.2,0.01,0.01,2800.0},{1.0,0.0174,0.2,0.01,0.01,2800.0}},	//カメラ2 x,y方向 L0 m,PH rad,l0 m,ph0 rad, phc rad,pix/rad
		}
	};

#define SEMI_AUTO_TARGET_MAX	6	// 半自動目標ホールド数

	double semi_target[SEMI_AUTO_TARGET_MAX][MOTION_ID_MAX]{//半自動デフォルト目標位置
	//	HST,	GNT,	TRY,	BH,		SLW,	OPR,	ASSY,	COM
		{10.0,	0.0,	0.0,	8.0,	0.0,	0.0,	0.0,	0.0},
		{5.0,	0.0,	0.0,	25.0,	0.0,	0.0,	0.0,	0.0},
		{18.0,	0.0,	0.0,	25.0,	PI180,	0.0,	0.0,	0.0},
		{10.0,	0.0,	0.0,	25.0,	PI90,	0.0,	0.0,	0.0},
		{10.0,	0.0,	0.0,	5.0,	PI90,	0.0,	0.0,	0.0},
		{15.0,	0.0,	0.0,	18.0,	-PI60,	0.0,	0.0,	0.0},
	};

	double Lb = 64.009;
	double Lm = 63.000;
	double La = 66.566;
	double Lmb = 1.676;
	double Lma = 4.981;
	double Lba = 5.512;
	double Th_pba = 2.00301;
	double Th_pbm = 0.90266;
	double Alpa_m = 0.02089;
	double Alpa_a = 0.06801;
	double Phbm = 2.20547;
	double Hp = 47.7;
	double Lp = 25.174;
	double Php = 1.6583;
	double Lmf = 2.9000;
	double Laf = 2.2910;

#define WIRE_ITEM_NUM		8		// ワイヤ関連パラメータ項目数
#define NW_ITEM_WIND		0		// ワイヤ掛数
#define NW_ITEM_WIND_BOOM	1		// ワイヤ掛数ブーム部
#define NW_ITEM_EX_TURN		2		// あだ巻数
#define NW_ITEM_GROOVE		3		// ドラム溝数
#define DRUM_ITEM_DIR		4		// １層ドラム径
#define DRUM_ITEM_WIRE_DIR	5		// ワイヤ径
#define DRUM_ITEM_DIR_ADD	6		// ドラム径層加算値
#define SIZE_ITEM_WIRE_LEN0	7		// 基準ワイヤ長
	
	double prm_nw[WIRE_ITEM_NUM][MOTION_ID_MAX]{
		//	HST,	GNT,	BHMH,	BH,		SLW,	AH,		ASSY,	COM
			{4.0,	0.0,	4.0,	6.0,	0.0,	2.0,	0.0,	0.0},	// ワイヤ掛数
			{2.0,	0.0,	2.0,	6.0,	0.0,	4.0,	0.0,	0.0},	// ワイヤ掛数ブーム部
			{4.0,	0.0,	6.0,	4.0,	0.0,	5.0,	0.0,	0.0},	// あだ巻数
			{15.0,	1.0,	10.0,	12.0,	1.0,	15.0,	0.0,	0.0},	// ドラム溝数
			{1.200,	0.50,	1.200,	1.1925,	0.308,	0.750,	0.0,	0.0},	// １層ドラム径
			{0.05,	0.0,	0.05,	0.0425,	0.0,	0.028,	0.0,	0.0},	// ワイヤ径
			{0.0866,0.0,	0.0866,	0.0736,	0.0,	0.0485,	0.0,	0.0},	// ドラム径層加算値
			{563.598,1.571,	563.598,525.906,0.968,	402.636,0.0,	0.0}	// 基準ワイヤ長
	};

	double Th_rest	= -0.7113;	//ジブレスト時の起伏角
	double D_min	= 41.425;	//最小半径時の起伏シーブ間距離
	double D_rest	= 83.905;	//ジブレスト時の起伏シーブ間距離
	double Hb_min	= 59.397;	//最小半径時の起伏高さ
	double Hb_rest	= -41.786;	//ジブレスト時起伏高さ
	double Dttb		= 3.652;	//TTB径
	double Nbh_drum = 67.974;	//引込ドラム回転数の和（引込＋主巻）
	double Load0_mh = 11000.0;	//主巻吊具荷重
	double Load0_ah = 1900.0;	//補巻吊具荷重

#define MOTION_ITEM_NUM			6		// 動作関連パラメータ項目数
#define MOTION_ITEM_END_POS_F	0		// 正転方向軸極限位置（m,rad）
#define MOTION_ITEM_END_POS_R	1		// 逆転方向軸極限位置（m,rad）
#define MOTION_ITEM_LIM_V_F		2		// 正転方向軸極限速度（m/s,rad/s）
#define MOTION_ITEM_LIM_V_R		3		// 逆転方向軸極限速度（m/s,rad/s）
#define MOTION_ITEM_RATE_A_F	4		// 正転方向軸定格加速度（m/s2,rad/s2）
#define MOTION_ITEM_RATE_A_R	5		// 逆転方向軸定格加速度（m/s2,rad/s2）

	double prm_move[MOTION_ITEM_NUM][MOTION_ID_MAX]{
		//	HST,		GNT,		BHMH,		BH,			SLW,		AH,			ASSY,	COM
			{79.0,		28.333,		0.0,		21.0,		3.142,		88.0,		0.0,	0.0},	// 正転方向軸極限位置（m,rad）
			{-5.550,	0.011234,	0.0,		62.0,		-3.142,		-5.408,		0.0,	0.0},	// 逆転方向軸極限位置（m,rad）
			{0.35,		0.5,		0.0,		0.208,		0.035,		0.4,		0.0,	0.0},	// 正転方向軸極限速度（m/s,rad/s）
			{-0.35,		-0.5,		0.0,		-0.208,		-0.035,		-0.4,		0.0,	0.0},	// 逆転方向軸極限速度（m/s,rad/s）
			{0.078,		0.063,		0.0,		0.139,		0.004,		0.133,		0.0,	0.0},	// 正転方向軸定格加速度（m/s2,rad/s2）
			{-0.078,	-0.063,		0.0,		-0.139,		-0.004,		-0.133,		0.0,	0.0}	// 逆転方向軸定格加速度（m/s2,rad/s2）
	};


#define DRIVE_ITEM_NUM			4		// ドライブ関連パラメータ項目数
#define DRIVE_ITEM_RPS			0		// モータ定格回転数（100%）
#define DRIVE_ITEM_GEAR			1		// 減速比
#define DRIVE_ITEM_ACC_TIME		2		// 加速時間（100%)
#define DRIVE_ITEM_RATE_V		3		// 定格速度（100%)

	double prm_drv[DRIVE_ITEM_NUM][MOTION_ID_MAX]{
	//	HST,		GNT,		BHMH,		BH,			SLW,		AH,			ASSY,	COM
		{12.5,		28.333,		0.0,		29.1667,	29.167,		16.667,		0.0,	0.0},	// モータ定格回転数（rps　100%）
		{0.007823,	0.011234,	0.0,		0.004399,	0.0000191,	0.008418,	0.0,	0.0},	// 減速比(ドラム回転/モータ回転）
		{1.5,		8.0,		0.0,		1.5,		9.0,		1.5,		0.0,	0.0},	// 加速時間（100%)
		{0.1170,	0.5,		0.0,		0.2080,		0.035,		0.2000,		0.0,	0.0}	// 定格速度（m/s, rad/s)
	};

}ST_SPEC, * LPST_SPEC;




