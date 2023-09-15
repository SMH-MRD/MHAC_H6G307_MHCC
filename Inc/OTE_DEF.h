#pragma once

#include <winsock.h>
#include <time.h>

#define OTE_IF_UNICAST_PORT_PC			10050	//端末ユニキャストIF受信ポート
#define OTE_IF_UNICAST_PORT_OTE			10051	//端末ユニキャストIF受信ポート

#define OTE_IF_PC_MULTICAST_PORT_PC		20080	//制御Pマルチキャスト受信ポート
#define OTE_IF_OTE_MULTICAST_PORT_PC	20081	//制御Pマルチキャスト受信ポート
#define OTE_IF_PC_MULTICAST_PORT_OTE	20082	//制御Pマルチキャスト受信ポート
#define OTE_IF_OTE_MULTICAST_PORT_OTE	20083	//制御Pマルチキャスト受信ポート

#define ID_OTE_EVENT_CODE_CONST             0
#define ID_OTE_EVENT_CODE_STAT_REPORT       1
#define ID_OTE_EVENT_CODE_CONNECTED         1

#define ID_TE_CONNECT_STATUS_OFF_LINE       0
#define ID_TE_CONNECT_STATUS_STANDBY        3
#define ID_TE_CONNECT_STATUS_ACTIVE         1
#define ID_TE_CONNECT_STATUS_WAITING        2

#define ID_PC_CONNECT_CODE_ENABLE           1
#define ID_PC_CONNECT_CODE_DISABLE          0

//電文フォーマット
/******* 共通メッセージヘッダ部 ***********/
typedef struct OteComHead {
	INT32       myid;		//装置ID
	INT32       code;		//要求内容コード
	SOCKADDR_IN addr;		//送信元受信アドレス
	INT32       status;		//状態
	INT32       tgid;		//接続中機器ID
}ST_OTE_HEAD, * LPST_OTE_HEAD;
/******* PCマルチキャストメッセージ（ヘッダのみ） ***********/
typedef struct PcMMsg {
	ST_OTE_HEAD         head;
}ST_PC_M_MSG, * LPST_PC_M_MSG;
/******* OTEマルチキャストメッセージ構造体 ***********/
#define N_CRANE_PC_MAX      32
typedef struct OteMBody {
	UCHAR       pc_enable[N_CRANE_PC_MAX];	//接続可能端末フラグ
	INT32	    n_remote_wait;  //接続待ち遠隔操作卓台数
	INT32	    onbord_seqno;   //機側接続シーケンス番号
	INT32	    remote_seqno;   //遠隔卓接続シーケンス番号
	INT32	    my_seqno;       //自身の接続シーケンス番号
}ST_OTE_M_BODY, * LPST_OTE_M_BODY;
typedef struct OteMMsg {
	ST_OTE_HEAD         head;
	ST_OTE_M_BODY    body;
}ST_OTE_M_MSG, * LPST_OTE_M_MSG;
/******* PCユニキャストメッセージ構造体 ***********/
#define PLC_IO_MONT_WORD_NUM		112
typedef struct PcUBody {
	INT32      pos[8];                         //位置FB
	INT32      v_fb[8];                        //速度FB
	INT32      v_ref[8];                       //速度指令
	INT32      hp_pos[4];                      //吊点位置FB
	INT32      ld_pos[4];                      //吊荷位置FB
	INT32      ld_v_fb[4];                     //吊荷速度FB
	INT32      tg_pos[4];                      //目標位置座標1
	INT32      tg_pos_semi[6][4];              //半自動目標位置座標S1-L3
	INT16      lamp[64];                       //ランプ表示
	INT16      notch_pos[8];                   //ノッチランプ表示
	INT16      cam_inf[8];                     //遠隔操作カメラ情報
	INT16	   plc_data[PLC_IO_MONT_WORD_NUM]; //PLCモニタリングデータ
}ST_PC_U_BODY, * LPST_PC_U_BODY;
typedef struct PcUMsg {
	ST_OTE_HEAD         head;
	ST_PC_U_BODY    body;
}ST_PC_U_MSG, * LPST_PC_U_MSG;
/******* OTEユニキャスト通信メッセージ構造体 ***********/
typedef struct OteUBody {
	INT32     tg_pos1[4];         //目標位置座標1
	INT32     tg_dist1[4];        //目標までの距離1
	INT32     tg_pos2[4];         //目標位置座標2
	INT32     tg_dist2[4];        //目標までの距離2
	INT16     pb[64];             //ランプ表示
	INT16     notch_pos[8];       //ノッチ入力位置
	INT16     cam_inf[8];         //操作端カメラ情報
}ST_OTE_U_BODY, * LPST_OTE_U_BODY;
typedef struct OteUMsg {
	ST_OTE_HEAD         head;
	ST_OTE_U_BODY    body;
}ST_OTE_U_MSG, * LPST_OTE_U_MSG;