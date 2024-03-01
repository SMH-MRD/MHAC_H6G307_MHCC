#include "CSIM.h"
#include "CWorkWindow_SIM.h"
#include "Spec.h"
#include "CVector3.h"
#include "SIM.h"

extern ST_SPEC def_spec;

static ST_SWY_SENS_PRM CamPrm;

CSIM::CSIM() {
    // ���L�������I�u�W�F�N�g�̃C���X�^���X��
    pSimulationStatusObj    = new CSharedMem;
    pPLCioObj               = new CSharedMem;
    pCraneStatusObj         = new CSharedMem;
    pAgentInfObj            = new CSharedMem;

    // MOB �I�u�W�F�N�g�̃C���X�^���X��
    pCrane      = new CJC();     //�N���[���̃��f��
    pLoad       = new CLoad();   //�努�ׂ݉̃��f��
    pLoad2      = new CLoad();   //�⊪�ׂ݉̃��f��
    pSIM_work   = &sim_stat_workbuf;

    out_size    = 0;
 
    memset(&sim_stat_workbuf, 0, sizeof(ST_SIMULATION_STATUS));   //���L�������ւ̏o�̓Z�b�g��Ɨp�o�b�t�@
};
CSIM::~CSIM() {
    // ���L�������I�u�W�F�N�g�̉��
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
};      //�o�̓o�b�t�@�Z�b�g

//******************************************************************************************
// init_proc()
//******************************************************************************************
int CSIM::init_proc() {
     // ���L�������擾
     // �o�͗p���L�������擾
    out_size = sizeof(ST_SIMULATION_STATUS);
    if (OK_SHMEM != pSimulationStatusObj->create_smem(SMEM_SIMULATION_STATUS_NAME, (DWORD)out_size, MUTEX_SIMULATION_STATUS_NAME)) {
        mode |= SIM_IF_SIM_MEM_NG;
    }

    // ���͗p���L�������擾
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

   //CraneStat�����オ��҂�
    while (pCraneStat->is_crane_status_ok !=true) {
        Sleep(100);
    }
    //PLC IF�����オ��҂�
    while (pPLC->healthy_cnt < 10) {
        Sleep(100);
    }

    //�N���[���d�l�̃Z�b�g
    pCrane->pPLC = pPLC;
    pCrane->pCraneStat = pCraneStat;
    pCrane->pSimStat = &sim_stat_workbuf;
    pCrane->set_spec(&def_spec);

    //�N���[���̏�����ԃZ�b�g 
    pCrane->init_crane(SYSTEM_TICK_ms / 1000.0);

    //�݉׵�޼ު�Ăɸڰݵ�޼ު�Ă�R�t��
    pLoad->set_crane(pCrane);
    pLoad2->set_crane(pCrane);

    //�ׂ݉̏�����ԃZ�b�g 
    Vector3 _r;
    _r.x = pCrane->r.x , _r.y = pCrane->r.y, _r.z = pCrane->r0[ID_HOIST];    //�努�݉׈ʒu
    Vector3 _v(0.0, 0.0, 0.0);                                               //�݉ב��x
   
    //�努
    pLoad->init_mob(SYSTEM_TICK_ms / 1000.0, _r, _v);
    pLoad->set_m(def_spec.Load0_mh);
    pLoad->set_type(ID_HOIST);

    //�⊪
    double th0 = acos(SIM_INIT_MHR / def_spec.Lm);
    double ar0 = def_spec.La * cos(th0 - def_spec.rad_Lm_La);
    _r.x = ar0 * cos(SIM_INIT_TH) + SIM_INIT_X; _r.y = ar0 * sin(SIM_INIT_TH); _r.z = pCrane->r0[ID_AHOIST];
  
    pLoad2->init_mob(SYSTEM_TICK_ms / 1000.0, _r, _v);
    pLoad2->set_m(def_spec.Load0_ah);
    pLoad2->set_type(ID_AHOIST);

    //�U��p�v�Z�p�J�����p�����[�^�Z�b�g
  
    return int(mode & 0xff00);
}
//*********************************************************************************************
// input()
//*********************************************************************************************
int CSIM::input() {
    sim_stat_workbuf.helthy_cnt++;

    //MAIN�v���Z�X(Environment�^�X�N�̃w���V�[�M����荞�݁j
    source_counter = pCraneStat->env_act_count;

    //PLC �w�߂�100%��1.0�@�h������]���x�w�߂ŃZ�b�g
    pCrane->set_v_ref(pPLC->nv_ref[ID_HOIST], pPLC->nv_ref[ID_GANTRY], pPLC->nv_ref[ID_SLEW], pPLC->nv_ref[ID_BOOM_H], pPLC->nv_ref[ID_AHOIST]);
     //�X�L�����^�C���Z�b�g dt�̓}���`���f�B�A�^�C�}�@�R�[���o�b�N�ŃZ�b�g
    pCrane->set_dt(dt);
    pLoad->set_dt(dt);
    pLoad2->set_dt(dt);

    //�ړ��Ɍ����
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

     pCrane->update_break_status(); //�u���[�L��ԍX�V

     pCrane->timeEvolution();       //�N���[���̈ʒu,���x�v�Z
     pLoad->timeEvolution();        //�ׂ݉̈ʒu,���x�v�Z
     pLoad2->timeEvolution();       //�ׂ݉̈ʒu,���x�v�Z
       
     pLoad->r.add(pLoad->dr);       //�݉׈ʒu�X�V
     pLoad2->r.add(pLoad2->dr);       //�݉׈ʒu�X�V

     pLoad->v.add(pLoad->dv);       //�݉ב��x�X�V
     pLoad2->v.add(pLoad2->dv);       //�݉ב��x�X�V

     pLoad->update_relative_vec();  //�݉גݓ_���΃x�N�g���X�V(���[�v�x�N�g���@L,vL)
     pLoad2->update_relative_vec();  //�݉גݓ_���΃x�N�g���X�V(���[�v�x�N�g���@L,vL)

    return 0;
}
//*********************************************************************************************
// output()
//*********************************************************************************************

static int sway_if_scan = SWAY_IF_MIN_SCAN_MS/ SYSTEM_TICK_ms;//�U��Z���T���M�^�C�~���O�J�E���^
int CSIM::output() {

    sim_stat_workbuf.mode = this->mode;                         //���[�h�Z�b�g
    sim_stat_workbuf.helthy_cnt = my_helthy_counter++;          //�w���V�[�J�E���^�Z�b�g

    set_cran_motion();  //�N���[���̈ʒu�A���x���Z�b�g

    if(my_helthy_counter% sway_if_scan == 0) set_sway_io();    //�U��Z���TIO���Z�b�g
    
    
    if (out_size) { //�o�͏���
        memcpy_s(poutput, out_size, &sim_stat_workbuf, out_size);
    }

    return 0;
}
//*********************************************************************************************
// set_cran_motion() �N���[���ʒu�A���x���Z�b�g
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
// output() �U��Z���T�M���Z�b�g
//*********************************************************************************************

static double thcamx_last=0, thcamy_last=0;
static int sim_counter = 0;

int CSIM::set_sway_io() {
      
    // �X�Όv���o�p�x�͋[�@�Ƃ肠���������g��
    sim_counter++;
    double th_tilx = (double)(sim_counter % 1000) * 0.0063;//0.0063 = 2��/1000 10msec scan 10�b����
    double a_til = 0.001745;//�X�Ίp�U�� 0.1deg�@1deg 0.01745rad
    double tilt_x = 0.0;
    //double tilt_y = a_til * sin(th_tilx);
    double tilt_y = 0.0;
    double tilt_dx = 0.0;
    double tilt_dy = 0.0;//�ց�2��/10=0.63
    //double tilt_dy = a_til * 0.63 * cos(th_tilx);//�ց�2��/10=0.63

    // �N���[��xy���W���J����xy���W�ɉ�]�ϊ��@���@�p�xrad�ɕϊ��@
 
    double sin_th_sl = sin(pCrane->r0[ID_SLEW]);//sin(th_sl)
    double cos_th_sl = cos(pCrane->r0[ID_SLEW]);//cos(th_sl)
 
    //�努
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
      
    
    //�⊪
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
    sim_stat_workbuf.tht_swx_ah = thtx2;                 //�O��l�ێ�

    double tan_thty2 = (Lah * sin(phy) - swy_dc) / (Lah * cos(phy) + swy_hc);
    double thty2 = atan(tan_thty2) - swy_thc;
    double dthty2 = thty2 - sim_stat_workbuf.tht_swy_ah;
    sim_stat_workbuf.tht_swy_ah = thty2;                 //�O��l�ێ�

    
    //���b�Z�[�W�o�b�t�@�Z�b�g

    double swx_C = CamPrm.rad2pix[0][0][SID_PRM_AXIS_X];
    double swy_C = CamPrm.rad2pix[0][0][SID_PRM_AXIS_Y];

    //# �w�b�_���Z�b�g
    //�@��ID
    sim_stat_workbuf.rcv_msg.head.id[0] = 'S';
    sim_stat_workbuf.rcv_msg.head.id[1] = 'I';
    sim_stat_workbuf.rcv_msg.head.id[2] = 'M';
    sim_stat_workbuf.rcv_msg.head.id[3] = '1';

    //���ԃZ�b�g
    GetLocalTime(&sim_stat_workbuf.rcv_msg.head.time);

    //# �U��Z���T���
    //�@��̏��

    INT32 nx = 1440, ny = 1080;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.pix_x       = INT32(nx);            //��f��
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.pix_y       = INT32(ny);            //��f��
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.l0_x        = INT32(swx_l0 * 1000);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.l0_y        = INT32(swy_l0 * 1000);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.ph0_x       = INT32(swx_ph0 * 1000000);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.ph0_y       = INT32(swy_ph0 * 1000000);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.phc_x       = INT32(swx_phc * 1000000);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.phc_y       = INT32(swy_phc * 1000000);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.pixlrad_x   = INT32(swx_C);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.pixlrad_y   = INT32(swy_C);

    //�@���ԏ��
    sim_stat_workbuf.rcv_msg.body->cam_stat.mode                    = 0x01;     //���[�h
    sim_stat_workbuf.rcv_msg.body->cam_stat.error                   = 0x33;     //�G���[�X�e�[�^�X�Z�b�g      
    sim_stat_workbuf.rcv_msg.body->cam_stat.status                  = 0xf3;     //���o�X�e�[�^�X�Z�b�g
#if 0
    //�X�Όv���
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_x = (UINT32)(tilt_x * 1000000.0);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_y = (UINT32)(tilt_y * 1000000.0);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_dx = (UINT32)(tilt_dx * 1000000.0);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_dy = (UINT32)(tilt_dy * 1000000.0);

    //�U�ꌟ�o���
    //�J�������o�p�xpix
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].th_x = (INT32)(thtx * swx_C);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].th_y = (INT32)(thty * swy_C);

    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].th_x = (INT32)(thtx2 * swx_C);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].th_y = (INT32)(thty2 * swy_C);


    //�J�������o�p���xpix
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
    //�X�Όv���
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_x = tilt_x;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_y = tilt_y;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_dx = tilt_dx;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_dy = tilt_dy;

    //�U�ꌟ�o���
    //�J�������o�p�xpix
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].th_x = thtx * swx_C;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].th_y = thty * swy_C;

    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].th_x = thtx2 * swx_C;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].th_y = thty2 * swy_C;


    //�J�������o�p���xpix
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].dth_x = dthtx * swx_C * 1000 / SWAY_IF_MIN_SCAN_MS;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].dth_y = dthty * swy_C * 1000 / SWAY_IF_MIN_SCAN_MS;

    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].dth_x = dthtx2 * swx_C * 1000 / SWAY_IF_MIN_SCAN_MS;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].dth_y = dthty2 * swy_C * 1000 / SWAY_IF_MIN_SCAN_MS;

    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].th_x0 = (atan(swx_dc / (swx_hc + L)) - swx_thc) * swx_C;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].th_y0 = (atan(swy_dc / (swx_hc + L)) - swy_thc) * swy_C;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].dpx_tgs = 100;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].dpy_tgs = 100;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].tg_size = 1000;
    //�U�ꒆ�S�ʒu
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].th_x0 = (atan(-swx_dc / (swx_hc + Lah)) - swx_thc) * swx_C;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].th_y0 = (atan(-swy_dc / (swx_hc + Lah)) - swy_thc) * swy_C;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].dpx_tgs = 80;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].dpy_tgs = 80;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG2].tg_size = 800;
    //�������
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
