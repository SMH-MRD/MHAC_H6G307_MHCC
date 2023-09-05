#pragma once

//MCプロトコル
#define IP_ADDR_MC_CLIENT	"192.168.1.100"
#define IP_ADDR_MC_SERVER	"192.168.1.50"

#define PORT_MC_CLIENT		3020		//制御PCポート
#define PORT_MC_SERVER		3010		//CPU本体の時のPLCポート
//#define PORT_MC_SERVER		1025	//Ethernetユニットの時


#define ID_SOCK_MC_CLIENT   PORT_MC_CLIENT
#define ID_SOCK_MC_SERVER   PORT_MC_SERVER

#define MC_ADDR_D_READ		10300
#define MC_SIZE_D_READ		100
#define MC_ADDR_D_WRITE		10200
#define MC_SIZE_D_WRITE		100

#define I_CAB_AI_BH_R			0
#define I_CAB_AI_MH_LOAD		1
#define I_CAB_AI_AH_LOAD		2
#define I_CAB_AI_BH_ANGLE		3

#define I_CAB_DI_B220			0
#define I_CAB_DI_B230			1
#define I_CAB_DI_B240			2
#define I_CAB_DI_B250			3
#define I_CAB_DI_B260			4

#define I_HCNT_MH_AH			0
#define I_HCNT_BH_SL			1
#define I_HCNT_MH   			0
#define I_HCNT_AH   			1
#define I_HCNT_BH   			0
#define I_HCNT_SL   			1
#define I_ABS_MH   			    0
#define I_ABS_AH      			1
#define I_ERM_DI_X060			0
#define I_ERM_DI_X070			1
#define I_ERM_DI_X080			2
#define I_ERM_DI_X090			3
#define I_ERM_DI_X0A0			4
#define I_ERM_DI_X0B0			5
#define I_ERM_DI_X0C0			6
#define I_CC_LINK_GT   			0
#define I_CC_LINK_AH   			1
#define I_CC_LINK_SL   			2
#define I_CC_LINK_BH   			3
#define I_CC_LINK_MH1   		4
#define I_CC_LINK_MH2   		5
#define I_CC_LINK_X   		    0
#define I_CC_LINK_Y   		    0
#define I_CC_LINK_ITEM0   		1
#define I_CC_LINK_ITEM1   		2
#define I_CC_LINK_ITEM2   		3
#define I_CC_LINK_ITEM3   		4

typedef struct st_PLCwrite_tag {
    INT16 helthy;
    INT16 auto_ctrl;
    INT16 cab_ai[4];
    INT16 cab_di[5];
    INT16 spare0[28];   //12-39予備
    INT16 pc_fault[2];
    INT16 spare1[6];    //42-47予備
    INT16 hcnt_stat[2]; 
    INT32 hcnt[2][2];
    INT32 abscnt[2];
    INT16 eroom_x[7];
    INT16 cc_link_stat;
    INT16 cc_link_inv_r[6][5];
}ST_PLC_WRITE, * LPST_PLC_WRITE;

union PLC_WRITE_BUF {
    INT16 D[MC_SIZE_D_WRITE];
    ST_PLC_WRITE  wbuf;
};

#define IR_CAB_AI_BH_BK		    0
#define IR_CAB_AI_BH_R		    1
#define IR_CAB_AI_MH_LOAD		2
#define IR_CAB_AI_AH_LOAD		3
#define IR_CAB_AI_BH_ANGLE	    4

#define IR_ERM_DO_B000			0
#define IR_ERM_DO_B010			1
#define IR_ERM_DO_B140			2
#define IR_ERM_DO_B150			3
#define IR_ERM_DO_B160			4
#define IR_FB_MH			    0
#define IR_FB_AH			    1
#define IR_FB_BH			    2
#define IR_FB_SL			    3
#define IR_FB_GT			    4

#define IR_ERM_DO_Y0D0			0
#define IR_ERM_DO_Y0E0			1
#define IR_ERM_DO_Y0F0			2

typedef struct st_PLCread_tag {
    INT16 helthy;
    INT32 cab_ai[5];
    INT16 cab_di[5];
    INT16 eroom_do[5];
    INT16 spare0[3];    //22-24予備
    INT32 pos[5];
    INT16 spd[5];
    INT16 plc_fault[17];
    INT16 spare1[3];    //57-59予備
    INT16 eroom_y[3];
    INT16 cc_link_inv_r[6][5];
}ST_PLC_READ, * LPST_PLC_READ;

union PLC_READ_BUF {
    INT16 D[MC_SIZE_D_READ];
    ST_PLC_READ  rbuf;
};

//ビット定義
#define ALL_BITS    0x0000 //WORDデータ
#define BIT0        0x0001
#define BIT1        0x0002
#define BIT2        0x0004
#define BIT3        0x0008
#define BIT4        0x0010
#define BIT5        0x0020
#define BIT6        0x0040
#define BIT7        0x0080
#define BIT8        0x0100
#define BIT9        0x0200
#define BIT10       0x0400
#define BIT11       0x0800
#define BIT12       0x1000
#define BIT13       0x2000
#define BIT14       0x4000
#define BIT15       0x8000

//ノッチビットパターン
#define PTN_NOTCH_0			0x0001
#define PTN_NOTCH_F1		0x0002
#define PTN_NOTCH_F2		0x000A
#define PTN_NOTCH_F3		0x001A
#define PTN_NOTCH_F4		0x003A
#define PTN_NOTCH_R1		0x0004
#define PTN_NOTCH_R2		0x000C
#define PTN_NOTCH_R3		0x001C
#define PTN_NOTCH_R4		0x003C


#define D_INDEX_CAB_X_IR			11
#define D_INDEX_CAB_X_IW			6
#define D_INDEX_ERM_Y_IR			17


//ノッチ情報
#define D_INDEX_MH_NOTCH			1
#define D_INDEX_AH_NOTCH			0
#define D_INDEX_BH_NOTCH			3
#define D_INDEX_SL_NOTCH			3
#define D_INDEX_GT_NOTCH			1

#define D_OFFSET_MH_NOTCH			0
#define D_OFFSET_AH_NOTCH			10
#define D_OFFSET_BH_NOTCH			0
#define D_OFFSET_SL_NOTCH			9
#define D_OFFSET_GT_NOTCH			9

//釦,スイッチ情報