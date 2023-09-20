#include "framework.h"
#include "CPLC_IF.h"
#include "PLC_IF.h"
#include "PLC_DEF.h"
#include <windowsx.h>
#include "Mdfunc.h"

extern ST_SPEC def_spec;
extern ST_KNL_MANAGE_SET    knl_manage_set;

CMCProtocol* pMCProtocol;  //MCプロトコルオブジェクト:
INT16 bit_mask[16] = {BIT0,BIT1,BIT2,BIT3,BIT4,BIT5,BIT6,BIT7,BIT8,BIT9,BIT10,BIT11,BIT12,BIT13,BIT14,BIT15};

CPLC_IF::CPLC_IF(HWND _hWnd_parent) {

    hWnd_parent = _hWnd_parent;
    // 共有メモリオブジェクトのインスタンス化
    pPLCioObj = new CSharedMem;
    pCraneStatusObj = new CSharedMem;
    pSimulationStatusObj = new CSharedMem;
    pAgentInfObj = new CSharedMem;
    pCSInfObj = new CSharedMem;

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

    pCSInf = (LPST_CS_INFO)pCSInfObj->get_pMap();                                           //CS共有メモリ


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

#if 0
    //CraneStat立ち上がり待ち
    while (pCrane->is_tasks_standby_ok == false) {
        Sleep(10);
    }
#endif

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
    return 0;
}

int CPLC_IF::parse_ote_com() {
    return 0;
}

int CPLC_IF::parse_auto_com() {



    return 0;
}


int CPLC_IF::parse() { 

    //### PLCリンク入力を翻訳
    parse_data_in();
  
    //### 遠隔端末出力内容を翻訳
    parse_ote_com();
 
    //共有メモリデータセット
    plc_if_workbuf.mode = this->mode;                   //モードセット

    return 0; 
}
//*********************************************************************************************
// output()
//*********************************************************************************************

int CPLC_IF::output() { 
    //PLC書き込みデータセット
    lp_PLCwrite->helthy = helthy_cnt++;   //### ヘルシー信号
 
     //共有メモリ出力処理
    if(out_size) { 
        memcpy_s(poutput, out_size, &plc_if_workbuf, out_size);
    }
    return 0;
}

//*********************************************************************************************
// set_notch_ref()
// AGENTタスクの速度指令をノッチ位置指令に変換してIO出力を設定
//*********************************************************************************************
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
