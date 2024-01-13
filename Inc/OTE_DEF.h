#pragma once

//#include <winsock.h>

#include "PLC_DEF.h"

#define ID_OTE0					0xFF
#define ID_PC0					0x1FF

//IPアドレス
#define OTE_IF_UNICAST_IP_PC				"192.168.1.100" 		//PCのOTE UNICAST　IF用IP	（受信,送信）
#define OTE_IF_UNICAST_IP_OTE0				OTE_IF_UNICAST_IP_PC	//デフォルトOTEのUNICAST用IP（受信,送信）

//#define IP_ADDR_PC_UNI_CAST_OTE		"192.168.1.100"
//#define IP_ADDR_OTE_UNI_CAST_OTE		"127.0.0.1"

#define OTE_IF_MULTICAST_IP_PC				"239.1.0.2"				//PCマルチキャスト用IP
#define OTE_IF_MULTICAST_IP_OTE				"239.1.0.1"				//OTEマルチキャスト用IP

#define OTE_IF_UNICAST_PORT_PC				10050					//ユニキャストPC受信ポート
#define OTE_IF_UNICAST_PORT_OTE				10051					//ユニキャスト端末受信ポート

#define OTE_IF_MULTICAST_PORT_PC2OTE		20080					//PC発信マルチキャストOTE受信ポート
#define OTE_IF_MULTICAST_PORT_OTE2PC		20081					//OTE発信マルチキャストPC受信ポート
#define OTE_IF_MULTICAST_PORT_PC2PC			20082					//PC発信マルチキャストPC受信ポート
#define OTE_IF_MULTICAST_PORT_OTE2OTE		20083					//OTE発信マルチキャストOTE受信ポート

#define ID_OTE_MODE_REMOTE_OPE				0x00000001
#define ID_OTE_MODE_MONITOR					0x00000002
#define ID_OTE_MODE_REMOTE_ACTIVE			0x00000010

//操作端末のボタン、ランプ数
#define N_OTE_PNL_ITEMS			128
#define N_OTE_PNL_STATIC		128
#define N_OTE_PNL_NOTCH			128
#define N_OTE_PNL_PB			128

//**************************************************************************************
//通信電文フォーマット
//**************************************************************************************
#define CODE_ITE_RES_ACK		1
#define CODE_ITE_RES_NAK		1


/******* 共通メッセージヘッダ部 ***********/
typedef struct OteComHead {
	INT32       myid;		//装置ID
	INT32       code;		//要求内容コード
	SOCKADDR_IN addr;		//送信元受信アドレス
	INT32       status;		//状態
	INT32       tgid;		//接続中機器ID
}ST_OTE_HEAD, * LPST_OTE_HEAD;
/******* PCマルチキャストメッセージ ***********/
typedef struct PcMBody {
	INT32 pos[MOTION_ID_MAX];
}ST_PC_M_BODY, * LPST_PC_M_BODY;

typedef struct PcMMsg {
	ST_OTE_HEAD         head;
	ST_PC_M_BODY		body;
}ST_PC_M_MSG, * LPST_PC_M_MSG;
/******* OTEマルチキャストメッセージ構造体 ***********/
#define N_CRANE_PC_MAX      32
typedef struct OteMBody {
	UCHAR       pc_enable[N_CRANE_PC_MAX];		//接続可能端末フラグ
	INT32	    n_remote_wait;					//接続待ち遠隔操作卓台数
	INT32	    onbord_seqno;					//機側接続シーケンス番号
	INT32	    remote_seqno;					//遠隔卓接続シーケンス番号
	INT32	    my_seqno;						//自身の接続シーケンス番号
}ST_OTE_M_BODY, * LPST_OTE_M_BODY;
typedef struct OteMMsg {
	ST_OTE_HEAD      head;
	ST_OTE_M_BODY    body;
}ST_OTE_M_MSG, * LPST_OTE_M_MSG;
/******* PCユニキャストメッセージ構造体 ***********/
#define PLC_IO_MONT_WORD_NUM		100

#define ID_PUC_BODY_LD_MHX				0
#define ID_PUC_BODY_LD_MHY				1
#define ID_PUC_BODY_LD_MHZ				2
#define ID_PUC_BODY_LD_MHTH				3
#define ID_PUC_BODY_LD_AHX				0
#define ID_PUC_BODY_LD_AHY				1
#define ID_PUC_BODY_LD_AHZ				2
#define ID_PUC_BODY_LD_AHTH				3

#define ID_PUC_BODY_TG_MAIN				0
#define ID_PUC_BODY_TG_S1				1
#define ID_PUC_BODY_TG_S2				2
#define ID_PUC_BODY_TG_S3				3
#define ID_PUC_BODY_TG_L1				4
#define ID_PUC_BODY_TG_L2				5
#define ID_PUC_BODY_TG_L3				6

#define OTE_LAMP_COM_OFF				0
#define OTE_LAMP_COM_ON					1
#define OTE_LAMP_COM_FLICK				2

#define OTE_LAMP_FLICK_COUNT			0x0008
#define OTE_STATIC_UPDATE_COUNT			0x0004


typedef struct OteLampCom {
	UINT8 com;
	UINT8 color;
}ST_OTE_LAMP_COM, * LPST_OTE_LAMP_COM;

typedef struct PcUBody {
	ST_OTE_LAMP_COM		pb_lamp[N_OTE_PNL_PB];
	ST_OTE_LAMP_COM		notch_lamp[N_OTE_PNL_NOTCH];
	//INT16				plc_in[PLC_IO_MONT_WORD_NUM];//PLC IFデータ
	ST_PLC_READ			plc_in;//PLC IFデータ
	INT32				pos[MOTION_ID_MAX];			//位置FB
	INT32				v_fb[MOTION_ID_MAX];		//速度FB
	INT32				v_ref[MOTION_ID_MAX];		//速度指令
	INT32				ld_pos[8];					//吊荷位置FB
	INT32				ld_spd[8];					//吊荷位置FB
	INT16				brk[MOTION_ID_MAX];			//ブレーキ状態
	INT16				tg_pos[8][MOTION_ID_MAX];	//目標位置座標
}ST_PC_U_BODY, * LPST_PC_U_BODY;
typedef struct PcUMsg {
	ST_OTE_HEAD     head;
	ST_PC_U_BODY    body;
}ST_PC_U_MSG, * LPST_PC_U_MSG;
/******* OTEユニキャスト通信メッセージ構造体 ***********/

#define ID_OTE_NOTCH_POS_HOLD	0
#define ID_OTE_NOTCH_POS_TRIG	1
#define ID_OTE_NOTCH_POS_CNT	2

typedef struct OteUBody {
	UINT16		pb_ope[128];					//操作卓PB入力
	UINT16		pb_notch[128];				//操作卓ノッチ入力
	INT16		notch_pos[2][MOTION_ID_MAX];	//ノッチ入力位置
	INT16		tg_pos[8];					//設定目標位置
}ST_OTE_U_BODY, * LPST_OTE_U_BODY;
typedef struct OteUMsg {
	ST_OTE_HEAD         head;
	ST_OTE_U_BODY    body;
}ST_OTE_U_MSG, * LPST_OTE_U_MSG;




