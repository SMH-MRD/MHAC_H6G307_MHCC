#include "CSIM.h"
#include "CWorkWindow_SIM.h"
#include "Spec.h"
#include "CVector3.h"
#include "SIM.h"

extern ST_SPEC def_spec;

CSIM::CSIM() {
    // ���L�������I�u�W�F�N�g�̃C���X�^���X��
    pSimulationStatusObj = new CSharedMem;
    pPLCioObj = new CSharedMem;
    pCraneStatusObj = new CSharedMem;
    pAgentInfObj = new CSharedMem;

    // MOB �I�u�W�F�N�g�̃C���X�^���X��
    pCrane = new CCrane(); //�N���[���̃��f��
    pLoad = new CLoad();   //�ׂ݉̃��f��
    pSIM_work = &sim_stat_workbuf;

    out_size = 0;
 
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
    set_outbuf(pPLCioObj->get_pMap());
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

   //CraneStat�����オ��҂�
    while (pCraneStat->is_tasks_standby_ok ==false) {
        Sleep(10);
    }

    //�N���[���d�l�̃Z�b�g
    pCrane->set_spec(&def_spec);

    //�N���[���̏�����ԃZ�b�g 
    pCrane->init_crane(SYSTEM_TICK_ms / 1000.0);

    //�N���[���I�u�W�F�N�g��PLC_IO�̃|�C���^�n��
    pCrane->set_plc(pPLC);


    //�݉׵�޼ު�Ăɸڰݵ�޼ު�Ă�R�t��
    pLoad->set_crane(pCrane);

    //�ׂ݉̏�����ԃZ�b�g 
    Vector3 _r(SIM_INIT_R * cos(SIM_INIT_TH) + SIM_INIT_X, SIM_INIT_R * sin(SIM_INIT_TH), def_spec.boom_high - SIM_INIT_L);  //�ݓ_�ʒu
    Vector3 _v(0.0, 0.0, 0.0);                          //�ݓ_�ʑ��x
    pLoad->init_mob(SYSTEM_TICK_ms / 1000.0, _r, _v);
    pLoad->set_m(SIM_INIT_M);


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

    //PLC ����
    pCrane->set_v_ref(
        pAgent->v_ref[ID_HOIST],
        pAgent->v_ref[ID_GANTRY],
        pAgent->v_ref[ID_SLEW],
        pAgent->v_ref[ID_BOOM_H]
    );

    //�X�L�����^�C���Z�b�g dt�̓}���`���f�B�A�^�C�}�@�R�[���o�b�N�ŃZ�b�g
    pCrane->set_dt(dt);
    pLoad->set_dt(dt);

    //�ړ��Ɍ����
    for (int i = 0; i < MOTION_ID_MAX;i++) {
        pCrane->is_fwd_endstop[i] = pCraneStat->is_fwd_endstop[i];
        pCrane->is_rev_endstop[i] = pCraneStat->is_rev_endstop[i];
    }


    return 0;
}
//*********************************************************************************************
// parse()
//*********************************************************************************************
static int sim_act_last,wait_count=0;
int CSIM::parse() {


    //************** ���[�h�؂�ւ��������������@**************
    {
        if (sim_act_last != is_sim_active_mode()) wait_count = 100;//PLC IO �X�V�҂��J�E���^�Z�b�g
        else { if (wait_count > 0)wait_count--; }
        //���[�h�؂�ւ���PLC IF�X�V��҂��ď�����
        if (wait_count == 90) {//100msec��
            Sleep(1000);//PLC_IF�̐ؕς��҂�
            if (is_sim_active_mode()) {
                pCrane->set_mode(MOB_MODE_SIM);
                //�ׂ݉̏�����ԃZ�b�g 
                Vector3 _r(SIM_INIT_R * cos(SIM_INIT_TH) + SIM_INIT_X, SIM_INIT_R * sin(SIM_INIT_TH), def_spec.boom_high - SIM_INIT_L);  //�ݓ_�ʒu
                Vector3 _v(0.0, 0.0, 0.0);                          //�ݓ_�ʑ��x
                pCrane->init_crane(dt);
                pLoad->init_mob(dt, _r, _v);
                pLoad->set_m(SIM_INIT_M);
            }
            else {
                pCrane->set_mode(MOB_MODE_PLC);
                //�ׂ݉̏�����ԃZ�b�g 
                Vector3 _r(pPLC->status.pos[ID_BOOM_H] * cos(pPLC->status.pos[ID_SLEW]) + pPLC->status.pos[ID_GANTRY],
                    pPLC->status.pos[ID_BOOM_H] * sin(pPLC->status.pos[ID_SLEW]),
                    pPLC->status.pos[ID_HOIST]);                    //�ݓ_�ʒu

                Vector3 _v(0.0, 0.0, 0.0);                          //�ݓ_���x
                pCrane->init_crane(dt);
                pLoad->init_mob(dt, _r, _v);
                pLoad->set_m(SIM_INIT_M + pPLC->status.weight);     //�݉׏d�Z�b�g
            }
        }
    } //************** ���[�h�؂�ւ��������������@**************
    


     pCrane->update_break_status(); //�u���[�L��ԍX�V
     pCrane->timeEvolution();       //�N���[���̈ʒu,���x�v�Z
     pLoad->timeEvolution();        //�ׂ݉̈ʒu,���x�v�Z
   
     if (sim_act_last != is_sim_active_mode()) {
         pLoad->dr.x = 0.0;pLoad->dr.y = 0.0;pLoad->dr.z = 0.0;
         pLoad->dv.x = 0.0;pLoad->dv.y = 0.0;pLoad->dv.z = 0.0;
     }
     
     pLoad->r.add(pLoad->dr);       //�݉׈ʒu�X�V
     pLoad->v.add(pLoad->dv);       //�݉ב��x�X�V
     pLoad->update_relative_vec();  //�݉גݓ_���΃x�N�g���X�V(���[�v�x�N�g���@L,vL)

     sim_act_last = is_sim_active_mode();


    return 0;
}
//*********************************************************************************************
// output()
//*********************************************************************************************

static int sway_if_scan = SWAY_IF_MIN_SCAN_MS/ SYSTEM_TICK_ms;
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
    sim_stat_workbuf.status.v_fb[ID_HOIST] = pCrane->v0[ID_HOIST];
    sim_stat_workbuf.status.v_fb[ID_GANTRY] = pCrane->v0[ID_GANTRY];
    sim_stat_workbuf.status.v_fb[ID_SLEW] = pCrane->v0[ID_SLEW];
    sim_stat_workbuf.status.v_fb[ID_BOOM_H] = pCrane->v0[ID_BOOM_H];

    sim_stat_workbuf.status.pos[ID_HOIST] = pCrane->r0[ID_HOIST];
    sim_stat_workbuf.status.pos[ID_GANTRY] = pCrane->r0[ID_GANTRY];
    sim_stat_workbuf.status.pos[ID_SLEW] = pCrane->r0[ID_SLEW];
    sim_stat_workbuf.status.pos[ID_BOOM_H] = pCrane->r0[ID_BOOM_H];

    sim_stat_workbuf.L = pLoad->L;
    sim_stat_workbuf.vL = pLoad->vL;

    return 0;
}
//*********************************************************************************************
// output() �U��Z���T�M���Z�b�g
//*********************************************************************************************

static double thcamx_last=0, thcamy_last=0;
static int sim_counter = 0;

int CSIM::set_sway_io() {
      
    // �X�Όv���o�p�x
    sim_counter++;
    double th_tilx = (double)(sim_counter % 1000) * 0.0063;//0.0063 = 2��/1000 10msec scan 10�b����
    double a_til = 0.00173;
    double tilt_x = a_til * sin(th_tilx);
    double tilt_y = 0.0;
    double tilt_dx = a_til * 0.62 * cos(th_tilx);
    double tilt_dy = 0.0;

    
    // �N���[��xy���W���J����xy���W�ɉ�]�ϊ��@���@�p�xrad�ɕϊ��@
    double th = pCrane->r0[ID_SLEW];//����p�x
    double thx = asin(((pLoad->L.x) * sin(th) + (pLoad->L.y) * -cos(th)) / pCrane->l_mh);
    double thy = asin(((pLoad->L.x) * cos(th) + (pLoad->L.y) * sin(th)) / pCrane->l_mh);
    double dthx = asin(((pLoad->vL.x) * sin(th) + (pLoad->vL.y) * -cos(th)) / pCrane->l_mh);
    double dthy = asin(((pLoad->vL.x) * cos(th) + (pLoad->vL.y) * sin(th)) / pCrane->l_mh);

   
    // �J������t�I�t�Z�b�g�l�̌v�Z

    double L = pCraneStat->mh_l;
    double T = pCraneStat->T;
    double w = pCraneStat->w;

    double swx_L0 = pCraneStat->spec.SwayCamParam[0][0][0][SID_L0];
    double swy_L0 = pCraneStat->spec.SwayCamParam[0][0][1][SID_L0];

    double swx_PH0 = pCraneStat->spec.SwayCamParam[0][0][0][SID_PH0];
    double swy_PH0 = pCraneStat->spec.SwayCamParam[0][0][1][SID_PH0];

    double swx_l0 = pCraneStat->spec.SwayCamParam[0][0][0][SID_l0];
    double swy_l0 = pCraneStat->spec.SwayCamParam[0][0][1][SID_l0];

    double swx_ph0 = pCraneStat->spec.SwayCamParam[0][0][0][SID_ph0];
    double swy_ph0 = pCraneStat->spec.SwayCamParam[0][0][1][SID_ph0];

    double swx_phc = pCraneStat->spec.SwayCamParam[0][0][0][SID_phc];
    double swy_phc = pCraneStat->spec.SwayCamParam[0][0][1][SID_phc];

    double swx_C = pCraneStat->spec.SwayCamParam[0][0][0][SID_PIXlRAD];
    double swy_C = pCraneStat->spec.SwayCamParam[0][0][1][SID_PIXlRAD];
    
    double xrx = swx_L0 * sin(swx_PH0) + swx_l0 * sin(swx_ph0 + tilt_x);
    double yrx = swx_L0 * cos(swx_PH0) + swx_l0 * cos(swx_ph0 + tilt_x);
    double xry = swy_L0 * sin(swy_PH0) + swy_l0 * sin(swy_ph0 + tilt_y);
    double yry = swy_L0 * cos(swy_PH0) + swy_l0 * cos(swy_ph0 + tilt_y);
    double tan_thx = (L * sin(thx) - xrx) / (L * cos(thx) - yrx);
    double tan_thy = (L * sin(thy) - xry) / (L * cos(thy) - yry);

    double thx_ = atan(tan_thx); //�Ɓ���t+��c�{��t�{��0
    double thy_ = atan(tan_thy);
    double dthx_ = dthx / (1.0 - (yrx + xrx * tan_thx) / L / (cos(thx) + sin(thx) * tan_thx));
    double dthy_ = dthy / (1.0 - (yry + xry * tan_thy) / L / (cos(thy) + sin(thy) * tan_thy));

    //���b�Z�[�W�o�b�t�@�Z�b�g

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

    INT32 nx = 1024, ny = 768;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.pix_x       = INT32(nx);     //��f��
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.pix_y       = INT32(ny);      //��f��
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.l0_x        = INT32(swx_l0*1000);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_spec.l0_y        = INT32(swy_l0*1000);
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

    //�X�Όv���
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_x = (UINT32)(tilt_x * 1000000.0);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_y = (UINT32)(tilt_y * 1000000.0);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_dx = (UINT32)(tilt_dx * 1000000.0);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].cam_stat.tilt_dy = (UINT32)(tilt_dy * 1000000.0);

    //�U�ꌟ�o���
    //�J�������o�p�xpix
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].th_x = (INT32)((thx_ - tilt_x - swx_ph0 - swx_phc) * swx_C);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].th_y = (INT32)((thy_ - tilt_y - swy_ph0 - swy_phc) * swy_C);
    //�J�������o�p���xpix
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].dth_x = (INT32)((dthx_ - tilt_dx) * swx_C);
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].dth_y = (INT32)((dthy_ - tilt_dy) * swy_C);
 
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].th_x0 = nx/2;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].th_y0 = ny/2;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].dpx_tgs = 100;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].dpy_tgs = 100;
    sim_stat_workbuf.rcv_msg.body[SID_CAMERA1].tg_stat[SWAY_SENSOR_TG1].tg_size = 1000;

    //�������
    sim_stat_workbuf.rcv_msg.body->info[0] = 'S';
    sim_stat_workbuf.rcv_msg.body->info[1] = 'i';
    sim_stat_workbuf.rcv_msg.body->info[2] = 'm';
    sim_stat_workbuf.rcv_msg.body->info[3] = 'I';
    sim_stat_workbuf.rcv_msg.body->info[4] = 'n';
    sim_stat_workbuf.rcv_msg.body->info[5] = 'f';
    sim_stat_workbuf.rcv_msg.body->info[6] = 'o';
    sim_stat_workbuf.rcv_msg.body->info[7] = '\0';

    //�V�~�����[�^���W�b�N�`�F�b�N�p�o�b�t�@�Z�b�g�iMON�ɃV�~�����[�^�̌��f�[�^�\���p�j
    sim_stat_workbuf.sway_io.th[ID_SLEW] = thx;
    sim_stat_workbuf.sway_io.th[ID_BOOM_H] = thy;
    sim_stat_workbuf.sway_io.dth[ID_SLEW] = dthx;
    sim_stat_workbuf.sway_io.dth[ID_BOOM_H] = dthy;


#if 0

        swx.L0 = pCraneStat->spec.SwayCamParam[0][0][0][SID_L0];
    swy.L0 = pCraneStat->spec.SwayCamParam[0][0][1][SID_L0];

    swx.PH0 = pCraneStat->spec.SwayCamParam[0][0][0][SID_PH0];
    swy.PH0 = pCraneStat->spec.SwayCamParam[0][0][1][SID_PH0];

    swx.l0 = pCraneStat->spec.SwayCamParam[0][0][0][SID_l0];
    swy.l0 = pCraneStat->spec.SwayCamParam[0][0][1][SID_l0];

    swx.ph0 = pCraneStat->spec.SwayCamParam[0][0][0][SID_ph0];
    swy.ph0 = pCraneStat->spec.SwayCamParam[0][0][1][SID_ph0];

    swx.phc = pCraneStat->spec.SwayCamParam[0][0][0][SID_phc];
    swy.phc = pCraneStat->spec.SwayCamParam[0][0][1][SID_phc];

    swx.C = 1.0 / pCraneStat->spec.SwayCamParam[0][0][0][SID_PIXlRAD];
    swy.C = 1.0 / pCraneStat->spec.SwayCamParam[0][0][1][SID_PIXlRAD];

    double phx = tilt_x + cx;
    double phy = tilt_y + cy;
    double offset_thx = asin(ax * sin(phx + bx) / L);
    double offset_thy = asin(ay * sin(phy + by) / L);

 
    //�J�������o�p�xrad
    double th_camx = thx - offset_thx - phx;
    double th_camy = thy - offset_thy - phy;
    //�J�������o�p���xrad
    double dth_camx = (th_camx - thcamx_last) / pCrane->dt;
    double dth_camy = (th_camy - thcamy_last) / pCrane->dt;

    thcamx_last = th_camx;
    thcamy_last = th_camy;


#endif     
    return 0;
}
