#include "CPLC_IF.h"
#include "PLC_IF.h"
#include "PLC_IO_DEF.h"
#include "CWorkWindow_PLC.h"
#include <windows.h>
#include "Mdfunc.h"

extern ST_SPEC def_spec;
extern ST_KNL_MANAGE_SET    knl_manage_set;

CPLC_IF::CPLC_IF() {
    // ���L�������I�u�W�F�N�g�̃C���X�^���X��
    pPLCioObj = new CSharedMem;
    pCraneStatusObj = new CSharedMem;
    pSimulationStatusObj = new CSharedMem;
    pAgentInfObj = new CSharedMem;
    pCSInfObj = new CSharedMem;

    out_size = 0;
    memset(&melnet,0,sizeof(ST_MELSEC_NET)) ;      //PLC�����N�\����
    memset(&plc_io_workbuf,0,sizeof(ST_PLC_IO));   //���L�������ւ̏o�̓Z�b�g��Ɨp�o�b�t�@

    melnet.chan = MELSEC_NET_CH;
    melnet.mode = 0;
    melnet.path = NULL;
    melnet.err = 0;
    melnet.status = 0;
    melnet.retry_cnt = MELSEC_NET_RETRY_CNT;
    melnet.read_size_b = sizeof(ST_PLC_READ_B);                         //PLC��LW��bit�ŃZ�b�g�����LB�͖��g�p
    melnet.read_size_w = sizeof(ST_PLC_READ_W);
    melnet.write_size_b = sizeof(ST_PLC_WRITE_B);
    melnet.write_size_w = sizeof(ST_PLC_WRITE_W);

};
CPLC_IF::~CPLC_IF() {
    // ���L�������I�u�W�F�N�g�̉��
    delete pPLCioObj;
    delete pCraneStatusObj;
    delete pSimulationStatusObj;
    delete pAgentInfObj;
    delete pCSInfObj;
};

int CPLC_IF::set_outbuf(LPVOID pbuf) {
    poutput = pbuf;return 0;
};      //�o�̓o�b�t�@�Z�b�g

//******************************************************************************************
// init_proc()
//******************************************************************************************
int CPLC_IF::init_proc() {

    // ���L�������擾

     // �o�͗p���L�������擾
    out_size = sizeof(ST_PLC_IO);
    if (OK_SHMEM != pPLCioObj->create_smem(SMEM_PLC_IO_NAME, (DWORD)out_size, MUTEX_PLC_IO_NAME)) {
        mode |= PLC_IF_PLC_IO_MEM_NG;
    }
    set_outbuf(pPLCioObj->get_pMap());
 
    // ���͗p���L�������擾
    if (OK_SHMEM != pSimulationStatusObj->create_smem(SMEM_SIMULATION_STATUS_NAME, sizeof(ST_SIMULATION_STATUS), MUTEX_SIMULATION_STATUS_NAME)) {
        mode |= PLC_IF_SIM_MEM_NG;
    }
    pSim = (LPST_SIMULATION_STATUS)pSimulationStatusObj->get_pMap();                        //�V�~�����[�^���L������

    if (OK_SHMEM != pCraneStatusObj->create_smem(SMEM_CRANE_STATUS_NAME, sizeof(ST_CRANE_STATUS), MUTEX_CRANE_STATUS_NAME)) {
        mode |= PLC_IF_CRANE_MEM_NG;
    }
    pCrane = (LPST_CRANE_STATUS)pCraneStatusObj->get_pMap();                                //�N���[���X�e�[�^�X���L������

    if (OK_SHMEM != pAgentInfObj->create_smem(SMEM_AGENT_INFO_NAME, sizeof(ST_AGENT_INFO), MUTEX_AGENT_INFO_NAME)){
        mode |= PLC_IF_AGENT_MEM_NG;
    }
    pAgentInf = (LPST_AGENT_INFO)pAgentInfObj->get_pMap();                                  //Agent�R���g���[�����L������

    if (OK_SHMEM != pCSInfObj->create_smem(SMEM_CS_INFO_NAME, sizeof(ST_CS_INFO), MUTEX_CS_INFO_NAME)) {
        mode |= PLC_IF_CS_MEM_NG;
    }

    pCSInf = (LPST_CS_INFO)pCSInfObj->get_pMap();                                           //CS���L������

    for (int i = 0;i < 4;i++) {
        this->melnet.is_force_set_active[i] = false;
        this->melnet.forced_dat[i] = 0;
        this->melnet.forced_index[i] = 0;
    }

    //PLC�A�h���X�}�b�v�Z�b�g
    ST_PLC_OUT_BMAP plc_out_b_map;
    melnet.plc_b_map = plc_out_b_map;
    ST_PLC_OUT_WMAP plc_out_w_map;
    melnet.plc_w_map = plc_out_w_map;
    ST_PC_OUT_BMAP pc_out_b_map;
    melnet.pc_b_map = pc_out_b_map;
    ST_PC_OUT_WMAP pc_out_w_map;
    melnet.pc_w_map = pc_out_w_map;

#if 0
    //CraneStat�����オ��҂�
    while (pCrane->is_tasks_standby_ok == false) {
        Sleep(10);
    }
#endif

    return int(mode & 0xff00);
}
//*********************************************************************************************
// input()
//*********************************************************************************************
int CPLC_IF::input() {
    
    plc_io_workbuf.helthy_cnt++;

        //PLC ���͏���
        //MELSECNET����m�F
    if ((!melnet.status) && (!(plc_io_workbuf.helthy_cnt % melnet.retry_cnt))) {
        if (!(melnet.err = mdOpen(melnet.chan, melnet.chan, &melnet.path)))
            melnet.status = MELSEC_NET_OK;
    }
        //PLC Read
    if (melnet.status == MELSEC_NET_OK) {
        //LB�ǂݍ��݁@����

        //B�ǂݍ���
        melnet.err = mdReceiveEx(melnet.path,   //�`���l���̃p�X
            MELSEC_NET_NW_NO,                   //�l�b�g���[�N�ԍ�           
            MELSEC_NET_SOURCE_STATION,          //�ǔ�
            MELSEC_NET_CODE_LB,                 //�f�o�C�X�^�C�v
            MELSEC_NET_B_READ_START,            //�擪�f�o�C�X
            &melnet.read_size_b,                //�ǂݍ��݃o�C�g�T�C�Y
            melnet.plc_b_out);                   //�ǂݍ��݃o�b�t�@

         //W�ǂݍ���
        melnet.err = mdReceiveEx(melnet.path,    //�`���l���̃p�X
            MELSEC_NET_NW_NO,                   //�l�b�g���[�N�ԍ�      
            MELSEC_NET_SOURCE_STATION,          //�ǔ�
            MELSEC_NET_CODE_LW,                 //�f�o�C�X�^�C�v
            MELSEC_NET_W_READ_START,            //�擪�f�o�C�X
            &melnet.read_size_w,                //�ǂݍ��݃o�C�g�T�C�Y
            melnet.plc_w_out);     //�ǂݍ��݃o�b�t�@
        if (melnet.err != 0)melnet.status = MELSEC_NET_RECEIVE_ERR;
    }
    //�����Z�b�g
    if (melnet.is_force_set_active[MEL_FORCE_PLC_B])melnet.plc_b_out[melnet.forced_index[MEL_FORCE_PLC_B]] = melnet.forced_dat[MEL_FORCE_PLC_B];
    if (melnet.is_force_set_active[MEL_FORCE_PLC_W])melnet.plc_w_out[melnet.forced_index[MEL_FORCE_PLC_W]] = melnet.forced_dat[MEL_FORCE_PLC_W];
      
    //MAIN�v���Z�X(Environment�^�X�N�̃w���V�[�M����荞�݁j
    source_counter = pCrane->env_act_count;

     return 0;
}
//*********************************************************************************************
// parse()
//*********************************************************************************************
int CPLC_IF::parse() { 

    CWorkWindow_PLC* pWorkWindow;

    //### �w���V�[�M��
    helthy_cnt++;

    //### PLC�����N���͂����
    parse_notch_com();
    

    //�^�]��������e 
    //PLC IF����̓��͐M����荞�݁iplc_io_workbuf)
    parse_ope_com();
    //�f�o�b�O���[�h�̎��A�f�o�b�O�p����E�B���h����̓��͒l�ŏ㏑��
    if( is_debug_mode() && (pWorkWindow->stOpePaneStat.chk_input_disable == FALSE)) set_debug_status();
   
    //### �Z���T���o���e�捞
    //PLC IF����̓��͐M����荞�݁iplc_io_workbuf)
    parse_sensor_fb();
    //�V�~�����[�V�������[�h���V�~�����[�V�����̌��ʂŏ㏑��
#ifdef _DVELOPMENT_MODE
    if (pSim->mode & SIM_ACTIVE_MODE) {
        set_sim_status();
    }
#endif

    //�u���[�L��Ԏ�荞��
    parce_brk_status();

    //### PLC�ւ̏o�͐M���o�b�t�@�Z�b�g
    
    set_notch_ref();  //�m�b�`�o�͐M���Z�b�g
    set_bit_coms();   //�r�b�g�o�͐M���Z�b�g
    set_ao_coms();    //�A�i���O�o�͐M���Z�b�g

    return 0; 
}
//*********************************************************************************************
// output()
//*********************************************************************************************
int CPLC_IF::output() { 
 
    plc_io_workbuf.mode = this->mode;                   //���[�h�Z�b�g
    plc_io_workbuf.helthy_cnt = my_helthy_counter++;    //�w���V�[�J�E���^�Z�b�g
    
    //���L�������o�͏���
    if(out_size) { 
        memcpy_s(poutput, out_size, &plc_io_workbuf, out_size);
    }
 
    //�����Z�b�g
    if (melnet.is_force_set_active[MEL_FORCE_PC_B])melnet.pc_b_out[melnet.forced_index[MEL_FORCE_PC_B]] = melnet.forced_dat[MEL_FORCE_PC_B];
    if (melnet.is_force_set_active[MEL_FORCE_PC_W])melnet.pc_w_out[melnet.forced_index[MEL_FORCE_PC_W]] = melnet.forced_dat[MEL_FORCE_PC_W];
  
    if (melnet.is_forced_pc_ctrl) melnet.pc_b_out[melnet.pc_b_map.com_pc_ctr_act[ID_WPOS]] |= melnet.pc_b_map.com_pc_ctr_act[ID_BPOS];
    if (melnet.is_forced_emulate) melnet.pc_b_out[melnet.pc_b_map.com_plc_emulate_act[ID_WPOS]] |= melnet.pc_b_map.com_plc_emulate_act[ID_BPOS];
   


    //MELSECNET�ւ̏o�͏���
    if (melnet.status == MELSEC_NET_OK) {
        //LB��������
 //       melnet.pc_b_out[15] = 0xaaaa;
        melnet.err = mdSendEx(melnet.path,  //�`���l���̃p�X
            MELSEC_NET_MY_NW_NO,            //�l�b�g���[�N�ԍ�   
            MELSEC_NET_MY_STATION,          //�ǔ�
            MELSEC_NET_CODE_LB,             //�f�o�C�X�^�C�v
            MELSEC_NET_B_WRITE_START,       //�擪�f�o�C�X
            &melnet.write_size_b,           //�������݃o�C�g�T�C�Y
            melnet.pc_b_out); //�\�[�X�o�b�t�@
        //LW��������
 //       melnet.pc_w_out[79] = 0xbbbb;
        melnet.err = mdSendEx(melnet.path,  //�`���l���̃p�X
            MELSEC_NET_MY_NW_NO,            //�l�b�g���[�N�ԍ�  
            MELSEC_NET_MY_STATION,          //�ǔ�
            MELSEC_NET_CODE_LW,             //�f�o�C�X�^�C�v
            MELSEC_NET_W_WRITE_START,       //�擪�f�o�C�X
            &melnet.write_size_w,           //�������݃o�C�g�T�C�Y
            melnet.pc_w_out); //�\�[�X�o�b�t�@

        if (melnet.err < 0)melnet.status = MELSEC_NET_SEND_ERR;
    }

    return 0;
}
//*********************************************************************************************
// set_debug_status()
// �f�o�b�O���[�h�i�f�o�b�O�p����E�B���h�j���͓��e�Z�b�g
//*********************************************************************************************
int CPLC_IF::set_debug_status() {
    
    CWorkWindow_PLC* pWorkWindow;

    plc_io_workbuf.ui.notch_pos[ID_HOIST]       = pWorkWindow->stOpePaneStat.slider_mh - MH_SLIDAR_0_NOTCH;
    plc_io_workbuf.ui.notch_pos[ID_GANTRY]      = pWorkWindow->stOpePaneStat.slider_gt - GT_SLIDAR_0_NOTCH;
    plc_io_workbuf.ui.notch_pos[ID_BOOM_H]      = pWorkWindow->stOpePaneStat.slider_bh - BH_SLIDAR_0_NOTCH;
    plc_io_workbuf.ui.notch_pos[ID_SLEW]        = -(pWorkWindow->stOpePaneStat.slider_slew - SLW_SLIDAR_0_NOTCH);//���@�ƃm�b�`�́{�������t

    plc_io_workbuf.ui.PB[ID_PB_ESTOP] = pWorkWindow->stOpePaneStat.check_estop;

    plc_io_workbuf.ui.PB[ID_PB_ANTISWAY_ON]     = pWorkWindow->stOpePaneStat.button_antisway;
    plc_io_workbuf.ui.PB[ID_PB_REMOTE_MODE]     = pWorkWindow->stOpePaneStat.check_remote_mode;
    plc_io_workbuf.ui.PB[ID_PB_AUTO_START]      = pWorkWindow->stOpePaneStat.check_auto_start;
    plc_io_workbuf.ui.PB[ID_PB_AUTO_RESET]      = pWorkWindow->stOpePaneStat.button_auto_reset;
    plc_io_workbuf.ui.PB[ID_PB_AUTO_MODE]       = pWorkWindow->stOpePaneStat.button_auto_mode;
    plc_io_workbuf.ui.PB[ID_PB_AUTO_SET_Z]      = pWorkWindow->stOpePaneStat.button_set_z;
    plc_io_workbuf.ui.PB[ID_PB_AUTO_SET_XY]     = pWorkWindow->stOpePaneStat.button_set_xy;


    plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_S1]  = pWorkWindow->stOpePaneStat.check_s1;
    plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_S2]  = pWorkWindow->stOpePaneStat.check_s2;
    plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_S3]  = pWorkWindow->stOpePaneStat.check_s3;
    plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_L1]  = pWorkWindow->stOpePaneStat.check_l1;
    plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_L2]  = pWorkWindow->stOpePaneStat.check_l2;
    plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_L3]  = pWorkWindow->stOpePaneStat.check_l3;

    
    plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE_ON]  = pWorkWindow->stOpePaneStat.check_source1_on;
    plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE_OFF] = pWorkWindow->stOpePaneStat.check_source1_off;
    plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE2_ON] = pWorkWindow->stOpePaneStat.check_source2_on;
    plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE2_OFF]= pWorkWindow->stOpePaneStat.check_source2_off;

    plc_io_workbuf.ui.PB[ID_PB_FAULT_RESET]     = pWorkWindow->stOpePaneStat.button_fault_reset;

    plc_io_workbuf.ui.PB[ID_PB_MH_P1]           = pWorkWindow->stOpePaneStat.button_mh_p1;
    plc_io_workbuf.ui.PB[ID_PB_MH_P2]           = pWorkWindow->stOpePaneStat.button_mh_p2;
    plc_io_workbuf.ui.PB[ID_PB_MH_M1]           = pWorkWindow->stOpePaneStat.button_mh_m1;
    plc_io_workbuf.ui.PB[ID_PB_MH_M2]           = pWorkWindow->stOpePaneStat.button_mh_m2;
    plc_io_workbuf.ui.PB[ID_PB_SL_P1]           = pWorkWindow->stOpePaneStat.button_sl_p1;
    plc_io_workbuf.ui.PB[ID_PB_SL_P2]           = pWorkWindow->stOpePaneStat.button_sl_p2;
    plc_io_workbuf.ui.PB[ID_PB_SL_M1]           = pWorkWindow->stOpePaneStat.button_sl_m1;
    plc_io_workbuf.ui.PB[ID_PB_SL_M2]           = pWorkWindow->stOpePaneStat.button_sl_m2;
    plc_io_workbuf.ui.PB[ID_PB_BH_P1]           = pWorkWindow->stOpePaneStat.button_bh_p1;
    plc_io_workbuf.ui.PB[ID_PB_BH_P2]           = pWorkWindow->stOpePaneStat.button_bh_p2;
    plc_io_workbuf.ui.PB[ID_PB_BH_M1]           = pWorkWindow->stOpePaneStat.button_bh_m1;
    plc_io_workbuf.ui.PB[ID_PB_BH_M2]           = pWorkWindow->stOpePaneStat.button_bh_m2;

    plc_io_workbuf.ui.PB[ID_PB_PARK]            = pWorkWindow->stOpePaneStat.button_park;
    plc_io_workbuf.ui.PB[ID_PB_GRND]            = pWorkWindow->stOpePaneStat.button_grnd;
    plc_io_workbuf.ui.PB[ID_PB_PICK]            = pWorkWindow->stOpePaneStat.button_pick;

    return 0;
}


//*********************************************************************************************
// closeIF()
//*********************************************************************************************
int CPLC_IF::closeIF() {

   //MELSECNET����N���[�Y
        melnet.err = mdClose(melnet.path);
        melnet.status = MELSEC_NET_CLOSE;
   return 0;
}

//*********************************************************************************************
// set_notch_ref()
// AGENT�^�X�N�̑��x�w�߂��m�b�`�ʒu�w�߂ɕϊ�����IO�o�͂�ݒ�
//*********************************************************************************************
int CPLC_IF::set_notch_ref() {

    //���m�b�`
    //�m�b�`�N���A
    melnet.pc_b_out[melnet.pc_b_map.com_hst_notch_0[ID_WPOS]] &= NOTCH_PTN0_CLR;
        
    if ((pAgentInf->v_ref[ID_HOIST] < (def_spec.notch_spd_r[ID_HOIST][NOTCH_1]))             //�w�߂�-1�m�b�`��菬
        || (pAgentInf->v_ref[ID_HOIST] > (def_spec.notch_spd_f[ID_HOIST][NOTCH_1]))) {       //�w�߂�+1�m�b�`����
        //�m�b�`�Z�b�g
        if (pAgentInf->v_ref[ID_HOIST] < def_spec.notch_spd_r[ID_HOIST][NOTCH_1]) {     //�t�]1�m�b�`�ȉ�

            if (pAgentInf->v_ref[ID_HOIST] < def_spec.notch_spd_r[ID_HOIST][NOTCH_4]) {         //-5�m�b�`����
                melnet.pc_b_out[melnet.pc_b_map.com_hst_notch_r5[ID_WPOS]] |= melnet.pc_b_map.com_hst_notch_r5[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_HOIST] = -5;
            }
            else if (pAgentInf->v_ref[ID_HOIST] < def_spec.notch_spd_r[ID_HOIST][NOTCH_3]) {
                melnet.pc_b_out[melnet.pc_b_map.com_hst_notch_r4[ID_WPOS]] |= melnet.pc_b_map.com_hst_notch_r4[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_HOIST] = -4;
            }
            else if (pAgentInf->v_ref[ID_HOIST] < def_spec.notch_spd_r[ID_HOIST][NOTCH_2]) {
                melnet.pc_b_out[melnet.pc_b_map.com_hst_notch_r3[ID_WPOS]] |= melnet.pc_b_map.com_hst_notch_r3[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_HOIST] = -3;
            }
            else if (pAgentInf->v_ref[ID_HOIST] < def_spec.notch_spd_r[ID_HOIST][NOTCH_1]) {
                melnet.pc_b_out[melnet.pc_b_map.com_hst_notch_r2[ID_WPOS]] |= melnet.pc_b_map.com_hst_notch_r2[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_HOIST] = -2;
            }
            else {
                melnet.pc_b_out[melnet.pc_b_map.com_hst_notch_r1[ID_WPOS]] |= melnet.pc_b_map.com_hst_notch_r1[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_HOIST] = -1;
            }
        }
        else if (pAgentInf->v_ref[ID_HOIST] > def_spec.notch_spd_f[ID_HOIST][NOTCH_1]) { //���]1�m�b�`�ȏ�

            if (pAgentInf->v_ref[ID_HOIST] > def_spec.notch_spd_f[ID_HOIST][NOTCH_4]) {
                melnet.pc_b_out[melnet.pc_b_map.com_hst_notch_f5[ID_WPOS]] |= melnet.pc_b_map.com_hst_notch_f5[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_HOIST] = 5;
            }
            else if (pAgentInf->v_ref[ID_HOIST] > def_spec.notch_spd_f[ID_HOIST][NOTCH_3]) {
                melnet.pc_b_out[melnet.pc_b_map.com_hst_notch_f4[ID_WPOS]] |= melnet.pc_b_map.com_hst_notch_f4[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_HOIST] = 4;
            }
            else if (pAgentInf->v_ref[ID_HOIST] > def_spec.notch_spd_f[ID_HOIST][NOTCH_2]) {
                melnet.pc_b_out[melnet.pc_b_map.com_hst_notch_f3[ID_WPOS]] |= melnet.pc_b_map.com_hst_notch_f3[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_HOIST] = 3;
            }
            else if (pAgentInf->v_ref[ID_HOIST] > def_spec.notch_spd_f[ID_HOIST][NOTCH_1]) {
                melnet.pc_b_out[melnet.pc_b_map.com_hst_notch_f2[ID_WPOS]] |= melnet.pc_b_map.com_hst_notch_f2[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_HOIST] = 2;
            }
            else {
                melnet.pc_b_out[melnet.pc_b_map.com_hst_notch_f1[ID_WPOS]] |= melnet.pc_b_map.com_hst_notch_f1[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_HOIST] = 1;
            }
        }
    }
    else {//0�m�b�`
        melnet.pc_b_out[melnet.pc_b_map.com_hst_notch_0[ID_WPOS]] |= melnet.pc_b_map.com_hst_notch_0[ID_BPOS];
        plc_io_workbuf.status.notch_ref[ID_HOIST] = 0;
    }
 
    //���s�m�b�`
   //�m�b�`�N���A
    melnet.pc_b_out[melnet.pc_b_map.com_gnt_notch_0[ID_WPOS]] &= NOTCH_PTN1_CLR;
 
    if ((pAgentInf->v_ref[ID_GANTRY] < (def_spec.notch_spd_r[ID_GANTRY][NOTCH_1]))             //�w�߂�-1�m�b�`��菬
        || (pAgentInf->v_ref[ID_GANTRY] > (def_spec.notch_spd_f[ID_GANTRY][NOTCH_1]))) {       //�w�߂�+1�m�b�`����
        //�m�b�`�Z�b�g
        if (pAgentInf->v_ref[ID_GANTRY] < def_spec.notch_spd_r[ID_GANTRY][NOTCH_1]) {     //�t�]1�m�b�`�ȉ�

            if (pAgentInf->v_ref[ID_GANTRY] < def_spec.notch_spd_r[ID_GANTRY][NOTCH_5]) {
                melnet.pc_b_out[melnet.pc_b_map.com_gnt_notch_r5[ID_WPOS]] |= melnet.pc_b_map.com_gnt_notch_r5[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_GANTRY] = -5;
            }
            else if (pAgentInf->v_ref[ID_GANTRY] < def_spec.notch_spd_r[ID_GANTRY][NOTCH_4]) {
                melnet.pc_b_out[melnet.pc_b_map.com_gnt_notch_r4[ID_WPOS]] |= melnet.pc_b_map.com_gnt_notch_r4[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_GANTRY] = -4;
            }
            else if (pAgentInf->v_ref[ID_GANTRY] < def_spec.notch_spd_r[ID_GANTRY][NOTCH_3]) {
                melnet.pc_b_out[melnet.pc_b_map.com_gnt_notch_r3[ID_WPOS]] |= melnet.pc_b_map.com_gnt_notch_r3[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_GANTRY] = -3;
            }
            else if (pAgentInf->v_ref[ID_GANTRY] < def_spec.notch_spd_r[ID_GANTRY][NOTCH_2]) {
                melnet.pc_b_out[melnet.pc_b_map.com_gnt_notch_r2[ID_WPOS]] |= melnet.pc_b_map.com_gnt_notch_r2[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_GANTRY] = -2;
            }
            else {
                melnet.pc_b_out[melnet.pc_b_map.com_gnt_notch_r1[ID_WPOS]] |= melnet.pc_b_map.com_gnt_notch_r1[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_GANTRY] = -1;
            }
        }
        else if (pAgentInf->v_ref[ID_GANTRY] > def_spec.notch_spd_f[ID_GANTRY][NOTCH_1]) { //���]1�m�b�`�ȏ�

            if (pAgentInf->v_ref[ID_GANTRY] > def_spec.notch_spd_f[ID_GANTRY][NOTCH_5]) {
                melnet.pc_b_out[melnet.pc_b_map.com_gnt_notch_f5[ID_WPOS]] |= melnet.pc_b_map.com_gnt_notch_f5[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_GANTRY] = 5;
            }
            else if (pAgentInf->v_ref[ID_GANTRY] > def_spec.notch_spd_f[ID_GANTRY][NOTCH_4]) {
                melnet.pc_b_out[melnet.pc_b_map.com_gnt_notch_f4[ID_WPOS]] |= melnet.pc_b_map.com_gnt_notch_f4[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_GANTRY] = 4;
            }
            else if (pAgentInf->v_ref[ID_GANTRY] > def_spec.notch_spd_f[ID_GANTRY][NOTCH_3]) {
                melnet.pc_b_out[melnet.pc_b_map.com_gnt_notch_f3[ID_WPOS]] |= melnet.pc_b_map.com_gnt_notch_f3[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_GANTRY] = 3;
            }
            else if (pAgentInf->v_ref[ID_GANTRY] > def_spec.notch_spd_f[ID_GANTRY][NOTCH_2]) {
                melnet.pc_b_out[melnet.pc_b_map.com_gnt_notch_f2[ID_WPOS]] |= melnet.pc_b_map.com_gnt_notch_f2[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_GANTRY] = 2;
            }
            else {
                melnet.pc_b_out[melnet.pc_b_map.com_gnt_notch_f1[ID_WPOS]] |= melnet.pc_b_map.com_gnt_notch_f1[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_GANTRY] = 1;
            }
        }
    }
    else {//0�m�b�`
        melnet.pc_b_out[melnet.pc_b_map.com_gnt_notch_0[ID_WPOS]] |= melnet.pc_b_map.com_gnt_notch_0[ID_BPOS];
        plc_io_workbuf.status.notch_ref[ID_GANTRY] = 0;
    }
 
    //�����m�b�`
    //�m�b�`�N���A
    melnet.pc_b_out[melnet.pc_b_map.com_bh_notch_0[ID_WPOS]] &= NOTCH_PTN0_CLR;
    

    if ((pAgentInf->v_ref[ID_BOOM_H] < (def_spec.notch_spd_r[ID_BOOM_H][NOTCH_1]))             //�w�߂�-1�m�b�`��菬
        || (pAgentInf->v_ref[ID_BOOM_H] > (def_spec.notch_spd_f[ID_BOOM_H][NOTCH_1]))) {       //�w�߂�+1�m�b�`����
        //�m�b�`�Z�b�g
       
        //!!�����͐���PC�i�o�{�j���x���t�]�ɂȂ�̂Ńm�b�`�̃Z�b�g�p�^�[�����t�ɂȂ�

        if (pAgentInf->v_ref[ID_BOOM_H] < def_spec.notch_spd_r[ID_BOOM_H][NOTCH_1]) {     //�t�]1�m�b�`�ȉ�

            if (pAgentInf->v_ref[ID_BOOM_H] < def_spec.notch_spd_r[ID_BOOM_H][NOTCH_4]) {
                melnet.pc_b_out[melnet.pc_b_map.com_bh_notch_r5[ID_WPOS]] |= melnet.pc_b_map.com_bh_notch_f5[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_BOOM_H] = -5;
            }
            else if (pAgentInf->v_ref[ID_BOOM_H] < def_spec.notch_spd_r[ID_BOOM_H][NOTCH_3]) {
                melnet.pc_b_out[melnet.pc_b_map.com_bh_notch_r4[ID_WPOS]] |= melnet.pc_b_map.com_bh_notch_f4[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_BOOM_H] = -4;
            }
            else if (pAgentInf->v_ref[ID_BOOM_H] < def_spec.notch_spd_r[ID_BOOM_H][NOTCH_2]) {
                melnet.pc_b_out[melnet.pc_b_map.com_bh_notch_r3[ID_WPOS]] |= melnet.pc_b_map.com_bh_notch_f3[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_BOOM_H] = -3;
            }
            else if (pAgentInf->v_ref[ID_BOOM_H] < def_spec.notch_spd_r[ID_BOOM_H][NOTCH_1]) {
                melnet.pc_b_out[melnet.pc_b_map.com_bh_notch_r2[ID_WPOS]] |= melnet.pc_b_map.com_bh_notch_f2[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_BOOM_H] = -2;
            }
            else {
                melnet.pc_b_out[melnet.pc_b_map.com_bh_notch_r1[ID_WPOS]] |= melnet.pc_b_map.com_bh_notch_f1[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_BOOM_H] = -1;
            }
        }
        else if (pAgentInf->v_ref[ID_BOOM_H] > def_spec.notch_spd_f[ID_BOOM_H][NOTCH_1]) { //���]1�m�b�`�ȏ�

            if (pAgentInf->v_ref[ID_BOOM_H] > def_spec.notch_spd_f[ID_BOOM_H][NOTCH_4]) {
                melnet.pc_b_out[melnet.pc_b_map.com_bh_notch_f5[ID_WPOS]] |= melnet.pc_b_map.com_bh_notch_r5[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_BOOM_H] = 5;
            }
            else if (pAgentInf->v_ref[ID_BOOM_H] > def_spec.notch_spd_f[ID_BOOM_H][NOTCH_3]) {
                melnet.pc_b_out[melnet.pc_b_map.com_bh_notch_f4[ID_WPOS]] |= melnet.pc_b_map.com_bh_notch_r4[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_BOOM_H] = 4;
            }
            else if (pAgentInf->v_ref[ID_BOOM_H] > def_spec.notch_spd_f[ID_BOOM_H][NOTCH_2]) {
                melnet.pc_b_out[melnet.pc_b_map.com_bh_notch_f3[ID_WPOS]] |= melnet.pc_b_map.com_bh_notch_r3[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_BOOM_H] = 3;
            }
            else if (pAgentInf->v_ref[ID_BOOM_H] > def_spec.notch_spd_f[ID_BOOM_H][NOTCH_1]) {
                melnet.pc_b_out[melnet.pc_b_map.com_bh_notch_f2[ID_WPOS]] |= melnet.pc_b_map.com_bh_notch_r2[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_BOOM_H] = 2;
            }
            else {
                melnet.pc_b_out[melnet.pc_b_map.com_bh_notch_f1[ID_WPOS]] |= melnet.pc_b_map.com_bh_notch_r1[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_BOOM_H] = 1;
            }
        }
    }
    else {//0�m�b�`
        melnet.pc_b_out[melnet.pc_b_map.com_bh_notch_0[ID_WPOS]] |= melnet.pc_b_map.com_bh_notch_0[ID_BPOS];
        plc_io_workbuf.status.notch_ref[ID_BOOM_H] = 0;
    }

    //����m�b�`
    //�m�b�`�N���A
    melnet.pc_b_out[melnet.pc_b_map.com_slw_notch_0[ID_WPOS]] &= NOTCH_PTN1_CLR;
  
    //!!����͐���PC�i�E��{�j���x���t�]�ɂȂ�̂Ńm�b�`�̃Z�b�g�p�^�[�����t�ɂȂ�
    if ((pAgentInf->v_ref[ID_SLEW] < (def_spec.notch_spd_r[ID_SLEW][NOTCH_1]))             //�w�߂�-1�m�b�`��菬
        || (pAgentInf->v_ref[ID_SLEW] > (def_spec.notch_spd_f[ID_SLEW][NOTCH_1]))) {       //�w�߂�+1�m�b�`����
        //�m�b�`�Z�b�g
 
        if (pAgentInf->v_ref[ID_SLEW] < def_spec.notch_spd_r[ID_SLEW][NOTCH_1]) {     //�t�]1�m�b�`�ȉ�

            if (pAgentInf->v_ref[ID_SLEW] < def_spec.notch_spd_r[ID_SLEW][NOTCH_4]) {
                melnet.pc_b_out[melnet.pc_b_map.com_slw_notch_r5[ID_WPOS]] |= melnet.pc_b_map.com_slw_notch_f5[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_SLEW] = -5;
            }
            else if (pAgentInf->v_ref[ID_SLEW] < def_spec.notch_spd_r[ID_SLEW][NOTCH_3]) {
                melnet.pc_b_out[melnet.pc_b_map.com_slw_notch_r4[ID_WPOS]] |= melnet.pc_b_map.com_slw_notch_f4[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_SLEW] = -4;
            }
            else if (pAgentInf->v_ref[ID_SLEW] < def_spec.notch_spd_r[ID_SLEW][NOTCH_2]) {
                melnet.pc_b_out[melnet.pc_b_map.com_slw_notch_r3[ID_WPOS]] |= melnet.pc_b_map.com_slw_notch_f3[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_SLEW] = -3;
            }
            else if (pAgentInf->v_ref[ID_SLEW] < def_spec.notch_spd_r[ID_SLEW][NOTCH_1]) {
                melnet.pc_b_out[melnet.pc_b_map.com_slw_notch_r2[ID_WPOS]] |= melnet.pc_b_map.com_slw_notch_f2[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_SLEW] = -2;
            }
            else {
                melnet.pc_b_out[melnet.pc_b_map.com_slw_notch_r1[ID_WPOS]] |= melnet.pc_b_map.com_slw_notch_f1[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_SLEW] = -1;
            }
        }
        else if (pAgentInf->v_ref[ID_SLEW] > def_spec.notch_spd_f[ID_SLEW][NOTCH_1]) { //���]1�m�b�`�ȏ�

            if (pAgentInf->v_ref[ID_SLEW] > def_spec.notch_spd_f[ID_SLEW][NOTCH_4]) {
                melnet.pc_b_out[melnet.pc_b_map.com_slw_notch_f5[ID_WPOS]] |= melnet.pc_b_map.com_slw_notch_r5[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_SLEW] = 5;
            }
            else if (pAgentInf->v_ref[ID_SLEW] > def_spec.notch_spd_f[ID_SLEW][NOTCH_3]) {
                melnet.pc_b_out[melnet.pc_b_map.com_slw_notch_f4[ID_WPOS]] |= melnet.pc_b_map.com_slw_notch_r4[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_SLEW] = 4;
            }
            else if (pAgentInf->v_ref[ID_SLEW] > def_spec.notch_spd_f[ID_SLEW][NOTCH_2]) {
                melnet.pc_b_out[melnet.pc_b_map.com_slw_notch_f3[ID_WPOS]] |= melnet.pc_b_map.com_slw_notch_r3[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_SLEW] = 3;
            }
            else if (pAgentInf->v_ref[ID_SLEW] > def_spec.notch_spd_f[ID_SLEW][NOTCH_1]) {
                melnet.pc_b_out[melnet.pc_b_map.com_slw_notch_f2[ID_WPOS]] |= melnet.pc_b_map.com_slw_notch_r2[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_SLEW] = 2;
            }
            else {
                melnet.pc_b_out[melnet.pc_b_map.com_slw_notch_f1[ID_WPOS]] |= melnet.pc_b_map.com_slw_notch_r1[ID_BPOS];
                plc_io_workbuf.status.notch_ref[ID_SLEW] = 1;
            }
        }
    }
    else {//0�m�b�`
        melnet.pc_b_out[melnet.pc_b_map.com_slw_notch_0[ID_WPOS]] |= melnet.pc_b_map.com_slw_notch_0[ID_BPOS];
        plc_io_workbuf.status.notch_ref[ID_SLEW] = 0;
    }
     
    return 0;
}

//*********************************************************************************************
//set_bit_coms()
//CS�����v�\��, AGENT�^�X�N, ����p�l���̃r�b�g�w�߂ɉ�����IO�o�͂�ݒ�
//*********************************************************************************************
int CPLC_IF::set_bit_coms() {
    CWorkWindow_PLC* pWorkWindow;
    
    //����N���b�N �V�X�e���J�E���^�𗘗p 25msec counter 64*0.025=1.6
    if (knl_manage_set.sys_counter& 0x40) melnet.pc_b_out[melnet.pc_b_map.healty[ID_WPOS]] |= melnet.pc_b_map.healty[ID_BPOS];
    else melnet.pc_b_out[melnet.pc_b_map.healty[ID_WPOS]] &= ~melnet.pc_b_map.healty[ID_BPOS];


    //����PC����w�ߓ���r�b�g
    if (pAgentInf->auto_active[ID_SLEW] || pAgentInf->auto_active[ID_BOOM_H])
        melnet.pc_b_out[melnet.pc_b_map.com_pc_ctr_act[ID_WPOS]] |= melnet.pc_b_map.com_pc_ctr_act[ID_BPOS];
    else melnet.pc_b_out[melnet.pc_b_map.com_pc_ctr_act[ID_WPOS]] &= ~melnet.pc_b_map.com_pc_ctr_act[ID_BPOS];

    //����PC����̃G�~�����[�^�w�߃r�b�g
    if (pSim->mode & SIM_ACTIVE_MODE)
        melnet.pc_b_out[melnet.pc_b_map.com_plc_emulate_act[ID_WPOS]] |= melnet.pc_b_map.com_plc_emulate_act[ID_BPOS];
    else melnet.pc_b_out[melnet.pc_b_map.com_plc_emulate_act[ID_WPOS]] &= ~melnet.pc_b_map.com_plc_emulate_act[ID_BPOS];

    //����~PB
    if (pAgentInf->PLC_PB_com[ID_PB_ESTOP]) melnet.pc_b_out[melnet.pc_b_map.com_estop[ID_WPOS]] |= melnet.pc_b_map.com_estop[ID_BPOS];
    else melnet.pc_b_out[melnet.pc_b_map.com_estop[ID_WPOS]] &= ~melnet.pc_b_map.com_estop[ID_BPOS];
    //�劲1��
    if (pAgentInf->PLC_PB_com[ID_PB_CTRL_SOURCE_ON]) melnet.pc_b_out[melnet.pc_b_map.com_ctrl_source_on[ID_WPOS]] |= melnet.pc_b_map.com_ctrl_source_on[ID_BPOS];
    else melnet.pc_b_out[melnet.pc_b_map.com_ctrl_source_on[ID_WPOS]] &= ~melnet.pc_b_map.com_ctrl_source_on[ID_BPOS];
    //�劲1��
    if (pAgentInf->PLC_PB_com[ID_PB_CTRL_SOURCE_OFF]) melnet.pc_b_out[melnet.pc_b_map.com_ctrl_source_off[ID_WPOS]] |= melnet.pc_b_map.com_ctrl_source_off[ID_BPOS];
    else melnet.pc_b_out[melnet.pc_b_map.com_ctrl_source_off[ID_WPOS]] &= ~melnet.pc_b_map.com_ctrl_source_off[ID_BPOS];
    //�劲2��
    if (pAgentInf->PLC_PB_com[ID_PB_CTRL_SOURCE2_ON]) melnet.pc_b_out[melnet.pc_b_map.com_ctrl_source2_on[ID_WPOS]] |= melnet.pc_b_map.com_ctrl_source2_on[ID_BPOS];
    else melnet.pc_b_out[melnet.pc_b_map.com_ctrl_source2_on[ID_WPOS]] &= ~melnet.pc_b_map.com_ctrl_source2_on[ID_BPOS];
    //�劲2��
    if (pAgentInf->PLC_PB_com[ID_PB_CTRL_SOURCE2_OFF]) melnet.pc_b_out[melnet.pc_b_map.com_ctrl_source2_off[ID_WPOS]] |= melnet.pc_b_map.com_ctrl_source2_off[ID_BPOS];
    else melnet.pc_b_out[melnet.pc_b_map.com_ctrl_source2_off[ID_WPOS]] &= ~melnet.pc_b_map.com_ctrl_source2_off[ID_BPOS];

    //�̏჊�Z�b�g
    if (pAgentInf->PLC_PB_com[ID_PB_FAULT_RESET]) melnet.pc_b_out[melnet.pc_b_map.com_fault_reset[ID_WPOS]] |= melnet.pc_b_map.com_fault_reset[ID_BPOS];
    else melnet.pc_b_out[melnet.pc_b_map.com_fault_reset[ID_WPOS]] &= ~melnet.pc_b_map.com_fault_reset[ID_BPOS];

    //PC speed ref mode
    if (pWorkWindow->stOpePaneStat.chk_PC_ref_spd) melnet.pc_b_out[melnet.pc_b_map.com_pc_analog_ref_mode[ID_WPOS]] |= melnet.pc_b_map.com_pc_analog_ref_mode[ID_BPOS];
    else melnet.pc_b_out[melnet.pc_b_map.com_pc_analog_ref_mode[ID_WPOS]] &= ~melnet.pc_b_map.com_pc_analog_ref_mode[ID_BPOS];

    //Sim fb
    if (pWorkWindow->stOpePaneStat.chk_sim_fb) melnet.pc_b_out[melnet.pc_b_map.com_pc_fb[ID_WPOS]] |= melnet.pc_b_map.com_pc_fb[ID_BPOS];
    else melnet.pc_b_out[melnet.pc_b_map.com_pc_fb[ID_WPOS]] &= ~melnet.pc_b_map.com_pc_fb[ID_BPOS];
   
    //�����v��

    //�U��~��OFF�����v
    if (pCSInf->ui_lamp[ID_PB_ANTISWAY_OFF])
        melnet.pc_b_out[melnet.pc_b_map.lamp_as_off[ID_WPOS]] |= melnet.pc_b_map.lamp_as_off[ID_BPOS];
    else 
        melnet.pc_b_out[melnet.pc_b_map.lamp_as_off[ID_WPOS]] &= ~melnet.pc_b_map.lamp_as_off[ID_BPOS];
    //�U��~��ON�����v
    if (pCSInf->ui_lamp[ID_PB_ANTISWAY_ON])
        melnet.pc_b_out[melnet.pc_b_map.lamp_as_on[ID_WPOS]] |= melnet.pc_b_map.lamp_as_on[ID_BPOS];
    else 
        melnet.pc_b_out[melnet.pc_b_map.lamp_as_on[ID_WPOS]] &= ~melnet.pc_b_map.lamp_as_on[ID_BPOS];

    //�������[�h�����v
    if (pCSInf->ui_lamp[ID_PB_AUTO_MODE])
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_mode[ID_WPOS]] |= melnet.pc_b_map.lamp_auto_mode[ID_BPOS];
    else
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_mode[ID_WPOS]] &= ~melnet.pc_b_map.lamp_auto_mode[ID_BPOS];

    //�����N�������v
    if (pCSInf->ui_lamp[ID_PB_AUTO_START])
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_start[ID_WPOS]] |= melnet.pc_b_map.lamp_auto_start[ID_BPOS];
    else 
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_start[ID_WPOS]] &= ~melnet.pc_b_map.lamp_auto_start[ID_BPOS];

    //�����R�}���h�����v
    if (pCSInf->ui_lamp[ID_PB_PARK])
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_park[ID_WPOS]] |= melnet.pc_b_map.lamp_auto_park[ID_BPOS];
    else
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_park[ID_WPOS]] &= ~melnet.pc_b_map.lamp_auto_park[ID_BPOS];
    if (pCSInf->ui_lamp[ID_PB_PICK])
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_pick[ID_WPOS]] |= melnet.pc_b_map.lamp_auto_pick[ID_BPOS];
    else
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_pick[ID_WPOS]] &= ~melnet.pc_b_map.lamp_auto_pick[ID_BPOS];
    if (pCSInf->ui_lamp[ID_PB_GRND])
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_grnd[ID_WPOS]] |= melnet.pc_b_map.lamp_auto_grnd[ID_BPOS];
    else
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_grnd[ID_WPOS]] &= ~melnet.pc_b_map.lamp_auto_grnd[ID_BPOS];

 
    //�ڕW�ʒu�Z�b�g�����v
    if (pCSInf->ui_lamp[ID_PB_AUTO_SET_Z])
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_set_z[ID_WPOS]] |= melnet.pc_b_map.lamp_auto_set_z[ID_BPOS];
    else
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_set_z[ID_WPOS]] &= ~melnet.pc_b_map.lamp_auto_set_z[ID_BPOS];

    if (pCSInf->ui_lamp[ID_PB_AUTO_SET_XY])
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_set_xy[ID_WPOS]] |= melnet.pc_b_map.lamp_auto_set_xy[ID_BPOS];
    else
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_set_xy[ID_WPOS]] &= ~melnet.pc_b_map.lamp_auto_set_xy[ID_BPOS];

    
    //�����������v
    if (pCSInf->semiauto_lamp[SEMI_AUTO_S1])
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_tg_s1[ID_WPOS]] |= melnet.pc_b_map.lamp_auto_tg_s1[ID_BPOS];
    else 
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_tg_s1[ID_WPOS]] &= ~melnet.pc_b_map.lamp_auto_tg_s1[ID_BPOS];

    if (pCSInf->semiauto_lamp[SEMI_AUTO_S2])
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_tg_s2[ID_WPOS]] |= melnet.pc_b_map.lamp_auto_tg_s2[ID_BPOS];
    else 
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_tg_s2[ID_WPOS]] &= ~melnet.pc_b_map.lamp_auto_tg_s2[ID_BPOS];

    if (pCSInf->semiauto_lamp[SEMI_AUTO_S3])
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_tg_s3[ID_WPOS]] |= melnet.pc_b_map.lamp_auto_tg_s3[ID_BPOS];
    else 
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_tg_s3[ID_WPOS]] &= ~melnet.pc_b_map.lamp_auto_tg_s3[ID_BPOS];

    if (pCSInf->semiauto_lamp[SEMI_AUTO_L1])
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_tg_l1[ID_WPOS]] |= melnet.pc_b_map.lamp_auto_tg_l1[ID_BPOS];
    else 
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_tg_l1[ID_WPOS]] &= ~melnet.pc_b_map.lamp_auto_tg_l1[ID_BPOS];

    if (pCSInf->semiauto_lamp[SEMI_AUTO_L2])
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_tg_l2[ID_WPOS]] |= melnet.pc_b_map.lamp_auto_tg_l2[ID_BPOS];
    else 
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_tg_l2[ID_WPOS]] &= ~melnet.pc_b_map.lamp_auto_tg_l2[ID_BPOS];

    if (pCSInf->semiauto_lamp[SEMI_AUTO_L3])
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_tg_l3[ID_WPOS]] |= melnet.pc_b_map.lamp_auto_tg_l3[ID_BPOS];
    else 
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_tg_l3[ID_WPOS]] &= ~melnet.pc_b_map.lamp_auto_tg_l3[ID_BPOS];

    //�����R�}���h�����v
    if (pCSInf->ui_lamp[ID_PB_PICK])
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_pick[ID_WPOS]] |= melnet.pc_b_map.lamp_auto_pick[ID_BPOS];
    else
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_pick[ID_WPOS]] &= ~melnet.pc_b_map.lamp_auto_pick[ID_BPOS];

    if (pCSInf->ui_lamp[ID_PB_GRND])
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_grnd[ID_WPOS]] |= melnet.pc_b_map.lamp_auto_grnd[ID_BPOS];
    else
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_grnd[ID_WPOS]] &= ~melnet.pc_b_map.lamp_auto_grnd[ID_BPOS];

    if (pCSInf->ui_lamp[ID_PB_PARK])
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_park[ID_WPOS]] |= melnet.pc_b_map.lamp_auto_park[ID_BPOS];
    else
        melnet.pc_b_out[melnet.pc_b_map.lamp_auto_park[ID_WPOS]] &= ~melnet.pc_b_map.lamp_auto_park[ID_BPOS];
   
    return 0;
}

//*********************************************************************************************
//set_ao_coms()
// AGENT�^�X�N�̃A�i���O�w�߁A�w���V�[�M�����̏o�̓Z�b�g
//*********************************************************************************************
int CPLC_IF::set_ao_coms() {
 
    melnet.pc_w_out[melnet.pc_w_map.helthy[ID_WPOS]] = helthy_cnt;

    //���x�w�߃A�i���O�@0.1%�P�ʁj
    //�����͓����{�o��-
    melnet.pc_w_out[melnet.pc_w_map.spd_ref_bh[ID_WPOS]] = (INT16)(-1000 * pAgentInf->v_ref[ID_BOOM_H]/pCrane->spec.notch_spd_f[ID_BOOM_H][NOTCH_5]);
    //����͍���-�E��+
    melnet.pc_w_out[melnet.pc_w_map.spd_ref_slw[ID_WPOS]] = (INT16)(-1000 * pAgentInf->v_ref[ID_SLEW] / pCrane->spec.notch_spd_f[ID_SLEW][NOTCH_5]);
    melnet.pc_w_out[melnet.pc_w_map.spd_ref_hst[ID_WPOS]] = (INT16)(1000 * pAgentInf->v_ref[ID_HOIST] / pCrane->spec.notch_spd_f[ID_HOIST][NOTCH_5]);
    melnet.pc_w_out[melnet.pc_w_map.spd_ref_gnt[ID_WPOS]] = (INT16)(1000 * pAgentInf->v_ref[ID_GANTRY] / pCrane->spec.notch_spd_f[ID_GANTRY][NOTCH_5]);

    return 0;
}

//*********************************************************************************************
// parse_notch_com()
// UI�m�b�`�w�ߓǂݍ���
//*********************************************************************************************
int CPLC_IF::parse_notch_com() {
    
    INT16 check_i;
 
    //�����m�b�`
    //���ɑΉ�����bit�𒊏o
    check_i = melnet.plc_w_out[melnet.plc_w_map.com_hst_notch_0[ID_WPOS]] & NOTCH_PTN0_ALL;

    if(check_i & NOTCH_PTN0_0) plc_io_workbuf.ui.notch_pos[ID_HOIST] = NOTCH_0;             //0�m�b�`
    else if(check_i & NOTCH_PTN0_F1) {                                                      //���]�r�b�g�i1�m�b�`�jON
      if(check_i == NOTCH_PTN0_F5) plc_io_workbuf.ui.notch_pos[ID_HOIST] = NOTCH_5;         //�ȉ��r�b�g�p�^�[�����ƍ�
      else if (check_i == NOTCH_PTN0_F4) plc_io_workbuf.ui.notch_pos[ID_HOIST] = NOTCH_4;
      else if (check_i == NOTCH_PTN0_F3) plc_io_workbuf.ui.notch_pos[ID_HOIST] = NOTCH_3;
      else if (check_i == NOTCH_PTN0_F2) plc_io_workbuf.ui.notch_pos[ID_HOIST] = NOTCH_2;
      else plc_io_workbuf.ui.notch_pos[ID_HOIST] = NOTCH_1;
    }
    else if (check_i & NOTCH_PTN0_R1) {                                                     //�t�]�r�b�g�i1�m�b�`�jON
      if (check_i == NOTCH_PTN0_R5) plc_io_workbuf.ui.notch_pos[ID_HOIST] = -NOTCH_5;       //�ȉ��r�b�g�p�^�[�����ƍ�
      else if (check_i == NOTCH_PTN0_R4) plc_io_workbuf.ui.notch_pos[ID_HOIST] = -NOTCH_4;
      else if (check_i == NOTCH_PTN0_R3) plc_io_workbuf.ui.notch_pos[ID_HOIST] = -NOTCH_3;
      else if (check_i == NOTCH_PTN0_R2) plc_io_workbuf.ui.notch_pos[ID_HOIST] =- NOTCH_2;
      else plc_io_workbuf.ui.notch_pos[ID_HOIST] = -NOTCH_1;
    }
    else plc_io_workbuf.ui.notch_pos[ID_HOIST] = NOTCH_0;
        
    //���s�m�b�`
    check_i = melnet.plc_w_out[melnet.plc_w_map.com_gnt_notch_0[ID_WPOS]] & NOTCH_PTN1_ALL;

    if (check_i & NOTCH_PTN1_0) plc_io_workbuf.ui.notch_pos[ID_GANTRY] = NOTCH_0;
    else if (check_i & NOTCH_PTN1_F1) {
        if (check_i == NOTCH_PTN1_F5) plc_io_workbuf.ui.notch_pos[ID_GANTRY] = NOTCH_5;
        else if (check_i == NOTCH_PTN1_F4) plc_io_workbuf.ui.notch_pos[ID_GANTRY] = NOTCH_4;
        else if (check_i == NOTCH_PTN1_F3) plc_io_workbuf.ui.notch_pos[ID_GANTRY] = NOTCH_3;
        else if (check_i == NOTCH_PTN1_F2) plc_io_workbuf.ui.notch_pos[ID_GANTRY] = NOTCH_2;
        else plc_io_workbuf.ui.notch_pos[ID_GANTRY] = NOTCH_1;
    }
    else if (check_i & NOTCH_PTN1_R1) {
        if (check_i == NOTCH_PTN1_R5) plc_io_workbuf.ui.notch_pos[ID_GANTRY] = -NOTCH_5;
        else if (check_i == NOTCH_PTN1_R4) plc_io_workbuf.ui.notch_pos[ID_GANTRY] = -NOTCH_4;
        else if (check_i == NOTCH_PTN1_R3) plc_io_workbuf.ui.notch_pos[ID_GANTRY] = -NOTCH_3;
        else if (check_i == NOTCH_PTN1_R2) plc_io_workbuf.ui.notch_pos[ID_GANTRY] = -NOTCH_2;
        else plc_io_workbuf.ui.notch_pos[ID_GANTRY] = -NOTCH_1;
    }
    else plc_io_workbuf.ui.notch_pos[ID_GANTRY] = NOTCH_0;
    
    //�����m�b�`
    check_i = melnet.plc_w_out[melnet.plc_w_map.com_bh_notch_0[ID_WPOS]] & NOTCH_PTN0_ALL;

    if (check_i & NOTCH_PTN0_0) plc_io_workbuf.ui.notch_pos[ID_BOOM_H] = NOTCH_0;
    else if (check_i & NOTCH_PTN0_F1) {
        if (check_i == NOTCH_PTN0_F5) plc_io_workbuf.ui.notch_pos[ID_BOOM_H] = NOTCH_5;
        else if (check_i == NOTCH_PTN0_F4) plc_io_workbuf.ui.notch_pos[ID_BOOM_H] = NOTCH_4;
        else if (check_i == NOTCH_PTN0_F3) plc_io_workbuf.ui.notch_pos[ID_BOOM_H] = NOTCH_3;
        else if (check_i == NOTCH_PTN0_F2) plc_io_workbuf.ui.notch_pos[ID_BOOM_H] = NOTCH_2;
        else plc_io_workbuf.ui.notch_pos[ID_BOOM_H] = NOTCH_1;
    }
    else if (check_i & NOTCH_PTN0_R1) {
        if (check_i == NOTCH_PTN0_R5) plc_io_workbuf.ui.notch_pos[ID_BOOM_H] = -NOTCH_5;
        else if (check_i == NOTCH_PTN0_R4) plc_io_workbuf.ui.notch_pos[ID_BOOM_H] = -NOTCH_4;
        else if (check_i == NOTCH_PTN0_R3) plc_io_workbuf.ui.notch_pos[ID_BOOM_H] = -NOTCH_3;
        else if (check_i == NOTCH_PTN0_R2) plc_io_workbuf.ui.notch_pos[ID_BOOM_H] = -NOTCH_2;
        else plc_io_workbuf.ui.notch_pos[ID_BOOM_H] = -NOTCH_1;
    }
    else plc_io_workbuf.ui.notch_pos[ID_BOOM_H] = NOTCH_0;

    //����m�b�`
    check_i = melnet.plc_w_out[melnet.plc_w_map.com_gnt_notch_0[ID_WPOS]] & NOTCH_PTN1_ALL;

    if (check_i & NOTCH_PTN1_0) plc_io_workbuf.ui.notch_pos[ID_SLEW] = NOTCH_0;
    else if (check_i & NOTCH_PTN1_F1) {
        if (check_i == NOTCH_PTN1_F5) plc_io_workbuf.ui.notch_pos[ID_SLEW] = NOTCH_5;
        else if (check_i == NOTCH_PTN1_F4) plc_io_workbuf.ui.notch_pos[ID_SLEW] = NOTCH_4;
        else if (check_i == NOTCH_PTN1_F3) plc_io_workbuf.ui.notch_pos[ID_SLEW] = NOTCH_3;
        else if (check_i == NOTCH_PTN1_F2) plc_io_workbuf.ui.notch_pos[ID_SLEW] = NOTCH_2;
        else plc_io_workbuf.ui.notch_pos[ID_SLEW] = NOTCH_1;
    }
    else if (check_i & NOTCH_PTN1_R1) {
        if (check_i == NOTCH_PTN1_R5) plc_io_workbuf.ui.notch_pos[ID_SLEW] = -NOTCH_5;
        else if (check_i == NOTCH_PTN1_R4) plc_io_workbuf.ui.notch_pos[ID_SLEW] = -NOTCH_4;
        else if (check_i == NOTCH_PTN1_R3) plc_io_workbuf.ui.notch_pos[ID_SLEW] = -NOTCH_3;
        else if (check_i == NOTCH_PTN1_R2) plc_io_workbuf.ui.notch_pos[ID_SLEW] = -NOTCH_2;
        else plc_io_workbuf.ui.notch_pos[ID_SLEW] = -NOTCH_1;
    }
    else plc_io_workbuf.ui.notch_pos[ID_SLEW] = NOTCH_0;
    
    return 0;

}

//*********************************************************************************************
// parse_ope_com()
// �^�]������M����荞��
//*********************************************************************************************
int CPLC_IF::parse_ope_com() {

    //PB��荞�݂�OFF Delay ON���o�ŃJ�E���g�l�Z�b�g��0�܂Ō��Z
    
    //����PB�ނ�W���W�X�^���
    if (melnet.plc_w_out[melnet.plc_w_map.com_estop[ID_WPOS]] & melnet.plc_w_map.com_estop[ID_BPOS])  plc_io_workbuf.ui.PB[ID_PB_ESTOP] = PLC_IO_OFF_DELAY_COUNT;
    else if (plc_io_workbuf.ui.PB[ID_PB_ESTOP] > 0) plc_io_workbuf.ui.PB[ID_PB_ESTOP]--;
    else;

    if (melnet.plc_w_out[melnet.plc_w_map.com_ctrl_source_on[ID_WPOS]] & melnet.plc_w_map.com_ctrl_source_on[ID_BPOS])  plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE_ON] = PLC_IO_OFF_DELAY_COUNT;
    else if (plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE_ON] > 0) plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE_ON]--;
    else;

    if (melnet.plc_w_out[melnet.plc_w_map.com_ctrl_source_off[ID_WPOS]] & melnet.plc_w_map.com_ctrl_source_off[ID_BPOS])  plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE_OFF] = PLC_IO_OFF_DELAY_COUNT;
    else if (plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE_OFF] > 0)plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE_OFF]--;
    else;

    if (melnet.plc_w_out[melnet.plc_w_map.com_ctrl_source_off[ID_WPOS]] & melnet.plc_w_map.com_ctrl_source_off[ID_BPOS])  plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE_OFF] = PLC_IO_OFF_DELAY_COUNT;
    else if (plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE_OFF] > 0)plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE_OFF]--;
    else;

    if (melnet.plc_w_out[melnet.plc_w_map.com_ctrl_source2_on[ID_WPOS]] & melnet.plc_w_map.com_ctrl_source2_on[ID_BPOS])  plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE2_ON] = PLC_IO_OFF_DELAY_COUNT;
    else if (plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE2_ON] > 0)plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE2_ON]--;
    else;

    if (melnet.plc_w_out[melnet.plc_w_map.com_ctrl_source2_off[ID_WPOS]] & melnet.plc_w_map.com_ctrl_source2_off[ID_BPOS])  plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE2_OFF] = PLC_IO_OFF_DELAY_COUNT;
    else if (plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE2_OFF] > 0)plc_io_workbuf.ui.PB[ID_PB_CTRL_SOURCE2_OFF]--;
    else;

 
      
    //�ǉ�PB�ނ�B���W�X�^���
    if (melnet.plc_b_out[melnet.plc_b_map.PB_as_on[ID_WPOS]] & melnet.plc_b_map.PB_as_on[ID_BPOS])  plc_io_workbuf.ui.PB[ID_PB_ANTISWAY_ON] = PLC_IO_OFF_DELAY_COUNT;
    else if (plc_io_workbuf.ui.PB[ID_PB_ANTISWAY_ON] > 0)plc_io_workbuf.ui.PB[ID_PB_ANTISWAY_ON]--;
    else;

    if (melnet.plc_b_out[melnet.plc_b_map.PB_as_off[ID_WPOS]] & melnet.plc_b_map.PB_as_off[ID_BPOS])  plc_io_workbuf.ui.PB[ID_PB_ANTISWAY_OFF] = PLC_IO_OFF_DELAY_COUNT;
    else if (plc_io_workbuf.ui.PB[ID_PB_ANTISWAY_OFF] > 0)plc_io_workbuf.ui.PB[ID_PB_ANTISWAY_OFF]--;
    else;

    if (melnet.plc_b_out[melnet.plc_b_map.PB_auto_start[ID_WPOS]] & melnet.plc_b_map.PB_auto_start[ID_BPOS]) plc_io_workbuf.ui.PB[ID_PB_AUTO_START] = PLC_IO_OFF_DELAY_COUNT;
    else if (plc_io_workbuf.ui.PB[ID_PB_AUTO_START] > 0)plc_io_workbuf.ui.PB[ID_PB_AUTO_START]--;
    else;

    if (melnet.plc_b_out[melnet.plc_b_map.PB_auto_target_s1[ID_WPOS]] & melnet.plc_b_map.PB_auto_target_s1[ID_BPOS]) plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_S1] = PLC_IO_OFF_DELAY_COUNT;
    else if (plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_S1] > 0)plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_S1]--;
    else;

    if (melnet.plc_b_out[melnet.plc_b_map.PB_auto_target_s2[ID_WPOS]] & melnet.plc_b_map.PB_auto_target_s2[ID_BPOS]) plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_S2] = PLC_IO_OFF_DELAY_COUNT;
    else if (plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_S2] > 0)plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_S2]--;
    else;

    if (melnet.plc_b_out[melnet.plc_b_map.PB_auto_target_s3[ID_WPOS]] & melnet.plc_b_map.PB_auto_target_s3[ID_BPOS]) plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_S3] = PLC_IO_OFF_DELAY_COUNT;
    else if (plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_S3] > 0)plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_S3]--;
    else;

    if (melnet.plc_b_out[melnet.plc_b_map.PB_auto_set_z[ID_WPOS]] & melnet.plc_b_map.PB_auto_set_z[ID_BPOS]) plc_io_workbuf.ui.PB[ID_PB_AUTO_SET_Z] = PLC_IO_OFF_DELAY_COUNT;
    else if (plc_io_workbuf.ui.PB[ID_PB_AUTO_SET_Z] > 0)plc_io_workbuf.ui.PB[ID_PB_AUTO_SET_Z]--;
    else;

    if (melnet.plc_b_out[melnet.plc_b_map.PB_auto_target_l1[ID_WPOS]] & melnet.plc_b_map.PB_auto_target_l1[ID_BPOS]) plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_L1] = PLC_IO_OFF_DELAY_COUNT;
    else if (plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_L1] > 0)plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_L1]--;
    else;

    if (melnet.plc_b_out[melnet.plc_b_map.PB_auto_target_l2[ID_WPOS]] & melnet.plc_b_map.PB_auto_target_l2[ID_BPOS]) plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_L2] = PLC_IO_OFF_DELAY_COUNT;
    else if (plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_L2] > 0)plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_L2]--;
    else;

    if (melnet.plc_b_out[melnet.plc_b_map.PB_auto_target_l3[ID_WPOS]] & melnet.plc_b_map.PB_auto_target_l3[ID_BPOS]) plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_L3] = PLC_IO_OFF_DELAY_COUNT;
    else if (plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_L3] > 0)plc_io_workbuf.ui.PBsemiauto[SEMI_AUTO_L3]--;
    else;
 
    if (melnet.plc_b_out[melnet.plc_b_map.PB_auto_set_xy[ID_WPOS]] & melnet.plc_b_map.PB_auto_set_xy[ID_BPOS]) plc_io_workbuf.ui.PB[ID_PB_AUTO_SET_XY] = PLC_IO_OFF_DELAY_COUNT;
    else if (plc_io_workbuf.ui.PB[ID_PB_AUTO_SET_XY] > 0)plc_io_workbuf.ui.PB[ID_PB_AUTO_SET_XY]--;
    else;

    //�@�ヂ�[�h
    if (melnet.plc_b_out[melnet.plc_b_map.PB_mode_crane[ID_WPOS]] & melnet.plc_b_map.PB_mode_crane[ID_BPOS]) plc_io_workbuf.ui.PB[ID_PB_CRANE_MODE] = L_ON;
    else  plc_io_workbuf.ui.PB[ID_PB_CRANE_MODE] = L_OFF;

    //���u���[�h
    if (melnet.plc_b_out[melnet.plc_b_map.PB_mode_remote[ID_WPOS]] & melnet.plc_b_map.PB_mode_remote[ID_BPOS]) plc_io_workbuf.ui.PB[ID_PB_REMOTE_MODE] = L_ON;
    else  plc_io_workbuf.ui.PB[ID_PB_REMOTE_MODE] = L_OFF;

    return 0;
}

//*********************************************************************************************
// parse_brk_status()
// �u���[�L��ԓǂݍ��� �i���jOTE�`�F�b�N�p
//*********************************************************************************************
int CPLC_IF::parce_brk_status() {

    //#### OTE�`�F�b�N�p�@��
    for (int i = 0;i < MOTION_ID_MAX;i++) {
        if ((plc_io_workbuf.status.v_fb[i] == 0.0) && (plc_io_workbuf.status.v_ref[i] == 0.0)) {
            plc_io_workbuf.status.brk[i] = L_ON;
        }
        else {
            plc_io_workbuf.status.brk[i] = L_OFF;
        }
    }
    return 0;
}


//*********************************************************************************************
// set_sim_status()
// ���xFB,�ʒuFB,�����v��
//*********************************************************************************************
int CPLC_IF::set_sim_status() {

    plc_io_workbuf.status.v_fb[ID_HOIST] = pSim->status.v_fb[ID_HOIST];
    plc_io_workbuf.status.v_fb[ID_GANTRY] = pSim->status.v_fb[ID_GANTRY];
    plc_io_workbuf.status.v_fb[ID_BOOM_H] = pSim->status.v_fb[ID_BOOM_H];
    plc_io_workbuf.status.v_fb[ID_SLEW] = pSim->status.v_fb[ID_SLEW];

    plc_io_workbuf.status.pos[ID_HOIST] = pSim->status.pos[ID_HOIST];
    plc_io_workbuf.status.pos[ID_GANTRY] = pSim->status.pos[ID_GANTRY];
    plc_io_workbuf.status.pos[ID_BOOM_H] = pSim->status.pos[ID_BOOM_H];
    plc_io_workbuf.status.pos[ID_SLEW] = pSim->status.pos[ID_SLEW];

    //�劲�����v�@���u���[�h
    if (plc_io_workbuf.ui.PB[ID_PB_REMOTE_MODE]) {
        plc_io_workbuf.ui.LAMP[ID_PB_CRANE_MODE] = L_OFF;
        plc_io_workbuf.ui.LAMP[ID_PB_REMOTE_MODE] = L_ON;
    }
    else {
        plc_io_workbuf.ui.LAMP[ID_PB_CRANE_MODE] = L_ON;
        plc_io_workbuf.ui.LAMP[ID_PB_REMOTE_MODE] = L_OFF;
    }
    plc_io_workbuf.ui.LAMP[ID_PB_CTRL_SOURCE_ON] = L_ON;
    plc_io_workbuf.ui.LAMP[ID_PB_CTRL_SOURCE_OFF] = L_OFF;
    plc_io_workbuf.ui.LAMP[ID_PB_CTRL_SOURCE2_ON] = L_ON;
    plc_io_workbuf.ui.LAMP[ID_PB_CTRL_SOURCE2_OFF] = L_OFF;

    plc_io_workbuf.ui.LAMP[ID_LAMP_NOTCH_POS_HST] = plc_io_workbuf.ui.notch_pos[ID_HOIST];
    plc_io_workbuf.ui.LAMP[ID_LAMP_NOTCH_POS_GNT] = plc_io_workbuf.ui.notch_pos[ID_GANTRY];
    plc_io_workbuf.ui.LAMP[ID_LAMP_NOTCH_POS_TRY] = plc_io_workbuf.ui.notch_pos[ID_TROLLY];
    plc_io_workbuf.ui.LAMP[ID_LAMP_NOTCH_POS_BH] = plc_io_workbuf.ui.notch_pos[ID_BOOM_H];
    plc_io_workbuf.ui.LAMP[ID_LAMP_NOTCH_POS_SLW] = plc_io_workbuf.ui.notch_pos[ID_SLEW];

    return 0;
}

//*********************************************************************************************
// parse_sensor_fb()
// �Z���T�M����荞��
//*********************************************************************************************
int CPLC_IF::parse_sensor_fb() {

    plc_io_workbuf.status.v_fb[ID_HOIST] = def_spec.notch_spd_f[ID_HOIST][5] * (double)melnet.plc_w_out[melnet.plc_w_map.spd_hst_fb[ID_WPOS]] / 1000.0;
    plc_io_workbuf.status.v_fb[ID_GANTRY] = def_spec.notch_spd_f[ID_GANTRY][5] * (double)melnet.plc_w_out[melnet.plc_w_map.spd_gnt_fb[ID_WPOS]] / 1000.0;
    //�����͓����{�o��-�œ����Ă���
    plc_io_workbuf.status.v_fb[ID_BOOM_H] = -1.0 * def_spec.notch_spd_f[ID_BOOM_H][5] * (double)melnet.plc_w_out[melnet.plc_w_map.spd_bh_fb[ID_WPOS]] / 1000.0;
    //����͍���-�E��+�œ����Ă���
    plc_io_workbuf.status.v_fb[ID_SLEW] = -1.0 * def_spec.notch_spd_f[ID_SLEW][5] * (double)melnet.plc_w_out[melnet.plc_w_map.spd_slw_fb[ID_WPOS]] / 1000.0;

    plc_io_workbuf.status.weight = (double)melnet.plc_w_out[melnet.plc_w_map.load_fb[ID_WPOS]] * 100.0; //Kg
    
    plc_io_workbuf.status.pos[ID_HOIST] = (double)melnet.plc_w_out[melnet.plc_w_map.pos_hst_fb[ID_WPOS]] / 10.0;        //m�@PLC�����0.1m�P�ʁj
    plc_io_workbuf.status.pos[ID_GANTRY] = (double)melnet.plc_w_out[melnet.plc_w_map.pos_gnt_fb[ID_WPOS]] / 10.0;       //m�@PLC�����0.1m�P�ʁj
  
    plc_io_workbuf.status.pos[ID_BOOM_H] =(double)melnet.plc_w_out[melnet.plc_w_map.pos_bh_fb[ID_WPOS]] / 10.0;   //m�@PLC�����0.1m�P�ʁj
    plc_io_workbuf.status.pos[ID_SLEW] = (3600.0-(double)melnet.plc_w_out[melnet.plc_w_map.pos_slw_fb[ID_WPOS]]) * PI1DEG/10.0;  //rad PLC�����0.1deg�P�ʁj
    return 0;
}

int CPLC_IF::mel_set_force(int id, bool bset, int index, WORD value) {

    if (id < 0 || id >4) return 0;

    if (id == MEL_FORCE_RESET) {
        for (int i = 0;i < 4;i++) {
            melnet.is_force_set_active[i] = false;
            melnet.forced_index[i] = 0;
            melnet.forced_dat[i] = 0;
        }
    }
    else if (bset == true) {
        melnet.is_force_set_active[id] =true;
        melnet.forced_index[id] = index;
        melnet.forced_dat[id] = value;
    }
    else {
        melnet.is_force_set_active[id] = false;
        melnet.forced_index[id] = 0;
        melnet.forced_dat[id] = 0;
    }
    return 1;
}