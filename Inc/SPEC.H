#pragma once

#include "common_def.h"

#ifndef MOTION_ID_MAX

#define MOTION_ID_MAX   8  //制御軸大数

#endif//

/*** 仕様定義 ***/

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

#define SPD_NORM			1.0 //正規化速度1％=1.0

	double notch_spd_f[MOTION_ID_MAX][NOTCH_MAX] = {			//# ノッチ指令速度（機上）1倍速時
	{ 0.0,	0.012,	0.035,	0.07,	0.117,	0.117 },			//[ID_HOIST]	m/s
	{ 0.0,	0.04,	0.125,	0.25,	0.416,	0.416 },			//[ID_GANTRY]	m/s
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_TROLLY]	m/s
	{ 0.0,	0.021,	0.063,	0.125,	0.208,	0.208 },				//[ID_BOOM_H]	m/s
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

	double notch_spd_nrm_f[MOTION_ID_MAX][NOTCH_MAX] = {		//# ノッチ指令速度（正規化）%
	{ 0.0,	0.1,	0.3,	0.6,	1.0,	1.0 },				//[ID_HOIST]	%
	{ 0.0,	0.1,	0.3,	0.6,	1.0,	1.0 },				//[ID_GANTRY]	%
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_TROLLY]	%
	{ 0.0,	0.1,	0.3,	0.6,	1.0,	1.0 },				//[ID_BOOM_H]	%
	{ 0.0,	0.1,	0.3,	0.6,	1.0,	1.0 },				//[ID_SLEW]		%
	{ 0.0,	0.1,	0.3,	0.6,	1.0,	1.0 },				//[ID_AHOIST]	%
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_H_ASSY];
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_MOTION1];
	};
	double notch_spd_nrm_r[MOTION_ID_MAX][NOTCH_MAX] = {			//# ノッチ指令速度（正規化）%
	{ 0.0,	-0.1,	-0.3,	-0.6,	-1.0,	-1.0 },				//[ID_HOIST]	%
	{ 0.0,	-0.1,	-0.3,	-0.6,	-1.0,	-1.0},				//[ID_GANTRY]	%
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_TROLLY]	%
	{ 0.0,	-0.1,	-0.3,	-0.6,	-1.0,	-1.0  },			//[ID_BOOM_H]	%
	{ 0.0,	-0.1,	-0.3,	-0.6,	-1.0,	-1.0 },				//[ID_SLEW]		%
	{ 0.0,	-0.1,	-0.3,	-0.6,	-1.0,	-1.0  },			//[ID_AHOIST]	%
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_H_ASSY];
	{ 0.0,	0.0,	0.0,	0.0,	0.0,	0.0 },				//[ID_MOTION1];
	};

#define N_NOTCH_RATIO	4
	double notch_spd_retio[MOTION_ID_MAX][N_NOTCH_RATIO] = {	//倍速パターン
	{ 1.0,2.0,3.0,1.0},		//[ID_HOIST]	1倍　2倍　3倍
	{ 1.0,1.0,1.0,1.0},		//[ID_GANTRY]	
	{ 1.0,1.0,1.0,1.0},		//[ID_TROLLY]	
	{ 1.0,1.0,1.0,1.0},		//[ID_BOOM_H]	
	{ 1.0,0.75,1.0,1.0},	//[ID_SLEW]		1倍　0.75倍	
	{ 1.0,2.0,1.0,1.0},		//[ID_AHOIST]	1倍　2倍	
	{ 1.0,1.0,1.0,1.0},		//[ID_H_ASSY];
	{ 1.0,1.0,1.0,1.0}		//[ID_MOTION1];
	};

#if 0
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
#endif

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

	double accdec100[MOTION_ID_MAX][DIRECTION_MAX][ACCDEC_MAX] = {	//# 各動作ドラム回加減速度%
	{{ 66.667,	-66.667},	{-66.667,	66.667 }},				//[ID_HOIST]	n/s2
	{{ 0.0812,	-0.0812},	{-0.0812,	0.0812 }},				//[ID_GANTRY]	n/s2
	{{ 0.0,		-0.0},		{-0.0,		0.0 }},					//[ID_TROLLY]	n/s2
	{{ 12.5,	-12.5},		{-12.5,		12.5 }},				//[ID_BOOM_H]	n/s2
	{{ 11.111,	-11.111},	{-11.111,	11.111 }},				//[ID_SLEW]		n/s2
	{{ 66.667,	-66.667},	{-66.667,	66.667 }},				//[ID_AHOIST]	n/s2
	{{ 0.0,		0.0},		{0.0,		0.0 }},					//[ID_H_ASSY]
	{{ 0.0,		0.0},		{0.0,		0.0 }},					//[ID_MOTION1]
	};

	double accdec_d[MOTION_ID_MAX][DIRECTION_MAX][ACCDEC_MAX] = {	//# 各動作ドラム回転加減速度
	{{ 0.363,	-0.363},	{-0.363,	0.363 }},				//[ID_HOIST]	n/s2
	{{ 0.0812,	-0.0812},	{-0.0812,	0.0812 }},				//[ID_GANTRY]	n/s2
	{{ 1.0,		-1.0},		{-1.0,		1.0 }},					//[ID_TROLLY]	n/s2
	{{ 0.0855,	-0.0855},	{-0.0855,	0.0855 }},				//[ID_BOOM_H]	n/s2
	{{ 0.01301,	-0.01301},	{-0.01301,	0.01301 }},				//[ID_SLEW]		n/s2
	{{ 0.463,	-0.463},	{-0.463,	0.463 }},				//[ID_AHOIST]	n/s2
	{{ 0.0,	0.0},			{0.0,	0.0 }},						//[ID_H_ASSY]
	{{ 0.0,	0.0},			{0.0,	0.0 }},						//[ID_MOTION1]
	};


#define POS_LIMIT_TYPE_MAX	4
#define END_LIMIT			0
#define SPD_LIMIT1			1
#define SPD_LIMIT2			2
#define SPD_LIMIT3			3
	
	double pos_limit[MOTION_ID_MAX][DIRECTION_MAX][POS_LIMIT_TYPE_MAX] = {//# 極限寸法								//# 各種寸法
	{{ 79.2,0.0,0.0,0.0},	{ -5.8,0.0,0.0,0.0}},				//[ID_HOIST]
	{{ 200.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_GANTRY]
	{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_TROLLY]
	{{ 67.0,0.0,0.0,0.0},	{ 20.5,0.0,0.0,0.0}},				//[ID_BOOM_H]
	{{ 0.0,0.0,0.0,0.0},	{ 0.0,0.0,0.0,0.0}},				//[ID_SLEW]
	{{ 88.2,0.0,0.0,0.0},	{-5.65,0.0,0.0,0.0}},				//[ID_AHOIST]
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
	double Lm = 63.000;			//ジブpin-主巻シーブ中心距離
	double La_add = 4.981;		//ジブ主補巻シーブ間距離
	double rad_mh_ah = 2.371;	//ジブ主補巻シーブ間角度
	double La = 66.566;			//ジブpin-補巻シーブ中心距離
	double rad_Lm_La = 0.0537;	//ジブ主補巻シーブ間角度
	double Lmb = 1.676;
	double Lma = 4.981;
	double Lba = 5.512;
	double Th_pba = 2.00301;
	double Alpa_db = 0.90266;
	double Alpa_b = 0.02109;		//ジブ主巻軸−起伏軸間角度
	double Alpa_a = 0.05373;		//ジブ補巻軸−主巻軸間角度
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
			{563.598,1.571,	563.598,525.906,0.968,	402.636,0.0,	0.0}	// 基準ワイヤ長(旋回はTTB円周
	};

	double Th_rest	= -0.7113;	//ジブレスト時の起伏角
	double D_min	= 41.425;	//最小半径時の起伏シーブ間距離
	double D_rest	= 83.905;	//ジブレスト時の起伏シーブ間距離
	double Hb_min	= 59.397;	//最小半径時の起伏高さ
	double Hb_rest	= -41.786;	//ジブレスト時起伏高さ
	double Dttb		= 3.652;	//TTB径
	double Kttb		= 0.08434;	//ピニオン径/TTB径
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


#define DRIVE_ITEM_NUM			8		// ドライブ関連パラメータ項目数
#define DRIVE_ITEM_RPS			0		// モータ定格回転数（100%）
#define DRIVE_ITEM_GEAR			1		// 減速比
#define DRIVE_ITEM_ACC_TIME		2		// 加速時間（100%)
#define DRIVE_ITEM_RATE_V		3		// 定格速度（100%)
#define DRIVE_ITEM_RATE_NV		4		// ドラム回転速度（100%)
#define DRIVE_ITEM_RATE_NA		5		// ドラム回転加速度（100%)
#define DRIVE_ITEM_NMAX_RATIO	6		// ドラム最大回転数比率（100%->1.0)
#define DRIVE_ITEM_RPM			7		// モータ定格回転数（100%）


	double prm_drv[DRIVE_ITEM_NUM][MOTION_ID_MAX]{
	//	HST,		GNT,		BHMH,		BH,			SLW,		AH,			ASSY,	COM
		{12.5,		28.333,		0.0,		29.1667,	29.1667,	16.667,		0.0,	0.0},	// モータ定格回転数（rps　100%）
		{0.007823,	0.011234,	0.0,		0.004399,	0.002336,	0.008418,	0.0,	0.0},	// 減速比(ドラム回転/モータ回転）
		{1.5,		8.0,		0.0,		1.5,		9.0,		1.5,		0.0,	0.0},	// 加速時間（100%)
		{0.1170,	0.5,		0.0,		0.2080,		0.035,		0.2000,		0.0,	0.0},	// 定格速度（m/s, rad/s：100％)
		{0.09775,	0.3183,		0.0,		0.1283,		0.06815,	0.1403,		0.0,	0.0},	// 定格ドラム回転速度（100％rps)
		{0.065,		0.0398,		0.0,		0.8553,		0.007572,	0.0935,		0.0,	0.0},	// 定格ドラム回転加速度（rps2)
		{3.0,		1.0,		1.0,		1.0,		1.0,		2.0,		0.0,	0.0},	// ドラム最大回転数比率（100%->1.0)
		{750.0,		1700.0,		0.0,		1750.0,		1750.0,		1000.0,		0.0,	0.0}	// モータ定格回転数（rps　100%）
	};

}ST_SPEC, * LPST_SPEC;




