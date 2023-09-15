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

typedef struct st_PLCwrite_tag {
    INT16 helthy;               //PCヘルシー出力信号
    INT16 auto_ctrl;            //自動制御フラグ
    INT16 cab_ai[4];            //運転室PLC AI信号【将来用】
    INT16 cab_di[5];            //運転室PLC→電気室PLC b出力
    INT16 spare0[28];           //12-39予備   
    INT16 pc_fault[2];          //PC検出異常マップ
    INT16 spare1[6];            //42-47予備
    //以下SIMULATORによる上書き用
    INT16 hcnt_stat[2];         //電気室PLC 高速カウンタユニットステータス信号 
    INT32 hcnt[2][2];           //電気室PLC 高速カウンタユニットカウント値 
    INT32 abscnt[2];            //電気室PLC アブソコーダカウント値 
    INT16 eroom_x[7];           //電気室PLC Xレジスタ入力値
    INT16 cc_link_unit_stat;    //電気室PLC CC LINKユニットステータス
    INT16 cc_link_inv_r[6][5];  //電気室PLC CC LINKインバータ入力信号
}ST_PLC_WRITE, * LPST_PLC_WRITE;

union PLC_WRITE_BUF {
    INT16 D[MC_SIZE_D_WRITE];
    ST_PLC_WRITE  wbuf;
};

typedef struct st_PLCread_tag {
    INT16 helthy;               //PLCヘルシーカウンタ
    INT32 cab_w[5];             //運転室PLC→電気室PLC W出力
    INT16 cab_b[5];             //運転室PLC→電気室PLC b出力
    INT16 eroom_b[5];           //電気室PLC b出力
    INT16 spare0[3];            //22-24予備
    INT32 pos[5];               //各軸位置信号
    INT16 spd[5];               //各軸速度信号
    INT16 plc_fault[17];        //各軸速度信号
    INT16 spare1[3];            //57-59予備
     //以下SIMULATORロジック入力用
    INT16 eroom_y[3];           //電気室PLC Y出力
    INT16 cc_link_inv_w[6][5];
}ST_PLC_READ, * LPST_PLC_READ;  //電気室PLC CC LINKインバータ出力信号

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
    POINT ctrl_on           = { 0,0 };
    POINT ctrl_off          = { 0,1 };
    POINT mh_spd_low        = { 0,2 };
    POINT mh_spd_middle     = { 0,3 };
    POINT bh_normal         = { 0,4 };
    POINT bh_down           = { 0,5 };
    POINT bh_rest           = { 0,6 };
    POINT mh_high_spd       = { 0,7 };
    POINT ah_single         = { 0,8 };
    POINT ah_multi          = { 0,9 };
    POINT notch_ah          = { 0,10};
    POINT notch_mh          = { 1,0 };
    POINT load_swayless     = { 1,6 };
    POINT gt_spd_30         = { 1,7 };
    POINT gt_spd_35         = { 1,8 };
    POINT notch_gt          = { 1,9 };
    POINT cab_estp          = { 1,15};
    POINT whip_5t_ng        = { 2,0 };
    POINT mlim_acc_low      = { 2,1 };
    POINT mlim_3times       = { 2,2 };
    POINT mlim_load_90      = { 2,3 };
    POINT mlim_load_100     = { 2,4 };
    POINT mlim_high_load    = { 2,5 };
    POINT mlim_normal       = { 2,6 };
    POINT wind_spd16        = { 2,7 };
    POINT bz_stop           = { 2,8 };
    POINT fault_reset       = { 2,9 };
    POINT il_bypass         = { 2,10 };
    POINT mhbrk_opn_normal  = { 2,11 };
    POINT mhbrk_opn_abnormal= { 2,12 };
    POINT mh_emr_open       = { 2,13 };
    POINT ah_emr_lower      = { 2,14 };
    POINT trace             = { 2,15 };
    POINT notch_bh          = { 3,0 };
    POINT ah_sel_low_spd    = { 3,6 };
    POINT ah_sel_high_spd   = { 3,7 };
    POINT foot_brk          = { 3,8 };
    POINT notch_sl          = { 3,9 };
    POINT siren             = { 4,0 };
    POINT camera_ghange     = { 4,1 };
    POINT slew_fix          = { 4,2 };
    POINT camera_sel_east   = { 4,3 };
    POINT camera_sel_conj   = { 4,4 };
    POINT camera_sel_west   = { 4,5 };
    POINT high_pow_ON       = { 4,6 };
    POINT high_pow_OFF      = { 4,7 };
    POINT slew_notch5       = { 4,8 };
    POINT jib_approch_wide  = { 4,9 };
    POINT jib_approch_narrow= { 4,10 };
    POINT spare0            = { 4,11 };
    POINT jib_approch_stop  = { 4,12 };
    POINT gt_warm_shock_on  = { 4,13 };
    POINT gt_warm_shock_off = { 4,14 };
    POINT ah_high_pos_load  = { 4,15 };
};

#define ID_ERMPLC_B0                           0
#define ID_ERMPLC_B1                           1
#define ID_ERMPLC_B14                          2
#define ID_ERMPLC_B15                          3
#define ID_ERMPLC_B16                          4
struct ERMPLC_BOUT_MAP {
    //B0-B1F
    POINT cycle_bit                     = { 0,0 };
    POINT cycle_bit_remote              = { 0,1 };
    POINT fault_bz                      = { 0,7 };
    POINT concurrent_limit_area         = { 0,8 };
    POINT il_bypass                     = { 0,9 };
    POINT inv_fault_time_on             = { 0,11};
    POINT fault_item_detect             = { 1,0 };
    POINT mh_edl_start                  = { 1,5 };
    POINT ah_edl_start                  = { 1,6 };
    //B0-B1F
    POINT got_useful_func               = { 2,0 };
    POINT hook_pos_in_hlimit            = { 2,1 };
    POINT hook_pos_in_high_pos          = { 2,2 };
    POINT hook_pos_in_under_hlimit      = { 2,3 };
    POINT hook_pos_bh_down_over         = { 2,4 };
    POINT hook_pos_bh_down_under        = { 2,5 };
    POINT hook_pos_bh_down_normal       = { 2,6 };
    POINT mh_high1_con_up_slow_area     = { 2,7 };
    POINT mh_high1_con_up_stop_area     = { 2,8 };
    POINT mh_normal_up_slow_area        = { 2,9 };
    POINT mh_normal_con_up_slow1_area   = { 2,10 };
    POINT mh_normal_con_up_slow2_area   = { 2,11 };
    POINT mh_normal_con_up_stop_area    = { 2,12 };
    POINT ctrl_source_ok_bz             = { 4,0 };
    POINT ctrl_source_mc_ok             = { 4,1 };
    POINT fault_pl                      = { 4,2 };
    POINT ctrl_standby                  = { 4,3 };
    POINT load_disp_ok                  = { 4,4 };
    POINT high_pos_operation_mode       = { 4,8 };
    POINT bh_rest_mode                  = { 4,9 };
    POINT bh_fixed_pl                   = { 4,10 };
    POINT ah_active_jc                  = { 4,12 };
    POINT bh_fixed_pl2                  = { 4,13 };
    POINT motive_power_ok               = { 4,14 };
 };

#define ID_ERMPLC_YD0                           0
#define ID_ERMPLC_YE0                           1
#define ID_ERMPLC_YF0                           2
struct ERMPLC_YOUT_MAP {
    //D0-F0
    POINT il_0notch                     = { 0,0 };
    POINT mh_brk_mc                     = { 0,1 };
    POINT ah_brk_mc                     = { 0,2 };
    POINT bh_brk_mc                     = { 0,3 };
    POINT sl_brk_mc                     = { 0,4 };
    POINT gt_brk_mc                     = { 0,5 };
    POINT inv_reel_preset               = { 0,11 };
    POINT inv_reel_power_on             = { 0,12 };
    POINT inv_reel_gt_start             = { 0,13 };
    POINT inv_reel_fault_reset          = { 0,14 };
    POINT mh_fan_mc                     = { 2,0 };
    POINT bh_fan_mc                     = { 2,1 };
    POINT db_convertor_fan_mc           = { 2,2 };
    POINT ah_fan_mc                     = { 2,3 };
    POINT gt_alarm                      = { 2,4 };
    POINT initial_charge                = { 2,5 };
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
    POINT mh_high_area_emr_up_lim = { ID_ERMPLC_X60,0 };
    POINT spare0 = { ID_ERMPLC_X60,1 };
    POINT spare1 = { ID_ERMPLC_X60,2 };
    POINT mh_normal_area_emr_up_lim = { ID_ERMPLC_X60,3 };
    POINT bh_down_area_emr_up_lim = { ID_ERMPLC_X60,4 };
    POINT mh_emr_lower_lim = { ID_ERMPLC_X60,5 };
    POINT bh_emr_in_lim = { ID_ERMPLC_X60,6 };
    POINT bh_normal_in_lim = { ID_ERMPLC_X60,7 };
    POINT slw_spd_change = { ID_ERMPLC_X60,8 };
    POINT spare2 = { ID_ERMPLC_X60,9 };
    POINT spare3 = { ID_ERMPLC_X60,10 };
    POINT high_pos_emr_out_lim = { ID_ERMPLC_X60,11 };
    POINT bh_emr_out_lim = { ID_ERMPLC_X60,12 };
    POINT bh_down_area_out_lim = { ID_ERMPLC_X60,13 };
    POINT bh_rest_slow = { ID_ERMPLC_X60,14 };
    POINT bh_rest_lim = { ID_ERMPLC_X60,15 };

    POINT muliti_transmit_ok = { ID_ERMPLC_X70,0 };
    POINT spare4 = { ID_ERMPLC_X70,1 };
    POINT gt_anticol_stop = { ID_ERMPLC_X70,2 };
    POINT gt_anticol_alarm = { ID_ERMPLC_X70,3 };
    POINT gt_anticol_fault = { ID_ERMPLC_X70,4 };
    POINT gt_east_lim_slow = { ID_ERMPLC_X70,5 };
    POINT gt_east_lim_stop = { ID_ERMPLC_X70,6 };
    POINT gt_west_lim_slow = { ID_ERMPLC_X70,7 };
    POINT gt_west_lim_stop = { ID_ERMPLC_X70,8 };
    POINT gt_fix_open = { ID_ERMPLC_X70,9 };
    POINT gt_motor_thermal_trip = { ID_ERMPLC_X70,10 };
    POINT spare5 = { ID_ERMPLC_X70,11 };
    POINT mh_motor_fan_mc = { ID_ERMPLC_X70,12 };
    POINT ah_motor_fan_mc = { ID_ERMPLC_X70,13 };
    POINT bh_motor_fan_mc = { ID_ERMPLC_X70,14 };
    POINT DB_conv_fan_mc = { ID_ERMPLC_X70,15 };
    POINT DB_over_heat = { ID_ERMPLC_X80,0 };
    POINT mh_pos_preset = { ID_ERMPLC_X80,1 };
    POINT mh_data_preset = { ID_ERMPLC_X80,2 };
    POINT slw_panel_high_temp = { ID_ERMPLC_X80,3 };
    POINT leg_emr_stop = { ID_ERMPLC_X80,4 };
    POINT slew_pos_0 = { ID_ERMPLC_X80,5 };
    POINT slew_pos_180 = { ID_ERMPLC_X80,6 };
    POINT slew_grease_active = { ID_ERMPLC_X80,7 };
    POINT slew_grease_fault = { ID_ERMPLC_X80,8 };
    POINT ah_camlim_high_area_emr_up = { ID_ERMPLC_X80,9 };
    POINT ah_camlim_normal_area1_emr_up = { ID_ERMPLC_X80,10 };
    POINT ah_camlim_normal_area2_emr_up = { ID_ERMPLC_X80,11 };
    POINT ah_camlim_bh_down_area_emr_up = { ID_ERMPLC_X80,12 };
    POINT ah_camlim_emr_low = { ID_ERMPLC_X80,13 };
    POINT spare6 = { ID_ERMPLC_X80,14 };
    POINT spare7 = { ID_ERMPLC_X80,15 };

    POINT conv_1_trip = { ID_ERMPLC_X90,0 };
    POINT inv_ctrl_trip = { ID_ERMPLC_X90,1 };
    POINT brk_control_source_trip = { ID_ERMPLC_X90,2 };
    POINT DB_fan_trip = { ID_ERMPLC_X90,3 };
    POINT motor_fan_trip = { ID_ERMPLC_X90,4 };
    POINT spare8 = { ID_ERMPLC_X90,5 };
    POINT slw_grease_trip = { ID_ERMPLC_X90,6 };
    POINT spare9 = { ID_ERMPLC_X90,7 };
    POINT aero_panel_fault = { ID_ERMPLC_X90,8 };
    POINT remote_trace = { ID_ERMPLC_X90,9 };
    POINT inv_reel_battery_low = { ID_ERMPLC_X90,10 };
    POINT inv_reel_gt_ok = { ID_ERMPLC_X90,11 };
    POINT inv_reel_standby = { ID_ERMPLC_X90,12 };
    POINT inv_reel_inv_normal = { ID_ERMPLC_X90,13 };
    POINT spare10 = { ID_ERMPLC_X90,14 };
    POINT inv_reel_escape_enable = { ID_ERMPLC_X90,15 };

    POINT motive_power_ok = { ID_ERMPLC_XA0,0 };
    POINT conv1_mc = { ID_ERMPLC_XA0,1 };
    POINT conv2_mc = { ID_ERMPLC_XA0,2 };
    POINT ctrl_brk_mc = { ID_ERMPLC_XA0,3 };
    POINT mh_brk_mc = { ID_ERMPLC_XA0,4 };
    POINT bh_brk_mc = { ID_ERMPLC_XA0,5 };
    POINT gt_brk_mc = { ID_ERMPLC_XA0,6 };
    POINT ah_brk_mc = { ID_ERMPLC_XA0,7 };
    POINT initial_charge_mc = { ID_ERMPLC_XA0,8 };
    POINT spare11 = { ID_ERMPLC_XA0,9 };
    POINT jib_approch_alarm_enable = { ID_ERMPLC_XA0,10 };
    POINT jib_approch_alarm_disable = { ID_ERMPLC_XA0,11 };
    POINT conv1_over_heat = { ID_ERMPLC_XA0,12 };
    POINT conv2_fuse_down = { ID_ERMPLC_XA0,13 };
    POINT conv2_over_heat = { ID_ERMPLC_XA0,14 };
    POINT conv1_fuse_down = { ID_ERMPLC_XA0,15 };

    POINT thermal_trip_ah_brk = { ID_ERMPLC_XB0,0 };
    POINT thermal_trip_bh_motor = { ID_ERMPLC_XB0,1 };
    POINT thermal_trip_mh_fan = { ID_ERMPLC_XB0,2 };
    POINT thermal_trip_bh_fan = { ID_ERMPLC_XB0,3 };
    POINT thermal_trip_DB_fan = { ID_ERMPLC_XB0,4 };
    POINT thermal_trip_mh_brk = { ID_ERMPLC_XB0,5 };
    POINT thermal_trip_bh_brk = { ID_ERMPLC_XB0,6 };
    POINT thermal_trip_ah_fan = { ID_ERMPLC_XB0,7 };
    POINT braking_unit1_fault = { ID_ERMPLC_XB0,8 };
    POINT braking_unit2_fault = { ID_ERMPLC_XB0,9 };
    POINT braking_unit3_fault = { ID_ERMPLC_XB0,10 };
    POINT braking_unit4_fault = { ID_ERMPLC_XB0,11 };
    POINT braking_unit5_fault = { ID_ERMPLC_XB0,12 };
    POINT braking_unit6_fault = { ID_ERMPLC_XB0,13 };
    POINT braking_unit7_fault = { ID_ERMPLC_XB0,14 };
    POINT braking_unit8_fault = { ID_ERMPLC_XB0,15 };

    POINT mh_preset = { ID_ERMPLC_XC0,0 };
    POINT ah_preset = { ID_ERMPLC_XC0,1 };
    POINT spare12 = { ID_ERMPLC_XC0,2 };
    POINT spare13 = { ID_ERMPLC_XC0,3 };
    POINT spare14 = { ID_ERMPLC_XC0,4 };
    POINT spare15 = { ID_ERMPLC_XC0,5 };
    POINT spare16 = { ID_ERMPLC_XC0,6 };
    POINT spare17 = { ID_ERMPLC_XC0,7 };
    POINT spare18 = { ID_ERMPLC_XC0,8 };
    POINT spare19 = { ID_ERMPLC_XC0,9 };
    POINT spare20 = { ID_ERMPLC_XC0,10 };
    POINT spare21 = { ID_ERMPLC_XC0,11 };
    POINT spare22 = { ID_ERMPLC_XC0,12 };
    POINT spare23 = { ID_ERMPLC_XC0,13 };
    POINT spare24 = { ID_ERMPLC_XC0,14 };
    POINT spare25 = { ID_ERMPLC_XC0,15 };
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
