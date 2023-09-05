#pragma once
#include "COMMON_DEF.H"
#include "CSharedMem.h"	    //# 共有メモリクラス
#include "CPushButton.h"

#include "PLC_IO_DEF_MELNET.h"
#include "PLC_IO_DEF_MC.h"

#define PLC_IF_PLC_IO_MEM_NG        0x8000
#define PLC_IF_CRANE_MEM_NG         0x4000
#define PLC_IF_SIM_MEM_NG           0x2000
#define PLC_IF_AGENT_MEM_NG         0x1000
#define PLC_IF_CS_MEM_NG            0x0800



class CPLC_IF :    public CBasicControl
{
public:
    CPLC_IF(HWND hWnd_parent);
    ~CPLC_IF();
 
    WORD helthy_cnt=0;
    HWND hWnd_parent;       //親ウィンドウのハンドル

    //オーバーライド
    int set_outbuf(LPVOID); //出力バッファセット
    int init_proc();        //初期化処理
    int input();            //入力処理
    int parse();            //メイン処理
    int output();           //出力処理

    //追加メソッド
    int set_debug_status(); //デバッグモード時にデバッグパネルウィンドウからの入力で出力内容を上書き
    int set_sim_status();   //デバッグモード時にSimulatorからの入力で出力内容を上書き
    int closeIF();
#ifdef _TYPE_MELSECNET
    ST_MELSEC_NET   melnet;
    LPST_MELSEC_NET get_melnet() { return &melnet; }
#endif
    LPST_PLC_WRITE lp_PLCwrite;
    LPST_PLC_READ  lp_PLCread;

    SOCKADDR_IN addrinc, addrins, addrfrom;         //MCプロトコル用ソケットアドレス

    void set_debug_mode(int id) {
        if (id) mode |= PLC_IF_PC_DBG_MODE;
        else    mode &= ~PLC_IF_PC_DBG_MODE;
        return;
    }
#ifdef _TYPE_MELSECNET
    void set_pc_ctrl_forced(bool b) {
        if (b) melnet.is_forced_pc_ctrl = true;
        else melnet.is_forced_pc_ctrl = false;
        return;
    }
    void set_plc_emu_forced(bool b) {
        if (b) melnet.is_forced_emulate = true;
        else melnet.is_forced_emulate = false;
        return;
    }
#endif
    void set_pc_ctrl_forced(bool b) {
        return;
    }
    void set_plc_emu_forced(bool b) {
         return;
    }

    int is_debug_mode() { return(mode & PLC_IF_PC_DBG_MODE); }
    int mel_set_force(int id, bool bset, int index, WORD value);

private:
    //# 出力用共有メモリオブジェクトポインタ:
    CSharedMem* pPLCioObj;
    //# 入力用共有メモリオブジェクトポインタ:
    CSharedMem* pCraneStatusObj;
    CSharedMem* pSimulationStatusObj;
    CSharedMem* pAgentInfObj;
    CSharedMem* pCSInfObj;

 
    ST_PLC_IO plc_io_workbuf;   //共有メモリへの出力セット作業用バッファ

    LPST_SIMULATION_STATUS pSim;    //シミュレータステータス
    LPST_CRANE_STATUS pCrane;
    LPST_AGENT_INFO pAgentInf;
    LPST_CS_INFO pCSInf;

    int parse_notch_com();
    int parce_brk_status();
    int parse_ope_com();
    int parse_sensor_fb();
    int set_notch_ref();
    int set_bit_coms();
    int set_ao_coms();
 };
