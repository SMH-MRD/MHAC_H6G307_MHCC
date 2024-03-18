#include "framework.h"
#include "CPLC_IF.h"
#include "PLC_IF.h"
#include "PLC_DEF.h"
#include <windowsx.h>
#include "Mdfunc.h"
#include "OTE0panel.h"

extern ST_SPEC def_spec;
extern ST_KNL_MANAGE_SET    knl_manage_set;

CMCProtocol* pMCProtocol;  //MCプロトコルオブジェクト:
INT16 bit_mask[16] = {BIT0,BIT1,BIT2,BIT3,BIT4,BIT5,BIT6,BIT7,BIT8,BIT9,BIT10,BIT11,BIT12,BIT13,BIT14,BIT15};
ST_PLC_NOTCH_PTN notch_ptn;//PLCのノッチ信号入力パターン

CABPLC_BOUT_MAP cab_bout_map;
CABPLC_BIN_MAP cab_bin_map;
ERMPLC_BOUT_MAP erm_bout_map;
ERMPLC_YOUT_MAP erm_yout_map;
ERMPLC_UXIN_MAP  erm_uxin_map;
ERMPLC_XIN_MAP  erm_xin_map;
ERMPLC_M900_MAP  erm_m900_map;
INV_IF_X_MAP inv_x_map; 
INV_IF_Y_MAP inv_y_map;

CPLC_IF::CPLC_IF(HWND _hWnd_parent) {

    hWnd_parent = _hWnd_parent;
    // 共有メモリオブジェクトのインスタンス化
    pPLCioObj = new CSharedMem;
    pCraneStatusObj = new CSharedMem;
    pSimulationStatusObj = new CSharedMem;
    pAgentInfObj = new CSharedMem;
    pCSInfObj = new CSharedMem;
    pOTEioObj = new CSharedMem;

    out_size = 0;

#ifdef _TYPE_MELSECNET
    memset(&melnet,0,sizeof(ST_MELSEC_NET)) ;      //PLCリンク構造体
    memset(&plc_if_workbuf,0,sizeof(ST_PLC_IO));   //共有メモリへの出力セット作業用バッファ
    melnet.chan = MELSEC_NET_CH;
    melnet.mode = 0;
    melnet.path = NULL;
    melnet.err = 0;
    melnet.status = 0;
    melnet.retry_cnt = MELSEC_NET_RETRY_CNT;
    melnet.read_size_b = sizeof(ST_PLC_READ_B);                         //PLCでLWのbitでセットする為LBは未使用
    melnet.read_size_w = sizeof(ST_PLC_READ_W);
    melnet.write_size_b = sizeof(ST_PLC_WRITE_B);
    melnet.write_size_w = sizeof(ST_PLC_WRITE_W);
#endif

};
CPLC_IF::~CPLC_IF() {
    // 共有メモリオブジェクトの解放
    delete pPLCioObj;
    delete pCraneStatusObj;
    delete pSimulationStatusObj;
    delete pAgentInfObj;
    delete pCSInfObj;
};
int CPLC_IF::set_outbuf(LPVOID pbuf) {
    poutput = pbuf;return 0;
};      //出力バッファセット
BOOL CPLC_IF::show_if_wnd() {
    return SetWindowPos(hWnd_plcif, HWND_TOP, IF_WND_INIT_POS_X, IF_WND_INIT_POS_Y, IF_WND_INIT_SIZE_W, IF_WND_INIT_SIZE_H, SWP_SHOWWINDOW);

}
BOOL CPLC_IF::hide_if_wnd() {
    return SetWindowPos(hWnd_plcif, HWND_TOP, IF_WND_INIT_POS_X, IF_WND_INIT_POS_Y, IF_WND_INIT_SIZE_W, IF_WND_INIT_SIZE_H, SWP_HIDEWINDOW);
}

//******************************************************************************************
/// <summary>
/// init_proc()
/// </summary>
/// <returns></returns>
 
static double knv_inv[MOTION_ID_MAX];//インバータの速度指令をドラムrpsへ変換する係数
int CPLC_IF::init_proc() {

    // 共有メモリ取得

    // 出力用共有メモリ取得
    out_size = sizeof(ST_PLC_IO);
    if (OK_SHMEM != pPLCioObj->create_smem(SMEM_PLC_IO_NAME, (DWORD)out_size, MUTEX_PLC_IO_NAME)) {
        mode |= PLC_IF_PLC_IO_MEM_NG;
    }
    set_outbuf(pPLCioObj->get_pMap());
 
    // 入力用共有メモリ取得
    if (OK_SHMEM != pSimulationStatusObj->create_smem(SMEM_SIMULATION_STATUS_NAME, sizeof(ST_SIMULATION_STATUS), MUTEX_SIMULATION_STATUS_NAME)) {
        mode |= PLC_IF_SIM_MEM_NG;
    }
    pSim = (LPST_SIMULATION_STATUS)pSimulationStatusObj->get_pMap();                        //シミュレータ共有メモリ

    if (OK_SHMEM != pCraneStatusObj->create_smem(SMEM_CRANE_STATUS_NAME, sizeof(ST_CRANE_STATUS), MUTEX_CRANE_STATUS_NAME)) {
        mode |= PLC_IF_CRANE_MEM_NG;
    }
    pCrane = (LPST_CRANE_STATUS)pCraneStatusObj->get_pMap();                                //クレーンステータス共有メモリ

    if (OK_SHMEM != pAgentInfObj->create_smem(SMEM_AGENT_INFO_NAME, sizeof(ST_AGENT_INFO), MUTEX_AGENT_INFO_NAME)){
        mode |= PLC_IF_AGENT_MEM_NG;
    }
    pAgentInf = (LPST_AGENT_INFO)pAgentInfObj->get_pMap();                                  //Agentコントロール共有メモリ

    if (OK_SHMEM != pCSInfObj->create_smem(SMEM_CS_INFO_NAME, sizeof(ST_CS_INFO), MUTEX_CS_INFO_NAME)) {
        mode |= PLC_IF_CS_MEM_NG;
    }
    pCSInf = (LPST_CS_INFO)pCSInfObj->get_pMap(); 
    
    
    if (OK_SHMEM != pOTEioObj->create_smem(SMEM_OTE_IO_NAME, sizeof(ST_OTE_IO), MUTEX_OTE_IO_NAME)) {
        mode |= PLC_IF_OTE_MEM_NG;
    }
    pOTEio = (LPST_OTE_IO)pOTEioObj->get_pMap();
 
    //MCプロトコルオブジェクトインスタンス化
    pMCProtocol = new CMCProtocol;

    //MCプロトコル通信用アドレス設定
    //# クライアント（受信）ソケットアドレス
    memset(&addrinc, 0, sizeof(addrinc));
    addrinc.sin_port = htons(PORT_MC_CLIENT);
    addrinc.sin_family = AF_INET;
    inet_pton(AF_INET, IP_ADDR_MC_CLIENT, &addrinc.sin_addr.s_addr);

    //# サーバー（送信先）ソケットアドレス
    memset(&addrins, 0, sizeof(addrins));
    addrins.sin_port = htons(PORT_MC_SERVER);
    addrins.sin_family = AF_INET;
    inet_pton(AF_INET, IP_ADDR_MC_SERVER, &addrins.sin_addr.s_addr);

    //MCプロトコルオブジェクト初期化
    hWnd_plcif = pMCProtocol->Initialize(hWnd_parent, &addrinc, &addrins, MC_ADDR_D_READ, MC_SIZE_D_READ, MC_ADDR_D_WRITE, MC_SIZE_D_WRITE);

    SetWindowPos(hWnd_plcif, HWND_TOP, IF_WND_INIT_POS_X, IF_WND_INIT_POS_Y, IF_WND_INIT_SIZE_W, IF_WND_INIT_SIZE_H, SWP_SHOWWINDOW);

    lp_PLCread = (LPST_PLC_READ)(pMCProtocol->mc_res_msg_r.res_data);
    lp_PLCwrite = (LPST_PLC_WRITE)(pMCProtocol->mc_req_msg_w.req_data);

    knv_inv[ID_HOIST]= def_spec.prm_drv[DRIVE_ITEM_NMAX_RATIO][ID_HOIST] * def_spec.prm_drv[DRIVE_ITEM_RATE_NV][ID_HOIST] / 20000.0;
    knv_inv[ID_AHOIST] = def_spec.prm_drv[DRIVE_ITEM_NMAX_RATIO][ID_AHOIST] * def_spec.prm_drv[DRIVE_ITEM_RATE_NV][ID_AHOIST] / 20000.0;
    knv_inv[ID_BOOM_H] = def_spec.prm_drv[DRIVE_ITEM_NMAX_RATIO][ID_BOOM_H] * def_spec.prm_drv[DRIVE_ITEM_RATE_NV][ID_BOOM_H] / 20000.0;
    knv_inv[ID_SLEW] = def_spec.prm_drv[DRIVE_ITEM_NMAX_RATIO][ID_SLEW] * def_spec.prm_drv[DRIVE_ITEM_RATE_NV][ID_SLEW] / 20000.0;
    knv_inv[ID_GANTRY] = def_spec.prm_drv[DRIVE_ITEM_NMAX_RATIO][ID_GANTRY] * def_spec.prm_drv[DRIVE_ITEM_RATE_NV][ID_GANTRY] / 20000.0;

    //データ変換計算用パラメータセット
    while (!pCrane->is_crane_status_ok) Sleep(100);//MAINプロセスの仕様取り込み完了待ち

    return int(mode & 0xff00);
}
//*********************************************************************************************
/// <summary>
/// input()
/// </summary>
/// <returns></returns>
int CPLC_IF::input() {

    //MAINプロセス(Environmentタスクのヘルシー信号取り込み）
    source_counter = pCrane->env_act_count;

    UINT16 plc_helthy = lp_PLCread->helthy;

    return 0;
}
//*********************************************************************************************
// parse()
//*********************************************************************************************
int CPLC_IF::clear_plc_write() { 
    memset(lp_PLCwrite, 0, sizeof(ST_PLC_WRITE));
    return 0; 
}

//*********************************************************************************************
// parse_data_in() PLCからの入力信号を展開
//*********************************************************************************************



static double th_bh_hold;//起伏角速度計算用バッファ
static double th_bh_dt = (double)(PLC_IF_TH_BH_CHK_COUNT * SYSTEM_TICK_ms) / 1000.0;//起伏角速度計算用バッファ

int CPLC_IF::parse_data_in() {


    //受信バッファの内容をワークバッファにコピー
    memcpy_s(&(plc_if_workbuf.input), sizeof(PLC_READ_BUF),lp_PLCread, sizeof(PLC_READ_BUF));

    //共有メモリ出力内容取り込みセット
    //単位m
    plc_if_workbuf.pos[ID_HOIST]    = (double)(plc_if_workbuf.input.rbuf.pos[0]) / 1000.0;  //アブソコーダのPLC計算結果mmをm単位で
    plc_if_workbuf.pos[ID_AHOIST]   = (double)(plc_if_workbuf.input.rbuf.pos[1]) / 1000.0;  //アブソコーダのPLC計算結果mmをm単位で
    plc_if_workbuf.pos[ID_BOOM_H]   = (double)(plc_if_workbuf.input.rbuf.pos[2]) / 100.0;   //モーメントリミッタの半径出力をm単位で

    //起伏角
    plc_if_workbuf.th_bh = acos(plc_if_workbuf.pos[ID_BOOM_H] / def_spec.Lm);               //起伏角acos(R/Lm)

   if ((plc_if_workbuf.healthy_cnt % PLC_IF_TH_BH_CHK_COUNT) == 0) { //200msec毎に計算
       plc_if_workbuf.dth_bh = (plc_if_workbuf.th_bh - th_bh_hold) / th_bh_dt;
       th_bh_hold = plc_if_workbuf.th_bh;
   }
  
    //ロープ長
    plc_if_workbuf.lmh = def_spec.Hp + def_spec.Lm * sin(plc_if_workbuf.th_bh) - plc_if_workbuf.pos[ID_HOIST];
    plc_if_workbuf.lah = def_spec.Hp + def_spec.La * sin(plc_if_workbuf.th_bh) - plc_if_workbuf.pos[ID_AHOIST];

    //旋回、起伏は高速カウンタとモーメントリミッタより
    //高速カウンタユニットのカウント値を変換　カウント値15000000の時旋回角0 20201510の時旋回角π
    //θ=count×π/(20201510-15000000)モータ1回転で　ピニオン減速比428.4　旋回角＝ピニオン回転数*2π*14/166 0.00000716144;//ピニオン回転位置
    //plc_if_workbuf.pos[ID_SLEW]     *= 14.0/166.0;//旋回角度（ピニオン径／TTB径）
    plc_if_workbuf.pos[ID_SLEW] = (double)(plc_if_workbuf.input.rbuf.pos[3] - (INT32)15000000) * 0.000000603977;//ピニオン回転位置
    //想定アブソコーダのPLC計算結果mmをm単位で
    plc_if_workbuf.pos[ID_GANTRY] = (double)(plc_if_workbuf.input.rbuf.pos[4]) / 1000.0;
   
    //PLC読み込み値は0.1％単位絶対値　SIMへのセットは符号化して1.0で正規化
    plc_if_workbuf.nv_tg[ID_GANTRY] = (double)(plc_if_workbuf.input.rbuf.spd_tg[0]) / 1000.0;
    plc_if_workbuf.nv_tg[ID_AHOIST] = (double)(plc_if_workbuf.input.rbuf.spd_tg[1]) / 1000.0;
    plc_if_workbuf.nv_tg[ID_SLEW] = (double)(plc_if_workbuf.input.rbuf.spd_tg[2]) / 1000.0;
    plc_if_workbuf.nv_tg[ID_BOOM_H] = (double)(plc_if_workbuf.input.rbuf.spd_tg[3]) / 1000.0;
    plc_if_workbuf.nv_tg[ID_HOIST] = (double)(plc_if_workbuf.input.rbuf.spd_tg[4]) / 1000.0;

    if (plc_if_workbuf.input.rbuf.inv_cc_y[ID_MC_INV_GNT] & 0x0002)   plc_if_workbuf.nv_tg[ID_GANTRY] *= -1.0;//逆転指令時
    if (plc_if_workbuf.input.rbuf.inv_cc_y[ID_MC_INV_AH] & 0x0002)   plc_if_workbuf.nv_tg[ID_AHOIST] *= -1.0;
    if (plc_if_workbuf.input.rbuf.inv_cc_y[ID_MC_INV_SLW] & 0x0002)   plc_if_workbuf.nv_tg[ID_SLEW] *= -1.0;
    if (plc_if_workbuf.input.rbuf.inv_cc_y[ID_MC_INV_BH] & 0x0002)   plc_if_workbuf.nv_tg[ID_BOOM_H] *= -1.0;
    if (plc_if_workbuf.input.rbuf.inv_cc_y[ID_MC_INV_MH1] & 0x0002)   plc_if_workbuf.nv_tg[ID_HOIST] *= -1.0;

    //INV 指令は±20000が±Nmax　SIMへのセットはドラムのrpsでセット
    plc_if_workbuf.nv_ref[ID_HOIST] = (double)(plc_if_workbuf.input.rbuf.inv_cc_Ww1[ID_MC_INV_MH1]) * knv_inv[ID_HOIST];
    if (plc_if_workbuf.input.rbuf.inv_cc_y[ID_MC_INV_MH1] & PLC_IF_INV_DIO_REV) plc_if_workbuf.nv_ref[ID_HOIST] *= -1.0;

    plc_if_workbuf.nv_ref[ID_AHOIST] = (double)(plc_if_workbuf.input.rbuf.inv_cc_Ww1[ID_MC_INV_AH]) * knv_inv[ID_AHOIST];
    if (plc_if_workbuf.input.rbuf.inv_cc_y[ID_MC_INV_AH] & PLC_IF_INV_DIO_REV) plc_if_workbuf.nv_ref[ID_AHOIST] *= -1.0;

    plc_if_workbuf.nv_ref[ID_BOOM_H] = (double)(plc_if_workbuf.input.rbuf.inv_cc_Ww1[ID_MC_INV_BH]) * knv_inv[ID_BOOM_H];
    if (plc_if_workbuf.input.rbuf.inv_cc_y[ID_MC_INV_BH] & PLC_IF_INV_DIO_REV) plc_if_workbuf.nv_ref[ID_BOOM_H] *= -1.0;

    plc_if_workbuf.nv_ref[ID_SLEW] = (double)(plc_if_workbuf.input.rbuf.inv_cc_Ww1[ID_MC_INV_SLW]) * knv_inv[ID_SLEW];
    if (plc_if_workbuf.input.rbuf.inv_cc_y[ID_MC_INV_SLW] & PLC_IF_INV_DIO_REV) plc_if_workbuf.nv_ref[ID_SLEW] *= -1.0;

    plc_if_workbuf.nv_ref[ID_GANTRY] = (double)(plc_if_workbuf.input.rbuf.inv_cc_Ww1[ID_MC_INV_GNT]) * knv_inv[ID_GANTRY];
    if (plc_if_workbuf.input.rbuf.inv_cc_y[ID_MC_INV_GNT] & PLC_IF_INV_DIO_REV) plc_if_workbuf.nv_ref[ID_GANTRY] *= -1.0;

    //速度FB
    //INV FBはrpmのアンサバックをそのままセット
    plc_if_workbuf.v_fb_rpm[ID_HOIST]  = (double)(plc_if_workbuf.input.rbuf.inv_cc_Wr1[ID_MC_INV_MH1]);
    plc_if_workbuf.v_fb_rpm[ID_AHOIST] = (double)(plc_if_workbuf.input.rbuf.inv_cc_Wr1[ID_MC_INV_AH]);
    plc_if_workbuf.v_fb_rpm[ID_BOOM_H] = (double)(plc_if_workbuf.input.rbuf.inv_cc_Wr1[ID_MC_INV_BH]);
    plc_if_workbuf.v_fb_rpm[ID_SLEW]   = (double)(plc_if_workbuf.input.rbuf.inv_cc_Wr1[ID_MC_INV_SLW]);
    plc_if_workbuf.v_fb_rpm[ID_GANTRY] = (double)(plc_if_workbuf.input.rbuf.inv_cc_Wr1[ID_MC_INV_GNT]);
    //速度FB　m/s　rad/s
    plc_if_workbuf.v_fb[ID_HOIST]   = plc_if_workbuf.v_fb_rpm[ID_HOIST]/def_spec.prm_drv[DRIVE_ITEM_RPM][ID_HOIST]* def_spec.prm_drv[DRIVE_ITEM_RATE_V][ID_HOIST];
    plc_if_workbuf.v_fb[ID_AHOIST]  = plc_if_workbuf.v_fb_rpm[ID_AHOIST] / def_spec.prm_drv[DRIVE_ITEM_RPM][ID_AHOIST] * def_spec.prm_drv[DRIVE_ITEM_RATE_V][ID_AHOIST];
    plc_if_workbuf.v_fb[ID_BOOM_H]  = plc_if_workbuf.v_fb_rpm[ID_BOOM_H] / def_spec.prm_drv[DRIVE_ITEM_RPM][ID_BOOM_H] * def_spec.prm_drv[DRIVE_ITEM_RATE_V][ID_BOOM_H];
    plc_if_workbuf.v_fb[ID_SLEW]    = plc_if_workbuf.v_fb_rpm[ID_SLEW] / def_spec.prm_drv[DRIVE_ITEM_RPM][ID_SLEW] * def_spec.prm_drv[DRIVE_ITEM_RATE_V][ID_SLEW];
    plc_if_workbuf.v_fb[ID_GANTRY]  = plc_if_workbuf.v_fb_rpm[ID_GANTRY] / def_spec.prm_drv[DRIVE_ITEM_RPM][ID_GANTRY] * def_spec.prm_drv[DRIVE_ITEM_RATE_V][ID_GANTRY];
    //速度FB　ノッチ表現
    plc_if_workbuf.v_fb_notch[ID_HOIST]     = get_notch_from_spd(ID_HOIST, plc_if_workbuf.v_fb[ID_HOIST]);
    plc_if_workbuf.v_fb_notch[ID_AHOIST]    = get_notch_from_spd(ID_AHOIST,plc_if_workbuf.v_fb[ID_AHOIST]);
    plc_if_workbuf.v_fb_notch[ID_BOOM_H]    = get_notch_from_spd(ID_BOOM_H,plc_if_workbuf.v_fb[ID_BOOM_H]);
    plc_if_workbuf.v_fb_notch[ID_SLEW]      = get_notch_from_spd(ID_SLEW,plc_if_workbuf.v_fb[ID_SLEW]);
    plc_if_workbuf.v_fb_notch[ID_GANTRY]    = get_notch_from_spd(ID_GANTRY,plc_if_workbuf.v_fb[ID_GANTRY]);

    //ブレーキ状態セット
    if (plc_if_workbuf.input.rbuf.erm_x[erm_xin_map.mh_brk_mc.x] & erm_xin_map.mh_brk_mc.y) plc_if_workbuf.brk[ID_HOIST] = true; else plc_if_workbuf.brk[ID_HOIST] = false;
    if (plc_if_workbuf.input.rbuf.erm_x[erm_xin_map.ah_brk_mc.x] & erm_xin_map.ah_brk_mc.y)plc_if_workbuf.brk[ID_AHOIST] = true; else plc_if_workbuf.brk[ID_AHOIST] = false;
    if (plc_if_workbuf.input.rbuf.erm_x[erm_xin_map.gt_brk_mc.x] & erm_xin_map.gt_brk_mc.y)plc_if_workbuf.brk[ID_GANTRY] = true; else plc_if_workbuf.brk[ID_GANTRY] = false;
    if (plc_if_workbuf.input.rbuf.erm_x[erm_xin_map.bh_brk_mc.x] & erm_xin_map.bh_brk_mc.y)plc_if_workbuf.brk[ID_BOOM_H] = true; else plc_if_workbuf.brk[ID_BOOM_H] = false;
    plc_if_workbuf.brk[ID_SLEW]  = true; //旋回はブレーキ無し

    //極限LIMIT
   //共有メモリへの極限信号状態セット
    plc_if_workbuf.endlim[ID_HOIST] = 0;
    if (!(plc_if_workbuf.input.rbuf.erm_x[erm_xin_map.mh_high_area_emr_up_lim.x] & erm_xin_map.mh_high_area_emr_up_lim.y))
        plc_if_workbuf.endlim[ID_HOIST] |= PLC_IF_LIMIT_COM_FWD_EMR;
    if (!(plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_down_area_emr_up_lim.x] & erm_xin_map.bh_down_area_emr_up_lim.y))
        plc_if_workbuf.endlim[ID_HOIST] |= PLC_IF_LIMIT_COM_FWD_EMR_EX1;
    if (!(plc_if_workbuf.input.rbuf.erm_x[erm_xin_map.mh_emr_lower_lim.x] & erm_xin_map.mh_emr_lower_lim.y))
        plc_if_workbuf.endlim[ID_HOIST] |= PLC_IF_LIMIT_COM_REV_EMR;

    plc_if_workbuf.endlim[ID_BOOM_H] = 0;
    if (!(plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_emr_in_lim.x] & erm_xin_map.bh_emr_in_lim.y))
        plc_if_workbuf.endlim[ID_BOOM_H] |= PLC_IF_LIMIT_COM_FWD_EMR;
    if (!(plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_normal_in_lim.x] & erm_xin_map.bh_normal_in_lim.y))
        plc_if_workbuf.endlim[ID_BOOM_H] |= PLC_IF_LIMIT_COM_FWD_NORM;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.slw_spd_change.x] |= erm_xin_map.slw_spd_change.y;
    if (!(plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.high_pos_emr_out_lim.x] & erm_xin_map.high_pos_emr_out_lim.y))
        plc_if_workbuf.endlim[ID_BOOM_H] |= PLC_IF_LIMIT_COM_REV_EMR_EX1;
    if (!(plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_emr_out_lim.x] & erm_xin_map.bh_emr_out_lim.y))
        plc_if_workbuf.endlim[ID_BOOM_H] |= PLC_IF_LIMIT_COM_REV_EMR;
    if (!(plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_down_area_out_lim.x] & erm_xin_map.bh_down_area_out_lim.y))
        plc_if_workbuf.endlim[ID_BOOM_H] |= PLC_IF_LIMIT_COM_REV_NORM_EX1;
    if (!(plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_rest_slow.x] & erm_xin_map.bh_rest_slow.y))
        plc_if_workbuf.endlim[ID_BOOM_H] |= PLC_IF_LIMIT_COM_REV_SLOW;
    if (!(plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_rest_lim.x] & erm_xin_map.bh_rest_lim.y))
        plc_if_workbuf.endlim[ID_BOOM_H] |= PLC_IF_LIMIT_COM_REV_NORM;

    //X70
    plc_if_workbuf.endlim[ID_GANTRY] = 0;
    if (plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_anticol_stop.x] & erm_xin_map.gt_anticol_stop.y) //衝突防止　信号1点なので正逆同時セット
        plc_if_workbuf.endlim[ID_GANTRY] |= (PLC_IF_LIMIT_COM_FWD_NORM_EX1 | PLC_IF_LIMIT_COM_REV_NORM_EX1);
    if (plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_east_lim_slow.x] & erm_xin_map.gt_east_lim_slow.y)
        plc_if_workbuf.endlim[ID_GANTRY] |= PLC_IF_LIMIT_COM_FWD_SLOW;
    if (plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_east_lim_stop.x] & erm_xin_map.gt_east_lim_stop.y)
        plc_if_workbuf.endlim[ID_GANTRY] |= PLC_IF_LIMIT_COM_FWD_NORM;
    if (plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_west_lim_slow.x] & erm_xin_map.gt_west_lim_slow.y)
        plc_if_workbuf.endlim[ID_GANTRY] |= PLC_IF_LIMIT_COM_REV_SLOW;
    if (plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_west_lim_stop.x] & erm_xin_map.gt_west_lim_stop.y)
        plc_if_workbuf.endlim[ID_GANTRY] |= PLC_IF_LIMIT_COM_REV_NORM;
 
    //X80
    plc_if_workbuf.endlim[ID_AHOIST] = 0;
    if (!(plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.ah_camlim_high_area_emr_up.x] & erm_xin_map.ah_camlim_high_area_emr_up.y))
        plc_if_workbuf.endlim[ID_AHOIST] |= PLC_IF_LIMIT_COM_FWD_EMR;
    if (!(plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.ah_camlim_normal_area1_emr_up.x] & erm_xin_map.ah_camlim_normal_area1_emr_up.y))
        plc_if_workbuf.endlim[ID_AHOIST] |= PLC_IF_LIMIT_COM_FWD_EMR_EX1;
    if (!(plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.ah_camlim_normal_area2_emr_up.x] & erm_xin_map.ah_camlim_normal_area2_emr_up.y))
        plc_if_workbuf.endlim[ID_AHOIST] |= PLC_IF_LIMIT_COM_FWD_EMR_EX2;
    if (!(plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.ah_camlim_bh_down_area_emr_up.x] & erm_xin_map.ah_camlim_bh_down_area_emr_up.y))
        plc_if_workbuf.endlim[ID_AHOIST] |= PLC_IF_LIMIT_COM_FWD_EMR_EX3;
    if (!(plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.ah_camlim_emr_low.x] & erm_xin_map.ah_camlim_emr_low.y))
        plc_if_workbuf.endlim[ID_AHOIST] |= PLC_IF_LIMIT_COM_REV_EMR;

    //運転モード
    if (plc_if_workbuf.input.rbuf.cab_bi[cab_bin_map.mh_spd_low.x] & cab_bin_map.mh_high_spd.y) {
        plc_if_workbuf.mh_spd_mode = PLC_IF_ID_MH21;
        plc_if_workbuf.v_ratio[ID_HOIST] = def_spec.notch_spd_retio[ID_HOIST][PLC_IF_ID_MH21];
    }
    else if (plc_if_workbuf.input.rbuf.cab_bi[cab_bin_map.mh_spd_low.x] & cab_bin_map.mh_spd_middle.y) {
        plc_if_workbuf.mh_spd_mode = PLC_IF_ID_MH14;
        plc_if_workbuf.v_ratio[ID_HOIST] = def_spec.notch_spd_retio[ID_HOIST][PLC_IF_ID_MH14];
     }
    else {
        plc_if_workbuf.mh_spd_mode = PLC_IF_ID_MH7;
        plc_if_workbuf.v_ratio[ID_HOIST] = def_spec.notch_spd_retio[ID_HOIST][PLC_IF_ID_MH7];
    }

    if (plc_if_workbuf.input.rbuf.cab_bi[cab_bin_map.ah_sel_high_spd.x] & cab_bin_map.ah_sel_high_spd.y) {
        plc_if_workbuf.mh_spd_mode = PLC_IF_ID_AH24;
        plc_if_workbuf.v_ratio[ID_AHOIST] = def_spec.notch_spd_retio[ID_HOIST][PLC_IF_ID_AH24];
    }
    else {
        plc_if_workbuf.mh_spd_mode = PLC_IF_ID_AH12;
        plc_if_workbuf.v_ratio[ID_AHOIST] = def_spec.notch_spd_retio[ID_HOIST][PLC_IF_ID_AH12];
    }

    if (plc_if_workbuf.input.rbuf.cab_bi[cab_bin_map.bh_rest.x] & cab_bin_map.bh_rest.y)
        plc_if_workbuf.mh_spd_mode = PLC_IF_ID_BH_REST;
    else if (plc_if_workbuf.input.rbuf.cab_bi[cab_bin_map.bh_down.x] & cab_bin_map.bh_down.y)
        plc_if_workbuf.mh_spd_mode = PLC_IF_ID_BH62;
    else
        plc_if_workbuf.mh_spd_mode = PLC_IF_ID_BH57;
      
 
    return 0;
}
//*********************************************************************************************
// parse_data_out() PLCへの出力信号生成
//*********************************************************************************************
int CPLC_IF::parse_data_out() {

    //PC 操作有効信号
    plc_if_workbuf.output.wbuf.ctrl_mode = (INT16)mode;

 //パネルシミュレーション
#pragma region OPEROOM
    // 非常停止　PB（主幹OFF　PB）　*PLC NORMAL CLOSE
    if ((pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_PB_HIJYOU]) || (pOTEio->ote_estop)) {
         plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.cab_estp.x] &= ~cab_bout_map.cab_estp.y; //非常停止
    }
    else {
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.cab_estp.x] |= cab_bout_map.cab_estp.y; //非常停止
    }

    if (pCSInf->ote_remote_status &= CS_CODE_OTE_REMOTE_ENABLE) {//端末操作有効
        // 主幹ON/OFF　PB
        if (pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_PB_SYUKAN]) {
           //トリガ時セット
            if (!(plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_off.x] & cab_bout_map.ctrl_off.y)) //切指令優先                     //主幹切 ON（切指令ON）
            {
                plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_on.x] &= ~cab_bout_map.ctrl_on.y;
            }
            else if ((plc_if_workbuf.input.rbuf.erm_bo[erm_bout_map.ctrl_source_mc_ok.x] & erm_bout_map.ctrl_source_mc_ok.y)
                && (plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_off.x] & cab_bout_map.ctrl_off.y)
                && !(plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_on.x] & cab_bout_map.ctrl_on.y))

            {//主幹入り時は主幹切PBと認識
                plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_off.x] &= ~cab_bout_map.ctrl_off.y;
            }
        
            else if (!(plc_if_workbuf.input.rbuf.erm_bo[erm_bout_map.ctrl_source_mc_ok.x] & erm_bout_map.ctrl_source_mc_ok.y)   //主幹投入指令 OFF
                && !(plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_on.x] & cab_bout_map.ctrl_on.y)                      //主幹入 OFF
                && (plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_off.x] & cab_bout_map.ctrl_off.y))                      //主幹切 ON（切指令ON）
            {
                plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_on.x] |= cab_bout_map.ctrl_on.y;
            }
            else;
        }
        else {
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_on.x] &= ~cab_bout_map.ctrl_on.y;
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_off.x] |= cab_bout_map.ctrl_off.y;//主幹　切はNormal Close
        }

        //故障リセット　IL　Bypass
        if (pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_PB_FLT_RESET])
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.fault_reset.x] |= cab_bout_map.fault_reset.y;
        else
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.fault_reset.x] &= ~cab_bout_map.fault_reset.y;

        if (pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_CHK_IL_BYPASS])
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.il_bypass.x] |= cab_bout_map.il_bypass.y;
        else
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.il_bypass.x] &= ~cab_bout_map.il_bypass.y;

        //***####$$$$$ ノッチ
        UINT ui_notch;
        if (pCSInf->auto_mode) {//AGENT 出力をセット;

            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_mh.x] &= notch_ptn.bits[ID_HOIST][PLC_IF_INDEX_NOTCH_PTN_CLR];
            if (pCSInf->auto_status[ID_HOIST] !=STAT_MANUAL) {
                plc_if_workbuf.v_com_notch[ID_HOIST] = get_notch_from_spd(ID_HOIST, pAgentInf->v_ref[ID_HOIST]);
                ui_notch = plc_if_workbuf.v_com_notch[ID_HOIST] + NOTCH_4;//ノッチ配列設定は-4ノッチが0
                plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_mh.x] |= notch_ptn.bits[ID_HOIST][ui_notch];
            }
            else {
                plc_if_workbuf.v_com_notch[ID_HOIST] = pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_HOIST] - NOTCH_4;
                plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_mh.x] |= notch_ptn.bits[ID_HOIST][pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_HOIST]];
            }

            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_gt.x] &= notch_ptn.bits[ID_GANTRY][PLC_IF_INDEX_NOTCH_PTN_CLR];
            if (pCSInf->auto_status[ID_GANTRY] != STAT_MANUAL) {
                plc_if_workbuf.v_com_notch[ID_GANTRY] = get_notch_from_spd(ID_GANTRY, pAgentInf->v_ref[ID_GANTRY]);
                ui_notch = plc_if_workbuf.v_com_notch[ID_GANTRY] + NOTCH_4;//ノッチ配列設定は-4ノッチが0
                plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_gt.x] |= notch_ptn.bits[ID_GANTRY][ui_notch];
            }
            else {
                plc_if_workbuf.v_com_notch[ID_HOIST] = pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_GANTRY] - NOTCH_4;
                 plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_gt.x] |= notch_ptn.bits[ID_GANTRY][pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_GANTRY]];
            }

            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_bh.x] &= notch_ptn.bits[ID_BOOM_H][PLC_IF_INDEX_NOTCH_PTN_CLR];
            if (pCSInf->auto_status[ID_BOOM_H] != STAT_MANUAL) {
                plc_if_workbuf.v_com_notch[ID_BOOM_H] = get_notch_from_spd(ID_BOOM_H, pAgentInf->v_ref[ID_BOOM_H]);
                ui_notch = plc_if_workbuf.v_com_notch[ID_BOOM_H] + NOTCH_4;//ノッチ配列設定は-4ノッチが0
                plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_bh.x] |= notch_ptn.bits[ID_BOOM_H][ui_notch];
            }
            else {
                  plc_if_workbuf.v_com_notch[ID_BOOM_H] = pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_BOOM_H] - NOTCH_4;
                  plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_bh.x] |= notch_ptn.bits[ID_BOOM_H][pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_BOOM_H]];
            }

             plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_sl.x] &= notch_ptn.bits[ID_SLEW][PLC_IF_INDEX_NOTCH_PTN_CLR];
            if (pCSInf->auto_status[ID_SLEW] != STAT_MANUAL) {
                plc_if_workbuf.v_com_notch[ID_SLEW] = get_notch_from_spd(ID_SLEW, pAgentInf->v_ref[ID_SLEW]);
                ui_notch = plc_if_workbuf.v_com_notch[ID_SLEW] + NOTCH_4;//ノッチ配列設定は-4ノッチが0
                plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_sl.x] |= notch_ptn.bits[ID_SLEW][ui_notch];
            }
            else {
                plc_if_workbuf.v_com_notch[ID_SLEW] = pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_SLEW] - NOTCH_4;
                plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_sl.x] |= notch_ptn.bits[ID_SLEW][pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_SLEW]];
            }

            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_ah.x] &= notch_ptn.bits[ID_AHOIST][PLC_IF_INDEX_NOTCH_PTN_CLR];
            if (pCSInf->auto_status[ID_AHOIST] != STAT_MANUAL) {
                plc_if_workbuf.v_com_notch[ID_AHOIST] = get_notch_from_spd(ID_AHOIST, pAgentInf->v_ref[ID_BOOM_H]);
                ui_notch = plc_if_workbuf.v_com_notch[ID_AHOIST] + NOTCH_4;//ノッチ配列設定は-4ノッチが0
                plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_ah.x] |= notch_ptn.bits[ID_AHOIST][ui_notch];
            }
            else {
                 plc_if_workbuf.v_com_notch[ID_AHOIST] = pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_AHOIST] - NOTCH_4;
                 plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_ah.x] |= notch_ptn.bits[ID_AHOIST][pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_AHOIST]];
            }
        }
        else {//OTEノッチ信号セット
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_mh.x] &= notch_ptn.bits[ID_HOIST][PLC_IF_INDEX_NOTCH_PTN_CLR];
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_mh.x] |= notch_ptn.bits[ID_HOIST][pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_HOIST]];
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_gt.x] &= notch_ptn.bits[ID_GANTRY][PLC_IF_INDEX_NOTCH_PTN_CLR];
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_gt.x] |= notch_ptn.bits[ID_GANTRY][pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_GANTRY]];
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_bh.x] &= notch_ptn.bits[ID_BOOM_H][PLC_IF_INDEX_NOTCH_PTN_CLR];
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_bh.x] |= notch_ptn.bits[ID_BOOM_H][pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_BOOM_H]];
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_sl.x] &= notch_ptn.bits[ID_SLEW][PLC_IF_INDEX_NOTCH_PTN_CLR];
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_sl.x] |= notch_ptn.bits[ID_SLEW][pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_SLEW]];
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_ah.x] &= notch_ptn.bits[ID_AHOIST][PLC_IF_INDEX_NOTCH_PTN_CLR];
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.notch_ah.x] |= notch_ptn.bits[ID_AHOIST][pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_AHOIST]];

            plc_if_workbuf.v_com_notch[ID_HOIST]    = pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_HOIST] - NOTCH_4;
            plc_if_workbuf.v_com_notch[ID_GANTRY]   = pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_GANTRY] - NOTCH_4;
            plc_if_workbuf.v_com_notch[ID_BOOM_H]   = pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_BOOM_H] - NOTCH_4;
            plc_if_workbuf.v_com_notch[ID_SLEW]     = pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_SLEW] - NOTCH_4;
            plc_if_workbuf.v_com_notch[ID_AHOIST]   = pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_AHOIST] - NOTCH_4;
          }

        //モード設定
        if (pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_RADIO_MHSPD_7   ])   plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mh_spd_low.x] |= cab_bout_map.mh_spd_low.y;
        else                                                            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mh_spd_low.x] &= ~cab_bout_map.mh_spd_low.y;
        
        if (pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_RADIO_MHSPD_14  ])   plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mh_spd_middle.x] |= cab_bout_map.mh_spd_middle.y;
        else                                                            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mh_spd_middle.x] &= ~cab_bout_map.mh_spd_middle.y;
        
        if (pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_RADIO_MHSPD_21  ])   plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mh_high_spd.x] |= cab_bout_map.mh_high_spd.y;
        else                                                            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mh_high_spd.x] &= ~cab_bout_map.mh_high_spd.y;
        
        if (pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_RADIO_AHSPD_14  ])   plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ah_sel_low_spd.x] |= cab_bout_map.ah_sel_low_spd.y;
        else                                                            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ah_sel_low_spd.x] &= ~cab_bout_map.ah_sel_low_spd.y;
        
        if (pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_RADIO_AHSPD_24  ])   plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ah_sel_high_spd.x] |= cab_bout_map.ah_sel_high_spd.y;
        else                                                            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ah_sel_high_spd.x] &= ~cab_bout_map.ah_sel_high_spd.y;
        
        if (pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_RADIO_BH_57     ])   plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.bh_normal.x] |= cab_bout_map.bh_normal.y;
        else                                                            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.bh_normal.x] &= ~cab_bout_map.bh_normal.y;
        
        if (pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_RADIO_BH_62     ])   plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.bh_down.x] |= cab_bout_map.bh_down.y;
        else                                                            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.bh_down.x] &= ~cab_bout_map.bh_down.y;
        
        if (pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_RADIO_BH_REST   ])   plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.bh_rest.x] |= cab_bout_map.bh_rest.y;
        else                                                            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.bh_rest.x] &= ~cab_bout_map.bh_rest.y;
        
        if (pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_CHK_GT_SHOCK]) { 
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.gt_warm_shock_on.x] |= cab_bout_map.gt_warm_shock_on.y;
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.gt_warm_shock_off.x] &= ~cab_bout_map.gt_warm_shock_off.y;
        }
        else {
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.gt_warm_shock_off.x] |= cab_bout_map.gt_warm_shock_off.y;
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.gt_warm_shock_on.x] &= ~cab_bout_map.gt_warm_shock_on.y;
        }
        
        if (pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_RADIO_JIB_NARROW]) {
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.jib_approch_narrow.x] |= cab_bout_map.jib_approch_narrow.y;
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.jib_approch_wide.x] &= ~cab_bout_map.jib_approch_wide.y;
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.jib_approch_stop.x] &= ~cab_bout_map.jib_approch_stop.y;
        }
        else if (pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_RADIO_JIB_WIDE]) {
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.jib_approch_narrow.x] &= ~cab_bout_map.jib_approch_narrow.y;
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.jib_approch_wide.x] |= cab_bout_map.jib_approch_wide.y;
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.jib_approch_stop.x] &= ~cab_bout_map.jib_approch_stop.y;
        }
        else {
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.jib_approch_narrow.x] &= ~cab_bout_map.jib_approch_narrow.y;
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.jib_approch_wide.x] &= cab_bout_map.jib_approch_wide.y;
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.jib_approch_stop.x] |=  ~cab_bout_map.jib_approch_stop.y;
        }
    }
    else {
        //端末無効時は、主幹OFFPB 非常停止　入力状態
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_on.x] &= ~cab_bout_map.ctrl_on.y;
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_off.x] &= ~cab_bout_map.ctrl_off.y;
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.cab_estp.x] &= ~cab_bout_map.cab_estp.y;

    }
 
#pragma endregion OPEROOM

#pragma region MOMENT Limitter
    //モーメントリミッタ
    double load_lim_mh, load_lim_ah;
    //DI生成用
    if (pSim->pos[ID_BOOM_H] < 25.0) {
        load_lim_mh = 300.0; load_lim_ah = 45.0;
    }
    else if (pSim->pos[ID_BOOM_H] < 57.0) {
        load_lim_mh = -5.274 * pSim->pos[ID_BOOM_H] + 406.78;
        load_lim_ah = 45.0;
    }
    else if (pSim->pos[ID_BOOM_H] < 62.0) {
        load_lim_mh = 60.0;
        load_lim_ah = 45.0;
    }
    else {
        load_lim_mh = 60.0;
        load_lim_ah = 45.0;
    }

    if (pSim->load[ID_HOIST].m - def_spec.Load0_mh > load_lim_mh) {
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mlim_load_100.x] &= ~cab_bout_map.mlim_load_100.y;
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mlim_load_90.x] |= cab_bout_map.mlim_load_90.y;
    }
    else if (pSim->load[ID_HOIST].m - def_spec.Load0_mh > load_lim_mh * 0.9) {
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mlim_load_100.x] |=cab_bout_map.mlim_load_100.y;
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mlim_load_90.x] |= cab_bout_map.mlim_load_90.y;
    }
    else {
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mlim_load_100.x] |= cab_bout_map.mlim_load_100.y;
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mlim_load_90.x] &= ~cab_bout_map.mlim_load_90.y;

    }
  
    if (pSim->load[ID_HOIST].m - def_spec.Load0_mh > load_lim_mh) {
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mlim_load_100.x] &= ~cab_bout_map.mlim_load_100.y;
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mlim_load_90.x] |= cab_bout_map.mlim_load_90.y;
    }

    if (pSim->load[ID_HOIST].m - def_spec.Load0_mh > 150.0) {
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mlim_high_load.x] &= ~cab_bout_map.mlim_high_load.y;
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mlim_3times.x] &= ~cab_bout_map.mlim_3times.y;
    }
    else if (pSim->load[ID_HOIST].m - def_spec.Load0_mh > 20.0) {
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mlim_high_load.x] |= cab_bout_map.mlim_high_load.y;
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mlim_3times.x] &= ~cab_bout_map.mlim_3times.y;
    }
    else {
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mlim_high_load.x] |= cab_bout_map.mlim_high_load.y;
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mlim_3times.x] |= cab_bout_map.mlim_3times.y;
    }

    if (pSim->load[ID_AHOIST].m - def_spec.Load0_ah > 5.0) 
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.whip_5t_ng.x] &= ~cab_bout_map.whip_5t_ng.y;
    else
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.whip_5t_ng.x] |= cab_bout_map.whip_5t_ng.y;
     
    plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mlim_normal.x] |= cab_bout_map.mlim_normal.y;

    //AI
    plc_if_workbuf.output.wbuf.cab_ai[0] = 0.0;//フットブレーキトルク　未使用

    plc_if_workbuf.output.wbuf.cab_ai[1] = (INT16)(pSim->pos[ID_BOOM_H]*100.0);//半径0.1m単位
    if (plc_if_workbuf.output.wbuf.cab_ai[1] < 0)plc_if_workbuf.output.wbuf.cab_ai[1] = 0;

    plc_if_workbuf.output.wbuf.cab_ai[2] = (INT16)pSim->load[ID_HOIST].m*0.0048485 + 6;//主巻荷重

    plc_if_workbuf.output.wbuf.cab_ai[3] = (INT16)pSim->load[ID_AHOIST].m * 0.032;//補巻荷重

    plc_if_workbuf.output.wbuf.cab_ai[4] = (INT16)(3183.1*pSim->th.p-247.8883);//起伏角
       
#pragma endregion MOMENT Limitter

#pragma region ERM UNITX
    //X20
     plc_if_workbuf.output.wbuf.erm_unitx[erm_uxin_map.hcnt_mh_ah_ready.x] |= erm_uxin_map.hcnt_mh_ah_ready.y;
    //X30
     plc_if_workbuf.output.wbuf.erm_unitx[erm_uxin_map.hcnt_bh_sl_ready.x] |= erm_uxin_map.hcnt_bh_sl_ready.y;
    //X40
     plc_if_workbuf.output.wbuf.erm_unitx[erm_uxin_map.abs_mh_ready.x] |= erm_uxin_map.abs_mh_ready.y;
    //X120
     plc_if_workbuf.output.wbuf.erm_unitx[erm_uxin_map.abs_ah_ready.x] |= erm_uxin_map.abs_ah_ready.y;
#pragma endregion ERM UNITX

#pragma region ERM X
    //X60 極限信号
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.mh_high_area_emr_up_lim.x] |= erm_xin_map.mh_high_area_emr_up_lim.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.mh_normal_area_emr_up_lim.x] |= erm_xin_map.mh_normal_area_emr_up_lim.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_down_area_emr_up_lim.x] |= erm_xin_map.bh_down_area_emr_up_lim.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.mh_emr_lower_lim.x] |= erm_xin_map.mh_emr_lower_lim.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_emr_in_lim.x] |= erm_xin_map.bh_emr_in_lim.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_normal_in_lim.x] |= erm_xin_map.bh_normal_in_lim.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.slw_spd_change.x] |= erm_xin_map.slw_spd_change.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.high_pos_emr_out_lim.x] |= erm_xin_map.high_pos_emr_out_lim.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_emr_out_lim.x] |= erm_xin_map.bh_emr_out_lim.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_down_area_out_lim.x] |= erm_xin_map.bh_down_area_out_lim.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_rest_slow.x] |= erm_xin_map.bh_rest_slow.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_rest_lim.x] |= erm_xin_map.bh_rest_lim.y;

    //X70
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.muliti_transmit_ok.x] |= erm_xin_map.muliti_transmit_ok.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_anticol_stop.x] |= erm_xin_map.gt_anticol_stop.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_anticol_alarm.x] |= erm_xin_map.gt_anticol_alarm.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_anticol_fault.x] |= erm_xin_map.gt_anticol_fault.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_east_lim_slow.x] |= erm_xin_map.gt_east_lim_slow.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_east_lim_stop.x] |= erm_xin_map.gt_east_lim_stop.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_west_lim_slow.x] |= erm_xin_map.gt_west_lim_slow.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_west_lim_stop.x] |= erm_xin_map.gt_west_lim_stop.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_fix_open.x] |= erm_xin_map.gt_fix_open.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_motor_thermal_trip.x] |= erm_xin_map.gt_motor_thermal_trip.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.mh_motor_fan_mc.x] |= erm_xin_map.mh_motor_fan_mc.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.ah_motor_fan_mc.x] |= erm_xin_map.ah_motor_fan_mc.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_motor_fan_mc.x] |= erm_xin_map.bh_motor_fan_mc.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.DB_conv_fan_mc.x] |= erm_xin_map.DB_conv_fan_mc.y;

    //X80
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.DB_over_heat.x] |= erm_xin_map.DB_over_heat.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.mh_pos_preset.x] |= erm_xin_map.mh_pos_preset.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.mh_data_preset.x] |= erm_xin_map.mh_data_preset.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.slw_panel_high_temp.x] |= erm_xin_map.slw_panel_high_temp.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.leg_emr_stop.x] |= erm_xin_map.leg_emr_stop.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.slew_pos_0.x] |= erm_xin_map.slew_pos_0.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.slew_pos_180.x] |= erm_xin_map.slew_pos_180.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.slew_grease_active.x] |= erm_xin_map.slew_grease_active.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.slew_grease_fault.x] |= erm_xin_map.slew_grease_fault.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.ah_camlim_high_area_emr_up.x] |= erm_xin_map.ah_camlim_high_area_emr_up.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.ah_camlim_normal_area1_emr_up.x] |= erm_xin_map.ah_camlim_normal_area1_emr_up.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.ah_camlim_normal_area2_emr_up.x] |= erm_xin_map.ah_camlim_normal_area2_emr_up.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.ah_camlim_bh_down_area_emr_up.x] |= erm_xin_map.ah_camlim_bh_down_area_emr_up.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.ah_camlim_emr_low.x] |= erm_xin_map.ah_camlim_emr_low.y;
 
    //X90
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.conv_1_trip.x] |= erm_xin_map.conv_1_trip.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.inv_ctrl_trip.x] |= erm_xin_map.inv_ctrl_trip.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.brk_control_source_trip.x] |= erm_xin_map.brk_control_source_trip.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.DB_fan_trip.x] |= erm_xin_map.DB_fan_trip.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.motor_fan_trip.x] |= erm_xin_map.motor_fan_trip.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.slw_grease_trip.x] |= erm_xin_map.slw_grease_trip.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.aero_panel_fault.x] |= erm_xin_map.aero_panel_fault.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.remote_trace.x] |= erm_xin_map.remote_trace.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.inv_reel_battery_low.x] |= erm_xin_map.inv_reel_battery_low.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.inv_reel_gt_ok.x] |= erm_xin_map.inv_reel_gt_ok.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.inv_reel_standby.x] |= erm_xin_map.inv_reel_standby.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.inv_reel_inv_normal.x] |= erm_xin_map.inv_reel_inv_normal.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.inv_reel_escape_enable.x] |= erm_xin_map.inv_reel_escape_enable.y;

    //XA0　MC　FB
    //共通,ブレーキ主幹MC コンバータMC　FB　共通主幹投入指令でON
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.motive_power_ok.x] |= erm_xin_map.motive_power_ok.y;

    if (plc_if_workbuf.input.rbuf.erm_900 & erm_m900_map.common_source_com.y) {
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.mc0.x] |= erm_xin_map.mc0.y;
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.conv2_mc.x] |= erm_xin_map.conv2_mc.y;
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.ctrl_brk_mc.x] |= erm_xin_map.ctrl_brk_mc.y;

    }
    else {
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.mc0.x] &= ~erm_xin_map.mc0.y;
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.conv2_mc.x] &= ~erm_xin_map.conv2_mc.y;
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.ctrl_brk_mc.x] &= ~erm_xin_map.ctrl_brk_mc.y;
    }
    //ブレーキFB
    if(plc_if_workbuf.input.rbuf.erm_y[erm_yout_map.mh_brk_mc.x] & erm_yout_map.mh_brk_mc.y)
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.mh_brk_mc.x] |= erm_xin_map.mh_brk_mc.y;
    else
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.mh_brk_mc.x] &= ~erm_xin_map.mh_brk_mc.y;
 
    if (plc_if_workbuf.input.rbuf.erm_y[erm_yout_map.ah_brk_mc.x] & erm_yout_map.ah_brk_mc.y)
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.ah_brk_mc.x] |= erm_xin_map.ah_brk_mc.y;
    else
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.ah_brk_mc.x] &= ~erm_xin_map.ah_brk_mc.y;

    if (plc_if_workbuf.input.rbuf.erm_y[erm_yout_map.bh_brk_mc.x] & erm_yout_map.bh_brk_mc.y)
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_brk_mc.x] |= erm_xin_map.bh_brk_mc.y;
    else
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_brk_mc.x] &= ~erm_xin_map.bh_brk_mc.y;

    if (plc_if_workbuf.input.rbuf.erm_y[erm_yout_map.gt_brk_mc.x] & erm_yout_map.gt_brk_mc.y)
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_brk_mc.x] |= erm_xin_map.gt_brk_mc.y;
    else
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_brk_mc.x] &= ~erm_xin_map.gt_brk_mc.y;
 

    //各軸主幹MC FB　主幹投入指令でON
#if 0//本製番PLCロジック無し
    if (plc_if_workbuf.input.rbuf.erm_900 & erm_m900_map.mh1_source_com.y)
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.mh_brk_mc.x] |= erm_xin_map.mh_brk_mc.y;
    else
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.mh_brk_mc.x] &= ~erm_xin_map.mh_brk_mc.y;

    if (plc_if_workbuf.input.rbuf.erm_900 & erm_m900_map.bh_source_com.y)
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_brk_mc.x] |= erm_xin_map.bh_brk_mc.y;
    else
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.bh_brk_mc.x] &= ~erm_xin_map.bh_brk_mc.y;

    if (plc_if_workbuf.input.rbuf.erm_900 & erm_m900_map.gt_source_com.y)
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_brk_mc.x] |= erm_xin_map.gt_brk_mc.y;
    else
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_brk_mc.x] &= ~erm_xin_map.gt_brk_mc.y;
    if (plc_if_workbuf.input.rbuf.erm_900 & erm_m900_map.ah_source_com.y)
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.ah_brk_mc.x] |= erm_xin_map.ah_brk_mc.y;
    else
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.ah_brk_mc.x] &= ~erm_xin_map.ah_brk_mc.y;
#endif
    //初期充電指令
    if (plc_if_workbuf.input.rbuf.erm_y[erm_yout_map.initial_charge.x] & erm_yout_map.initial_charge.y)
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.dbu_charge_mc.x] |= erm_xin_map.dbu_charge_mc.y;
    else
        plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.dbu_charge_mc.x] &= ~erm_xin_map.dbu_charge_mc.y;

    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.jib_approch_alarm_enable.x] |= erm_xin_map.jib_approch_alarm_enable.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.jib_approch_alarm_disable.x] |= erm_xin_map.jib_approch_alarm_disable.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.conv1_over_heat.x] |= erm_xin_map.conv1_over_heat.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.conv2_fuse_down.x] |= erm_xin_map.conv2_fuse_down.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.conv2_over_heat.x] |= erm_xin_map.conv2_over_heat.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.conv1_fuse_down.x] |= erm_xin_map.conv1_fuse_down.y;
    
    //XB0
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.thermal_trip_ah_brk.x] |= erm_xin_map.thermal_trip_ah_brk.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.thermal_trip_bh_motor.x] |= erm_xin_map.thermal_trip_bh_motor.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.thermal_trip_mh_fan.x] |= erm_xin_map.thermal_trip_mh_fan.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.thermal_trip_bh_fan.x] |= erm_xin_map.thermal_trip_bh_fan.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.thermal_trip_DB_fan.x] |= erm_xin_map.thermal_trip_DB_fan.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.thermal_trip_mh_brk.x] |= erm_xin_map.thermal_trip_mh_brk.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.thermal_trip_bh_brk.x] |= erm_xin_map.thermal_trip_bh_brk.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.thermal_trip_ah_fan.x] |= erm_xin_map.thermal_trip_ah_fan.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.braking_unit1_fault.x] |= erm_xin_map.braking_unit1_fault.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.braking_unit2_fault.x] |= erm_xin_map.braking_unit2_fault.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.braking_unit3_fault.x] |= erm_xin_map.braking_unit3_fault.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.braking_unit4_fault.x] |= erm_xin_map.braking_unit4_fault.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.braking_unit5_fault.x] |= erm_xin_map.braking_unit5_fault.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.braking_unit6_fault.x] |= erm_xin_map.braking_unit6_fault.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.braking_unit7_fault.x] |= erm_xin_map.braking_unit7_fault.y;
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.braking_unit8_fault.x] |= erm_xin_map.braking_unit8_fault.y;
 
    plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.mh_preset.x] |= erm_xin_map.mh_preset.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.ah_preset.x] |= erm_xin_map.ah_preset.y;
#pragma endregion ERM X

#pragma region PLC_CC_LINK

    //初期充電完了
    for (int i = 0; i < 6; i++) {
        plc_if_workbuf.output.wbuf.inv_cc_x[i] |= inv_x_map.charge_ok.y;
    }
    
    //インバータ
    for (int i = ID_MC_INV_GNT; i <= ID_MC_INV_MH2; i++) {
        //正転中FB
        if (plc_if_workbuf.input.rbuf.inv_cc_y[i] & 0x1) {
            plc_if_workbuf.output.wbuf.inv_cc_x[i] |= 0x1; //正転
            plc_if_workbuf.output.wbuf.inv_cc_Wr2[i] = 500;//トルク
        }
        else {
            plc_if_workbuf.output.wbuf.inv_cc_x[i] &= 0xfffe;
        }
        //逆転中FB
        if (plc_if_workbuf.input.rbuf.inv_cc_y[i] & 0x2) {
            plc_if_workbuf.output.wbuf.inv_cc_x[i] |= 0x2;//逆転
            if((i== ID_MC_INV_AH)|| (i == ID_MC_INV_MH1) || (i == ID_MC_INV_MH2))//トルク //とりあえず巻関連は逆転も正トルクとする。
                plc_if_workbuf.output.wbuf.inv_cc_Wr2[i] = 500;
            else
                plc_if_workbuf.output.wbuf.inv_cc_Wr2[i] = -500;
        }
        else{
            plc_if_workbuf.output.wbuf.inv_cc_x[i] &= 0xfffd;
        }
 
        if (!(plc_if_workbuf.input.rbuf.inv_cc_y[i] & 0x3)) {//正逆転無し
            plc_if_workbuf.output.wbuf.inv_cc_Wr2[i] = 0;//トルククリア
            plc_if_workbuf.output.wbuf.inv_cc_Wr1[i] = 0;//速度FBクリア
        }
        else {
            //モータ速度FB(rpm)
            double temp_d;
            if (i == ID_MC_INV_MH1) {
                 plc_if_workbuf.output.wbuf.inv_cc_Wr1[i] = (INT16)(pSim->nd[ID_HOIST].v / def_spec.prm_drv[DRIVE_ITEM_GEAR][ID_HOIST]*60.0);
            }
            if (i == ID_MC_INV_AH) {
                plc_if_workbuf.output.wbuf.inv_cc_Wr1[i] = (INT16)(pSim->nd[ID_AHOIST].v / def_spec.prm_drv[DRIVE_ITEM_GEAR][ID_AHOIST] * 60.0);
            }
            if (i == ID_MC_INV_GNT){
                plc_if_workbuf.output.wbuf.inv_cc_Wr1[i] = (INT16)(pSim->nd[ID_GANTRY].v / def_spec.prm_drv[DRIVE_ITEM_GEAR][ID_GANTRY] * 60.0);
            }
            if (i == ID_MC_INV_SLW) {
                plc_if_workbuf.output.wbuf.inv_cc_Wr1[i] = (INT16)(pSim->nd[ID_SLEW].v * 428.4 * 60.0);
            }
            if (i == ID_MC_INV_BH) {
                plc_if_workbuf.output.wbuf.inv_cc_Wr1[i] = (INT16)(pSim->nd[ID_BOOM_H].v / def_spec.prm_drv[DRIVE_ITEM_GEAR][ID_BOOM_H] * 60.0);
            }
 
        }
    }
 
#pragma endregion PLC_CC_LINK

#pragma region PLC_HCOUNTER_ABS
    //高速カウンタ
    plc_if_workbuf.output.wbuf.hcounter[0] = (UINT32)(100000000.0 - (80.86 - pSim->nd[ID_HOIST].p) * 261802.07);
    plc_if_workbuf.output.wbuf.hcounter[1] = (UINT32)(100000000.0 - (65.526- pSim->nd[ID_AHOIST].p) * 243302.4);
    plc_if_workbuf.output.wbuf.hcounter[2] = (UINT32)(72354000.0 + (64.0 - pSim->nd[ID_BOOM_H].p) * 465539.66);
//    plc_if_workbuf.output.wbuf.hcounter[2] = (UINT32)(72354000.0 - (64.0 - pSim->nd[ID_BOOM_H].p) * 465539.66);
    plc_if_workbuf.output.wbuf.hcounter[3] = (UINT32)(15000000.0 + pSim->nd[ID_SLEW].p * 877363.2);                 //ピニオン回転位置

    //アブソコーダ
    plc_if_workbuf.output.wbuf.absocoder[0] = (UINT32)(1024 * pSim->nd[ID_HOIST].p + 19280.0);//主巻 1928.0はドラム3層開始（3回転）で50000カウントの条件より
    plc_if_workbuf.output.wbuf.absocoder[1] = (UINT32)(1024 * pSim->nd[ID_AHOIST].p + 19280.0);//補巻1928.0はドラム3層開始（3回転）で50000カウントの
    plc_if_workbuf.output.wbuf.absocoder[2] = (UINT32)(pSim->pos[ID_GANTRY] *1000.0);//走行（仮想）はとりあえずmm単位とする

#pragma endregion PLC__HCOUNTER_ABS

     return 0;
}

//操作端末入力受付
int CPLC_IF::parse_ote_com() {
    //運転室PB
    //ノッチ
    //主巻
    UINT16 notch_code = get_notch_code(pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_HOIST]);
    notch_code = notch_code << cab_bout_map.notch_mh.y;
    lp_PLCwrite->cab_di[cab_bout_map.notch_mh.x] &= 0xffc0;
    lp_PLCwrite->cab_di[cab_bout_map.notch_mh.x] |= notch_code;

    //補巻
    notch_code = get_notch_code(pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_AHOIST]);
    notch_code = notch_code << cab_bout_map.notch_ah.y;
    lp_PLCwrite->cab_di[cab_bout_map.notch_ah.x] &= 0x03ff;
    lp_PLCwrite->cab_di[cab_bout_map.notch_ah.x] |= notch_code;

    //走行
    notch_code = get_notch_code(pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_GANTRY]);
    notch_code = notch_code << cab_bout_map.notch_gt.y;
    lp_PLCwrite->cab_di[cab_bout_map.notch_gt.x] &= 0x81ff;
    lp_PLCwrite->cab_di[cab_bout_map.notch_gt.x] |= notch_code;

    //引込
    notch_code = get_notch_code(pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_BOOM_H]);
    notch_code = notch_code << cab_bout_map.notch_bh.y;
    lp_PLCwrite->cab_di[cab_bout_map.notch_bh.x] &= 0xffc0;
    lp_PLCwrite->cab_di[cab_bout_map.notch_bh.x] |= notch_code;

    //旋回
    notch_code = get_notch_code(pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_SLEW]);
    notch_code = notch_code << cab_bout_map.notch_sl.y;
    lp_PLCwrite->cab_di[cab_bout_map.notch_sl.x] &= 0x81ff;
    lp_PLCwrite->cab_di[cab_bout_map.notch_sl.x] |= notch_code;
    
    return 0;
}

int CPLC_IF::parse_auto_com() {



    return 0;
}

int CPLC_IF::parse_sim_status() {
   //  lp_PLCwrite->erm_x[erm_xin_map.leg_emr_stop.x] &= ~erm_xin_map.leg_emr_stop.y;
    //脚部　非常停止
     lp_PLCwrite->erm_x[erm_xin_map.leg_emr_stop.x] |= erm_xin_map.leg_emr_stop.y;

    return 0;
}

int CPLC_IF::parse() { 

    //PLC書き込みデータセット
    lp_PLCwrite->helthy = helthy_cnt++;   //### ヘルシー信号

    //### PLCリンク入力を翻訳
    parse_data_in();
   
    //### PLC,他プロセスへの出力内容を翻訳
    parse_data_out();
  
    //### 遠隔端末出力内容を翻訳
    parse_ote_com();

    //### シミュレーションの結果を出力
    parse_sim_status();

    //共有メモリデータセット
    plc_if_workbuf.mode = this->mode;                   //モードセット

    return 0; 
}
//*********************************************************************************************
// output()
//*********************************************************************************************

int CPLC_IF::output() { 
        
    plc_if_workbuf.output.wbuf.helthy = plc_if_workbuf.healthy_cnt++;
 
     //共有メモリ出力処理
    if(out_size) { 
        memcpy_s(poutput, out_size, &plc_if_workbuf, out_size);
    }

    //PLC送信の内容をセット
    memcpy_s(lp_PLCwrite, sizeof(PLC_WRITE_BUF), &(plc_if_workbuf.output), sizeof(PLC_WRITE_BUF));

    return 0;
}

//*********************************************************************************************
// set_notch_ref()
// AGENTタスクの速度指令をノッチ位置指令に変換してIO出力を設定
//*********************************************************************************************

//*********************************************************************************************
/// <summary>
/// get_notch_code(INT16 notch)
/// notch位置 -5～5に応じたノッチ操作器のビットパターンを返す
/// PLCの割付に応じてbitシフトしてIOバッファにセット
/// </summary>
/// <returns></returns>
//*********************************************************************************************
UINT16 CPLC_IF::get_notch_code(INT16 notch) {
 UINT16 code=0;
 switch (notch) {
    case  0:code = PTN_NOTCH_0; break;
    case  1:code = PTN_NOTCH_F1; break;
    case  2:code = PTN_NOTCH_F2; break;
    case  3:code = PTN_NOTCH_F3; break;
    case  4:code = PTN_NOTCH_F4; break;
    case -1:code = PTN_NOTCH_R1; break;
    case -2:code = PTN_NOTCH_R2; break;
    case -3:code = PTN_NOTCH_R3; break;
    case -4:code = PTN_NOTCH_R4 ; break;
    default:code = PTN_NOTCH_0; break;
 }
 return code;
}

INT16 CPLC_IF::get_notch_pos(UINT16 code) {
    INT16 pos = 0;
    switch (code) {
    case PTN_NOTCH_0  :pos =  0; break;
    case PTN_NOTCH_F1 :pos =  1; break;
    case PTN_NOTCH_F2 :pos =  2; break;
    case PTN_NOTCH_F3 :pos =  3; break;
    case PTN_NOTCH_F4 :pos =  4; break;
    case PTN_NOTCH_R1 :pos = -1; break;
    case PTN_NOTCH_R2 :pos = -2; break;
    case PTN_NOTCH_R3 :pos = -3; break;
    case PTN_NOTCH_R4 :pos = -4; break;
    default           :pos =  0; break;
    }
    return pos;
}



//*********************************************************************************************
//set_ao_coms()
// AGENTタスクのアナログ指令、ヘルシー信号等の出力セット
//*********************************************************************************************
int CPLC_IF::set_ao_coms() {


    return 0;
}


//*********************************************************************************************
// parse_ope_com()
// 運転室操作信号取り込み
//*********************************************************************************************
int CPLC_IF::parse_ope_com() {

    return 0;
}

//*********************************************************************************************
// parse_brk_status()
// ブレーキ状態読み込み （仮）OTEチェック用
//*********************************************************************************************
int CPLC_IF::parce_brk_status() {

    //#### OTEチェック用　仮

    return 0;
}

//*********************************************************************************************
// parse_sensor_fb()
// センサ信号取り込み
//*********************************************************************************************
int CPLC_IF::parse_sensor_fb() {

    return 0;
}

//*********************************************************************************************
// get_notch_from_spd(int motion, double spd)
// 速度をノッチに変換-4～4
//*********************************************************************************************
INT16   CPLC_IF::get_notch_from_spd(int motion, double spd) {
    INT16 ans = 0;

    if (spd == 0.0) return 0;

    double spd_check = 1.1 * spd;        //10%増しで評価　ノッチ速度よりも少し低くても切り上げる為
    if (spd_check < 0.0) spd_check *= -1.0;//絶対値でチェック

    double retio;
    double* ptable = def_spec.notch_spd_f[ID_HOIST];
    switch (motion) {
    case ID_HOIST: {
        retio = plc_if_workbuf.v_ratio[ID_HOIST];
        ptable = def_spec.notch_spd_f[ID_HOIST];
     }break;
    case ID_GANTRY: {
        retio = 1.0;
        ptable = def_spec.notch_spd_f[ID_GANTRY];
    }break;
    case ID_BOOM_H: {
        retio = 1.0;
        ptable = def_spec.notch_spd_f[ID_BOOM_H];
    }break;
    case ID_SLEW: {
        retio = 1.0;
        ptable = def_spec.notch_spd_f[ID_SLEW];
    }break;
    case ID_AHOIST: {
        retio = def_spec.notch_spd_retio[ID_AHOIST][plc_if_workbuf.ah_spd_mode];
        ptable = def_spec.notch_spd_f[ID_AHOIST];
    }break;
    default:return 0;
        break;
    }//end switch

    for (int i = 3; i >-1; i--) {
        if (spd_check > *(ptable + i)*retio) {
            ans = INT16(i + 1); break;
        }
    }

    if (spd < 0.0) ans = -ans; 
    return ans;
}
