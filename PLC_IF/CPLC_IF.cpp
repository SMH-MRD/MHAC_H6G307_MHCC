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

CPLC_IF::CPLC_IF(HWND _hWnd_parent) {

    hWnd_parent = _hWnd_parent;
    // ���L�������I�u�W�F�N�g�̃C���X�^���X��
    pPLCioObj = new CSharedMem;
    pCraneStatusObj = new CSharedMem;
    pSimulationStatusObj = new CSharedMem;
    pAgentInfObj = new CSharedMem;
    pCSInfObj = new CSharedMem;

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

    pCSInf = (LPST_CS_INFO)pCSInfObj->get_pMap();                                           //CS���L������


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

#if 0
    //CraneStat�����オ��҂�
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
    return 0;
}

int CPLC_IF::parse_ote_com() {
    return 0;
}

int CPLC_IF::parse_auto_com() {



    return 0;
}


int CPLC_IF::parse() { 

    //### PLC�����N���͂�|��
    parse_data_in();
  
    //### ���u�[���o�͓��e��|��
    parse_ote_com();
 
    //���L�������f�[�^�Z�b�g
    plc_if_workbuf.mode = this->mode;                   //���[�h�Z�b�g

    return 0; 
}
//*********************************************************************************************
// output()
//*********************************************************************************************

int CPLC_IF::output() { 
    //PLC�������݃f�[�^�Z�b�g
    lp_PLCwrite->helthy = helthy_cnt++;   //### �w���V�[�M��
 
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
