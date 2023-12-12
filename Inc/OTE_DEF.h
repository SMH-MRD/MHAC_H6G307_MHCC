#pragma once

//#include <winsock.h>

//IPアドレス
#define OTE_IF_UNICAST_IP_PC				"192.168.1.100" 		//PCのOTE UNICAST　IF用IP	（受信,送信）
#define OTE_IF_UNICAST_IP_OTE0				OTE_IF_UNICAST_IP_PC	//デフォルトOTEのUNICAST用IP（受信,送信）

//#define IP_ADDR_PC_UNI_CAST_OTE		"192.168.1.100"
//#define IP_ADDR_OTE_UNI_CAST_OTE		"127.0.0.1"

#define OTE_IF_MULTICAST_IP_PC				"239.1.0.2"					//PCマルチキャスト用IP
#define OTE_IF_MULTICAST_IP_OTE				"239.1.0.1"					//OTEマルチキャスト用IP

#define OTE_IF_UNICAST_PORT_PC				10050					//PCユニキャストIF受信ポート
#define OTE_IF_UNICAST_PORT_OTE				10051					//端末ユニキャストIF受信ポート

#define OTE_IF_MULTICAST_PORT_PC2OTE		20080					//PC発信マルチキャスト受信ポート
#define OTE_IF_MULTICAST_PORT_OTE2PC		20081					//OTE発信マルチキャスト受信ポート
#define OTE_IF_MULTICAST_PORT_PC2PC			20082					//PC発信マルチキャストOTE受信ポート
#define OTE_IF_MULTICAST_PORT_OTE2OTE		20083					//OTE発信マルチキャストOTE受信ポート

#define ID_OTE_MODE_REMOTE_OPE				0x00000001
#define ID_OTE_MODE_MONITOR					0x00000002
#define ID_OTE_MODE_REMOTE_ACTIVE			0x00000010


//**************************************************************************************
//通信電文フォーマット
//**************************************************************************************
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
	UCHAR       pc_enable[N_CRANE_PC_MAX];		//接続可能端末フラグ
	INT32	    n_remote_wait;					//接続待ち遠隔操作卓台数
	INT32	    onbord_seqno;					//機側接続シーケンス番号
	INT32	    remote_seqno;					//遠隔卓接続シーケンス番号
	INT32	    my_seqno;						//自身の接続シーケンス番号
}ST_OTE_M_BODY, * LPST_OTE_M_BODY;
typedef struct OteMMsg {
	ST_OTE_HEAD         head;
	ST_OTE_M_BODY    body;
}ST_OTE_M_MSG, * LPST_OTE_M_MSG;
/******* PCユニキャストメッセージ構造体 ***********/
#define PLC_IO_MONT_WORD_NUM		150
typedef struct PcUBody {
	INT16      plc_in[PLC_IO_MONT_WORD_NUM];	//位置FB
	INT32      pos[8];							//位置FB
	INT32      v_fb[8];							//速度FB
	INT32      v_ref[8];						//速度指令
	INT32      ld_pos[4];						//吊荷位置FB
	INT32      ld_v_fb[4];						//吊荷速度FB
	INT32      tg_pos[8];						//目標位置座標1
	INT32      tg_pos_semi[6][8];				//半自動目標位置座標S1-L3
	INT16      lamp[64];						//ランプ表示
	INT16      notch_pos[8];					//ノッチランプ表示
	INT16      cam_inf[8];						//遠隔操作カメラ情報
}ST_PC_U_BODY, * LPST_PC_U_BODY;
typedef struct PcUMsg {
	ST_OTE_HEAD     head;
	ST_PC_U_BODY    body;
}ST_PC_U_MSG, * LPST_PC_U_MSG;
/******* OTEユニキャスト通信メッセージ構造体 ***********/
typedef struct OteUBody {
	INT16     pb_ope[64];			//操作卓相当PB入力
	INT16     notch_pos[8];			//ノッチ入力位置
	INT16     pb_auto[64];			//自動PB入力
	INT32     tg_pos[8];			//設定目標位置
	INT32     cam_inf[8];			//操作端カメラ情報
	INT32     spare[16];			//予備
	INT16	  sub_monitor_mode;		//オプションモニタ選択状態
	INT16	  ote_mode;			//OTEの操作モード
}ST_OTE_U_BODY, * LPST_OTE_U_BODY;
typedef struct OteUMsg {
	ST_OTE_HEAD         head;
	ST_OTE_U_BODY    body;
}ST_OTE_U_MSG, * LPST_OTE_U_MSG;


typedef struct StOTE_IO {
	ST_OTE_U_BODY	ote_in;
	ST_PC_U_BODY	ote_out;
	INT32 OTEstatus;
	INT32 OTEactive;				//接続中の端末ID　接続断の時0
}ST_OTE_IO, * LPST_OTE_IO;

//OTE PB INDEX pb_ope
#define ID_OTE_PB_CTRL_SOURCE			0
#define ID_OTE_PB_ESTOP					1