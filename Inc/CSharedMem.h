#pragma once

#include <string>
#include "common_def.h"
#include "spec.h"
#include "CVector3.h"
#include "SwySensor_DEF.h"
#include "OTE_DEF.h"
#include "PLC_DEF.h"


#define SMEM_CRANE_STATUS_NAME			L"CRANE_STATUS"
#define SMEM_SWAY_STATUS_NAME			L"SWAY_STATUS"
#define SMEM_OPERATION_STATUS_NAME		L"OPERATION_STATUS"
#define SMEM_FAULT_STATUS_NAME			L"FAULT_STATUS"
#define SMEM_SIMULATION_STATUS_NAME		L"SIMULATION_STATUS"
#define SMEM_PLC_IO_NAME				L"PLC_IO"
#define SMEM_SWAY_IO_NAME				L"SWAY_IO"
#define SMEM_REMOTE_IO_NAME				L"REMOTE_IO"
#define SMEM_CS_INFO_NAME				L"CS_INFO"
#define SMEM_POLICY_INFO_NAME			L"POLICY_INFO"
#define SMEM_AGENT_INFO_NAME			L"AGENT_INFO"
#define SMEM_OTE_IO_NAME				L"OTE_IO"
#define SMEM_CLIENT_IO_NAME				L"CLIENT_IO"
#define SMEM_JOB_IO_NAME				L"JOB_IO"

#define MUTEX_CRANE_STATUS_NAME			L"MU_CRANE_STATUS"
#define MUTEX_SWAY_STATUS_NAME			L"MU_SWAY_STATUS"
#define MUTEX_OPERATION_STATUS_NAME		L"MU_OPERATION_STATUS"
#define MUTEX_FAULT_STATUS_NAME			L"MU_FAULT_STATUS"
#define MUTEX_SIMULATION_STATUS_NAME	L"MU_SIMULATION_STATUS"
#define MUTEX_PLC_IO_NAME				L"MU_PLC_IO"
#define MUTEX_SWAY_IO_NAME				L"MU_SWAY_IO"
#define MUTEX_OTE_IO_NAME				L"MU_OTE_IO"
#define MUTEX_CS_INFO_NAME				L"MU_CS_INFO"
#define MUTEX_POLICY_INFO_NAME			L"MU_POLICY_INFO"
#define MUTEX_AGENT_INFO_NAME			L"MU_AGENT_INFO"
#define MUTEX_CLIENT_IO_NAME			L"MU_CLIENT_IO"
#define MUTEX_JOB_IO_NAME				L"MU_JOB_IO"

#define SMEM_OBJ_ID_CRANE_STATUS		0
#define SMEM_OBJ_ID_SWAY_STATUS			1
#define SMEM_OBJ_ID_OPERATION_STATUS	2
#define SMEM_OBJ_ID_FAULT_STATUS		3
#define SMEM_OBJ_ID_SIMULATION_STATUS	4
#define SMEM_OBJ_ID_PLC_IO				5
#define SMEM_OBJ_ID_SWAY_IO				6
#define SMEM_OBJ_ID_REMOTE_IO			7
#define SMEM_OBJ_ID_CS_INFO				8
#define SMEM_OBJ_ID_POLICY_INFO			9
#define SMEM_OBJ_ID_AGENT_INFO			10
#define SMEM_OBJ_ID_CLIENT_IO			11
#define SMEM_OBJ_ID_JOB_IO				12

//  共有メモリステータス
#define	OK_SHMEM						0	// 共有メモリ 生成/破棄正常
#define	ERR_SHMEM_CREATE				-1	// 共有メモリ Create異常
#define	ERR_SHMEM_VIEW					-2	// 共有メモリ View異常
#define	ERR_SHMEM_NOT_AVAILABLE			-3	// 共有メモリ View異常
#define	ERR_SHMEM_MUTEX					-4	// 共有メモリ View異常

#define SMEM_DATA_SIZE_MAX				1000000	//共有メモリ割り当て最大サイズ　1Mbyte	

using namespace std;

typedef struct StMoveSet {
	double p;
	double v;
	double a;
}ST_MOVE_SET, * LPST_MOVE_SET;

/****************************************************************************/
/*   PLC IO定義構造体                                                     　*/
/* 　PLC_IF PROCがセットする共有メモリ上の情報　　　　　　　　　　　　　　  */
#pragma region PLC IO
#define PLC_IF_REMOTE_OFF_MODE		0x00000000		//機上運転モード（PCからの出力無効,端末にて状態のモニタのみ）
#define PLC_IF_RMT_MODE_0			0x00000001		//遠隔運転モード（PCからの出力有効　通常運用モード）
#define PLC_IF_SIM_MODE_1			0x00000003		//操作端末からの操作入力有効通常運用モード
#define PLC_IF_SIM_MODE_2			0x00000007		//PLC切り離しモード（PLC IFで機器への指定（ブレーキ,MC等）をシミュレート）

#define PLC_DRUM_LAYER_MAX			10				//ドラム層数最大値

// IO割付内容は、PLC_DEF.hに定義
// PLC_状態信号構造体（機上センサ信号)
#define PLC_IF_BRK_OPEN				true
#define PLC_IF_BRK_CLOSE			false

#define PLC_IF_INV_DIO_FWD			0x0001
#define PLC_IF_INV_DIO_REV			0x0002

#define PLC_IF_LIMIT_COM_FWD_NORM		0x0000001
#define PLC_IF_LIMIT_COM_REV_NORM		0x0000002
#define PLC_IF_LIMIT_COM_FWD_EMR		0x0000004
#define PLC_IF_LIMIT_COM_REV_EMR		0x0000008
#define PLC_IF_LIMIT_COM_FWD_SLOW		0x0000010
#define PLC_IF_LIMIT_COM_REV_SLOW		0x0000020
#define PLC_IF_LIMIT_COM_FWD_SLOW2		0x0000040
#define PLC_IF_LIMIT_COM_REV_SLOW2		0x0000080
#define PLC_IF_LIMIT_COM_FWD_NORM_EX1	0x0000100
#define PLC_IF_LIMIT_COM_REV_NORM_EX1	0x0000200
#define PLC_IF_LIMIT_COM_FWD_EMR_EX1	0x0000400
#define PLC_IF_LIMIT_COM_REV_EMR_EX1	0x0000800
#define PLC_IF_LIMIT_COM_FWD_NORM_EX2	0x0001000
#define PLC_IF_LIMIT_COM_REV_NORM_EX2	0x0002000
#define PLC_IF_LIMIT_COM_FWD_EMR_EX2	0x0004000
#define PLC_IF_LIMIT_COM_REV_EMR_EX2	0x0008000
#define PLC_IF_LIMIT_COM_FWD_NORM_EX3	0x0010000
#define PLC_IF_LIMIT_COM_REV_NORM_EX3	0x0020000
#define PLC_IF_LIMIT_COM_FWD_EMR_EX3	0x0040000
#define PLC_IF_LIMIT_COM_REV_EMR_EX3	0x0080000
										   
typedef struct StPLC_IO {
	INT32 mode;
	INT32 healthy_cnt;
	double v_fb[MOTION_ID_MAX];						//速度FB
	double nv_ref[MOTION_ID_MAX];					//PLCのINVへの速度指令出力
	double nv_tg[MOTION_ID_MAX];					//PLC目標速度
	double trq_fb_01per[MOTION_ID_MAX];				//トルクFB
	double pos[MOTION_ID_MAX];						//位置FB
	double weight;									//主巻荷重FB
	double weight_ah;								//補巻荷重FB
	double th_bh;									//起伏角 acos(R/Lm)
	double dth_bh;									//起伏角速度
	double lmh;										//主巻ロープ長
	double lah;										//補巻ロープ長

	INT16 brk[MOTION_ID_MAX];						//ブレーキ状態FB 0閉
	INT32 endlim[MOTION_ID_MAX];					//極限センサ状態
	INT16 notch_ref[MOTION_ID_MAX];					//ノッチ指令入力FB（OTE入力含む）
	PLC_READ_BUF		input;						//PLCからの読み取り信号生値
	PLC_WRITE_BUF		output;						//PLCへの書き込み信号生値

	double notch_spd[MOTION_ID_MAX][NOTCH_MAX];			//# ノッチ速度設定現在値

}ST_PLC_IO, * LPST_PLC_IO;

#pragma endregion PLC IO定義構造体
/****************************************************************************/
/*   操作端末卓信号定義構造体                                  　         　*/
/* 　OTE_IF PROCがセットする共有メモリ上の情報　　　　　　　          　    */
#pragma region OTE
typedef struct StOTE_IO {
	ST_OTE_U_MSG	ote_umsg_in;		//端末からの受信データ（ユニキャスト）
	ST_PC_U_MSG		ote_umsg_out;		//端末への送信データ（ユニキャスト）
	UINT32			id_ote;				//ユニキャスト接続端末ID
	UINT32			ote_u_silent_cnt;  	//受信カウント変化無しでカウントアップ
}ST_OTE_IO, * LPST_OTE_IO;

#pragma endregion 操作端末卓信号定義構造体
/****************************************************************************/
/*   振れセンサ信号定義構造体                                  　         　*/
/* 　SWAY_PC_IFがセットする共有メモリ上の情報　      　　　　　　           */
#pragma region SWAY SENSOR
#define SENSOR_CAM_MAX				 2//カメラ数
#define SENSOR_TARGET_MAX            4//検出ターゲット最大数
#define SID_TG1                      0//ターゲットID
#define SID_TG2                      1
#define SID_TG3                      2
#define SID_TG4                      3

#define TG_LAMP_NUM_MAX              3//ターゲット毎のランプ最大数

#define SID_RED                      0
#define SID_GREEN                    1
#define SID_BLUE                     2

#define SWAY_FAULT_ITEM_MAX			 4//異常検出項目数
#define SID_COMMON_FLT               0

#define SID_CAM_X              ID_SLEW
#define SID_CAM_Y              ID_BOOM_H
#define SID_CAM_XY             ID_TROLLY
#define SID_LOAD_MH            0
#define SID_LOAD_AH            1
#define SID_N_LOAD             2
#define SID_N_AXIS             4

typedef struct StSwayIO {
	DWORD proc_mode;
	DWORD helthy_cnt;

	char sensorID[4];
	WORD mode[SENSOR_TARGET_MAX];							//ターゲットサ検出モード
	WORD status[SENSOR_TARGET_MAX];							//ターゲットサ検出状態
	DWORD fault[SWAY_FAULT_ITEM_MAX];						//センサ異常状態
	double pix_size[SENSOR_TARGET_MAX][TG_LAMP_NUM_MAX];	//ターゲット検出PIXEL数（面積）
	double tilt[SENSOR_CAM_MAX][SID_N_AXIS];				//傾斜角
	double dtilt[SENSOR_CAM_MAX][SID_N_AXIS];				//傾斜角
	double th[SID_N_LOAD][MOTION_ID_MAX];					//吊荷振角			rad
	double dth[SID_N_LOAD][MOTION_ID_MAX];					//吊荷振角速度		rad/s
	double dthw[SID_N_LOAD][MOTION_ID_MAX];					//吊荷振角速度/ω　	rad
	double ph[SID_N_LOAD][MOTION_ID_MAX];					//吊荷位相平面位相	rad
	double rad_amp2[SID_N_LOAD][MOTION_ID_MAX];				//吊荷振幅の2乗		rad2
	double cam_pix[SID_N_LOAD][MOTION_ID_MAX];				//振れセンサカメラPIX位置
}ST_SWAY_IO, * LPST_SWAY_IO;

#pragma endregion 振れセンサ信号定義構造体
/****************************************************************************/
/*   シミュレーション信号定義構造体                                  　   　*/
/* 　SIM PROCがセットする共有メモリ上の情報　　　　　　　          　    　 */
#pragma region SIMULATOR
typedef struct StSimLoad {//負荷
	double m;	//質量
	double wx;	//幅
	double dy;	//奥行
	double hz;	//高さ
}ST_SIM_LOAD, * LPST_SIM_LOAD;

#define SIM_ACTIVE_MODE			0x00000100					//シミュレーション実行モード
#define SIM_SWAY_PACKET_MODE	0x00000010					//振れセンサパケット送信モード
#define SIM_N_SENSOR			128							//センサ数
typedef struct StSimulationStatus {
	DWORD mode;
	DWORD helthy_cnt;
	PLC_WRITE_BUF plc_w;
	ST_SWAY_IO sway_io;
	Vector3 L, vL;											//ﾛｰﾌﾟﾍﾞｸﾄﾙ(主巻振れ）
	Vector3 L2, vL2;										//ﾛｰﾌﾟﾍﾞｸﾄﾙ(補巻振れ）
	double a_fb[MOTION_ID_MAX];								//軸座標加速度fb
	double v_fb[MOTION_ID_MAX];								//軸座標速度fb
	double pos[MOTION_ID_MAX];								//軸座標位置fb
	double mtrq[MOTION_ID_MAX];								//モータトルクfb
	ST_SIM_LOAD load[MOTION_ID_MAX];						//軸負荷（巻は荷重）
	double rad_cam_x, rad_cam_y, w_cam_x, w_cam_y;			//カメラ座標振れ角,振れ角速度
	double kbh;												//引込半径に依存する速度、加速度補正係数
	ST_SWAY_RCV_MSG rcv_msg;
	ST_SWAY_SND_MSG snd_msg;
	INT32 sonsor[SIM_N_SENSOR];								//センサ状態

	ST_MOVE_SET	nd[MOTION_ID_MAX];							//ドラム回転動作
	ST_MOVE_SET	d;									//ポスト‐起伏シーブ間状態（距離・速度・加速度）
	ST_MOVE_SET	db;									//ジブポスト‐起伏シーブ間状態（距離・速度・加速度）
	ST_MOVE_SET	ph;									//φ
	ST_MOVE_SET	phb;								//φ
	ST_MOVE_SET	th;									//θ

	UINT32 i_layer[MOTION_ID_MAX];					//ドラム現在層数
	double n_layer[MOTION_ID_MAX];					//ドラム現在層巻取数
	double l_drum[MOTION_ID_MAX];					//ドラム巻取り量

	ST_MOVE_SET	lrm;								//主巻ロープ長
	ST_MOVE_SET	lra;								//補巻ロープ長

	double T;										//主巻振れ周期
	double Tah;										//補巻振れ周期
	double w;										//主巻振れ角周波数
	double wah;										//補巻振れ角周波数

	double tht_swx_mh, tht_swy_mh, tht_swx_ah, tht_swy_ah;

}ST_SIMULATION_STATUS, * LPST_SIMULATION_STATUS;
#pragma endregion シミュレーション信号定義構造体
//****************************************************************************
/// <summary>
/// クレーン状態定義構造体 
/// Environmentタスクがセットする共有メモリ上の情報　
/// </summary>
#pragma region CRANE STATUS クレーン状態定義構造体
#define DBG_PLC_IO				0x00000001
#define DBG_SWAY_IO				0x00000100
#define DBG_ROS_IO				0x00010000
#define DBG_SIM_ACT				0X01000000

#define N_PC_FAULT_WORDS		16			//制御PC検出フォルトbitセットWORD数
#define N_PLC_FAULT_WORDS		32			//PLC検出フォルトbitセットWORD数

#define OPERATION_MODE_OTE_ACTIVE	0x0000001
#define OPERATION_MODE_OTE_ONBOARD	0x0000002
#define OPERATION_MODE_REMOTE		0x0000004
#define OPERATION_MODE_SIMULATOR	0x0100000
#define OPERATION_MODE_PLC_DBGIO	0x0001000

#define N_SWAY_DIR				4

typedef struct stEnvSubproc {

	bool is_plcio_join = false;
	bool is_sim_join = false;
	bool is_sway_join = false;
	bool is_ote_join = false;

} ST_ENV_SUBPROC, LPST_ENV_SUBPROC;

#define MANUAL_MODE				0
#define ANTI_SWAY_MODE			1
#define SEMI_AUTO_ACTIVE		2
#define AUTO_ACTIVE				3
#define BITSEL_SEMIAUTO			0x0001
#define BITSEL_AUTO				0x0002

#define SPD0_CHECK_RETIO		0.1

#define STAT_ACC				0
#define STAT_DEC				1

#define PB_TRIG_COUNT			1

#define DRUM_LAYER_MAX	10	//ドラ巻取り層数最大値

typedef struct StCraneStatus {
//Event Update				:イベント条件で更新
	bool is_tasks_standby_ok;							//タスクの立ち上がり確認
	bool is_crane_status_ok=false;							//クレーンステータス初期化完了
	ST_SPEC spec;										//クレーン仕様

//Periodical Update			：定周期更新
	DWORD env_act_count=0;								//ヘルシー信号
	ST_ENV_SUBPROC subproc_stat;						//サブプロセスの状態
	WORD operation_mode;								//運転モード　機上,リモート

	Vector3 rc;											//クレーン主巻吊点のクレーン基準点とのx,y,z相対座標
	Vector3 rl;											//主巻吊荷のクレーン吊点とのx,y,z相対座標
	Vector3 rcam_m;										//主巻振れセンサ検出x,y,z座標 m

	Vector3 rc_a;										//クレーン補巻吊点のクレーン基準点とのx,y,z相対座標
	Vector3 rl_a;										//補巻吊荷のクレーン吊点とのx,y,z相対座標
	Vector3 rcam_m_a;									//補巻振れセンサ検出x,y,z座標 m
	
	double notch_spd_ref[MOTION_ID_MAX];				//ノッチ速度指令

	double mh_l;										//ロープ長
	double T;											//振周期		s
	double w;											//振角周波数	/s
	double w2;											//振角周波数の2乗

	double ah_l;										//ロープ長
	double T_ah;										//振周期		s
	double w_ah;										//振角周波数	/s
	double w2_ah;										//振角周波数の2乗
	
	double R;											//旋回半径

	WORD faultPC[N_PC_FAULT_WORDS];						//PLC検出異常
	WORD faultPLC[N_PLC_FAULT_WORDS];					//制御PC検出異常

	bool is_fwd_endstop[MOTION_ID_MAX];					//正転極限判定
	bool is_rev_endstop[MOTION_ID_MAX];					//逆転極限判定

	INT32 OTE_req_status;								//操作端末要求状態

	INT32 notch0;										//0ノッチ判定総合
	INT32 notch0_crane;									//0ノッチ判定PLC


	double Cdr[MOTION_ID_MAX][PLC_DRUM_LAYER_MAX];	//ドラム1層円周
	double Ldr[MOTION_ID_MAX][PLC_DRUM_LAYER_MAX];	//ドラム層巻取量
	double Kdr[MOTION_ID_MAX][PLC_DRUM_LAYER_MAX];	//ドラム層円周倍率

	ST_MOVE_SET	axc[MOTION_ID_MAX];					//軸動作
	ST_MOVE_SET	d;									//主巻ポスト‐起伏シーブ間状態（距離・速度・加速度）
	ST_MOVE_SET	ph;									//φ
	ST_MOVE_SET	th;									//θ
	ST_MOVE_SET	hm0;								//主巻シーブ起伏高さ
	ST_MOVE_SET	ha0;								//補巻シーブ起伏高さ
	ST_MOVE_SET	nd[MOTION_ID_MAX];					//ドラム回転動作
	UINT32 i_layer[MOTION_ID_MAX];					//ドラム現在層数
	double n_layer[MOTION_ID_MAX];					//ドラム現在層巻取数
	double l_drum[MOTION_ID_MAX];					//ドラム巻取り量
	ST_MOVE_SET	lrm;								//主巻ロープ長
	ST_MOVE_SET	lra;								//補巻ロープ長


}ST_CRANE_STATUS, * LPST_CRANE_STATUS;
#pragma endregion 
/****************************************************************************/
/*   JOB COMMAND情報管理構造体                                   　   　	*/
/* 　JOB,COMMAND生成、実行に関する情報　　　　　　　 　						*/
#pragma region JOB COMMAND
/****************************************************************************/
/*   運動要素定義構造体                                                     */
/* 　加速、定速、減速等の一連の動作は、この要素の組み合わせで構成します。   */
/****************************************************************************/

//レシピ　Type
#define CTR_TYPE_WAIT_TIME					0x0000	//待機（時間経過待ち）
#define CTR_TYPE_WAIT_POS_HST				0x0101	//巻到達待ち
#define CTR_TYPE_WAIT_POS_GNT				0x0102	//走行到達待ち
#define CTR_TYPE_WAIT_POS_TRY				0x0104	//横行到達待ち
#define CTR_TYPE_WAIT_POS_BH				0x0108	//引込到達待ち
#define CTR_TYPE_WAIT_POS_SLW				0x0110	//旋回到達待ち
#define CTR_TYPE_WAIT_LAND					0x0120	//着床待ち
#define CTR_TYPE_WAIT_PH_SINGLE				0x0201	//位相待ち 1点
#define CTR_TYPE_WAIT_PH_DOUBLE				0x0202	//位相待ち 2点

#define CTR_TYPE_VOUT_TIME					0x1000  //ステップ速度　時間完了
#define CTR_TYPE_VOUT_V						0x1001  //ステップ速度　速度到達完了
#define CTR_TYPE_VOUT_POS					0x1002  //ステップ速度　位置到達完了
#define CTR_TYPE_VOUT_PHASE     			0x1004  //ステップ速度　位相到達完了
#define CTR_TYPE_VOUT_LAND					0x1008  //ステップ速度　着床完了

#define CTR_TYPE_AOUT_TIME					0x2000  //加速速度　時間完了
#define CTR_TYPE_AOUT_V						0x2001  //加速速度　速度到達完了
#define CTR_TYPE_AOUT_POS					0x2002  //加速速度　位置到達完了
#define CTR_TYPE_AOUT_PHASE     			0x2004  //加速速度　位相到達完了
#define CTR_TYPE_AOUT_LAND					0x2008  //加速速度　着床完了

#define CTR_TYPE_FINE_POS					0x8001	//微小位置合わせ
#define CTR_TYPE_FB_SWAY					0x8002	//FB振れ止め
#define CTR_TYPE_FB_SWAY_POS				0x8004	//FB振れ止め位置決め

#define TIME_LIMIT_CONFIRMATION				0.1		//パターン出力調整時間 秒
#define TIME_LIMIT_FINE_POS					10.0	//微小位置合わせ制限時間 秒
#define TIME_LIMIT_ERROR_DETECT				120		//異常検出時間
#define N_STEP_OPTION_MAX					8

#define STEP_OPT_PHASE_SINGLE				0		//起動位相の配列インデックス　SINGLE
#define STEP_OPT_PHASE_FWD					0		//起動位相の配列インデックス　DOUBLE　正方向用
#define STEP_OPT_PHASE_REV					1		//起動位相の配列インデックス　DOUBLE　逆方向用
#define STEP_OPT_PHASE_CHK_RANGE			2		//起動位相の配列インデックス　DOUBLE　逆方向用

typedef struct stMotionElement {	//運動要素
	//recipe
	int type;								//制御種別
	double _a;								//目標加減速度
	double _v;								//目標速度
	double _p;								//目標位置
	double _t;								//予定継続時間
	int time_count;							//予定継続時間のカウンタ変換値
	double opt_d[N_STEP_OPTION_MAX];		//オプションdouble
	int opt_i[N_STEP_OPTION_MAX];			//オプションint
	//status
	int act_count;
	int status;
}ST_MOTION_STEP, * LPST_MOTION_STEP;

/****************************************************************************/
/*   動作内容定義構造体（単軸）												*/
/* 　単軸の目標状態に移行する動作パターンを運動要素の組み合わせで実現します */
/****************************************************************************/
#define M_STEP_MAX	32

//Recipe
typedef struct stMotionRecipe {					//移動パターン
	//CS set
	int axis_id;
	int motion_type;
	int option_i;								//オプション条件
	int direction;								//動作方向(-1,0,+1)
	int n_step;									//動作構成要素数
	int time_limit;								//タイムオーバー判定値
	ST_MOTION_STEP steps[M_STEP_MAX];			//動作定義要素配列

	//Agent set
	int i_hot_step;								//実行中要素配列index -1で完了
	int motion_act_count;						//動作実行時間カウントカウント数
	int fin_code;								//完了コード

}ST_MOTION_RECIPE, * LPST_MOTION_RECIPE;

/********************************************************************************/
/*   軸連動運転内容(COMMAND)定義構造体                             　　　　　　 */
/* 　目的動作を実現する運転内容を単軸動作の組み合わせで実現します               */
/********************************************************************************/

/*** コマンド種類 ***/
#define COM_TYPE_MASK			0x0F00      
#define COM_TYPE_PICK			0x0100
#define COM_TYPE_GRND			0x0200
#define COM_TYPE_PARK			0x0400
#define COM_TYPE_FROM_TO		0x0800
#define COM_TYPE_NON 			0x0000

/*** コマンド種類 ***/
#define STAT_MASK               0x00FF      //ステータス部マスク
#define STAT_NA					0x0000      //適用不可
#define STAT_STANDBY			0x0001      //適用不可
#define STAT_TRIGED             0x0002      //起動済実行待ち
#define STAT_ACTIVE             0x0004      //実行中報告
#define STAT_SUSPENDED          0x0008      //一時停止報告
#define STAT_ABOTED             0x0010      //中断
#define STAT_END				0x0020      //正常完了
#define STAT_REQ_WAIT           0x0080      //要求待ち
#define STAT_POSITIVE			0x1000		//OK
#define STAT_SUCCEED			0x1000		//成功
#define STAT_ACK				0x1001		//正常受付
#define STAT_NAK				0x8000		//NG
#define STAT_ABNORMAL			0x8000		//失敗
#define STAT_TIME_OVER			0x8010		//タイムオーバー
#define STAT_ABNORMAL_END       0x8020      //異常完了
#define STAT_LOGICAL_ERROR		0x8004      //整合性異常
#define STAT_CODE_ERROR			0x8008      //適合コード無し

typedef struct StPosTargets {
	double pos[MOTION_ID_MAX];
	bool is_held[MOTION_ID_MAX];				//目標位置ホールド中フラグ
}ST_POS_TARGETS, * LPST_POS_TARGETS;

typedef struct StCommandCode {
	int i_list;
	int i_job;
	int i_recipe;
}ST_COM_CODE, * LPST_COM_CODE;

typedef struct stCommandSet {
	//POLICY SET
	ST_COM_CODE com_code;
	int n_motion;									//コマンドで動作する軸の数
	int active_motion[MOTION_ID_MAX];				//コマンド動作する軸のID配列（MOTION　RECIPEの対象配列）
	ST_MOTION_RECIPE recipe[MOTION_ID_MAX];
	ST_POS_TARGETS target;							//目標位置	

	//AGENT SET
	int motion_status[MOTION_ID_MAX];
	int recipe_counter[MOTION_ID_MAX];
	int com_status;

}ST_COMMAND_SET, * LPST_COMMAND_SET;

#define JOB_COMMAND_MAX				10			//　JOBを構成するコマンド最大数

//# Policy タスクセット領域

#define MODE_PC_CTRL		0x00000001
#define MODE_ANTISWAY		0x00010000
#define MODE_RMOTE_PANEL	0x00000100

/************************************************************************************/
/*   作業内容（JOB)定義構造体                                 　     　　　　　　	*/
/* 　ClientService タスクがセットする共有メモリ上の情報								*/
/* 　JOB	:From-Toの搬送コマンド													*/
/*   COMMAND:1つのJOBを、複数のコマンドで構成	PICK GRAND PARK						*/
/* 　JOB	:From-Toの搬送作業													*/
/************************************************************************************/
#define JOB_REGIST_MAX				10			//　JOB登録最大数
#define JOB_N_STEP_SEMIAUTO			1
#define COM_RECIPE_OPTION_N			8

typedef struct stComRecipe {
	//CS SET
	int id;
	int type;									//JOB種別（PICK,GRND,PARK...）
	int time_limit;								//JOB構成コマンド数
	ST_POS_TARGETS target;						//各コマンドの目標位置	

	int option_i[COM_RECIPE_OPTION_N];
	double option_d[COM_RECIPE_OPTION_N];

	ST_COMMAND_SET comset;						//レシピを展開したコマンドセット
	int status;

	SYSTEMTIME time_start;
	SYSTEMTIME time_end;
}ST_COM_RECIPE, * LPST_COM_RECIPE;

typedef struct stJobSet {
	int list_id;								//登録されているJOB listのid
	int id;										//登録されているJOB list内でのid
	int status;									//現在実行対象のJOBの状態
	int n_com;									//JOB構成コマンド数
	int type;									//JOB種別（JOB,半自動,OPERATION））
	ST_COM_RECIPE recipe[JOB_COMMAND_MAX];	//各コマンドのレシピ
	int i_hot_com;

	SYSTEMTIME time_start;
	SYSTEMTIME time_end;
}ST_JOB_SET, * LPST_JOB_SET;

//JOB LIST
typedef struct _stJobList {
	int id;
	int type;									//JOB種別（JOB,半自動）
	int n_hold_job;								//未完Job数
	int i_job_hot;								//次完了待ちJob(実行中or待機中）	  id
	ST_JOB_SET job[JOB_REGIST_MAX];				//登録job
}ST_JOB_LIST, * LPST_JOB_LIST;

#define N_JOB_LIST						2				//JOB LIST登録数
#define ID_JOBTYPE_JOB					0				//JOB Type index番号
#define ID_JOBTYPE_SEMI					1				//SEMIAUTO Type index番号
#define ID_JOBTYPE_ANTISWAY				2				//FB ANTISWAY Type index番号
#define JOB_HOLD_MAX					10				//保持可能JOB最大数

typedef struct stJobIO {
	ST_JOB_LIST	job_list[N_JOB_LIST];
}ST_JOB_IO, * LPST_JOB_IO;

#pragma endregion 作業内容（JOB)定義構造体 
/****************************************************************************/
/*   Client Service	情報定義構造体                                   　   　*/
/* 　Client Serviceタスクがセットする共有メモリ上の情報　　　　　　　 　    */
#pragma region CLIENT

#define CS_SEMIAUTO_TG_SEL_DEFAULT      0
#define CS_SEMIAUTO_TG_SEL_CLEAR		0
#define CS_SEMIAUTO_TG_SEL_ACTIVE       1
#define CS_SEMIAUTO_TG_SEL_FIXED        2
#define CS_SEMIAUTO_TG_MAX				16

#define CS_ID_SEMIAUTO_TOUCH_PT			8

#define CS_ID_NOTCH_POS_HOLD			0
#define CS_ID_NOTCH_POS_TRIG			1

#define CS_CODE_OTE_REMOTE_ENABLE		0x1

typedef struct stCSInfo {
	//UI関連
	ST_OTE_LAMP_COM ote_pb_lamp[N_OTE_PNL_PB];							//端末ランプ表示指令
	ST_OTE_LAMP_COM ote_notch_lamp[N_OTE_PNL_NOTCH];					//端末ランプ表示指令
	int	semi_auto_selected;												//選択中の半自動ID
	int ote_remote_status;												//操作端末の状態　0bit：端末信号有効
	INT16	notch_pos[2][MOTION_ID_MAX];								//ノッチ指定値

	ST_POS_TARGETS semi_auto_setting_target[CS_SEMIAUTO_TG_MAX];		//半自動設定目標位置
	ST_POS_TARGETS semi_auto_selected_target;							//半自動選択目標位置
	INT32 semi_auto_selected_target_for_view[MOTION_ID_MAX];			//半自動選択目標位置(カメラ座標）
	INT32 hunging_point_for_view[MOTION_ID_MAX];						//半自動選択目標位置(カメラ座標）	

	int command_type;													//PARK,PICK,GRND
	int tg_sel_trigger_z = L_OFF, tg_sel_trigger_xy = L_OFF;			//目標位置の設定入力（半自動PB、モニタタッチ）があったかどうかの判定値
	int target_set_z = CS_SEMIAUTO_TG_SEL_FIXED, target_set_xy = CS_SEMIAUTO_TG_SEL_FIXED;		//Z座標目標位置確定
	LPST_JOB_SET p_active_job;
	int job_set_event;

	//自動,遠隔設定（モード）
	int auto_mode;														//自動モード
	int antisway_mode;													//振れ止めモード
	int estop_active;													//非常停止動作中
	int ote_notch_dist_mode;											//タブレット目標入力　移動距離指定

	double ote_camera_height_m;											//操作端末VIEWのカメラ設置高さ

}ST_CS_INFO, * LPST_CS_INFO;

#define N_JOB_TARGET_MAX	10
#define N_JOB_OPTION_MAX	10

typedef struct stClientRcvMSG {
	int req_code;
	int JOB_ID;
	double target[N_JOB_TARGET_MAX][MOTION_ID_MAX];
	int option[N_JOB_OPTION_MAX];
}ST_CLIENT_RCV_MSG, * LPST_CLIENT_RCV_MSG;

typedef struct stClientSndMSG {
	int fb_code;
	int JOB_ID;
	int option[N_JOB_OPTION_MAX];
}ST_CLIENT_SND_MSG, * LPST_CLIENT_SND_MSG;

#define N_CLIENT_MSG_HOLD_MAX	10

typedef struct stClientIO {
	int i_rcv_hot;
	int i_snd_hot;
	ST_CLIENT_RCV_MSG rmsg[N_CLIENT_MSG_HOLD_MAX];
	ST_CLIENT_SND_MSG smsg[N_CLIENT_MSG_HOLD_MAX];
}ST_CLIENT_IO, * LPST_CLIENT_IO;

#pragma endregion Client Service情報定義構造体
/****************************************************************************/
/*   Policy	情報定義構造体                                   　			  　*/
/* 　Policy	タスクがセットする共有メモリ上の情報　　　　　　　		 　		*/
#pragma region POLICY
#define FAULT_MAP_W_SIZE	64	//フォルトマップサイズ
typedef struct stPolicyInfo {

	WORD fault_map[FAULT_MAP_W_SIZE];

}ST_POLICY_INFO, * LPST_POLICY_INFO;
#pragma endregion Policy情報定義構造体
/****************************************************************************/
/*   Agent	情報定義構造体                                   　   　		*/
/* 　Agent	タスクがセットする共有メモリ上の情報　　　　　　　 　			*/
#pragma region AGENT

/*** ジョブ,コマンドステータス ***/
//auto_ongoing用
#define AUTO_TYPE_JOB_MASK       0x7000      //JOB種別部マスク
#define AUTO_TYPE_SEL_MASK       0xf000      //自動種別部マスク
#define AUTO_TYPE_MANUAL		 0x0000
#define AUTO_TYPE_JOB            0x1000      //種別JOB
#define AUTO_TYPE_SEMIAUTO       0x2000      //種別半自動
#define AUTO_TYPE_OPERATION      0x4000      //種別クレーン操作
#define AUTO_TYPE_FB_ANTI_SWAY	 0x8000

//antisway_on_going用
#define ANTISWAY_ALL_MANUAL		 0x00000000      //振れ止め無し
#define ANTISWAY_BH_ACTIVE		 0x00000001      //引込振止実行中
#define ANTISWAY_BH_PAUSED		 0x00000002      //引込振止一時停止
#define ANTISWAY_BH_COMPLETE	 0x00000008      //引込振止完了
#define ANTISWAY_SLEW_ACTIVE	 0x00000010      //旋回振止実行
#define ANTISWAY_SLEW_PAUSED	 0x00000020      //旋回振止一時停止
#define ANTISWAY_SLEW_COMPLETE	 0x00000080      //旋回振止完了 

//axis_status用
#define AG_AXIS_STAT_FB0				0x000000001	 //停止中
#define AG_AXIS_STAT_PC_ENABLE			0x800000000	 //PC指令可
#define AG_AXIS_STAT_AUTO_ENABLE		0x100000000	 //自動可
#define AG_AXIS_STAT_ANTISWAY_ENABLE	0x200000000	 //振れ止め可

typedef struct stAgentInfo {

	ST_COMMAND_SET st_as_comset;					//振れ止め用コマンドセット
	ST_POS_TARGETS auto_pos_target;					//自動目標位置
	double dist_for_target[MOTION_ID_MAX];			//目標までの距離
	int auto_on_going;								//実行中の自動種別
	int antisway_on_going;							//実行中振れ止め
	int pc_ctrl_mode;								//PCからの指令で動作させる軸の指定
	int auto_active[MOTION_ID_MAX];					//自動実行中フラグ(軸毎)
	int axis_status[MOTION_ID_MAX];					//各軸の状態（fb0,異常等）

	double v_ref[MOTION_ID_MAX];					//速度指令出力値
	ST_COMMAND_SET st_active_com_inf;				//実行中コマンドセット情報

	int as_count[MOTION_ID_MAX];					//振れ止めレシピ作成呼び出し回数
	int command_count;								//コマンドレシピ作成呼び出し回数


}ST_AGENT_INFO, * LPST_AGENT_INFO;

#pragma endregion AGENT情報定義構造体
/****************************************************************************/
/*共有メモリクラス定義														*/
static char smem_dummy_buf[SMEM_DATA_SIZE_MAX];
class CSharedMem
{
public:
	CSharedMem();
	~CSharedMem();

	int smem_available;			//共有メモリ有効
	int data_size;				//データサイズ

	int create_smem(LPCTSTR szName, DWORD dwSize, LPCTSTR szMuName);
	int delete_smem();
	int clear_smem();

	wstring wstr_smem_name;

	HANDLE get_hmutex() { return hMutex; }
	LPVOID get_pMap() { return pMapTop; }

protected:
	HANDLE hMapFile;
	LPVOID pMapTop;
	DWORD  dwExist;

	HANDLE hMutex;
};


