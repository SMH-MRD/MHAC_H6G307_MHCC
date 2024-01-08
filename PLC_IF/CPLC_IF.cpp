#include "framework.h"
#include "CPLC_IF.h"
#include "PLC_IF.h"
#include "PLC_DEF.h"
#include <windowsx.h>
#include "Mdfunc.h"
#include "OTE0panel.h""

extern ST_SPEC def_spec;
extern ST_KNL_MANAGE_SET    knl_manage_set;

CMCProtocol* pMCProtocol;  //MCプロトコルオブジェクト:
INT16 bit_mask[16] = {BIT0,BIT1,BIT2,BIT3,BIT4,BIT5,BIT6,BIT7,BIT8,BIT9,BIT10,BIT11,BIT12,BIT13,BIT14,BIT15};
ST_PLC_NOTCH_PTN notch_ptn;//PLCのノッチ信号入力パターン

CABPLC_BOUT_MAP cab_bout_map;
ERMPLC_BOUT_MAP erm_bout_map;
ERMPLC_YOUT_MAP erm_yout_map;
ERMPLC_XIN_MAP  erm_xin_map;

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


    //データ変換計算用パラメータセット
    while (!pCrane->is_crane_status_ok) Sleep(100);//MAINプロセスの仕様取り込み完了待ち

    for (int i = 0; i < MOTION_ID_MAX; i++) {//ドラム0層パラメータセット
        plc_if_workbuf.Cdr[i][0] = pCrane->spec.prm_nw[SIZE_ITEM_WIRE_LEN0][i];
        plc_if_workbuf.Ldr[i][0] = 0.0;
    }

    for (int j = 1; j < PLC_DRUM_LAYER_MAX; j++) {//ドラム1層以上パラメータセット{
        for (int i = 0; i < ID_AHOIST+1; i++) {//ドラム1層以上パラメータセット
            //ドラム円周
            plc_if_workbuf.Cdr[i][j] =( pCrane->spec.prm_nw[DRUM_ITEM_DIR][i] + ((double)j-1.0)* pCrane->spec.prm_nw[DRUM_ITEM_DIR_ADD][i]) * PI180;
            //ドラム層対ドラム1層円周比率
            plc_if_workbuf.Kdr[i][j] = (pCrane->spec.prm_nw[DRUM_ITEM_DIR][i] + ((double)j - 1.0) * pCrane->spec.prm_nw[DRUM_ITEM_DIR_ADD][i]) / pCrane->spec.prm_nw[DRUM_ITEM_DIR][i];
            //ドラム層巻取り量
            plc_if_workbuf.Ldr[i][j] = plc_if_workbuf.Ldr[i][j-1] + pCrane->spec.prm_nw[NW_ITEM_GROOVE][i]* plc_if_workbuf.Cdr[i][j];
        }
    }

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

int CPLC_IF::parse_data_in() {

    //受信バッファの内容をワークバッファにコピー
    memcpy_s(&(plc_if_workbuf.input), sizeof(PLC_READ_BUF),lp_PLCread, sizeof(PLC_READ_BUF));

    return 0;
}

int CPLC_IF::parse_data_out() {

    //PC 操作有効信号
    plc_if_workbuf.output.wbuf.ctrl_mode = mode;
 

    if (pCSInf->ote_remote_status &= CS_CODE_OTE_REMOTE_ENABLE) {//端末操作有効
        // 主幹ON　PB
        if(pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_PB_SYUKAN])   
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_on.x] |= cab_bout_map.ctrl_on.y;
        else                                                    
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_on.x] &= ~cab_bout_map.ctrl_on.y;

        // 非常停止　PB（主幹OFF　PB）
        if((pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_PB_HIJYOU])|| !(pOTEio->ote_umsg_in.body.pb_notch[ID_OTE_GRIP_ESTOP]))
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_off.x] |= cab_bout_map.ctrl_off.y;
        else
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_off.x] &= ~cab_bout_map.ctrl_off.y;

        if (pCSInf->auto_mode) {
            //AGENT 出力をセット;
        }
        else {
            //OTEノッチ信号セット
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
          }
    }
    else {
        //端末無効時は、主幹OFF入力状態
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_on.x] &= ~cab_bout_map.ctrl_on.y;
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_off.x] |= cab_bout_map.ctrl_off.y;

    }
     return 0;
}

int CPLC_IF::parse_ote_com() {
    //運転室PB
#if 0
    //主幹入
     if (pOTEio->ote_in.pb_ope[OTE_INDEX_PB_CTR_SOURCE])
        lp_PLCwrite->cab_di[cab_bout_map.ctrl_on.x] |= cab_bout_map.ctrl_on.y;
    else
        lp_PLCwrite->cab_di[cab_bout_map.ctrl_on.x] &= ~cab_bout_map.ctrl_on.y;

     //主幹切PB　常時ON
        lp_PLCwrite->cab_di[cab_bout_map.ctrl_on.x] |= cab_bout_map.ctrl_off.y;
 

     if (pOTEio->ote_in.pb_ope[OTE_INDEX_PB_FAULT_RESET])
         lp_PLCwrite->cab_di[cab_bout_map.fault_reset.x] |= cab_bout_map.fault_reset.y;
     else
         lp_PLCwrite->cab_di[cab_bout_map.fault_reset.x] &= ~cab_bout_map.fault_reset.y;

    if (pOTEio->ote_in.pb_ope[OTE_INDEX_CHK_ESTOP])//緊急停止はノーマルクローズ
        lp_PLCwrite->cab_di[cab_bout_map.cab_estp.x] &= ~cab_bout_map.cab_estp.y;
    else
     lp_PLCwrite->cab_di[cab_bout_map.cab_estp.x] |= cab_bout_map.cab_estp.y;
#endif
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

int CPLC_IF::set_notch_ref() {

    return 0;
}

//*********************************************************************************************
//set_bit_coms()
//CSランプ表示, AGENTタスク, 操作パネルのビット指令に応じてIO出力を設定
//*********************************************************************************************
int CPLC_IF::set_bit_coms() {
 

    return 0;
}

//*********************************************************************************************
//set_ao_coms()
// AGENTタスクのアナログ指令、ヘルシー信号等の出力セット
//*********************************************************************************************
int CPLC_IF::set_ao_coms() {


    return 0;
}

//*********************************************************************************************
// parse_notch_com()
// UIノッチ指令読み込み
//*********************************************************************************************
int CPLC_IF::parse_notch_com() {
    
    INT16 check_i;

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
