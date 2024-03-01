#include "CSIM.h"
#include "CWorkWindow_SIM.h"
#include "Spec.h"
#include "CVector3.h"
#include "SIM.h"

extern ST_SPEC def_spec;

static ST_SWY_SENS_PRM CamPrm;

CSIM::CSIM() {
    // 共有メモリオブジェクトのインスタンス化
    pSimulationStatusObj    = new CSharedMem;
    pPLCioObj               = new CSharedMem;
    pCraneStatusObj         = new CSharedMem;
    pAgentInfObj            = new CSharedMem;

    // MOB オブジェクトのインスタンス化
    pCrane      = new CJC();     //クレーンのモデル
    pLoad       = new CLoad();   //主巻吊荷のモデル
    pLoad2      = new CLoad();   //補巻吊荷のモデル
    pSIM_work   = &sim_stat_workbuf;

    out_size    = 0;
 
    memset(&sim_stat_workbuf, 0, sizeof(ST_SIMULATION_STATUS));   //共有メモリへの出力セット作業用バッファ
};
CSIM::~CSIM() {
    // 共有メモリオブジェクトの解放
    delete pPLCioObj;
    delete pCraneStatusObj;
    delete pSimulationStatusObj;
    delete pAgentInfObj;

    delete pCrane;
    delete pLoad;
    delete pLoad2;
 
};

int CSIM::set_outbuf(LPVOID pbuf) {
    poutput = pbuf;return 0;
};      //出力バッファセット

//******************************************************************************************
// init_proc()
//******************************************************************************************
int CSIM::init_proc() {
     // 共有メモリ取得
     // 出力用共有メモリ取得
    out_size = sizeof(ST_SIMULATION_STATUS);
    if (OK_SHMEM != pSimulationStatusObj->create_smem(SMEM_SIMULATION_STATUS_NAME, (DWORD)out_size, MUTEX_SIMULATION_STATUS_NAME)) {
        mode |= SIM_IF_SIM_MEM_NG;
    }

    // 入力用共有メモリ取得
    if (OK_SHMEM != pPLCioObj->create_smem(SMEM_PLC_IO_NAME, sizeof(ST_PLC_IO), MUTEX_PLC_IO_NAME)) {
        mode |= SIM_IF_PLC_IO_MEM_NG;
    }
 //   set_outbuf(pPLCioObj->get_pMap());

    if (OK_SHMEM != pCraneStatusObj->create_smem(SMEM_CRANE_STATUS_NAME, sizeof(ST_CRANE_STATUS), MUTEX_CRANE_STATUS_NAME)) {
        mode |= SIM_IF_CRANE_MEM_NG;
    }
    if (OK_SHMEM != pAgentInfObj->create_smem(SMEM_AGENT_INFO_NAME, sizeof(ST_AGENT_INFO), MUTEX_AGENT_INFO_NAME)) {
        mode |= SIM_IF_AGENT_MEM_NG;
    }

    set_outbuf(pSimulationStatusObj->get_pMap());

    pCraneStat = (LPST_CRANE_STATUS)pCraneStatusObj->get_pMap();
    pPLC = (LPST_PLC_IO)pPLCioObj->get_pMap();
    pAgent = (LPST_AGENT_INFO)pAgentInfObj->get_pMap();

    pSIM_work = &sim_stat_workbuf;

   //CraneStat立ち上がり待ち
    while (pCraneStat->is_crane_status_ok !=true) {
        Sleep(100);
    }
    //PLC IF立ち上がり待ち
    while (pPLC->healthy_cnt < 10) {
        Sleep(100);
    }

    //クレーン仕様のセット
    pCrane->pPLC = pPLC;
    pCrane->pCraneStat = pCraneStat;
    pCrane->pSimStat = &sim_stat_workbuf;
    pCrane->set_spec(&def_spec);

    //クレーンの初期状態セット 
    pCrane->init_crane(SYSTEM_TICK_ms / 1000.0);

    //吊荷ｵﾌﾞｼﾞｪｸﾄにｸﾚｰﾝｵﾌﾞｼﾞｪｸﾄを紐付け
    pLoad->set_crane(pCrane);
    pLoad2->set_crane(pCrane);

    //吊荷の初期状態セット 
    Vector3 _r;
    _r.x = pCrane->r.x , _r.y = pCrane->r.y, _r.z = pCrane->r0[ID_HOIST];    //主巻吊荷位置
    Vector3 _v(0.0, 0.0, 0.0);                                               //吊荷速度
   
    //主巻
    pLoad->init_mob(SYSTEM_TICK_ms / 1000.0, _r, _v);
    pLoad->set_m(def_spec.Load0_mh);
    pLoad->set_type(ID_HOIST);

    //補巻
    double th0 = acos(SIM_INIT_MHR / def_spec.Lm);
    double ar0 = def_spec.La * cos(th0 - def_spec.rad_Lm_La);
    _r.x = ar0 * cos(SIM_INIT_TH) + SIM_INIT_X; _r.y = ar0 * sin(SIM_INIT_TH); _r.z = pCrane->r0[ID_AHOIST];
  
    pLoad2->init_mob(SYSTEM_TICK_ms / 1000.0, _r, _v);
    pLoad2->set_m(def_spec.Load0_ah);
    pLoad2->set_type(ID_AHOIST);

    //振れ角計算用カメラパラメータセット
  
    return int(mode & 0xff00);
}
//*********************************************************************************************
// input()
//*********************************************************************************************
int CSIM::input() {
    sim_stat_workbuf.helthy_cnt++;

    //MAINプロセス(Environmentタスクのヘルシー信号取り込み）
    source_counter = pCraneStat->env_act_count;

    //PLC 指令は100%→1.0　ドラム回転速度指令でセット
    pCrane->set_v_ref(pPLC->nv_ref[ID_HOIST], pPLC->nv_ref[ID_GANTRY], pPLC->nv_ref[ID_SLEW], pPLC->nv_ref[ID_BOOM_H], pPLC->nv_ref[ID_AHOIST]);
     //スキャンタイムセット dtはマルチメディアタイマ　コールバックでセット
    pCrane->set_dt(dt);
    pLoad->set_dt(dt);
    pLoad2->set_dt(dt);

    //移動極限状態
    for (int i = 0; i < MOTION_ID_MAX;i++) {
        pCrane->is_fwd_endstop[i] = pPLC->endlim[i] & PLC_IF_LIMIT_COM_FWD_EMR;
        pCrane->is_rev_endstop[i] = pPLC->endlim[i] & PLC_IF_LIMIT_COM_REV_EMR;
    }
    return 0;
}
//*********************************************************************************************
// parse()
//*********************************************************************************************
static int sim_act_last,wait_count=0;
int CSIM::parse() {

     pCrane->update_break_status(); //ブレーキ状態更新

     pCrane->timeEvolution();       //クレーンの位置,速度計算
     pLoad->timeEvolution();        //吊荷の位置,速度計算
     pLoad2->timeEvolution();       //吊荷の位置,速度計算
       
     pLoad->r.add(pLoad->dr);       //吊荷位置更新
     pLoad2->r.add(pLoad2->dr);       //吊荷位置更新

     pLoad->v.add(pLoad->dv);       //吊荷速度更新
     pLoad2->v.add(pLoad2->dv);       //吊荷速度更新

     pLoad->update_relative_vec();  //吊荷吊点相対ベクトル更新(ロープベクトル　L,vL)
     pLoad2->update_relative_vec();  //吊荷吊点相対ベクトル更新(ロープベクトル　L,vL)

    return 0;
}
//*********************************************************************************************
// output()
//*********************************************************************************************

static int sway_if_scan = SWAY_IF_MIN_SCAN_MS/ SYSTEM_TICK_ms;//振れセンサ送信タイミングカウンタ
int CSIM::output() {

    sim_stat_workbuf.mode = this->mode;                         //モードセット
    sim_stat_workbuf.helthy_cnt = my_helthy_counter++;          //ヘルシーカウンタセット

    set_cran_motion();  //クレーンの位置、速度情報セット

    if(my_helthy_counter% sway_if_scan == 0) set_sway_io();    //振れセンサIO情報セット
    
    
    if (out_size) { //出力処理
        memcpy_s(poutput, out_size, &sim_stat_workbuf, out_size);
    }

    return 0;
}
//*********************************************************************************************
// set_cran_motion() クレーン位置、速度情報セット
//*********************************************************************************************
int CSIM::set_cran_motion() {

    sim_stat_workbuf.a_fb[ID_HOIST] = pCrane->a0[ID_HOIST];
    sim_stat_workbuf.a_fb[ID_GANTRY] = pCrane->a0[ID_GANTRY];
    sim_stat_workbuf.a_fb[ID_SLEW] = pCrane->a0[ID_SLEW];
    sim_stat_workbuf.a_fb[ID_BOOM_H] = pCrane->a0[ID_BOOM_H];
    sim_stat_workbuf.a_fb[ID_AHOIST] = pCrane->a0[ID_AHOIST];

    sim_stat_workbuf.v_fb[ID_HOIST] = pCrane->v0[ID_HOIST];
    sim_stat_workbuf.v_fb[ID_GANTRY] = pCrane->v0[ID_GANTRY];
    sim_stat_workbuf.v_fb[ID_SLEW] = pCrane->v0[ID_SLEW];
    sim_stat_workbuf.v_fb[ID_BOOM_H] = pCrane->v0[ID_BOOM_H];
    sim_stat_workbuf.v_fb[ID_AHOIST] = pCrane->v0[ID_AHOIST];

    sim_stat_workbuf.pos[ID_HOIST] = pCrane->r0[ID_HOIST];
    sim_stat_workbuf.pos[ID_GANTRY] = pCrane->r0[ID_GANTRY];
    sim_stat_workbuf.pos[ID_SLEW] = pCrane->r0[ID_SLEW];
    sim_stat_workbuf.pos[ID_BOOM_H] = pCrane->r0[ID_BOOM_H];
    sim_stat_workbuf.pos[ID_AHOIST] = pCrane->r0[ID_AHOIST];

    sim_stat_workbuf.L = pLoad->L;
    sim_stat_workbuf.vL = pLoad->vL;
    sim_stat_workbuf.L2 = pLoad2->L;
    sim_stat_workbuf.vL2 = pLoad2->vL;

    return 0;
}
//*********************************************************************************************
// output() 振れセンサ信号セット
//*********************************************************************************************

static double thcamx_last=0, thcamy_last=0;
static int sim_counter = 0;

int CSIM::set_sway_io() {
      
    // 傾斜計検出角度模擬　とりあえず正弦波で
    sim_counter++;
    double th_tilx = (double)(sim_counter % 1000) * 0.0063;//0.0063 = 2π/1000 10msec scan 10秒周期
    double a_til = 0.001745;//傾斜角振幅 0.1deg　1deg 0.01745rad
    double tilt_x = 0.0;
    //double tilt_y = a_til * sin(th_tilx);
    double tilt_y = 0.0;
    double tilt_dx = 0.0;
    double tilt_dy = 0.0;//ω＝2π/10=0.63
    //double tilt_dy = a_til * 0.63 * cos(th_tilx);//ω＝2π/10=0.63

    // クレーンxy座標をカメラxy座標に回転変換　→　角度radに変換　
 
    double sin_th_sl = sin(pCrane->r0[ID_SLEW]);//sin(th_sl)
    double cos_th_sl = cos(pCrane->r0[ID_SLEW]);//cos(th_sl)
 
    //主巻
    double L = sim_stat_workbuf.lrm.p;

    double phx = asin(((pLoad->L.x) * sin_th_sl + (pLoad->L.y) * -cos_th_sl) / L);
    double phy = asin(((pLoad->L.x) * cos_th_sl + (pLoad->L.y) * sin_th_sl) / L);
    double dphx = asin(((pLoad->vL.x) * sin_th_sl + (pLoad->vL.y) * -cos_th_sl) / L);
    double dphy = asin(((pLoad->vL.x) * cos_th_sl + (pLoad->vL.y) * sin_th_sl) / L);

    double swx_L = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X][SID_D0];
    double swy_L = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y][SID_D0];
    double swx_PH = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X][SID_row];
    double swy_PH = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y][SID_row];

    double swx_l0 = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X][SID_l0];
    double swy_l0 = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y][SID_l0];

    double swx_ph0 = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X][SID_ph0];
    double swy_ph0 = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y][SID_ph0];

    double swx_phc = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X][SID_phc];
    double swy_phc = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y][SID_phc];
  
    double swx_th0 = swx_ph0 + tilt_x;
    double swy_th0 = swy_ph0 + tilt_y;

    double swx_thc = -(swx_phc + tilt_x);
    double swy_thc = -(swy_phc + tilt_y);

    double swx_dc = swx_L*sin(swx_PH) + swx_l0 * sin(swx_th0);                              //x0+l0sin(th0)
    double swy_dc = swy_L*cos(swy_PH + sim_stat_workbuf.th.p) + swy_l0 * sin(swy_th0);      //y0+l0sin(th0)
    double swx_hc = swy_L * sin(swy_PH + sim_stat_workbuf.th.p) + swx_l0 * cos(swx_th0);
    double swy_hc = swx_hc;

    double tan_thtx = (L * sin(phx) - swx_dc) / (L * cos(phx) + swx_hc);
    double thtx = atan(tan_thtx)- swx_thc;
    double dthtx = thtx - sim_stat_workbuf.tht_swx_mh;
    sim_stat_workbuf.tht_swx_mh = thtx;

    double tan_thty = (L * sin(phy) - swy_dc) / (L * cos(phy) + swy_hc);
    double thty = atan(tan_thty) - swy_thc;
    double dthty = thty - sim_stat_workbuf.tht_swy_mh;
    sim_stat_workbuf.tht_swy_mh = thty;
      
    
    //補巻
    swx_L = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X][SID_D02];
    swy_L = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y][SID_D02];
    swx_PH = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_X][SID_row2];
    swy_PH = CamPrm.arr[SID_SENSOR1][SID_CAMERA1][SID_PRM_AXIS_Y][SID_row2];

    double Lah = sim_stat_workbuf.lra.p;

    phx = asin(((pLoad2->L.x) * sin_th_sl + (pLoad2->L.y) * -cos_th_sl) / Lah);
    phy = asin(((pLoad2->L.x) * cos_th_sl + (pLoad2->L.y) * sin_th_sl) / Lah);
    dphx = asin(((pLoad2->vL.x) * sin_th_sl + (pLoad2->vL.y) * -cos_th_sl) / Lah);
    dphy = asin(((pLoad2->vL.x) * cos_th_sl + (pLoad2->vL.y) * sin_th_sl) / Lah);
   
    swx_dc = swx_L * sin(swx_PH) + swx_l0 * sin(swx_th0);                              //x0+l0sin(th0)
    swy_dc = swy_L * cos(swy_PH + sim_stat_workbuf.th.p) + swy_l0 * sin(swy_th0);      //y0+l0sin(th0)
    swx_hc = swy_L * sin(swy_PH + sim_stat_workbuf.th.p) + swx_l0 * cos(swx_th0);
    swy_hc = swx_hc;

    double tan_thtx2 = (Lah * sin(phx) - swx_dc) / (Lah * cos(phx) + swx_hc);
    double thtx2 = atan(tan_thtx2) - swx_thc;
    double dthtx2 = thtx2 - sim_stat_workbuf.tht_swx_ah;
    sim_stat_workbuf.tht_swx_ah = thtx2;                 //前回値保持

    double tan_thty2 = (Lah * sin(phy) - swy_dc) / (Lah * cos(phy) + swy_hc);
    double thty2 = atan(tan_thty2) - swy_thc;
    double dthty2 = thty2 - sim_stat_workbuf.tht_swy_ah;
    sim_stat_workbuf.tht_swy_ah = thty2;                 //前回値保持

    
    //メッセージバッファセット

    double swx_C = CamPrm.rad2pix[0][0][SID_PRM_AXIS_X];
    double swy_C = CamPrm.rad2pix[0][0][SID_PRM_AXIS_Y];

    //# ヘッダ情報セット
    //機器ID
    sim_stat_workbuf.rcv_msg.head.id[0] = 'S';
    sim_stat_workbuf.rcv_msg.head.id[1] = 'I';
    sim_stat_workbuf.rcv_msg.head.id[2] = 'M';
    sim_stat_workbuf.rcv_msg.head.id[3] = '1';

    //時間セット
    GetLocalTime(&sim_stat_workbuf.rcv_msg.head.time);

    //# 振れセンサ情報
    //機器個体情報

    INT32 nx = 1440, ny = 1080;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.pix_x       = INT32(nx);            //画素数
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.pix_y       = INT32(ny);            //画素数
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.l0_x        = INT32(swx_l0 * 1000);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.l0_y        = INT32(swy_l0 * 1000);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.ph0_x       = INT32(swx_ph0 * 1000000);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.ph0_y       = INT32(swy_ph0 * 1000000);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.phc_x       = INT32(swx_phc * 1000000);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.phc_y       = INT32(swy_phc * 1000000);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.pixlrad_x   = INT32(swx_C);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.pixlrad_y   = INT32(swy_C);

    //機器状態情報
    sim_stat_workbuf.rcv_msg.body->cam_stat.mode                    = 0x01;     //モード
    sim_stat_workbuf.rcv_msg.body->cam_stat.error                   = 0x33;     //エラーステータスセット      
    sim_stat_workbuf.rcv_msg.body->cam_stat.status                  = 0xf3;     //検出ステータスセット
#if 0
    //傾斜計情報
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_x = (UINT32)(tilt_x * 1000000.0);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_y = (UINT32)(tilt_y * 1000000.0);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_dx = (UINT32)(tilt_dx * 1000000.0);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_dy = (UINT32)(tilt_dy * 1000000.0);

    //振れ検出情報
    //カメラ検出角度pix
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].th_x = (INT32)(thtx * swx_C);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].th_y = (INT32)(thty * swy_C);

    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].th_x = (INT32)(thtx2 * swx_C);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].th_y = (INT32)(thty2 * swy_C);


    //カメラ検出角速度pix
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].dth_x = (INT32)(dthtx * swx_C) * 1000 / SWAY_IF_MIN_SCAN_MS;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].dth_y = (INT32)(dthty * swy_C) * 1000 / SWAY_IF_MIN_SCAN_MS;

    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].dth_x = (INT32)(dthtx2 * swx_C) * 1000 / SWAY_IF_MIN_SCAN_MS;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].dth_y = (INT32)(dthty2 * swy_C) * 1000 / SWAY_IF_MIN_SCAN_MS;
 
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].th_x0 = atan(-swx_dc / swx_hc) - swx_thc;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].th_y0 = atan(-swy_dc / swx_hc) - swy_thc;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].dpx_tgs = 100;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].dpy_tgs = 100;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].tg_size = 1000;

    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].th_x0 = atan(-swx_dc / swx_hc) - swx_thc;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].th_y0 = atan(-swy_dc / swx_hc) - swy_thc;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].dpx_tgs = 80;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].dpy_tgs = 80;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].tg_size = 800;
#endif
    //傾斜計情報
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_x = tilt_x;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_y = tilt_y;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_dx = tilt_dx;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_dy = tilt_dy;

    //振れ検出情報
    //カメラ検出角度pix
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].th_x = thtx * swx_C;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].th_y = thty * swy_C;

    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].th_x = thtx2 * swx_C;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].th_y = thty2 * swy_C;


    //カメラ検出角速度pix
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].dth_x = dthtx * swx_C * 1000 / SWAY_IF_MIN_SCAN_MS;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].dth_y = dthty * swy_C * 1000 / SWAY_IF_MIN_SCAN_MS;

    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].dth_x = dthtx2 * swx_C * 1000 / SWAY_IF_MIN_SCAN_MS;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].dth_y = dthty2 * swy_C * 1000 / SWAY_IF_MIN_SCAN_MS;

    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].th_x0 = (atan(swx_dc / (swx_hc + L)) - swx_thc) * swx_C;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].th_y0 = (atan(swy_dc / (swx_hc + L)) - swy_thc) * swy_C;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].dpx_tgs = 100;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].dpy_tgs = 100;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].tg_size = 1000;
    //振れ中心位置
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].th_x0 = (atan(-swx_dc / (swx_hc + Lah)) - swx_thc) * swx_C;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].th_y0 = (atan(-swy_dc / (swx_hc + Lah)) - swy_thc) * swy_C;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].dpx_tgs = 80;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].dpy_tgs = 80;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].tg_size = 800;
    //文字情報
    sim_stat_workbuf.rcv_msg.body->info[0] = 'S';
    sim_stat_workbuf.rcv_msg.body->info[1] = 'i';
    sim_stat_workbuf.rcv_msg.body->info[2] = 'm';
    sim_stat_workbuf.rcv_msg.body->info[3] = 'I';
    sim_stat_workbuf.rcv_msg.body->info[4] = 'n';
    sim_stat_workbuf.rcv_msg.body->info[5] = 'f';
    sim_stat_workbuf.rcv_msg.body->info[6] = 'o';
    sim_stat_workbuf.rcv_msg.body->info[7] = '\0';


    return 0;
}
