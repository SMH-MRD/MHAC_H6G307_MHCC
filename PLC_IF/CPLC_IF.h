#pragma once
#include "COMMON_DEF.H"
#include "CSharedMem.h"	    //# 共有メモリクラス
#include "PLC_DEF.h"

#define PLC_IF_PLC_IO_MEM_NG        0x8000
#define PLC_IF_CRANE_MEM_NG         0x4000
#define PLC_IF_SIM_MEM_NG           0x2000
#define PLC_IF_AGENT_MEM_NG         0x1000
#define PLC_IF_CS_MEM_NG            0x0800
#define PLC_IF_OTE_MEM_NG           0x0400

//-IF Windowの配置設定
#define IF_WND_INIT_SIZE_W			530		//-IF Windowの初期サイズ　W
#define IF_WND_INIT_SIZE_H			600		//-IF Windowの初期サイズ　H
#define IF_WND_INIT_POS_X			858	//-IF Windowの初期位置設定　X
#define IF_WND_INIT_POS_Y			0		//-IF Windowの初期位置設定　Y

#define PLC_IF_SIM_HCNT_PLS1N       2048        //高速カウンタモータ1回転パルス
#define PLC_IF_SIM_HCNT_DEF         100000000   //デフォルトカウント値

typedef struct st_PanelSim {//パネルシミュレート用構造体
    double nscan1sec = 1000.0 / 25.0;//1秒間のスキャン回数
    INT32 hcnt[MOTION_ID_MAX] = {//高速カウンタカウント値
        PLC_IF_SIM_HCNT_DEF,PLC_IF_SIM_HCNT_DEF,PLC_IF_SIM_HCNT_DEF,PLC_IF_SIM_HCNT_DEF,PLC_IF_SIM_HCNT_DEF,PLC_IF_SIM_HCNT_DEF
    };
    double vcnt100[MOTION_ID_MAX] = {//100％速度でのカウント速度
        25600.0,0.0,0.0,5973.3,5973.3,34133.33
    };
    double vcnt1invscan[MOTION_ID_MAX] = {//インバータ１指令当りのスキャン毎カウント速度
        0.0096,0.0,0.0,0.074667,0.074667,0.085333
    };
    double ndrm[MOTION_ID_MAX] = {//ドラム回転数
        25600,0.0,0.0,64.0,0.0,65.526
    };
}ST_PANEL_SIM, * LPST_PANEL_SIM;

class CPLC_IF :    public CBasicControl
{
public:
    CPLC_IF(HWND hWnd_parent);
    ~CPLC_IF();
 
    WORD helthy_cnt=0;
    HWND hWnd_parent;       //親ウィンドウのハンドル
    HWND hWnd_plcif;

    ST_PANEL_SIM st_pnl_sim;

    //オーバーライド
    int set_outbuf(LPVOID); //出力バッファセット
    int init_proc();        //初期化処理
    int input();            //入力処理
    int parse();            //メイン処理
    int output();           //出力処理

    //追加メソッド
    BOOL show_if_wnd();
    BOOL hide_if_wnd();


    LPST_PLC_WRITE lp_PLCwrite; //PLC書き込みデータバッファアドレス
    LPST_PLC_READ  lp_PLCread;  //PLC読み込みデータバッファアドレス

    SOCKADDR_IN addrinc, addrins, addrfrom;         //MCプロトコル用ソケットアドレス

    void set_mode(DWORD id) {                       //0bit:操作指令有効　1bit：センサ状態指令有効 　2bit：FULL SIMULATION
        mode &= 0x00000000;
        mode |= id;
        return;
    }
 
private:
    //# 出力用共有メモリオブジェクトポインタ:
    CSharedMem* pPLCioObj;
    //# 入力用共有メモリオブジェクトポインタ:
    CSharedMem* pCraneStatusObj;
    CSharedMem* pSimulationStatusObj;
    CSharedMem* pAgentInfObj;
    CSharedMem* pCSInfObj;
    CSharedMem* pOTEioObj;
     
    ST_PLC_IO plc_if_workbuf;   //共有メモリへの出力セット作業用バッファ

    LPST_SIMULATION_STATUS pSim;    //シミュレータステータス
    LPST_CRANE_STATUS pCrane;
    LPST_AGENT_INFO pAgentInf;
    LPST_CS_INFO pCSInf;
    LPST_OTE_IO pOTEio;

    int clear_plc_write();

    int parse_data_in();
    int parse_data_out();
    int parse_ote_com();
    int parse_auto_com();
    int parse_sim_status();

    int parce_brk_status();
    int parse_ope_com();
    int parse_sensor_fb();
    int set_notch_ref();
    int set_ao_coms();

    UINT16 get_notch_code(INT16 notch);
    INT16 get_notch_pos(UINT16 code);
 };
