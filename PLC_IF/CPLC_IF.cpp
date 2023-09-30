#include "framework.h"
#include "CPLC_IF.h"
#include "PLC_IF.h"
#include "PLC_DEF.h"
#include <windowsx.h>
#include "Mdfunc.h"

extern ST_SPEC def_spec;
extern ST_KNL_MANAGE_SET    knl_manage_set;

CMCProtocol* pMCProtocol;  //MC�v���g�R���I�u�W�F�N�g:
INT16 bit_mask[16] = {BIT0,BIT1,BIT2,BIT3,BIT4,BIT5,BIT6,BIT7,BIT8,BIT9,BIT10,BIT11,BIT12,BIT13,BIT14,BIT15};

CABPLC_BOUT_MAP cab_bout_map;
ERMPLC_BOUT_MAP erm_bout_map;
ERMPLC_YOUT_MAP erm_yout_map;
ERMPLC_XIN_MAP  erm_xin_map;




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

int CPLC_IF::parse_data_in() {

    //��M�o�b�t�@�̓��e�����[�N�o�b�t�@�ɃR�s�[
    memcpy_s(&(plc_if_workbuf.input), sizeof(PLC_READ_BUF),lp_PLCread, sizeof(PLC_READ_BUF));

    return 0;
}

int CPLC_IF::parse_ote_com() {
    //�^�]��PB
    //�劲��
     if (pOTEio->ote_in.pb_ope[OTE_INDEX_PB_CTR_SOURCE])
        lp_PLCwrite->cab_di[cab_bout_map.ctrl_on.x] |= cab_bout_map.ctrl_on.y;
    else
        lp_PLCwrite->cab_di[cab_bout_map.ctrl_on.x] &= ~cab_bout_map.ctrl_on.y;

     //�劲��PB�@�펞ON
        lp_PLCwrite->cab_di[cab_bout_map.ctrl_on.x] |= cab_bout_map.ctrl_off.y;
 

     if (pOTEio->ote_in.pb_ope[OTE_INDEX_PB_FAULT_RESET])
         lp_PLCwrite->cab_di[cab_bout_map.fault_reset.x] |= cab_bout_map.fault_reset.y;
     else
         lp_PLCwrite->cab_di[cab_bout_map.fault_reset.x] &= ~cab_bout_map.fault_reset.y;

    if (pOTEio->ote_in.pb_ope[OTE_INDEX_CHK_ESTOP])//�ً}��~�̓m�[�}���N���[�Y
        lp_PLCwrite->cab_di[cab_bout_map.cab_estp.x] &= ~cab_bout_map.cab_estp.y;
    else
     lp_PLCwrite->cab_di[cab_bout_map.cab_estp.x] |= cab_bout_map.cab_estp.y;

    //�m�b�`
    //�努
    UINT16 notch_code = get_notch_code(pOTEio->ote_in.notch_pos[ID_HOIST]);
    notch_code = notch_code << cab_bout_map.notch_mh.y;
    lp_PLCwrite->cab_di[cab_bout_map.notch_mh.x] &= 0xffc0;
    lp_PLCwrite->cab_di[cab_bout_map.notch_mh.x] |= notch_code;

    //�⊪
    notch_code = get_notch_code(pOTEio->ote_in.notch_pos[ID_AHOIST]);
    notch_code = notch_code << cab_bout_map.notch_ah.y;
    lp_PLCwrite->cab_di[cab_bout_map.notch_ah.x] &= 0x03ff;
    lp_PLCwrite->cab_di[cab_bout_map.notch_ah.x] |= notch_code;

    //���s
    notch_code = get_notch_code(pOTEio->ote_in.notch_pos[ID_GANTRY]);
    notch_code = notch_code << cab_bout_map.notch_gt.y;
    lp_PLCwrite->cab_di[cab_bout_map.notch_gt.x] &= 0x81ff;
    lp_PLCwrite->cab_di[cab_bout_map.notch_gt.x] |= notch_code;

    //����
    notch_code = get_notch_code(pOTEio->ote_in.notch_pos[ID_BOOM_H]);
    notch_code = notch_code << cab_bout_map.notch_bh.y;
    lp_PLCwrite->cab_di[cab_bout_map.notch_bh.x] &= 0xffc0;
    lp_PLCwrite->cab_di[cab_bout_map.notch_bh.x] |= notch_code;

    //����
    notch_code = get_notch_code(pOTEio->ote_in.notch_pos[ID_SLEW]);
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
 
     //���L�������o�͏���
    if(out_size) { 
        memcpy_s(poutput, out_size, &plc_if_workbuf, out_size);
    }
    return 0;
}

//*********************************************************************************************
// set_notch_ref()
// AGENT�^�X�N�̑��x�w�߂��m�b�`�ʒu�w�߂ɕϊ�����IO�o�͂�ݒ�
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
//CS�����v�\��, AGENT�^�X�N, ����p�l���̃r�b�g�w�߂ɉ�����IO�o�͂�ݒ�
//*********************************************************************************************
int CPLC_IF::set_bit_coms() {
 

    return 0;
}

//*********************************************************************************************
//set_ao_coms()
// AGENT�^�X�N�̃A�i���O�w�߁A�w���V�[�M�����̏o�̓Z�b�g
//*********************************************************************************************
int CPLC_IF::set_ao_coms() {


    return 0;
}

//*********************************************************************************************
// parse_notch_com()
// UI�m�b�`�w�ߓǂݍ���
//*********************************************************************************************
int CPLC_IF::parse_notch_com() {
    
    INT16 check_i;

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
