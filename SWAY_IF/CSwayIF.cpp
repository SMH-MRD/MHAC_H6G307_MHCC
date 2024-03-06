#include "CSwayIF.h"
#include <windowsx.h>       //# コモンコントロール

#include <iostream>
#include <iomanip>
#include <sstream>

HWND CSwayIF::hWorkWnd;

int CSwayIF::iDispSensor = 0;
int CSwayIF::iDispBuf = 0;
int CSwayIF::iDispCam = 0;
int CSwayIF::iDispTg = 0;

ST_SWAY_WORK_WND CSwayIF::st_swy_work_wnd;

INT32 CSwayIF::cycle_min_ms;
INT32 CSwayIF::sens_mode;
INT32 CSwayIF::cal_mode = ID_SWAY_CAL_NORMAL;

LPST_CRANE_STATUS CSwayIF::pCraneStat;
LPST_SIMULATION_STATUS CSwayIF::pSimStat;
LPST_PLC_IO CSwayIF::pPLCio;

ST_SWAY_RCV_MSG CSwayIF::rcv_msg[N_SWAY_SENSOR][N_SWAY_SENSOR_RCV_BUF];
ST_SWAY_SND_MSG CSwayIF::snd_msg[N_SWAY_SENSOR][N_SWAY_SENSOR_SND_BUF];
int CSwayIF::i_rcv_msg[N_SWAY_SENSOR] = { 0,0,0 };
int CSwayIF::i_snd_msg[N_SWAY_SENSOR] = { 0,0,0 };

ST_SWAY_IO CSwayIF::sway_io_workbuf;
ST_SWAY_CAM_WORK CSwayIF::swx;
ST_SWAY_CAM_WORK CSwayIF::swy;
ST_SWAY_CAM_WORK CSwayIF::swx2;
ST_SWAY_CAM_WORK CSwayIF::swy2;

wstring CSwayIF::ws_sensor_err_msg[64];
wstring CSwayIF::ws_sensor_stat_msg[64];

static bool be_skiped_once_const_msg = false;

static ST_SWY_SENS_PRM CamPrm;

CSwayIF::CSwayIF() {
    // 共有メモリオブジェクトのインスタンス化
    hInst = NULL;
    pSwayIOObj = new CSharedMem;
    pCraneStatusObj = new CSharedMem;
    pSimulationStatusObj = new CSharedMem;
    pPLCioObj = new CSharedMem;

    out_size = 0;
    memset(&sway_io_workbuf, 0, sizeof(ST_SWAY_IO));   //共有メモリへの出力セット作業用バッファ
};
CSwayIF::~CSwayIF() {
    // 共有メモリオブジェクトの解放
    delete pSwayIOObj;
    delete pCraneStatusObj;
    delete pSimulationStatusObj;
    delete pPLCioObj;
};

int CSwayIF::set_outbuf(LPVOID pbuf) {
    poutput = pbuf; return 0;
};      //出力バッファセット

//******************************************************************************************
// init_proc()
//******************************************************************************************
int CSwayIF::init_proc() {

    // 共有メモリ取得

    // 出力用共有メモリ取得
    out_size = sizeof(ST_SWAY_IO);
    if (OK_SHMEM != pSwayIOObj->create_smem(SMEM_SWAY_IO_NAME, out_size, MUTEX_SWAY_IO_NAME)) {
        mode |= SWAY_IF_SWAY_IO_MEM_NG;
    }
    set_outbuf(pSwayIOObj->get_pMap());

    // 入力用共有メモリ取得
    if (OK_SHMEM != pSimulationStatusObj->create_smem(SMEM_SIMULATION_STATUS_NAME, sizeof(ST_SIMULATION_STATUS), MUTEX_SIMULATION_STATUS_NAME)) {
        mode |= SWAY_IF_SIM_MEM_NG;
    }

    if (OK_SHMEM != pCraneStatusObj->create_smem(SMEM_CRANE_STATUS_NAME, sizeof(ST_CRANE_STATUS), MUTEX_CRANE_STATUS_NAME)) {
        mode |= SWAY_IF_CRANE_MEM_NG;
    }

    if (OK_SHMEM != pPLCioObj->create_smem(SMEM_PLC_IO_NAME, sizeof(ST_PLC_IO), MUTEX_PLC_IO_NAME)) {
        mode |= SWAY_IF_PLC_IO_MEM_NG;
    }

    //共有クレーンステータス構造体のポインタセット
    pCraneStat = (LPST_CRANE_STATUS)(pCraneStatusObj->get_pMap());
    pSimStat = (LPST_SIMULATION_STATUS)(pSimulationStatusObj->get_pMap());
    pPLCio   = (LPST_PLC_IO)(pPLCioObj->get_pMap());

    //CraneStat立ち上がり待ち
  //  while (pCraneStat->is_tasks_standby_ok == false) {
  //      Sleep(10);
  //  }


    //振れ角計算用カメラパラメータデフォルト値セット

    //カメラ取付状態のパラメータをfalseで振れセンサの電文にセットされている値を採用する
    swx.is_read_from_msg = swy.is_read_from_msg = swx2.is_read_from_msg = swy2.is_read_from_msg = false;
 
    swx.D0 = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X][SID_D0];
    swy.D0 = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y][SID_D0];
    swx2.D0 = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X][SID_D02];
    swy2.D0 = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y][SID_D02];

    swx.row =CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X][SID_row];
    swy.row = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y][SID_row];
    swx2.row = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X][SID_row2];
    swy2.row = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y][SID_row2];

    swx.l0 = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X][SID_l0];
    swy.l0 = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y][SID_l0];
    swx2.l0 = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X][SID_l02];
    swy2.l0 = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y][SID_l02];

    swx.ph0 = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X][SID_ph0];
    swy.ph0 = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y][SID_ph0];
    swx2.ph0 = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X][SID_ph02];
    swy2.ph0 = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y][SID_ph02];

    swx.phc = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X][SID_phc];
    swy.phc = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y][SID_phc];
    swx2.phc = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X][SID_phc2];
    swy2.phc = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y][SID_phc2];

    swx.C = 1.0 / CamPrm.rad2pix[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X];
    swy.C = 1.0 / CamPrm.rad2pix[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y];
    swx2.C = 1.0 / CamPrm.rad2pix[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X];
    swy2.C = 1.0 / CamPrm.rad2pix[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y];

    cycle_min_ms = SW_SND_DEFAULT_SCAN;
    sens_mode = SW_SND_MODE_NORMAL;

    init_rcv_msg();     //受信バッファの初期化
    set_sensor_msg();   //振れセンサ電文コードに対応する状態表示テキストメッセージの登録

    return int(mode & 0xff00);
}
//*********************************************************************************************
// input()
//*********************************************************************************************
int CSwayIF::input() {

    LPST_CRANE_STATUS pcrane = (LPST_CRANE_STATUS)pCraneStatusObj->get_pMap();
    LPST_SIMULATION_STATUS psim = (LPST_SIMULATION_STATUS)pSimulationStatusObj->get_pMap();

    //MAINプロセス(Environmentタスクのヘルシー信号取り込み）
    source_counter = pcrane->env_act_count;

    //PLC 入力

    return 0;
}
//*********************************************************************************************
// parse()
//*********************************************************************************************
int CSwayIF::parse() {

    parse_sway_stat(&rcv_msg[SID_SENSOR1][i_rcv_msg[SID_SENSOR1]]);

    return 0;
}
//*********************************************************************************************
// output()
//*********************************************************************************************
int CSwayIF::output() {

    sway_io_workbuf.proc_mode = this->mode;              //モードセット
    sway_io_workbuf.helthy_cnt = my_helthy_counter++;    //ヘルシーカウンタセット

    if (out_size) { //出力処理
        memcpy_s(poutput, out_size, &sway_io_workbuf, out_size);
    }

    return 0;
}

//*********************************************************************************************
// 振れ計算用センサパラメータを通信電文より読み込み
//*********************************************************************************************
int CSwayIF::get_sensor_param_from_msg(LPST_SWAY_RCV_MSG pmsg) {
    swx.D0 = CamPrm.arr[0][0][0][SID_D0];
    swy.D0 = CamPrm.arr[0][0][1][SID_D0];

    swx.row = CamPrm.arr[0][0][0][SID_row];
    swy.row = CamPrm.arr[0][0][1][SID_row];

    swx.l0 = (double)pmsg->body[SWAY_SENSOR_CAM1].cam_spec.l0_x / 1000.0;       //mm->m
    swy.l0 = (double)pmsg->body[SWAY_SENSOR_CAM1].cam_spec.l0_y / 1000.0;       //mm->m

    swx.ph0 = (double)pmsg->body[SWAY_SENSOR_CAM1].cam_spec.ph0_x / 1000000.0;  //x10^6->x1
    swy.ph0 = (double)pmsg->body[SWAY_SENSOR_CAM1].cam_spec.ph0_y / 1000000.0;  //x10^6->x1

    swx.phc = (double)pmsg->body[SWAY_SENSOR_CAM1].cam_spec.phc_x / 1000000.0;  //x10^6->x1
    swy.phc = (double)pmsg->body[SWAY_SENSOR_CAM1].cam_spec.phc_y / 1000000.0;  //x10^6->x1

    if (pmsg->body[SWAY_SENSOR_CAM1].cam_spec.pixlrad_x > 1) {
        swx.C = 1.0 / (double)pmsg->body[SWAY_SENSOR_CAM1].cam_spec.pixlrad_x;
    }
    if (pmsg->body[SWAY_SENSOR_CAM1].cam_spec.pixlrad_x > 1) {
        swx.C = 1.0 / (double)pmsg->body[SWAY_SENSOR_CAM1].cam_spec.pixlrad_y;
    }
    swx.is_read_from_msg = true;
    swy.is_read_from_msg = true;
    return 0;
}

//*********************************************************************************************
// 振れセンサデータを解析
//*********************************************************************************************

int CSwayIF::parse_sway_stat(LPST_SWAY_RCV_MSG pmsg) {

    
    //カメラ検出PIX(OTE表示用
    //主巻
    sway_io_workbuf.cam_pix[SID_LOAD_MH][SID_CAM_X] = pmsg->body[SWAY_SENSOR_CAM1].tg_stat[SWAY_SENSOR_TG1].th_x;
    sway_io_workbuf.cam_pix[SID_LOAD_MH][SID_CAM_Y] = pmsg->body[SWAY_SENSOR_CAM1].tg_stat[SWAY_SENSOR_TG1].th_y;
    //補巻
    sway_io_workbuf.cam_pix[SID_LOAD_AH][SID_CAM_X] = pmsg->body[SWAY_SENSOR_CAM1].tg_stat[SWAY_SENSOR_TG2].th_x;
    sway_io_workbuf.cam_pix[SID_LOAD_AH][SID_CAM_Y] = pmsg->body[SWAY_SENSOR_CAM1].tg_stat[SWAY_SENSOR_TG2].th_y;

    //カメラ検出PIX→θt
    //主巻
    swx.tht = pmsg->body[SWAY_SENSOR_CAM1].tg_stat[SWAY_SENSOR_TG1].th_x * swx.C;
    swy.tht = pmsg->body[SWAY_SENSOR_CAM1].tg_stat[SWAY_SENSOR_TG1].th_y * swy.C;
    //補巻
    swx2.tht = pmsg->body[SWAY_SENSOR_CAM1].tg_stat[SWAY_SENSOR_TG2].th_x * swx.C;
    swy2.tht = pmsg->body[SWAY_SENSOR_CAM1].tg_stat[SWAY_SENSOR_TG2].th_y * swy.C;

    //カメラ検出DPIX→dθt
    //主巻
    swx.dtht = pmsg->body[SWAY_SENSOR_CAM1].tg_stat[SWAY_SENSOR_TG1].dth_x * swx.C;
    swy.dtht = pmsg->body[SWAY_SENSOR_CAM1].tg_stat[SWAY_SENSOR_TG1].dth_y * swy.C;
    //補巻
    swx2.dtht = pmsg->body[SWAY_SENSOR_CAM1].tg_stat[SWAY_SENSOR_TG2].dth_x * swx.C;
    swy2.dtht = pmsg->body[SWAY_SENSOR_CAM1].tg_stat[SWAY_SENSOR_TG2].dth_y * swy.C;

    //カメラハウジング傾斜角
    sway_io_workbuf.tilt[0][ID_SLEW] = swx.til = swx2.til = pmsg->body[SWAY_SENSOR_CAM1].cam_stat.tilt_x;
    sway_io_workbuf.tilt[0][ID_BOOM_H] = swy.til = swy2.til = pmsg->body[SWAY_SENSOR_CAM1].cam_stat.tilt_y;
    sway_io_workbuf.dtilt[0][ID_SLEW] = swx.dtil = swx2.dtil = pmsg->body[SWAY_SENSOR_CAM1].cam_stat.tilt_dx;
    sway_io_workbuf.dtilt[0][ID_BOOM_H] = swy.dtil = swy2.dtil = pmsg->body[SWAY_SENSOR_CAM1].cam_stat.tilt_dy;

  
    //計算用中間変数
    swx.th0 = (swx.til + swx.ph0);    
    swy.th0 = (swy.til + swy.ph0);    
    swx2.th0 = (swx2.til + swx2.ph0); 
    swy2.th0 = (swy2.til + swy2.ph0); 

    swx.thc = swx.til + swx.phc;
    swy.thc = swy.til + swy.phc;
    swx2.thc = swx2.til + swx2.phc;
    swy2.thc = swy2.til + swy2.phc;

    double dthcx = swx.dtil;
    double dthcy = swy.dtil;
    double dthcx2 = swx2.dtil;
    double dthcy2 = swy2.dtil;
    double dth0x = swx.dtil;
    double dth0y = swy.dtil;
    double dth0x2 = swx2.dtil;
    double dth0y2 = swy2.dtil;
    
    double th_bh = pPLCio->th_bh;
    double dth_bh = pPLCio->dth_bh;
        
    double cos_row = cos(swx.row);
    double sin_row = sin(swx.row);
    double cos_row2 = cos(swx2.row);
    double sin_row2 = sin(swx2.row);
    double cos_row_bh = cos(swy.row + th_bh);
    double sin_row_bh = sin(swy.row + th_bh);
    double cos_row_bh2 = cos(swy2.row + th_bh);
    double sin_row_bh2 = sin(swy2.row + th_bh);
    
    double cos_th0x = cos(swx.th0);
    double sin_th0x = sin(swx.th0);
    double cos_th0y = cos(swy.th0);
    double sin_th0y = sin(swy.th0);
    double cos_th0x2 = cos(swx.th0);
    double sin_th0x2 = sin(swx.th0);
    double cos_th0y2 = cos(swy.th0);
    double sin_th0y2 = sin(swy.th0);

    double cos_tcx = cos(swx.tht + swx.thc);
    double cos_tcy = cos(swy.tht + swy.thc);
    double sin_tcx = sin(swx.tht + swx.thc);
    double sin_tcy = sin(swy.tht + swy.thc);
    
    double cos_tcx2 = cos(swx2.tht + swx2.thc);
    double cos_tcy2 = cos(swy2.tht + swy2.thc);
    double sin_tcx2 = sin(swx2.tht + swx2.thc);
    double sin_tcy2 = sin(swy2.tht + swy2.thc);

    swx.d0  = swx.D0  * cos_row;
    swy.d0  = swy.D0  * cos_row_bh;
    swx2.d0 = swx2.D0 * cos_row2;
    swy2.d0 = swy2.D0 * cos_row_bh2;

    swy.h0  = swy.D0  * sin_row_bh;
    swx.h0  = swy.h0;
    swy2.h0 = swy2.D0 * sin_row_bh2;
    swx2.h0 = swy2.h0;
   
    swx.dc  = swx.d0  + swx.l0  * sin_th0x;
    swy.dc  = swy.d0  + swy.l0  * sin_th0y;
    swx2.dc = swx2.d0 + swx2.l0 * sin_th0x2;
    swy2.dc = swy2.d0 + swy2.l0 * sin_th0y2;

    double ddcx  =                                   swx.l0  * dth0x  * cos_th0x;
    double ddcy  = -swy.D0  * dth_bh * sin_row_bh  + swy.l0  * dth0y  * cos_th0y;
    double ddcx2 =                                   swx2.l0 * dth0x2 * cos_th0x2;
    double ddcy2 = -swy2.D0 * dth_bh * sin_row_bh2 + swy2.l0 * dth0y2 * cos_th0y2;

    swy.hc  = swy.h0  + swy.l0  * cos(swy.th0);
    swx.hc  = swy.hc;
    swy2.hc = swy2.h0 + swy2.l0 * cos(swy2.th0);
    swx2.hc = swy2.hc;

    double dhcy  =  swy.D0  * dth_bh * cos_row_bh   + swy.l0  * dth0y  * sin_th0y;
    double dhcx  =  dhcy; 
    double dhcy2 =  swy2.D0 * dth_bh * cos_row_bh2  + swy2.l0 * dth0y2 * sin_th0y2;
    double dhcx2 =  dhcy2;

    // double lmh = pPLCio->lmh, lah = pPLCio->lah;         //##########  デバッグ用仮処置
    double lmh = pSimStat->lrm.p, lah = pSimStat->lra.p;    //##########  デバッグ用仮処置

    //吊荷振れ角 
    double sin_phtc_x = (swx.dc * cos_tcx + swx.hc * sin_tcx)/lmh;
    sway_io_workbuf.th[SID_LOAD_MH][ID_SLEW] = asin(sin_phtc_x) + swx.tht + swx.thc;//接線方向は、旋回速度＋方向が+
    double sin_phtc_y = (swy.dc * cos_tcy + swy.hc * sin_tcy) / lmh;
    sway_io_workbuf.th[SID_LOAD_MH][ID_BOOM_H] = asin(sin_phtc_y) + swy.tht + swy.thc;//接線方向は、旋回速度＋方向が+
    double sin_phtc_x2 = (swx2.dc * cos_tcx2 + swx2.hc * sin_tcx2) /lah;
    sway_io_workbuf.th[SID_LOAD_AH][ID_SLEW] = asin(sin_phtc_x2) + swx2.tht + swx2.thc;//接線方向は、旋回速度＋方向が+
    double sin_phtc_y2 = (swy2.dc * cos_tcy2 + swy2.hc * sin_tcy2) / lah;
    sway_io_workbuf.th[SID_LOAD_AH][ID_BOOM_H] = asin(sin_phtc_y2) + swy2.tht + swy2.thc;//接線方向は、旋回速度＋方向が+
 
    //吊荷振れ角速度 

    double lmh2 = lmh * lmh;
    double lah2 = lah * lah;
   
    double X   = swx.dc * cos_tcx + swx.hc * sin_tcx;
    double dph = ddcx * cos_tcx + dhcx * sin_tcx + (swx.dtht + dthcx) * (swx.hc * cos_tcx - swx.dc * sin_tcx);
    dph /= sqrt(lmh2 - X * X);
    dph += (swx.dtht + dthcx);
    sway_io_workbuf.dth[SID_LOAD_MH][ID_SLEW] = dph;

    X   = swy.dc  * cos_tcy + swy.hc  * sin_tcy;
    dph = ddcy  * cos_tcy  + dhcy  * sin_tcy  + (swy.dtht + dthcy)   * (swy.hc * cos_tcy - swy.dc * sin_tcy);
    dph /= sqrt(lmh2 - X * X);
    dph += (swy.dtht + dthcy);
    sway_io_workbuf.dth[SID_LOAD_MH][ID_BOOM_H] = dph;

    X   = swx2.dc * cos_tcx2 + swx2.hc * sin_tcx2;
    dph = ddcx2 * cos_tcx2 + dhcx2 * sin_tcx2 + (swx2.dtht + dthcx2) * (swx2.hc * cos_tcx2 - swy.dc * sin_tcx2);
    dph /= sqrt(lah2 - X * X);
    dph += (swx2.dtht + dthcx2);
    sway_io_workbuf.dth[SID_LOAD_AH][ID_SLEW] = dph;

    X  = swy2.dc * cos_tcy2  + swy2.hc * sin_tcy2;
    dph = ddcy2 * cos_tcy2 + dhcy2 * sin_tcy2 + (swy2.dtht + dthcy2) * (swy2.hc * cos_tcy2 - swy2.dc * sin_tcy2);
    dph /= sqrt(lah2 - X * X);
    dph += (swy2.dtht + dthcy2);
    sway_io_workbuf.dth[SID_LOAD_AH][ID_BOOM_H] = dph;

    sway_io_workbuf.dthw[SID_LOAD_MH][SID_CAM_X] = sway_io_workbuf.dth[SID_LOAD_MH][SID_CAM_X]/ pCraneStat->w;
    sway_io_workbuf.dthw[SID_LOAD_MH][SID_CAM_Y] = sway_io_workbuf.dth[SID_LOAD_MH][SID_CAM_Y]/ pCraneStat->w;
    sway_io_workbuf.dthw[SID_LOAD_AH][SID_CAM_X] = sway_io_workbuf.dth[SID_LOAD_AH][SID_CAM_X] / pCraneStat->w2;
    sway_io_workbuf.dthw[SID_LOAD_AH][SID_CAM_Y] = sway_io_workbuf.dth[SID_LOAD_AH][SID_CAM_Y] / pCraneStat->w2;

    sway_io_workbuf.rad_amp2[SID_LOAD_MH][SID_CAM_X] = sway_io_workbuf.th[SID_LOAD_MH][SID_CAM_X] * sway_io_workbuf.th[SID_LOAD_MH][SID_CAM_X]+ sway_io_workbuf.dthw[SID_LOAD_MH][SID_CAM_X] * sway_io_workbuf.dthw[SID_LOAD_MH][SID_CAM_X];
    sway_io_workbuf.rad_amp2[SID_LOAD_MH][SID_CAM_Y] = sway_io_workbuf.th[SID_LOAD_MH][SID_CAM_Y] * sway_io_workbuf.th[SID_LOAD_MH][SID_CAM_Y]+ sway_io_workbuf.dthw[SID_LOAD_MH][SID_CAM_Y] * sway_io_workbuf.dthw[SID_LOAD_MH][SID_CAM_Y];

    //合成振幅  
    sway_io_workbuf.rad_amp2[SID_LOAD_MH][SID_CAM_XY] = sway_io_workbuf.rad_amp2[SID_LOAD_MH][SID_CAM_X] + sway_io_workbuf.rad_amp2[SID_LOAD_MH][SID_CAM_Y];
    sway_io_workbuf.rad_amp2[SID_LOAD_AH][SID_CAM_XY] = sway_io_workbuf.rad_amp2[SID_LOAD_AH][SID_CAM_X] + sway_io_workbuf.rad_amp2[SID_LOAD_AH][SID_CAM_Y];

    //位相(X軸）
    if (sway_io_workbuf.th[SID_LOAD_MH][SID_CAM_X] > 0.00001) {
        sway_io_workbuf.ph[SID_LOAD_MH][SID_CAM_X] = atan(sway_io_workbuf.dthw[SID_LOAD_MH][SID_CAM_X] / sway_io_workbuf.th[SID_LOAD_MH][SID_CAM_X]);
    }
    else if (sway_io_workbuf.th[SID_LOAD_MH][SID_CAM_X] < -0.00001) { // atan()引数の0割回避
        if (sway_io_workbuf.dth[SID_LOAD_MH][SID_CAM_X] >= 0.0) sway_io_workbuf.ph[SID_LOAD_MH][SID_CAM_X] = atan(sway_io_workbuf.dthw[SID_LOAD_MH][SID_CAM_X] / sway_io_workbuf.th[SID_LOAD_MH][SID_CAM_X]) + PI180;
        else                                                    sway_io_workbuf.ph[SID_LOAD_MH][SID_CAM_X] = atan(sway_io_workbuf.dthw[SID_LOAD_MH][SID_CAM_X] / sway_io_workbuf.th[SID_LOAD_MH][SID_CAM_X]) - PI180;
    }
    else { //位相は-π～πの範囲で表現
        if (sway_io_workbuf.dth[SID_LOAD_MH][SID_CAM_X] >= 0.0) sway_io_workbuf.ph[SID_LOAD_MH][SID_CAM_X] = PI90;
        else                                                    sway_io_workbuf.ph[SID_LOAD_MH][SID_CAM_X] = -PI90;
    }

    if (sway_io_workbuf.th[SID_LOAD_AH][SID_CAM_X] > 0.00001) {
        sway_io_workbuf.ph[SID_LOAD_AH][SID_CAM_X] = atan(sway_io_workbuf.dthw[SID_LOAD_AH][SID_CAM_X] / sway_io_workbuf.th[SID_LOAD_AH][SID_CAM_X]);
    }
    else if (sway_io_workbuf.th[SID_LOAD_AH][SID_CAM_X] < -0.00001) { // atan()引数の0割回避
        if (sway_io_workbuf.dth[SID_LOAD_AH][SID_CAM_X] >= 0.0) sway_io_workbuf.ph[SID_LOAD_AH][SID_CAM_X] = atan(sway_io_workbuf.dthw[SID_LOAD_AH][SID_CAM_X] / sway_io_workbuf.th[SID_LOAD_AH][SID_CAM_X]) + PI180;
        else                                                    sway_io_workbuf.ph[SID_LOAD_AH][SID_CAM_X] = atan(sway_io_workbuf.dthw[SID_LOAD_AH][SID_CAM_X] / sway_io_workbuf.th[SID_LOAD_AH][SID_CAM_X]) - PI180;
    }
    else { //位相は-π～πの範囲で表現
        if (sway_io_workbuf.dth[SID_LOAD_MH][SID_CAM_X] >= 0.0) sway_io_workbuf.ph[SID_LOAD_MH][SID_CAM_X] = PI90;
        else                                                    sway_io_workbuf.ph[SID_LOAD_MH][SID_CAM_X] = -PI90;
    }

    //位相(Y軸）
    if (sway_io_workbuf.th[SID_LOAD_MH][SID_CAM_Y] > 0.00001) {
        sway_io_workbuf.ph[SID_LOAD_MH][SID_CAM_Y] = atan(sway_io_workbuf.dthw[SID_LOAD_MH][SID_CAM_Y] / sway_io_workbuf.th[SID_LOAD_MH][SID_CAM_Y]);
    }
    else if (sway_io_workbuf.th[SID_LOAD_MH][SID_CAM_Y] < -0.00001) { // atan()引数の0割回避
        if (sway_io_workbuf.dth[SID_LOAD_MH][SID_CAM_Y] >= 0.0) sway_io_workbuf.ph[SID_LOAD_MH][SID_CAM_Y] = atan(sway_io_workbuf.dthw[SID_LOAD_MH][SID_CAM_Y] / sway_io_workbuf.th[SID_LOAD_MH][SID_CAM_Y]) + PI180;
        else                                                    sway_io_workbuf.ph[SID_LOAD_MH][SID_CAM_Y] = atan(sway_io_workbuf.dthw[SID_LOAD_MH][SID_CAM_Y] / sway_io_workbuf.th[SID_LOAD_MH][SID_CAM_Y]) - PI180;
    }
    else { //位相は-π～πの範囲で表現
        if (sway_io_workbuf.dth[SID_LOAD_MH][SID_CAM_Y] >= 0.0) sway_io_workbuf.ph[SID_LOAD_MH][SID_CAM_Y] = PI90;
        else                                                    sway_io_workbuf.ph[SID_LOAD_MH][SID_CAM_Y] = -PI90;
    }

    if (sway_io_workbuf.th[SID_LOAD_AH][SID_CAM_Y] > 0.00001) {
        sway_io_workbuf.ph[SID_LOAD_AH][SID_CAM_Y] = atan(sway_io_workbuf.dthw[SID_LOAD_AH][SID_CAM_Y] / sway_io_workbuf.th[SID_LOAD_AH][SID_CAM_Y]);
    }
    else if (sway_io_workbuf.th[SID_LOAD_AH][SID_CAM_Y] < -0.00001) { // atan()引数の0割回避
        if (sway_io_workbuf.dth[SID_LOAD_AH][SID_CAM_Y] >= 0.0) sway_io_workbuf.ph[SID_LOAD_AH][SID_CAM_Y] = atan(sway_io_workbuf.dthw[SID_LOAD_AH][SID_CAM_Y] / sway_io_workbuf.th[SID_LOAD_AH][SID_CAM_Y]) + PI180;
        else                                                    sway_io_workbuf.ph[SID_LOAD_AH][SID_CAM_Y] = atan(sway_io_workbuf.dthw[SID_LOAD_AH][SID_CAM_Y] / sway_io_workbuf.th[SID_LOAD_AH][SID_CAM_Y]) - PI180;
    }
    else { //位相は-π～πの範囲で表現
        if (sway_io_workbuf.dth[SID_LOAD_AH][SID_CAM_Y] >= 0.0) sway_io_workbuf.ph[SID_LOAD_AH][SID_CAM_Y] = PI90;
        else                                                    sway_io_workbuf.ph[SID_LOAD_AH][SID_CAM_Y] = -PI90;
    }

    return 0;
}
//*********************************************************************************************
//IF用ソケット
static WSADATA wsaData;
static SOCKET s;
static SOCKADDR_IN addrin;//受信ポートアドレス
static SOCKADDR_IN server;//送信先ポートアドレス
static int serverlen, nEvent;
static int nRtn = 0, nRcv = 0, nSnd = 0;
static u_short port = SWAY_IF_IP_SWAY_PORT_C;
static char szBuf[512];

std::wostringstream woMSG;
std::wstring wsMSG;


HWND CSwayIF::open_WorkWnd(HWND hwnd_parent) {
    InitCommonControls();//コモンコントロール初期化

    WNDCLASSEX wc;

    hInst = GetModuleHandle(0);

    ZeroMemory(&wc, sizeof(wc));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WorkWndProc;// !CALLBACKでreturnを返していないとWindowClassの登録に失敗する
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInst;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = TEXT("WorkWnd");
    wc.hIconSm = NULL;
    ATOM fb = RegisterClassExW(&wc);

    hWorkWnd = CreateWindow(TEXT("WorkWnd"),
        TEXT("SWAY SENSOR IF COMM_CHK"),
        WS_POPUPWINDOW | WS_VISIBLE | WS_CAPTION, WORK_WND_X, WORK_WND_Y, WORK_WND_W, WORK_WND_H,
        hwnd_parent,
        0,
        hInst,
        NULL);

    ShowWindow(hWorkWnd, SW_SHOW);
    UpdateWindow(hWorkWnd);

    return hWorkWnd;



    return 0;
}
//*********************************************************************************************
int CSwayIF::close_WorkWnd() {
    closesocket(s);
    WSACleanup();
    DestroyWindow(hWorkWnd);  //ウィンドウ破棄
    hWorkWnd = NULL;
    return 0;
}
//*********************************************************************************************
int CSwayIF::init_sock(HWND hwnd) {
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {    //WinSockの初期化
        perror("WSAStartup Error\n");
        return -1;
    }

    s = socket(AF_INET, SOCK_DGRAM, 0);                  //Socketオープン
    if (s < 0) {
        perror("socket失敗\n");
        return -2;
    }
    memset(&addrin, 0, sizeof(addrin));
    addrin.sin_port = htons(port);
    addrin.sin_family = AF_INET;
    inet_pton(AF_INET, CTRL_PC_IP_ADDR_SWAY, &addrin.sin_addr.s_addr);

    nRtn = bind(s, (LPSOCKADDR)&addrin, (int)sizeof(addrin)); //ソケットに名前を付ける
    if (nRtn == SOCKET_ERROR) {
        perror("bindエラーです\n");
        closesocket(s);
        WSACleanup();
        return -3;
    }

    nRtn = WSAAsyncSelect(s, hwnd, ID_UDP_EVENT, FD_READ | FD_WRITE | FD_CLOSE);

    if (nRtn == SOCKET_ERROR) {
        woMSG << L"非同期化失敗";
        closesocket(s);
        WSACleanup();
        return -4;
    }

    //送信先アドレス初期値設定
    memset(&server, 0, sizeof(server));
    server.sin_port = htons(SWAY_IF_IP_SWAY_PORT_S);
    server.sin_family = AF_INET;
    inet_pton(AF_INET, SWAY_SENSOR_IP_ADDR, &server.sin_addr.s_addr);

    return 0;
    ;
}

void  CSwayIF::init_rcv_msg() {
    for (int i = 0; i < N_SWAY_SENSOR; i++)
        for (int k = 0; k < N_SWAY_SENSOR_RCV_BUF; k++) {
            rcv_msg[i][k].head.id[0] = 'N';
            rcv_msg[i][k].head.id[1] = 'A';
            rcv_msg[i][k].head.id[2] = '\0';
            GetLocalTime(&rcv_msg[i][k].head.time);
            for (int l = 0; l < SWAY_SENSOR_N_CAM; l++) {
                rcv_msg[i][k].body[l].cam_spec.l0_x = (INT32)(CamPrm.arr[0][0][0][SID_l0]*1000.0);
                rcv_msg[i][k].body[l].cam_spec.l0_y = (INT32)(CamPrm.arr[0][0][1][SID_l0]*1000.0);
                rcv_msg[i][k].body[l].cam_spec.ph0_x = (INT32)(CamPrm.arr[0][0][0][SID_ph0]*100000.0);
                rcv_msg[i][k].body[l].cam_spec.ph0_y = (INT32)(CamPrm.arr[0][0][1][SID_ph0] * 100000.0);
  //              rcv_msg[i][k].body[l].cam_spec.phc_x = (INT32)(CamPrm.arr[0][0][0][SID_phc] * 100000.0);
  //              rcv_msg[i][k].body[l].cam_spec.phc_y = (INT32)(CamPrm.arr[0][0][1][SID_phc] * 100000.0);
                rcv_msg[i][k].body[l].cam_spec.pixlrad_x = (INT32)(CamPrm.rad2pix[0][0][0] * 100000.0);
                rcv_msg[i][k].body[l].cam_spec.pixlrad_y = (INT32)(CamPrm.rad2pix[0][0][1] * 100000.0);
                rcv_msg[i][k].body[l].cam_spec.pix_x = 1000;
                rcv_msg[i][k].body[l].cam_spec.pix_y = 1000;

                rcv_msg[i][k].body[l].cam_stat.error = 0;
                rcv_msg[i][k].body[l].cam_stat.mode = 0;
                rcv_msg[i][k].body[l].cam_stat.status = 0;
                rcv_msg[i][k].body[l].cam_stat.tilt_dx = 0;
                rcv_msg[i][k].body[l].cam_stat.tilt_dy = 0;
                rcv_msg[i][k].body[l].cam_stat.tilt_x = 0;
                rcv_msg[i][k].body[l].cam_stat.tilt_y = 0;

                for (int m = 0; m < SWAY_SENSOR_N_TARGET; m++) {
                    rcv_msg[i][k].body[l].tg_stat[m].dpx_tgs = 100;
                    rcv_msg[i][k].body[l].tg_stat[m].dpy_tgs = 100;
                    rcv_msg[i][k].body[l].tg_stat[m].dth_x = 0;
                    rcv_msg[i][k].body[l].tg_stat[m].dth_y = 0;
                    rcv_msg[i][k].body[l].tg_stat[m].th_x = 0;
                    rcv_msg[i][k].body[l].tg_stat[m].th_y = 0;
                    rcv_msg[i][k].body[l].tg_stat[m].tg_size = 0;
                    rcv_msg[i][k].body[l].tg_stat[m].th_x0 = 0;
                    rcv_msg[i][k].body[l].tg_stat[m].th_y0 = 0;
                }
            }
        }

}

//*********************************************************************************************
int CSwayIF::send_msg(int sensor_id, INT32 com_id) {

    i_snd_msg[sensor_id] = 0;

    snd_msg[sensor_id][i_snd_msg[sensor_id]].head.id[0] = 'P';
    snd_msg[sensor_id][i_snd_msg[sensor_id]].head.id[1] = 'C';
    snd_msg[sensor_id][i_snd_msg[sensor_id]].head.id[2] = '0';
    snd_msg[sensor_id][i_snd_msg[sensor_id]].head.id[3] = '1';
    snd_msg[sensor_id][i_snd_msg[sensor_id]].head.sockaddr = addrin;

    snd_msg[sensor_id][i_snd_msg[sensor_id]].body.command = com_id;
    snd_msg[sensor_id][i_snd_msg[sensor_id]].body.mode = sens_mode;
    snd_msg[sensor_id][i_snd_msg[sensor_id]].body.freq = cycle_min_ms;
    snd_msg[sensor_id][i_snd_msg[sensor_id]].body.d[0] = (INT32)(pCraneStat->mh_l * 1000);
    snd_msg[sensor_id][i_snd_msg[sensor_id]].body.d[1] = (INT32)(pCraneStat->mh_l * 1000);

    int n = sizeof(ST_SWAY_SND_MSG);

    nRtn = sendto(s, reinterpret_cast<const char*> (&snd_msg[SID_SENSOR1][i_snd_msg[SID_SENSOR1]]), n, 0, (LPSOCKADDR)&server, sizeof(ST_SWAY_SND_MSG));

    if (nRtn == n) {
        nSnd++;
        woMSG << L" SND len: " << nRtn << L"  Count :" << nSnd << L"    COM:" << snd_msg[SID_SENSOR1][i_snd_msg[SID_SENSOR1]].body.command
            << L"   scan:" << snd_msg[SID_SENSOR1][i_snd_msg[SID_SENSOR1]].body.freq << L"\n "
            << L"MODE:" << snd_msg[SID_SENSOR1][i_snd_msg[SID_SENSOR1]].body.mode
            << L"   D1:" << snd_msg[SID_SENSOR1][i_snd_msg[SID_SENSOR1]].body.d[0] << L"   D2:" << snd_msg[SID_SENSOR1][i_snd_msg[SID_SENSOR1]].body.d[1];

        be_skiped_once_const_msg = true;
    }
    else if (nRtn == SOCKET_ERROR) {
        woMSG << L" SOCKET ERROR: CODE ->   " << WSAGetLastError();
    }
    else {
        woMSG << L" sendto size ERROR ";
    }
    tweet2sndMSG(woMSG.str()); woMSG.str(L""); woMSG.clear();

    return nRtn;
}

//# ウィンドウへのメッセージ表示　wstring
void CSwayIF::tweet2statusMSG(const std::wstring& srcw) {
    SetWindowText(st_swy_work_wnd.hwndSTATMSG, srcw.c_str()); return;
};
void CSwayIF::tweet2rcvMSG(const std::wstring& srcw) {
    static HWND hwndSNDMSG;
    SetWindowText(st_swy_work_wnd.hwndRCVMSG, srcw.c_str()); return;
};
void CSwayIF::tweet2sndMSG(const std::wstring& srcw) {
    SetWindowText(st_swy_work_wnd.hwndSNDMSG, srcw.c_str()); return;
};
void CSwayIF::tweet2infMSG(const std::wstring& srcw) {
    SetWindowText(st_swy_work_wnd.hwndINFMSG, srcw.c_str()); return;
};

void CSwayIF::update_buf_set_disp_msg(HWND hwnd) {

    woMSG.str(L""); wsMSG.clear();
    if (iDispSensor == SID_SIM)
        woMSG << L"ID SIM" << L" Buf " << iDispBuf << L" CAM " << iDispCam + 1 << L" TG " << iDispTg + 1 << L"      NEXT->";
    else
        woMSG << L"ID   " << iDispSensor + 1 << L" Buf " << iDispBuf << L" CAM " << iDispCam + 1 << L" TG " << iDispTg + 1 << L"      NEXT-> ";

    if (IsDlgButtonChecked(hwnd, ID_PB_SWAY_IF_INFO_COMDATA) == BST_CHECKED)
        SetWindowText(st_swy_work_wnd.hwndDispBufMSG, woMSG.str().c_str()); woMSG.str(L""); wsMSG.clear();
    return;
}

//*********************************************************************************************
void CSwayIF::set_sensor_msg() {

    //エラーメッセージ
    ws_sensor_err_msg[0] = SW_TXT_ERR_CODE_CAMERA;
    ws_sensor_err_msg[1] = SW_TXT_ERR_CODE_TILT;
    ws_sensor_err_msg[4] = SW_TXT_ERR_CODE_DETECT_TG1;
    ws_sensor_err_msg[5] = SW_TXT_ERR_CODE_DETECT_TG2;


    //検出状態
    ws_sensor_stat_msg[0] = SW_TXT_STAT_TG1_DETECTED;
    ws_sensor_stat_msg[1] = SW_TXT_STAT_TG2_DETECTED;
    ws_sensor_stat_msg[4] = SW_TXT_STAT_INIT_SENSOR;
    ws_sensor_stat_msg[5] = SW_TXT_STAT_INIT_TILT;
    ws_sensor_stat_msg[6] = SW_TXT_STAT_0SET_SENSOR;
    ws_sensor_stat_msg[7] = SW_TXT_STAT_0SET_TILT;
}
//*********************************************************************************************

LRESULT CALLBACK CSwayIF::WorkWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

    HDC hdc;
    switch (msg) {
    case WM_DESTROY: {
        hWorkWnd = NULL;
    }return 0;
    case WM_CREATE: {

        InitCommonControls();//コモンコントロール初期化
        HINSTANCE hInst = GetModuleHandle(0);

        CreateWindowW(TEXT("STATIC"), L"STATUS", WS_CHILD | WS_VISIBLE | SS_LEFT,
            10, 5, 55, 20, hwnd, (HMENU)ID_STATIC_SWAY_IF_LABEL_RCV, hInst, NULL);
        st_swy_work_wnd.hwndSTATMSG = CreateWindowW(TEXT("STATIC"), L"-", WS_CHILD | WS_VISIBLE | SS_LEFT,
            70, 5, 440, 20, hwnd, (HMENU)ID_STATIC_SWAY_IF_LABEL_RCV, hInst, NULL);
        CreateWindowW(TEXT("STATIC"), L"RCV  ", WS_CHILD | WS_VISIBLE | SS_LEFT,
            10, 30, 55, 20, hwnd, (HMENU)ID_STATIC_SWAY_IF_LABEL_RCV, hInst, NULL);
        st_swy_work_wnd.hwndRCVMSG = CreateWindowW(TEXT("STATIC"), L"-", WS_CHILD | WS_VISIBLE | SS_LEFT,
            70, 30, 440, 40, hwnd, (HMENU)ID_STATIC_SWAY_IF_LABEL_RCV, hInst, NULL);
        CreateWindowW(TEXT("STATIC"), L"SND  ", WS_CHILD | WS_VISIBLE | SS_LEFT,
            10, 75, 55, 20, hwnd, (HMENU)ID_STATIC_SWAY_IF_LABEL_SND, hInst, NULL);
        st_swy_work_wnd.hwndSNDMSG = CreateWindowW(TEXT("STATIC"), L"-", WS_CHILD | WS_VISIBLE | SS_LEFT,
            70, 75, 440, 40, hwnd, (HMENU)ID_STATIC_SWAY_IF_LABEL_RCV, hInst, NULL);
        CreateWindowW(TEXT("STATIC"), L"Info ", WS_CHILD | WS_VISIBLE | SS_LEFT,
            10, 120, 55, 20, hwnd, (HMENU)ID_STATIC_SWAY_IF_LABEL_SND, hInst, NULL);
        st_swy_work_wnd.hwndINFMSG = CreateWindowW(TEXT("STATIC"), L"-", WS_CHILD | WS_VISIBLE | SS_LEFT,
            70, 120, 440, 280, hwnd, (HMENU)ID_STATIC_SWAY_IF_LABEL_RCV, hInst, NULL);

        if (init_sock(hwnd) == 0) {
            woMSG << L"SOCK OK";
            tweet2statusMSG(woMSG.str()); woMSG.str(L""); woMSG.clear();
            wsMSG = L"No RCV MSG";
            tweet2rcvMSG(wsMSG); wsMSG.clear();
            wsMSG = L"No SND MSG";
            tweet2sndMSG(wsMSG); wsMSG.clear();
        }
        else {
            tweet2statusMSG(woMSG.str()); woMSG.str(L""); woMSG.clear();
            wsMSG = L"No RCV MSG";
            tweet2rcvMSG(wsMSG); wsMSG.clear();
            wsMSG = L"No SND MSG";
            tweet2sndMSG(wsMSG); wsMSG.clear();

            close_WorkWnd();
        }

        iDispCam = iDispBuf = 0;
        st_swy_work_wnd.hwndDispBufMSG = CreateWindowW(TEXT("STATIC"), L"ID:  1 BUF:0 CAM:1 TG:1    NEXT->", WS_CHILD | WS_VISIBLE | SS_LEFT,
            70, 408, 220, 20, hwnd, (HMENU)ID_STATIC_SWAY_IF_DISP_SELBUF, hInst, NULL);

        st_swy_work_wnd.hwndCamChangePB = CreateWindow(L"BUTTON", L"ID", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            320, 403, 30, 30, hwnd, (HMENU)ID_PB_SWAY_IF_CHG_DISP_SENSOR, hInst, NULL);

        st_swy_work_wnd.hwndBufChangePB = CreateWindow(L"BUTTON", L"BUF", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            355, 403, 40, 30, hwnd, (HMENU)ID_PB_SWAY_IF_CHG_DISP_BUF, hInst, NULL);

        st_swy_work_wnd.hwndBufChangePB = CreateWindow(L"BUTTON", L"CAM", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            400, 403, 40, 30, hwnd, (HMENU)ID_PB_SWAY_IF_CHG_DISP_CAM, hInst, NULL);

        st_swy_work_wnd.hwndTargetChangePB = CreateWindow(L"BUTTON", L"TG", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            445, 403, 40, 30, hwnd, (HMENU)ID_PB_SWAY_IF_CHG_DISP_TG, hInst, NULL);


        st_swy_work_wnd.hwndInfComPB = CreateWindow(L"BUTTON", L"Com", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
            15, 150, 40, 30, hwnd, (HMENU)ID_PB_SWAY_IF_INFO_COMDATA, hInst, NULL);

        st_swy_work_wnd.hwndInfMsgPB = CreateWindow(L"BUTTON", L"MSG", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
            15, 185, 40, 30, hwnd, (HMENU)ID_PB_SWAY_IF_INFO_MSG, hInst, NULL);

        SendMessage(st_swy_work_wnd.hwndInfComPB, BM_SETCHECK, BST_CHECKED, 0L);

        CreateWindowW(TEXT("STATIC"), L" Min \n cycle", WS_CHILD | WS_VISIBLE | SS_LEFT,
            10, 300, 50, 40, hwnd, (HMENU)ID_STATIC_SWAY_IF_MINCYCLE, hInst, NULL);

        st_swy_work_wnd.hwndCycleUpPB = CreateWindow(L"BUTTON", L"10m↑", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            15, 350, 45, 30, hwnd, (HMENU)ID_PB_SWAY_IF_MIN_CYCLE_10mUP, hInst, NULL);

        st_swy_work_wnd.hwndCycleDnPB = CreateWindow(L"BUTTON", L"10m↓", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            15, 385, 45, 30, hwnd, (HMENU)ID_PB_SWAY_IF_MIN_CYCLE_10mDN, hInst, NULL);

        //振れ計算時にカメラ設置位置オフセットを0にして計算（振れセンサ生値確認用）するモードへの切り替え設定用 
        st_swy_work_wnd.h_pb_no_pos_offset = CreateWindow(L"BUTTON", L"NO OFF", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
            15, 450, 80, 25, hwnd, (HMENU)ID_CHECK_SWAY_CAL_NO_OFFSET, hInst, NULL);
    
        // 振れ計算時に傾斜計オフセットを0にして計算（振れセンサ生値確認用）するモードへの切り替え設定用
        st_swy_work_wnd.h_pb_no_til_offset = CreateWindow(L"BUTTON", L"NO TIL", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
            100, 450, 80, 25, hwnd, (HMENU)ID_CHECK_SWAY_CAL_NO_TILT, hInst, NULL);

        //振れセンサPC再起動指令ボタン
        st_swy_work_wnd.h_pb_pc_reset = CreateWindow(L"BUTTON", L"PC RESET", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            360, 450, 90, 25, hwnd, (HMENU)IDC_PB_PC_RESET, hInst, NULL);

        //スナップショット保存指令メッセージ送信ボタン
        st_swy_work_wnd.h_pb_img_save = CreateWindow(L"BUTTON", L"SSHOT", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            200, 450, 55, 25, hwnd, (HMENU)IDC_PB_SCREEN_SHOT, hInst, NULL);

        //### 振れセンサ調整用
        st_swy_work_wnd.h_static1 = CreateWindowW(TEXT("STATIC"), L"  SENSOR      0SET        RESET", WS_CHILD | WS_VISIBLE | SS_LEFT,
            10, 485, 260, 20, hwnd, (HMENU)IDC_STATIC_1, hInst, NULL);


        //メインウィンドウの操作ボタン有効カメラ1,2選択（当面常時1が有効：将来用）
        st_swy_work_wnd.h_pb_sel_sensor1 = CreateWindow(L"BUTTON", L"1", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
            30, 510, 20, 25, hwnd, (HMENU)IDC_PB_SENSOR_1, hInst, NULL);
        st_swy_work_wnd.h_pb_sel_sensor2 = CreateWindow(L"BUTTON", L"2", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
            50, 510, 20, 25, hwnd, (HMENU)IDC_PB_SENSOR_2, hInst, NULL);
        SendMessage(st_swy_work_wnd.h_pb_sel_sensor1, BM_SETCHECK, BST_CHECKED, 0L);//センサ１をチェック状態にしておく

        //カメラリセット指令メッセージ送信ボタン
        st_swy_work_wnd.h_pb_reset_sensor = CreateWindow(L"BUTTON", L"CAM", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            90, 510, 40, 25, hwnd, (HMENU)IDC_PB_0SET_CAMERA, hInst, NULL);

        //傾斜計リセット指令メッセージ送信ボタン
        st_swy_work_wnd.h_pb_reset_tilt = CreateWindow(L"BUTTON", L"TIL", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            135, 510, 30, 25, hwnd, (HMENU)IDC_PB_0SET_TILT, hInst, NULL);

        //カメラ0セット指令メッセージ送信ボタン
        st_swy_work_wnd.h_pb_0set_sensor = CreateWindow(L"BUTTON", L"CAM", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            180, 510, 40, 25, hwnd, (HMENU)IDC_PB_RESET_CAMERA, hInst, NULL);

        //傾斜計0セット指令メッセージ送信ボタン
        st_swy_work_wnd.h_pb_0set_tilt = CreateWindow(L"BUTTON", L"TIL", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            225, 510, 30, 25, hwnd, (HMENU)IDC_PB_RESET_TILT, hInst, NULL);

         //振れセンサ送信タイマ起動
        SetTimer(hwnd, ID_WORK_WND_TIMER, WORK_SCAN_TIME, NULL);

    }break;
    case WM_TIMER: {
        if (be_skiped_once_const_msg == false)
            send_msg(SID_SENSOR1, SW_SND_COM_CONST_DATA);
        be_skiped_once_const_msg = false;

    }break;

    case ID_UDP_EVENT: {
        nEvent = WSAGETSELECTEVENT(lp);
        switch (nEvent) {
        case FD_READ: {
            nRcv++;
            serverlen = (int)sizeof(server);

            SOCKADDR from_addr;
            sockaddr_in* psockaddr = (sockaddr_in*)&from_addr;
            int from_size = (int)sizeof(from_addr);

            nRtn = recvfrom(s, (char*)&rcv_msg[0][0], sizeof(ST_SWAY_RCV_MSG), 0, (SOCKADDR*)&from_addr, &from_size);

            if (nRtn == SOCKET_ERROR) {
                woMSG << L" recvfrom ERROR";
                if (IsDlgButtonChecked(hwnd, ID_PB_SWAY_IF_INFO_COMDATA) == BST_CHECKED)
                    tweet2rcvMSG(woMSG.str()); woMSG.str(L""); woMSG.clear();
            }
            else {
                ST_SWAY_RCV_MSG msg = rcv_msg[iDispSensor][iDispBuf];

                //  電文からパラメータ読み込み
                if ((swx.is_read_from_msg == false) || (swy.is_read_from_msg == false))
  //                  get_sensor_param_from_msg(&msg);

                //ヘッダ部表示
                woMSG << L" RCV len: " << nRtn << L" Count :" << nRcv;
                woMSG << L"\n IP: " << psockaddr->sin_addr.S_un.S_un_b.s_b1 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b2 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b3 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b4;
                woMSG << L" PORT: " << psockaddr->sin_port;
                tweet2rcvMSG(woMSG.str()); woMSG.str(L""); woMSG.clear();

                //受信データ表示
                if (IsDlgButtonChecked(hwnd, ID_PB_SWAY_IF_INFO_COMDATA) == BST_CHECKED) {


                    woMSG << L"Header" << L"  >> ID: " << msg.head.id[0] << msg.head.id[1] << msg.head.id[2] << msg.head.id[3];
                    //日時
                    woMSG << L"  " << msg.head.time.wMonth << L"/" << msg.head.time.wDay << L" " << msg.head.time.wHour << L":" << msg.head.time.wMinute << L":" << msg.head.time.wSecond;
                    //# 仕様
                    woMSG << L"\n\n@SPEC";
                    //画素数
                    woMSG << L"\n *nPIX x:" << msg.body[iDispCam].cam_spec.pix_x << L" y:" << msg.body[iDispCam].cam_spec.pix_y;

                    //カメラ取付距離,角度
                    woMSG << L"\n *l0 x:" << msg.body[iDispCam].cam_spec.l0_x << L" y:" << msg.body[iDispCam].cam_spec.l0_y << L"  *ph0 x:" << msg.body[iDispCam].cam_spec.ph0_x << L" y:" << msg.body[iDispCam].cam_spec.ph0_y;
                    woMSG << L"\n *phc x:" << msg.body[iDispCam].cam_spec.phc_x << L" y:" << msg.body[iDispCam].cam_spec.phc_y << L"  *Pix/Rad  x:" << msg.body[iDispCam].cam_spec.pixlrad_x << L" y:" << msg.body[iDispCam].cam_spec.pixlrad_y;

                    //# 機器状態
                    woMSG << L"\n@STATUS";
                    woMSG << L"\n *Mode:" << msg.body[iDispCam].cam_stat.mode << L" *STAT:" << msg.body[iDispCam].cam_stat.status << L" *ERR:" << msg.body[iDispCam].cam_stat.error;

                    //# Data
                    woMSG << L"\n@DATA";
                    //傾斜計
                    woMSG << L"\n *Til  X :" << msg.body[iDispCam].cam_stat.tilt_x << L"(" << (double)(msg.body[iDispCam].cam_stat.tilt_x) * 180.0 / PI180 / 1000000.0 << L"deg)  Y :" << msg.body[iDispCam].cam_stat.tilt_y << L"(" << (double)(msg.body[iDispCam].cam_stat.tilt_y) * 180.0 / PI180 / 1000000.0 << L"deg)";
                    woMSG << L"  dX :" << msg.body[iDispCam].cam_stat.tilt_dx << L" dY :" << msg.body[iDispCam].cam_stat.tilt_dy;
                    woMSG << L"\n *PIX x :" << msg.body[iDispCam].tg_stat[iDispTg].th_x << L" y :" << msg.body[iDispCam].tg_stat[iDispTg].th_y << L"  *dPIX x :" << msg.body[iDispCam].tg_stat[iDispTg].dth_x << L" y :" << msg.body[iDispCam].tg_stat[iDispTg].dth_y;
                    woMSG << L"\n *CENTER X0 :" << msg.body[iDispCam].tg_stat[iDispTg].th_x0 << L" Y0 :" << msg.body[iDispCam].tg_stat[iDispTg].th_y0 << L"\n *tgSize :" << msg.body[iDispCam].tg_stat[iDispTg].tg_size;
                    woMSG << L"\n *tg_dist x :" << msg.body[iDispCam].tg_stat[iDispTg].dpx_tgs << L" y :" << msg.body[iDispCam].tg_stat[iDispTg].dpy_tgs;
                }
                //受信内容メッセージ表示
                else {

                    woMSG << L"# Info MSG:\n";
                    woMSG << L" " << msg.body[iDispCam].info;
                    woMSG << L"\n\n";

                    INT32 msgbits = msg.body[iDispCam].cam_stat.status;
                    woMSG;
                    woMSG << L"# DETECT STATUS:\n";

                    for (int i = 0; i < 16; i++) {
                        if (msgbits & 0x1)
                            woMSG << L"    >" << ws_sensor_stat_msg[i] << L"\n";
                        msgbits = msgbits >> 1;
                    }

                    woMSG << L"\n";

                    msgbits = msg.body[iDispCam].cam_stat.error;
                    woMSG << L"# ERR STATUS:\n";
                    for (int i = 0; i < 16; i++) {
                        if (msgbits & 0x1)
                            woMSG << L"    >" << ws_sensor_err_msg[i] << L"\n";
                        msgbits = msgbits >> 1;
                    }

                }

                tweet2infMSG(woMSG.str()); woMSG.str(L""); woMSG.clear();

            }

        }break;
        case FD_WRITE: {

        }break;
        case FD_CLOSE: {
            ;
        }break;

        }
    }break;


    case SWAY_SENSOR__MSG_SEND_COM:
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        hdc = BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
    }break;
    case WM_COMMAND: {
        int wmId = LOWORD(wp);
        // 選択されたメニューの解析:
        switch (wmId)
        {
        case ID_PB_SWAY_IF_CHG_DISP_SENSOR:
            iDispSensor++;
            if (iDispSensor >= N_SWAY_SENSOR)iDispSensor = 0;
            update_buf_set_disp_msg(hwnd);

            break;

        case ID_PB_SWAY_IF_CHG_DISP_BUF:
            iDispBuf++;
            if (iDispBuf >= N_SWAY_SENSOR_RCV_BUF) iDispBuf = 0;
            update_buf_set_disp_msg(hwnd);
            break;

        case ID_PB_SWAY_IF_CHG_DISP_CAM:
            iDispCam++;
            if (iDispCam >= N_SWAY_SENSOR_CAMERA) iDispCam = 0;
            update_buf_set_disp_msg(hwnd);
            break;

        case ID_PB_SWAY_IF_CHG_DISP_TG:
            iDispTg++;
            if (iDispTg >= N_SWAY_SENSOR_TARGET) iDispTg = 0;
            update_buf_set_disp_msg(hwnd);
            break;

        case ID_PB_SWAY_IF_INFO_COMDATA:
            break;
        case  ID_PB_SWAY_IF_INFO_MSG:
            break;
        case  ID_PB_SWAY_IF_MIN_CYCLE_10mUP:
            cycle_min_ms += 10;
            break;
        case  ID_PB_SWAY_IF_MIN_CYCLE_10mDN:
            if (cycle_min_ms >= 20) cycle_min_ms -= 10;
            break;

        case  IDC_PB_SENSOR_1:
            break;
        case  IDC_PB_SENSOR_2:
            break;
        case  IDC_PB_0SET_CAMERA:
            if (IsDlgButtonChecked(hwnd, IDC_PB_SENSOR_1) == BST_CHECKED) send_msg(SID_SENSOR1, SW_SND_COM_CAMERA1_0SET);
            else send_msg(SID_SENSOR1, SW_SND_COM_CAMERA2_0SET);
            break;
        case  IDC_PB_0SET_TILT:
            if (IsDlgButtonChecked(hwnd, IDC_PB_SENSOR_1) == BST_CHECKED) send_msg(SID_SENSOR1, SW_SND_COM_TILT1_0SET);
            else send_msg(SID_SENSOR1, SW_SND_COM_TILT2_0SET);
            break;
        case  IDC_PB_RESET_CAMERA:
            if (IsDlgButtonChecked(hwnd, IDC_PB_SENSOR_1) == BST_CHECKED) send_msg(SID_SENSOR1, SW_SND_COM_CAMERAR1_RESET);
            else send_msg(SID_SENSOR1, SW_SND_COM_CAMERAR2_RESET);
            break;
        case  IDC_PB_RESET_TILT:
            if (IsDlgButtonChecked(hwnd, IDC_PB_SENSOR_1) == BST_CHECKED) send_msg(SID_SENSOR1, SW_SND_COM_TILT1_RESET);
            else send_msg(SID_SENSOR1, SW_SND_COM_TILT2_RESET);
            break;
        case  IDC_PB_PC_RESET:
            send_msg(SID_SENSOR1, SW_SND_COM_PC_RESET);
            break;
        case  IDC_PB_SCREEN_SHOT:
            send_msg(SID_SENSOR1, SW_SND_COM_SAVE_IMG);
            break;

        case  ID_CHECK_SWAY_CAL_NO_OFFSET:
            if (IsDlgButtonChecked(hwnd, ID_CHECK_SWAY_CAL_NO_OFFSET) == BST_CHECKED) cal_mode |= ID_SWAY_CAL_NO_OFFSET;
            else cal_mode &= ~ID_SWAY_CAL_NO_OFFSET;
            break;

        case  ID_CHECK_SWAY_CAL_NO_TILT:
            if (IsDlgButtonChecked(hwnd, ID_CHECK_SWAY_CAL_NO_TILT) == BST_CHECKED) cal_mode |= ID_SWAY_CAL_NO_TILT;
            else cal_mode &= ~ID_SWAY_CAL_NO_TILT;
            break;

        default: break;

        }
    }break;

    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }

    return 0;
}


