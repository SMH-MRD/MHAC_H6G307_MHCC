#pragma once

//MCプロトコル
#define IP_ADDR_MC_CLIENT	"192.168.1.100"
#define IP_ADDR_MC_SERVER	"192.168.1.50"

#define PORT_MC_CLIENT		3020		//制御PCポート
#define PORT_MC_SERVER		3010		//CPU本体の時のPLCポート
//#define PORT_MC_SERVER		1025	//Ethernetユニットの時


#define ID_SOCK_MC_CLIENT   PORT_MC_CLIENT
#define ID_SOCK_MC_SERVER   PORT_MC_SERVER

#define MC_ADDR_D_READ		10400
#define MC_SIZE_D_READ		100
#define MC_ADDR_D_WRITE		10200
#define MC_SIZE_D_WRITE		100

#define ID_MC_INV_GNT       0
#define ID_MC_INV_AH        1
#define ID_MC_INV_SLW       2
#define ID_MC_INV_BH        3
#define ID_MC_INV_MH1       4
#define ID_MC_INV_MH2       5

typedef struct ccinv_if_r_tag {
    INT16 x[6];
    INT16 Wr[6][4];
}ST_CCINV_READ, * LPST_CCINV_READ;
typedef struct ccinv_if_w_tag {
    INT16 y[6];
    INT16 Ww[6][4];
}ST_CCINV_WRITE, * LPST_CCINV_WRITE;

typedef struct st_PLCwrite_tag {//制御PC→PLC
    INT16 helthy;               //PCヘルシー出力信号
    INT16 auto_ctrl;            //自動制御フラグ
    INT32 cab_ai[5];            //運転室PLC AI信号【将来用】
    INT16 cab_di[6];            //運転室PLC→電気室PLC b出力
    INT16 spare0[6];            //予備 
    INT32 hcounter[4];          //高速カウンタユニット 
    INT32 absocoder[2];         //アブソコーダ 
    INT16 spare1[4];            //予備
    INT16 pc_fault[2];          //PC検出異常マップ
    INT16 spare2[20];           //予備
    INT16 erm_x[8];             //予備
    ST_CCINV_READ ccinv_r;      //インバータ読み込み値を上書き
 }ST_PLC_WRITE, * LPST_PLC_WRITE;

union PLC_WRITE_BUF {
    INT16 D[MC_SIZE_D_WRITE];
    ST_PLC_WRITE  wbuf;
};

#define ID_MC_ERM_BO_000    0
#define ID_MC_ERM_BO_010    1
#define ID_MC_ERM_BO_140    2
#define ID_MC_ERM_BO_150    3
#define ID_MC_ERM_BO_160    4
#define ID_MC_ERM_BO_170    5

#define MC_SETBIT_LAMP_CONTROL_SOURCE 0x0002

typedef struct st_PLCread_tag {
    INT16 helthy;               //PLCヘルシーカウンタ
    INT16 plc_ctrl;             // PLC運転モード
    INT32 cab_ai[5];            //運転室PLC→電気室PLC W出力
    INT16 cab_bi[6];            //運転室PLC→電気室PLC b出力
    INT16 erm_bo[6];            //電気室PLC b出力
    INT32 pos[5];               //各軸位置信号
    INT16 spd[6];               //各軸速度信号
    INT16 plc_fault[18];        //各軸速度信号
    INT16 erm_y[4];             //電気室PLC Y出力
    INT16 erm_x[8];             //電気室PLC X入力
    ST_CCINV_WRITE ccinv_w;  //予備
}ST_PLC_READ, * LPST_PLC_READ; 

union PLC_READ_BUF {
    INT16 D[MC_SIZE_D_READ];
    ST_PLC_READ  rbuf;
};

//PLC IOマッピング　point.x WORD INDEX, point.y BIT INDEX

#define ID_OPEPLC_B220                          0
#define ID_OPEPLC_B230                          1
#define ID_OPEPLC_B240                          2
#define ID_OPEPLC_B250                          3
#define ID_OPEPLC_B260                          4
struct CABPLC_BOUT_MAP {
    //B220-B26F
    POINT ctrl_on                           = { 0,0x0001 };
    POINT ctrl_off                          = { 0,0x0002 };
    POINT mh_spd_low                        = { 0,0x0004 };
    POINT mh_spd_middle                     = { 0,0x0008 };
    POINT bh_normal                         = { 0,0x0010 };
    POINT bh_down                           = { 0,0x0020 };
    POINT bh_rest                           = { 0,0x0040 };
    POINT mh_high_spd                       = { 0,0x0080 };
    POINT ah_multi                          = { 0,0x0100 };
    POINT ah_single                         = { 0,0x0200 };
    POINT notch_ah                          = { 0,10 };//ノッチのYはビットシフト量
    POINT notch_mh                          = { 1,0};//ノッチのYはビットシフト量
    POINT load_swayless                     = { 1,0x0040 };
    POINT gt_spd_30                         = { 1,0x0080 };
    POINT gt_spd_35                         = { 1,0x0100 };
    POINT notch_gt                          = { 1,9 };//ノッチのYはビットシフト量
    POINT cab_estp                          = { 1,0x8000};
    POINT whip_5t_ng                        = { 2,0x0001 };
    POINT mlim_acc_low                      = { 2,0x0002 };
    POINT mlim_3times                       = { 2,0x0004 };
    POINT mlim_load_90                      = { 2,0x0008 };
    POINT mlim_load_100                     = { 2,0x0010 };
    POINT mlim_high_load                    = { 2,0x0020 };
    POINT mlim_normal                       = { 2,0x0040 };
    POINT wind_spd16                        = { 2,0x0080 };
    POINT bz_stop                           = { 2,0x0100 };
    POINT fault_reset                       = { 2,0x0200 };
    POINT il_bypass                         = { 2,0x0400 };
    POINT mhbrk_opn_normal                  = { 2,0x0800 };
    POINT mhbrk_opn_abnormal                = { 2,0x1000 };
    POINT mh_emr_open                       = { 2,0x2000 };
    POINT ah_emr_lower                      = { 2,0x4000 };
    POINT trace                             = { 2,0x8000 };
    POINT notch_bh                          = { 3,0 };//ノッチのYはビットシフト量
    POINT ah_sel_low_spd                    = { 3,0x0040 };
    POINT ah_sel_high_spd                   = { 3,0x0080 };
    POINT foot_brk                          = { 3,0x0100 };
    POINT notch_sl                          = { 3,9 };//ノッチのYはビットシフト量
    POINT siren                             = { 4,0x0001 };
    POINT camera_ghange                     = { 4,0x0002 };
    POINT slew_fix                          = { 4,0x0004 };
    POINT camera_sel_east                   = { 4,0x0008 };
    POINT camera_sel_conj                   = { 4,0x0010 };
    POINT camera_sel_west                   = { 4,0x0020 };
    POINT high_pow_ON                       = { 4,0x0040 };
    POINT high_pow_OFF                      = { 4,0x0080 };
    POINT slew_notch5                       = { 4,0x0100 };
    POINT jib_approch_wide                  = { 4,0x0200 };
    POINT jib_approch_narrow                = { 4,0x0400 };
    POINT spare0                            = { 4,0x0800 };
    POINT jib_approch_stop                  = { 4,0x1000 };
    POINT gt_warm_shock_on                  = { 4,0x2000 };
    POINT gt_warm_shock_off                 = { 4,0x4000 };
    POINT ah_high_pos_load                  = { 4,0x5000 };
};

#define ID_ERMPLC_B0                           0
#define ID_ERMPLC_B1                           1
#define ID_ERMPLC_B14                          2
#define ID_ERMPLC_B15                          3
#define ID_ERMPLC_B16                          4
struct ERMPLC_BOUT_MAP {
    //B0-B1F
    POINT cycle_bit                     = { 0,0x0001 };
    POINT cycle_bit_remote              = { 0,0x0002 };
    POINT fault_bz                      = { 0,0x0080 };
    POINT concurrent_limit_area         = { 0,0x0100 };
    POINT il_bypass                     = { 0,0x0200 };
    POINT inv_fault_time_on             = { 0,0x0800 };
    POINT fault_item_detect             = { 1,0x0001 };
    POINT mh_edl_start                  = { 1,0x0020 };
    POINT ah_edl_start                  = { 1,0x0040 };
    //B0-B1F
    POINT got_useful_func               = { 2,0x0001 };
    POINT hook_pos_in_hlimit            = { 2,0x0002 };
    POINT hook_pos_in_high_pos          = { 2,0x0004 };
    POINT hook_pos_in_under_hlimit      = { 2,0x0008 };
    POINT hook_pos_bh_down_over         = { 2,0x0010 };
    POINT hook_pos_bh_down_under        = { 2,0x0020 };
    POINT hook_pos_bh_down_normal       = { 2,0x0040 };
    POINT mh_high1_con_up_slow_area     = { 2,0x0080 };
    POINT mh_high1_con_up_stop_area     = { 2,0x0100 };
    POINT mh_normal_up_slow_area        = { 2,0x0200 };
    POINT mh_normal_con_up_slow1_area   = { 2,0x0400 };
    POINT mh_normal_con_up_slow2_area   = { 2,0x0800 };
    POINT mh_normal_con_up_stop_area    = { 2,0x1000 };
    POINT ctrl_source_ok_bz             = { 4,0x0001 };
    POINT ctrl_source_mc_ok             = { 4,0x0002 };
    POINT fault_pl                      = { 4,0x0004 };
    POINT ctrl_standby                  = { 4,0x0008 };
    POINT load_disp_ok                  = { 4,0x0010 };
    POINT high_pos_operation_mode       = { 4,0x0100 };
    POINT bh_rest_mode                  = { 4,0x0200 };
    POINT bh_fixed_pl                   = { 4,0x0400 };
    POINT ah_active_jc                  = { 4,0x1000 };
    POINT bh_fixed_pl2                  = { 4,0x2000 };
    POINT motive_power_ok               = { 4,0x4000 };
 };

#define ID_ERMPLC_YD0                           0
#define ID_ERMPLC_YE0                           1
#define ID_ERMPLC_YF0                           2
struct ERMPLC_YOUT_MAP {
    //D0-F0
    POINT il_0notch                         = { 0,0x0001 };
    POINT mh_brk_mc                         = { 0,0x0002 };
    POINT ah_brk_mc                         = { 0,0x0004 };
    POINT bh_brk_mc                         = { 0,0x0008 };
    POINT sl_brk_mc                         = { 0,0x0010 };
    POINT gt_brk_mc                         = { 0,0x0020 };
    POINT inv_reel_preset                   = { 0,0x0800 };
    POINT inv_reel_power_on                 = { 0,0x1000 };
    POINT inv_reel_gt_start                 = { 0,0x2000 };
    POINT inv_reel_fault_reset              = { 0,0x4000 };
    POINT mh_fan_mc                         = { 2,0x0001 };
    POINT bh_fan_mc                         = { 2,0x0002 };
    POINT db_convertor_fan_mc               = { 2,0x0004 };
    POINT ah_fan_mc                         = { 2,0x0008 };
    POINT gt_alarm                          = { 2,0x0010 };
    POINT initial_charge                    = { 2,0x0020 };
};

#define ID_ERMPLC_X60                           0
#define ID_ERMPLC_X70                           1
#define ID_ERMPLC_X80                           2
#define ID_ERMPLC_X90                           3
#define ID_ERMPLC_XA0                           4
#define ID_ERMPLC_XB0                           5
#define ID_ERMPLC_XC0                           6
struct ERMPLC_XIN_MAP {
    //X60-C0
    POINT mh_high_area_emr_up_lim           = { ID_ERMPLC_X60,0x0001 };
    POINT spare0                            = { ID_ERMPLC_X60,0x0002 };
    POINT spare1                            = { ID_ERMPLC_X60,0x0004 };
    POINT mh_normal_area_emr_up_lim         = { ID_ERMPLC_X60,0x0008 };
    POINT bh_down_area_emr_up_lim           = { ID_ERMPLC_X60,0x0010 };
    POINT mh_emr_lower_lim                  = { ID_ERMPLC_X60,0x0020 };
    POINT bh_emr_in_lim                     = { ID_ERMPLC_X60,0x0040 };
    POINT bh_normal_in_lim                  = { ID_ERMPLC_X60,0x0080 };
    POINT slw_spd_change                    = { ID_ERMPLC_X60,0x0100 };
    POINT spare2                            = { ID_ERMPLC_X60,0x0200 };
    POINT spare3                            = { ID_ERMPLC_X60,0x0400 };
    POINT high_pos_emr_out_lim              = { ID_ERMPLC_X60,0x0800 };
    POINT bh_emr_out_lim                    = { ID_ERMPLC_X60,0x1000 };
    POINT bh_down_area_out_lim              = { ID_ERMPLC_X60,0x2000 };
    POINT bh_rest_slow                      = { ID_ERMPLC_X60,0x4000 };
    POINT bh_rest_lim                       = { ID_ERMPLC_X60,0x8000 };

    POINT muliti_transmit_ok                = { ID_ERMPLC_X70,0x0001 };
    POINT spare4                            = { ID_ERMPLC_X70,0x0002 };
    POINT gt_anticol_stop                   = { ID_ERMPLC_X70,0x0004 };
    POINT gt_anticol_alarm                  = { ID_ERMPLC_X70,0x0008 };
    POINT gt_anticol_fault                  = { ID_ERMPLC_X70,0x0010 };
    POINT gt_east_lim_slow                  = { ID_ERMPLC_X70,0x0020 };
    POINT gt_east_lim_stop                  = { ID_ERMPLC_X70,0x0040 };
    POINT gt_west_lim_slow                  = { ID_ERMPLC_X70,0x0080 };
    POINT gt_west_lim_stop                  = { ID_ERMPLC_X70,0x0100 };
    POINT gt_fix_open                       = { ID_ERMPLC_X70,0x0200 };
    POINT gt_motor_thermal_trip             = { ID_ERMPLC_X70,0x0400 };
    POINT spare5                            = { ID_ERMPLC_X70,0x0800 };
    POINT mh_motor_fan_mc                   = { ID_ERMPLC_X70,0x1000 };
    POINT ah_motor_fan_mc                   = { ID_ERMPLC_X70,0x2000 };
    POINT bh_motor_fan_mc                   = { ID_ERMPLC_X70,0x4000 };
    POINT DB_conv_fan_mc                    = { ID_ERMPLC_X70,0x8000 };

    POINT DB_over_heat                      = { ID_ERMPLC_X80,0x0001 };
    POINT mh_pos_preset                     = { ID_ERMPLC_X80,0x0002 };
    POINT mh_data_preset                    = { ID_ERMPLC_X80,0x0004 };
    POINT slw_panel_high_temp               = { ID_ERMPLC_X80,0x0008 };
    POINT leg_emr_stop                      = { ID_ERMPLC_X80,0x0010 };
    POINT slew_pos_0                        = { ID_ERMPLC_X80,0x0020 };
    POINT slew_pos_180                      = { ID_ERMPLC_X80,0x0040 };
    POINT slew_grease_active                = { ID_ERMPLC_X80,0x0080 };
    POINT slew_grease_fault                 = { ID_ERMPLC_X80,0x0100 };
    POINT ah_camlim_high_area_emr_up        = { ID_ERMPLC_X80,0x0200 };
    POINT ah_camlim_normal_area1_emr_up     = { ID_ERMPLC_X80,0x0400 };
    POINT ah_camlim_normal_area2_emr_up     = { ID_ERMPLC_X80,0x0800 };
    POINT ah_camlim_bh_down_area_emr_up     = { ID_ERMPLC_X80,0x1000 };
    POINT ah_camlim_emr_low                 = { ID_ERMPLC_X80,0x2000 };
    POINT spare6                            = { ID_ERMPLC_X80,0x4000 };
    POINT spare7                            = { ID_ERMPLC_X80,0x8000 };

    POINT conv_1_trip                       = { ID_ERMPLC_X90,0 };
    POINT inv_ctrl_trip                     = { ID_ERMPLC_X90,1 };
    POINT brk_control_source_trip           = { ID_ERMPLC_X90,2 };
    POINT DB_fan_trip                       = { ID_ERMPLC_X90,3 };
    POINT motor_fan_trip                    = { ID_ERMPLC_X90,4 };
    POINT spare8                            = { ID_ERMPLC_X90,5 };
    POINT slw_grease_trip                   = { ID_ERMPLC_X90,6 };
    POINT spare9                            = { ID_ERMPLC_X90,7 };
    POINT aero_panel_fault                  = { ID_ERMPLC_X90,8 };
    POINT remote_trace                      = { ID_ERMPLC_X90,9 };
    POINT inv_reel_battery_low              = { ID_ERMPLC_X90,0x0400 };
    POINT inv_reel_gt_ok                    = { ID_ERMPLC_X90,0x0800 };
    POINT inv_reel_standby                  = { ID_ERMPLC_X90,0x1000 };
    POINT inv_reel_inv_normal               = { ID_ERMPLC_X90,0x2000 };
    POINT spare10                           = { ID_ERMPLC_X90,0x4000 };
    POINT inv_reel_escape_enable            = { ID_ERMPLC_X90,0x8000 };

    POINT motive_power_ok                   = { ID_ERMPLC_XA0,0x0001 };
    POINT conv1_mc                          = { ID_ERMPLC_XA0,0x0002 };
    POINT conv2_mc                          = { ID_ERMPLC_XA0,0x0004 };
    POINT ctrl_brk_mc                       = { ID_ERMPLC_XA0,0x0008 };
    POINT mh_brk_mc                         = { ID_ERMPLC_XA0,0x0010 };
    POINT bh_brk_mc                         = { ID_ERMPLC_XA0,0x0020 };
    POINT gt_brk_mc                         = { ID_ERMPLC_XA0,0x0040 };
    POINT ah_brk_mc                         = { ID_ERMPLC_XA0,0x0080 };
    POINT initial_charge_mc                 = { ID_ERMPLC_XA0,0x0100 };
    POINT spare11                           = { ID_ERMPLC_XA0,0x0200 };
    POINT jib_approch_alarm_enable          = { ID_ERMPLC_XA0,0x0400 };
    POINT jib_approch_alarm_disable         = { ID_ERMPLC_XA0,0x0800 };
    POINT conv1_over_heat                   = { ID_ERMPLC_XA0,0x1000 };
    POINT conv2_fuse_down                   = { ID_ERMPLC_XA0,0x2000 };
    POINT conv2_over_heat                   = { ID_ERMPLC_XA0,0x4000 };
    POINT conv1_fuse_down                   = { ID_ERMPLC_XA0,0x8000 };

    POINT thermal_trip_ah_brk               = { ID_ERMPLC_XB0,0x0001 };
    POINT thermal_trip_bh_motor             = { ID_ERMPLC_XB0,0x0002 };
    POINT thermal_trip_mh_fan               = { ID_ERMPLC_XB0,0x0004 };
    POINT thermal_trip_bh_fan               = { ID_ERMPLC_XB0,0x0008 };
    POINT thermal_trip_DB_fan               = { ID_ERMPLC_XB0,0x0010 };
    POINT thermal_trip_mh_brk               = { ID_ERMPLC_XB0,0x0020 };
    POINT thermal_trip_bh_brk               = { ID_ERMPLC_XB0,0x0040 };
    POINT thermal_trip_ah_fan               = { ID_ERMPLC_XB0,0x0080 };
    POINT braking_unit1_fault               = { ID_ERMPLC_XB0,0x0100 };
    POINT braking_unit2_fault               = { ID_ERMPLC_XB0,0x0200 };
    POINT braking_unit3_fault               = { ID_ERMPLC_XB0,0x0400 };
    POINT braking_unit4_fault               = { ID_ERMPLC_XB0,0x0800 };
    POINT braking_unit5_fault               = { ID_ERMPLC_XB0,0x1000 };
    POINT braking_unit6_fault               = { ID_ERMPLC_XB0,0x2000 };
    POINT braking_unit7_fault               = { ID_ERMPLC_XB0,0x4000 };
    POINT braking_unit8_fault               = { ID_ERMPLC_XB0,0x8000 };

    POINT mh_preset                         = { ID_ERMPLC_XC0,0x0001 };
    POINT ah_preset                         = { ID_ERMPLC_XC0,0x0002 };
    POINT spare12                           = { ID_ERMPLC_XC0,0x0004 };
    POINT spare13                           = { ID_ERMPLC_XC0,0x0008 };
    POINT spare14                           = { ID_ERMPLC_XC0,0x0010 };
    POINT spare15                           = { ID_ERMPLC_XC0,0x0020 };
    POINT spare16                           = { ID_ERMPLC_XC0,0x0040 };
    POINT spare17                           = { ID_ERMPLC_XC0,0x0080 };
    POINT spare18                           = { ID_ERMPLC_XC0,0x0100 };
    POINT spare19                           = { ID_ERMPLC_XC0,0x0200 };
    POINT spare20                           = { ID_ERMPLC_XC0,0x0400 };
    POINT spare21                           = { ID_ERMPLC_XC0,0x0800 };
    POINT spare22                           = { ID_ERMPLC_XC0,0x1000 };
    POINT spare23                           = { ID_ERMPLC_XC0,0x2000 };
    POINT spare24                           = { ID_ERMPLC_XC0,0x4000 };
    POINT spare25                           = { ID_ERMPLC_XC0,0x8000 };
};
//INVERTOR CC LINK
#define ID_CC_LINK_INV_GT                       0
#define ID_CC_LINK_INV_AH                       1
#define ID_CC_LINK_INV_SL                       2
#define ID_CC_LINK_INV_BH                       3
#define ID_CC_LINK_INV_MH1                      4
#define ID_CC_LINK_INV_MH2                      5

#define ID_CC_LINK_X                            0
#define ID_CC_LINK_Y                            0
#define ID_CC_LINK_ITEM1                        1
#define ID_CC_LINK_ITEM2                        2
#define ID_CC_LINK_ITEM3                        3
#define ID_CC_LINK_ITEM4                        4
#define ID_CC_LINK_ITEM5                        5

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

//CC LINK
#define CC_PRM_MAX_RPM_MH   2250
#define CC_PRM_MAX_RPM_AH   2250
#define CC_PRM_MAX_RPM_BH   1750
#define CC_PRM_MAX_RPM_SLW  1750
#define CC_PRM_MAX_RPM_GT   2000
