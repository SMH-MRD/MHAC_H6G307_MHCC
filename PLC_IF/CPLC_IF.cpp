#include "framework.h"
#include "CPLC_IF.h"
#include "PLC_IF.h"
#include "PLC_DEF.h"
#include <windowsx.h>
#include "Mdfunc.h"
#include "OTE0panel.h""

extern ST_SPEC def_spec;
extern ST_KNL_MANAGE_SET    knl_manage_set;

CMCProtocol* pMCProtocol;  //MC�v���g�R���I�u�W�F�N�g:
INT16 bit_mask[16] = {BIT0,BIT1,BIT2,BIT3,BIT4,BIT5,BIT6,BIT7,BIT8,BIT9,BIT10,BIT11,BIT12,BIT13,BIT14,BIT15};
ST_PLC_NOTCH_PTN notch_ptn;//PLC�̃m�b�`�M�����̓p�^�[��

CABPLC_BOUT_MAP cab_bout_map;
ERMPLC_BOUT_MAP erm_bout_map;
ERMPLC_YOUT_MAP erm_yout_map;
ERMPLC_XIN_MAP  erm_xin_map;
ERMPLC_M900_MAP  erm_m900_map;
INV_IF_X_MAP inv_x_map; 
INV_IF_Y_MAP inv_y_map;

CPLC_IF::CPLC_IF(HWND _hWnd_parent) {

    hWnd_parent = _hWnd_parent;
    // ���L�������I�u�W�F�N�g�̃C���X�^���X��
    pPLCioObj = new CSharedMem;
    pCraneStatusObj = new CSharedMem;
    pSimulationStatusObj = new CSharedMem;
    pAgentInfObj = new CSharedMem;
    pCSInfObj = new CSharedMem;
    pOTEioObj = new CSharedMem;

    out_size = 0;

#ifdef _TYPE_MELSECNET
    memset(&melnet,0,sizeof(ST_MELSEC_NET)) ;      //PLC�����N�\����
    memset(&plc_if_workbuf,0,sizeof(ST_PLC_IO));   //���L�������ւ̏o�̓Z�b�g��Ɨp�o�b�t�@
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
#endif

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
    pCSInf = (LPST_CS_INFO)pCSInfObj->get_pMap(); 
    
    
    if (OK_SHMEM != pOTEioObj->create_smem(SMEM_OTE_IO_NAME, sizeof(ST_OTE_IO), MUTEX_OTE_IO_NAME)) {
        mode |= PLC_IF_OTE_MEM_NG;
    }
    pOTEio = (LPST_OTE_IO)pOTEioObj->get_pMap();
 
    //MC�v���g�R���I�u�W�F�N�g�C���X�^���X��
    pMCProtocol = new CMCProtocol;

    //MC�v���g�R���ʐM�p�A�h���X�ݒ�
    //# �N���C�A���g�i��M�j�\�P�b�g�A�h���X
    memset(&addrinc, 0, sizeof(addrinc));
    addrinc.sin_port = htons(PORT_MC_CLIENT);
    addrinc.sin_family = AF_INET;
    inet_pton(AF_INET, IP_ADDR_MC_CLIENT, &addrinc.sin_addr.s_addr);

    //# �T�[�o�[�i���M��j�\�P�b�g�A�h���X
    memset(&addrins, 0, sizeof(addrins));
    addrins.sin_port = htons(PORT_MC_SERVER);
    addrins.sin_family = AF_INET;
    inet_pton(AF_INET, IP_ADDR_MC_SERVER, &addrins.sin_addr.s_addr);

    //MC�v���g�R���I�u�W�F�N�g������
    hWnd_plcif = pMCProtocol->Initialize(hWnd_parent, &addrinc, &addrins, MC_ADDR_D_READ, MC_SIZE_D_READ, MC_ADDR_D_WRITE, MC_SIZE_D_WRITE);

    SetWindowPos(hWnd_plcif, HWND_TOP, IF_WND_INIT_POS_X, IF_WND_INIT_POS_Y, IF_WND_INIT_SIZE_W, IF_WND_INIT_SIZE_H, SWP_SHOWWINDOW);

    lp_PLCread = (LPST_PLC_READ)(pMCProtocol->mc_res_msg_r.res_data);
    lp_PLCwrite = (LPST_PLC_WRITE)(pMCProtocol->mc_req_msg_w.req_data);


    //�f�[�^�ϊ��v�Z�p�p�����[�^�Z�b�g
    while (!pCrane->is_crane_status_ok) Sleep(100);//MAIN�v���Z�X�̎d�l��荞�݊����҂�

    for (int i = 0; i < MOTION_ID_MAX; i++) {//�h����0�w�p�����[�^�Z�b�g
        plc_if_workbuf.Cdr[i][0] = pCrane->spec.prm_nw[SIZE_ITEM_WIRE_LEN0][i];
        plc_if_workbuf.Ldr[i][0] = 0.0;
    }

    for (int j = 1; j < PLC_DRUM_LAYER_MAX; j++) {//�h����1�w�ȏ�p�����[�^�Z�b�g{
        for (int i = 0; i < ID_AHOIST+1; i++) {//�h����1�w�ȏ�p�����[�^�Z�b�g
            //�h�����~��
            plc_if_workbuf.Cdr[i][j] =( pCrane->spec.prm_nw[DRUM_ITEM_DIR][i] + ((double)j-1.0)* pCrane->spec.prm_nw[DRUM_ITEM_DIR_ADD][i]) * PI180;
            //�h�����w�΃h����1�w�~���䗦
            plc_if_workbuf.Kdr[i][j] = (pCrane->spec.prm_nw[DRUM_ITEM_DIR][i] + ((double)j - 1.0) * pCrane->spec.prm_nw[DRUM_ITEM_DIR_ADD][i]) / pCrane->spec.prm_nw[DRUM_ITEM_DIR][i];
            //�h�����w������
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

    //MAIN�v���Z�X(Environment�^�X�N�̃w���V�[�M����荞�݁j
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
// parse_data_out() PLC����̓��͐M����W�J
//*********************************************************************************************
int CPLC_IF::parse_data_in() {

    //��M�o�b�t�@�̓��e�����[�N�o�b�t�@�ɃR�s�[
    memcpy_s(&(plc_if_workbuf.input), sizeof(PLC_READ_BUF),lp_PLCread, sizeof(PLC_READ_BUF));

    return 0;
}
//*********************************************************************************************
// parse_data_out() PLC�ւ̏o�͐M������
//*********************************************************************************************
int CPLC_IF::parse_data_out() {

    //PC ����L���M��
    plc_if_workbuf.output.wbuf.ctrl_mode = mode;
 
#pragma region OPEROOM
    // ����~�@PB�i�劲OFF�@PB�j�@*PLC NORMAL CLOSE
    if ((pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_PB_HIJYOU]) || !(pOTEio->ote_umsg_in.body.pb_notch[ID_OTE_GRIP_ESTOP])) {
         plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.cab_estp.x] &= ~cab_bout_map.cab_estp.y; //����~
    }
    else {
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.cab_estp.x] |= cab_bout_map.cab_estp.y; //����~
    }

    if (pCSInf->ote_remote_status &= CS_CODE_OTE_REMOTE_ENABLE) {//�[������L��
        // �劲ON/OFF�@PB
        if (pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_PB_SYUKAN]) {
           //�g���K���Z�b�g
            if (!(plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_off.x] & cab_bout_map.ctrl_off.y)) //�؎w�ߗD��                     //�劲�� ON�i�؎w��ON�j
            {
                plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_on.x] &= ~cab_bout_map.ctrl_on.y;
            }
            else if ((plc_if_workbuf.input.rbuf.erm_bo[erm_bout_map.ctrl_source_mc_ok.x] & erm_bout_map.ctrl_source_mc_ok.y)
                && (plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_off.x] & cab_bout_map.ctrl_off.y)
                && !(plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_on.x] & cab_bout_map.ctrl_on.y))

            {//�劲���莞�͎劲��PB�ƔF��
                plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_off.x] &= ~cab_bout_map.ctrl_off.y;
            }
        
            else if (!(plc_if_workbuf.input.rbuf.erm_bo[erm_bout_map.ctrl_source_mc_ok.x] & erm_bout_map.ctrl_source_mc_ok.y)   //�劲�����w�� OFF
                && !(plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_on.x] & cab_bout_map.ctrl_on.y)                      //�劲�� OFF
                && (plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_off.x] & cab_bout_map.ctrl_off.y))                      //�劲�� ON�i�؎w��ON�j
            {
                plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_on.x] |= cab_bout_map.ctrl_on.y;
            }
            else;
        }
        else {
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_on.x] &= ~cab_bout_map.ctrl_on.y;
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_off.x] |= cab_bout_map.ctrl_off.y;//�劲�@�؂�Normal Close
        }

        //�̏჊�Z�b�g�@IL�@Bypass
        if (pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_PB_FLT_RESET])
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.fault_reset.x] |= cab_bout_map.fault_reset.y;
        else
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.fault_reset.x] &= ~cab_bout_map.fault_reset.y;

        if (pOTEio->ote_umsg_in.body.pb_ope[ID_OTE_CHK_IL_BYPASS])
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.il_bypass.x] |= cab_bout_map.il_bypass.y;
        else
            plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.il_bypass.x] &= ~cab_bout_map.il_bypass.y;

        //�m�b�`
        if (pCSInf->auto_mode) {//AGENT �o�͂��Z�b�g;
            
        }
        else {//OTE�m�b�`�M���Z�b�g
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

        //���[�h�ݒ�
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
        //�[���������́A�劲OFFPB ����~�@���͏��
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_on.x] &= ~cab_bout_map.ctrl_on.y;
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.ctrl_off.x] &= ~cab_bout_map.ctrl_off.y;
        plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.cab_estp.x] &= ~cab_bout_map.cab_estp.y;

    }

    //���[�����g���~�b�^
    plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mlim_load_100.x] |= cab_bout_map.mlim_load_100.y;
    plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.mlim_normal.x] |= cab_bout_map.mlim_normal.y;
    plc_if_workbuf.output.wbuf.cab_di[cab_bout_map.whip_5t_ng.x] |= cab_bout_map.whip_5t_ng.y;
#pragma endregion OPEROOM

#pragma region ERM X
    //X60 �Ɍ��M��
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
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_anticol_stop.x] |= erm_xin_map.gt_anticol_stop.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_anticol_alarm.x] |= erm_xin_map.gt_anticol_alarm.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_anticol_fault.x] |= erm_xin_map.gt_anticol_fault.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_east_lim_slow.x] |= erm_xin_map.gt_east_lim_slow.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_east_lim_stop.x] |= erm_xin_map.gt_east_lim_stop.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_west_lim_slow.x] |= erm_xin_map.gt_west_lim_slow.y;
    //plc_if_workbuf.output.wbuf.erm_x[erm_xin_map.gt_west_lim_stop.x] |= erm_xin_map.gt_west_lim_stop.y;
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

    //XA0�@MC�@FB
    //����,�u���[�L�劲MC �R���o�[�^MC�@FB�@���ʎ劲�����w�߂�ON
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

    //�e���劲MC FB�@�劲�����w�߂�ON
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

    //�����[�d�w��
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

    //�����[�d����
    for (int i = 0; i < 6; i++) {
        plc_if_workbuf.output.wbuf.inv_cc_x[i] |= inv_x_map.charge_ok.y;
    }
    
    //�C���o�[�^
    //�g���NFB�͋[
    if (plc_if_workbuf.input.rbuf.inv_cc_Ww1[ID_MC_INV_MH1] & 0x3) {//���]or�t�] ON
        plc_if_workbuf.output.wbuf.inv_cc_Wr2[ID_MC_INV_MH1] = plc_if_workbuf.output.wbuf.inv_cc_Wr2[ID_MC_INV_MH2] = 500;
 //       INT16 imask = 0x3 | plc_if_workbuf.input.rbuf.inv_cc_Ww1[ID_MC_INV_MH1];
 //       plc_if_workbuf.output.wbuf.inv_cc_x[ID_MC_INV_MH1] |= imask;
 //       plc_if_workbuf.output.wbuf.inv_cc_x[ID_MC_INV_MH2] |= imask;
    }
    else {
        plc_if_workbuf.output.wbuf.inv_cc_Wr2[ID_MC_INV_MH1] = plc_if_workbuf.output.wbuf.inv_cc_Wr2[ID_MC_INV_MH2] = 0;
  //      plc_if_workbuf.output.wbuf.inv_cc_x[ID_MC_INV_MH1] &= 0xfffc;
  //      plc_if_workbuf.output.wbuf.inv_cc_x[ID_MC_INV_MH2] &= 0xfffc;
    }
    if (plc_if_workbuf.input.rbuf.inv_cc_Ww1[ID_MC_INV_AH] & 0x3) {//���]or�t�] ON
        plc_if_workbuf.output.wbuf.inv_cc_Wr2[ID_MC_INV_AH] = 500;
        INT16 imask = 0x3 | plc_if_workbuf.input.rbuf.inv_cc_Ww1[ID_MC_INV_AH];
        plc_if_workbuf.output.wbuf.inv_cc_x[ID_MC_INV_AH] |= imask;
    }
    else {
        plc_if_workbuf.output.wbuf.inv_cc_Wr2[ID_MC_INV_AH] = 0;
        plc_if_workbuf.output.wbuf.inv_cc_x[ID_MC_INV_AH] &= 0xfffc;
    }


#pragma endregion PLC_CC_LINK

     return 0;
}

int CPLC_IF::parse_ote_com() {
    //�^�]��PB
    //�m�b�`
    //�努
    UINT16 notch_code = get_notch_code(pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_HOIST]);
    notch_code = notch_code << cab_bout_map.notch_mh.y;
    lp_PLCwrite->cab_di[cab_bout_map.notch_mh.x] &= 0xffc0;
    lp_PLCwrite->cab_di[cab_bout_map.notch_mh.x] |= notch_code;

    //�⊪
    notch_code = get_notch_code(pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_AHOIST]);
    notch_code = notch_code << cab_bout_map.notch_ah.y;
    lp_PLCwrite->cab_di[cab_bout_map.notch_ah.x] &= 0x03ff;
    lp_PLCwrite->cab_di[cab_bout_map.notch_ah.x] |= notch_code;

    //���s
    notch_code = get_notch_code(pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_GANTRY]);
    notch_code = notch_code << cab_bout_map.notch_gt.y;
    lp_PLCwrite->cab_di[cab_bout_map.notch_gt.x] &= 0x81ff;
    lp_PLCwrite->cab_di[cab_bout_map.notch_gt.x] |= notch_code;

    //����
    notch_code = get_notch_code(pOTEio->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_BOOM_H]);
    notch_code = notch_code << cab_bout_map.notch_bh.y;
    lp_PLCwrite->cab_di[cab_bout_map.notch_bh.x] &= 0xffc0;
    lp_PLCwrite->cab_di[cab_bout_map.notch_bh.x] |= notch_code;

    //����
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
    //�r���@����~
     lp_PLCwrite->erm_x[erm_xin_map.leg_emr_stop.x] |= erm_xin_map.leg_emr_stop.y;

    return 0;
}

int CPLC_IF::parse() { 

    //PLC�������݃f�[�^�Z�b�g
    lp_PLCwrite->helthy = helthy_cnt++;   //### �w���V�[�M��

    //### PLC�����N���͂�|��
    parse_data_in();
   
    //### PLC,���v���Z�X�ւ̏o�͓��e��|��
    parse_data_out();
  
    //### ���u�[���o�͓��e��|��
    parse_ote_com();

    //### �V�~�����[�V�����̌��ʂ��o��
    parse_sim_status();
 
    //���L�������f�[�^�Z�b�g
    plc_if_workbuf.mode = this->mode;                   //���[�h�Z�b�g

    return 0; 
}
//*********************************************************************************************
// output()
//*********************************************************************************************

int CPLC_IF::output() { 
        
    plc_if_workbuf.output.wbuf.helthy = plc_if_workbuf.healthy_cnt++;
 
     //���L�������o�͏���
    if(out_size) { 
        memcpy_s(poutput, out_size, &plc_if_workbuf, out_size);
    }

    //PLC���M�̓��e���Z�b�g
    memcpy_s(lp_PLCwrite, sizeof(PLC_WRITE_BUF), &(plc_if_workbuf.output), sizeof(PLC_WRITE_BUF));

    return 0;
}

//*********************************************************************************************
// set_notch_ref()
// AGENT�^�X�N�̑��x�w�߂��m�b�`�ʒu�w�߂ɕϊ�����IO�o�͂�ݒ�
//*********************************************************************************************

//*********************************************************************************************
/// <summary>
/// get_notch_code(INT16 notch)
/// notch�ʒu -5�`5�ɉ������m�b�`�����̃r�b�g�p�^�[����Ԃ�
/// PLC�̊��t�ɉ�����bit�V�t�g����IO�o�b�t�@�ɃZ�b�g
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
// AGENT�^�X�N�̃A�i���O�w�߁A�w���V�[�M�����̏o�̓Z�b�g
//*********************************************************************************************
int CPLC_IF::set_ao_coms() {


    return 0;
}


//*********************************************************************************************
// parse_ope_com()
// �^�]������M����荞��
//*********************************************************************************************
int CPLC_IF::parse_ope_com() {

 
    return 0;
}

//*********************************************************************************************
// parse_brk_status()
// �u���[�L��ԓǂݍ��� �i���jOTE�`�F�b�N�p
//*********************************************************************************************
int CPLC_IF::parce_brk_status() {

    //#### OTE�`�F�b�N�p�@��

    return 0;
}

//*********************************************************************************************
// parse_sensor_fb()
// �Z���T�M����荞��
//*********************************************************************************************
int CPLC_IF::parse_sensor_fb() {

    return 0;
}
