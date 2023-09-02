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

//Dレジスタ割付定義
//WRITE Buffa UINT16[MC_SIZE_D_WRITE]
#define IW_MHCC_HELTHY_CNT		0
#define IW_CAB_AI_BH_R			1
#define IW_CAB_AI_MH_LOAD		2
#define IW_CAB_AI_AH_LOAD		3
#define IW_CAB_AI_BH_ANGLE		4
#define IW_CAB_DI_X030			5
#define IW_CAB_DI_X040			6
#define IW_CAB_DI_X050			7
#define IW_CAB_DI_X060			8
#define IW_CAB_DI_X070			9
#define IW_CAB_DI_X080			10
//11-29予備
#define IW_MHCC_FAULT1			40
#define IW_MHCC_FAULT2			41

#define IW_ERM_HCNT_0L			50
#define IW_ERM_HCNT_0H			51
#define IW_ERM_HCNT_1L			52
#define IW_ERM_HCNT_1H			53
#define IW_ERM_HCNT_2L			54
#define IW_ERM_HCNT_2H			55
#define IW_ERM_HCNT_3L			56
#define IW_ERM_HCNT_3H			57
#define IW_ERM_MH_ABSCNT_L		58
#define IW_ERM_MH_ABSCNT_H		59
#define IW_ERM_AH_ABSCNT_L		60
#define IW_ERM_AH_ABSCNT_H		61
#define IW_ERM_DI_X060			62
#define IW_ERM_DI_X070			63
#define IW_ERM_DI_X080			64
#define IW_ERM_DI_X090			65
#define IW_ERM_DI_X0A0			66
#define IW_ERM_DI_X0B0			67
#define IW_ERM_DI_X0C0			68
//59-69予備
#define IW_CC_GT_X0				70
#define IW_CC_GT_Wr0			71
#define IW_CC_GT_Wr1			72
#define IW_CC_GT_Wr2			73
#define IW_CC_GT_Wr3			74
#define IW_CC_AH_X0				75
#define IW_CC_AH_Wr0			76
#define IW_CC_AH_Wr1			77
#define IW_CC_AH_Wr2			78
#define IW_CC_AH_Wr3			79
#define IW_CC_SLW_X0			80
#define IW_CC_SLW_Wr0			81
#define IW_CC_SLW_Wr1			82
#define IW_CC_SLW_Wr2			83
#define IW_CC_SLW_Wr3			84
#define IW_CC_BH_X0				85
#define IW_CC_BH_Wr0			86
#define IW_CC_BH_Wr1			87
#define IW_CC_BH_Wr2			88
#define IW_CC_BH_Wr3			89
#define IW_CC_MH1_X0			90
#define IW_CC_MH1_Wr0			91
#define IW_CC_MH1_Wr1			92
#define IW_CC_MH1_Wr2			93
#define IW_CC_MH1_Wr3			94
#define IW_CC_MH2_X0			95
#define IW_CC_MH2_Wr0			96
#define IW_CC_MH2_Wr1			97
#define IW_CC_MH2_Wr2			98
#define IW_CC_MH2_Wr3			99
//90-99予備

//READ Buffa UINT16[MC_SIZE_D_READ]
#define IR_PLC_HELTHY_CNT		0
#define IW_CAB_AI_BH_R			1
#define IW_CAB_AI_MH_LOAD		2
#define IW_CAB_AI_AH_LOAD		3
#define IW_CAB_AI_BH_ANGLE		4
#define IW_CAB_DI_X030			5
#define IW_CAB_DI_X040			6
#define IW_CAB_DI_X050			7
#define IW_CAB_DI_X060			8
#define IW_CAB_DI_X070			9
#define IW_CAB_DI_X080			10
#define IR_CAB_DO_Y070			11

//12-29予備
#define IR_POS_FB_MH			30
#define IR_POS_FB_AH			31
#define IR_POS_FB_BH			32
#define IR_POS_FB_SL			33
#define IR_POS_FB_GT			34
#define IR_SPD_FB_MH			35
#define IR_SPD_FB_AH			36
#define IR_SPD_FB_BH			37
#define IR_SPD_FB_SL			38
#define IR_SPD_FB_GT			39

#define IR_FAULT_01				40
#define IR_FAULT_02				41
#define IR_FAULT_03				42
#define IR_FAULT_04				43
#define IR_FAULT_05				44
#define IR_FAULT_06				45
#define IR_FAULT_07				46
#define IR_FAULT_08				47
#define IR_FAULT_09				48
#define IR_FAULT_10				49
#define IR_FAULT_11				50
#define IR_FAULT_12				51
#define IR_FAULT_13				52
#define IR_FAULT_14				53
#define IR_FAULT_15				54
#define IR_FAULT_16				55
#define IR_FAULT_17				56
//57-59予備
#define IR_ERM_DO_Y0D0			60
#define IR_ERM_DO_Y0E0			61
#define IR_ERM_DO_Y0F0			62
#define IR_CC_GT_Y0				70
#define IR_CC_GT_Ww0			71
#define IR_CC_GT_Ww1			72
#define IR_CC_GT_Ww2			73
#define IR_CC_GT_Ww3			74
#define IR_CC_AH_Y0				75
#define IR_CC_AH_Ww0			76
#define IR_CC_AH_Ww1			77
#define IR_CC_AH_Ww2			78
#define IR_CC_AH_Ww3			79
#define IR_CC_SLW_Y0			80
#define IR_CC_SLW_Ww0			81
#define IR_CC_SLW_Ww1			82
#define IR_CC_SLW_Ww2			83
#define IR_CC_SLW_Ww3			84
#define IR_CC_BH_Y0				85
#define IR_CC_BH_Ww0			86
#define IR_CC_BH_Ww1			87
#define IR_CC_BH_Ww2			88
#define IR_CC_BH_Ww3			89
#define IR_CC_MH1_Y0			90
#define IR_CC_MH1_Ww0			91
#define IR_CC_MH1_Ww1			92
#define IR_CC_MH1_Ww2			93
#define IR_CC_MH1_Ww3			94
#define IR_CC_MH2_Y0			95
#define IR_CC_MH2_Ww0			96
#define IR_CC_MH2_Ww1			97
#define IR_CC_MH2_Ww2			98
#define IR_CC_MH2_Ww3			99
//90-99予備


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
//MAIN HOIST
#define NOTCH_MASK_MH	0x003F
#define NOTCH_0_MH	    0x0001
#define NOTCH_U1_MH		0x0002
#define NOTCH_U2_MH		0x000A
#define NOTCH_U3_MH		0x001A
#define NOTCH_U4_MH		0x003A
#define NOTCH_D1_MH		0x0004
#define NOTCH_D2_MH		0x000C
#define NOTCH_D3_MH		0x001C
#define NOTCH_D4_MH		0x003C

//AUX HOIST
#define NOTCH_MASK_AH	0x3F00
#define NOTCH_0_AH	    0x0100
#define NOTCH_U1_AH		0x0200
#define NOTCH_U2_AH		0x0A00
#define NOTCH_U3_AH		0x1A00
#define NOTCH_U4_AH		0x3A00
#define NOTCH_D1_AH		0x0400
#define NOTCH_D2_AH		0x0C00
#define NOTCH_D3_AH		0x1C00
#define NOTCH_D4_AH		0x3C00

//GANTRY
#define NOTCH_MASK_GT	0xFC00
#define NOTCH_0_GT	    0x0400
#define NOTCH_E1_GT		0x0800
#define NOTCH_E2_GT		0x2800
#define NOTCH_E3_GT		0x6800
#define NOTCH_E4_GT		0xE800
#define NOTCH_W1_GT		0x1000
#define NOTCH_W2_GT		0x3000
#define NOTCH_W3_GT		0x7000
#define NOTCH_W4_GT		0xF000

//BOOM HOIST
#define NOTCH_MASK_BH	0x003F
#define NOTCH_0_BH	    0x0001
#define NOTCH_I1_BH		0x0002
#define NOTCH_I2_BH		0x000A
#define NOTCH_I3_BH		0x001A
#define NOTCH_I4_BH		0x003A
#define NOTCH_O1_BH		0x0004
#define NOTCH_O2_BH		0x000C
#define NOTCH_O3_BH		0x001C
#define NOTCH_O4_BH		0x003C

//SLEW
#define NOTCH_MASK_SL	0x0FC0
#define NOTCH_0_SL	    0x0040
#define NOTCH_R1_SL		0x0080
#define NOTCH_R2_SL		0x0280
#define NOTCH_R3_SL		0x1680
#define NOTCH_R4_SL		0x3E80
#define NOTCH_L1_SL		0x0100
#define NOTCH_L2_SL		0x0300
#define NOTCH_L3_SL		0x1700
#define NOTCH_L4_SL		0x3F00
