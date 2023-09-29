#include "framework.h"
#include "COTE_IF.h"
#include <windowsx.h>       //# �R�����R���g���[��
#include <winsock2.h>

#include <iostream>
#include <iomanip>
#include <sstream>

#include "OTEpanel.H"

std::wstring COteIF::msg_ws;
std::wostringstream COteIF::msg_wos;

ST_OTE_IF_WND COteIF::st_work_wnd;
ST_OTE_IO COteIF::ote_io_workbuf;

bool COteIF::is_my_ote_active;
HWND COteIF::hWnd_parent;		//�e�E�B���h�E�̃n���h��
HWND COteIF::hWnd_if; 			//�ʐM�C�x���g�����p�E�B���h�E�n���h��
HWND COteIF::hWnd_sub[OTE_N_SUB_WND];//OTE�p�l���p�E�B���h�E�n���h��
HWND COteIF::hWnd_ifchk = NULL;			//�ʐM���j�^�E�B���h�E�n���h��

CSockAlpha* COteIF::pSockOteUniCastPc;		//PC���j�L���X�g�p�\�P�b�g
CSockAlpha* COteIF::pSockPcMultiCastPc;		//PC�}���`�L���X�gPC��M�p�\�P�b�g
CSockAlpha* COteIF::pSockOteMultiCastPc;	//OTE�}���`�L���X�gPC��M�p�\�P�b�g
CSockAlpha* COteIF::pSockPcUniCastOte;		//OTE���j�L���X�g�p�\�P�b�g
CSockAlpha* COteIF::pSockPcMultiCastOte;	//PC�}���`�L���X�gOTE��M�p�\�P�b�g
CSockAlpha* COteIF::pSockOteMultiCastOte;	//OTE�}���`�L���X�gOTE��M�p�\�P�b�g

SOCKADDR_IN COteIF::addrin_ote_u_pc;		//PC���j�L���X�g�p�A�h���X(PC��M�p)
SOCKADDR_IN COteIF::addrin_pc_m_pc;			//PC�}���`�L���X�g��M�A�h���X(PC��M�p)
SOCKADDR_IN COteIF::addrin_ote_m_pc;		//OTE�}���`�L���X�g��M�A�h���X(PC��M�p)
SOCKADDR_IN COteIF::addrin_pc_u_ote;		//OTE���j�L���X�g�p�A�h���X(OTE��M�p)
SOCKADDR_IN COteIF::addrin_pc_m_ote;		//PC�}���`�L���X�g��M�A�h���X(OTE��M�p)
SOCKADDR_IN COteIF::addrin_ote_m_ote;		//OTE�}���`�L���X�g��M�A�h���X(OTE��M�p)

SOCKADDR_IN COteIF::addrin_pc_m_pc_snd;		//PC��PC�}���`�L���X�g���M�A�h���X
SOCKADDR_IN COteIF::addrin_pc_m_ote_snd;	//PC��OTE�}���`�L���X�g��M�A�h���X
SOCKADDR_IN COteIF::addrin_ote_m_ote_snd;	//PC�}���`�L���X�g��M�A�h���X

SOCKADDR_IN COteIF::addr_active_ote;			//����M�����L���Ȓ[���̃A�h���X

LONG COteIF::cnt_snd_pc_u, COteIF::cnt_snd_pc_m_pc, COteIF::cnt_snd_pc_m_ote, COteIF::cnt_snd_ote_u, COteIF::cnt_snd_ote_m_pc, COteIF::cnt_snd_ote_m_ote;
LONG COteIF::cnt_rcv_pc_u, COteIF::cnt_rcv_pc_m_pc, COteIF::cnt_rcv_pc_m_ote, COteIF::cnt_rcv_ote_u, COteIF::cnt_rcv_ote_m_pc, COteIF::cnt_rcv_ote_m_ote;


ST_PC_U_MSG COteIF::st_msg_pc_u_snd;
ST_PC_M_MSG COteIF::st_msg_pc_m_snd;

ST_OTE_U_MSG COteIF::st_msg_ote_u_snd;
ST_OTE_M_MSG COteIF::st_msg_ote_m_snd;

ST_PC_M_MSG COteIF::st_msg_pc_m_pc_rcv;
ST_PC_M_MSG COteIF::st_msg_pc_m_ote_rcv;
ST_OTE_U_MSG COteIF::st_msg_pc_u_rcv;

ST_OTE_M_MSG COteIF::st_msg_ote_m_pc_rcv;
ST_OTE_M_MSG COteIF::st_msg_ote_m_ote_rcv;
ST_PC_U_MSG COteIF::st_msg_ote_u_rcv;

ST_OTE_U_MSG COteIF::st_ote_active_msg;	//����M�����L���Ȍ����b�Z�[�W

/*****************************************************************************/
/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="hWnd">�e�E�B���h�E�̃n���h��</param>
COteIF::COteIF(HWND hWnd) {
    hWnd_parent = hWnd;
    // ���L�������I�u�W�F�N�g�̃C���X�^���X��
    pOteIOObj = new CSharedMem;
    pCraneStatusObj = new CSharedMem;
    pSimulationStatusObj = new CSharedMem;
    pPLCioObj = new CSharedMem;
    pCSInfObj = new CSharedMem;
    pAgentInfObj = new CSharedMem;
    pSwayIO_Obj = new CSharedMem;

	cnt_snd_pc_u = cnt_snd_pc_m_pc = cnt_snd_pc_m_ote = cnt_snd_ote_u = cnt_snd_ote_m_pc = cnt_snd_ote_m_ote = 0;
	cnt_rcv_pc_u = cnt_rcv_pc_m_pc = cnt_rcv_pc_m_ote = cnt_rcv_ote_u = cnt_rcv_ote_m_pc = cnt_rcv_ote_m_ote = 0;

	is_my_ote_active = true;
 };
/*****************************************************************************/
/// <summary>
/// �f�X�g���N�^
/// </summary>
COteIF::~COteIF() {
    // ���L�������I�u�W�F�N�g�̉��
    delete pOteIOObj;
    delete pCraneStatusObj;
    delete pSimulationStatusObj;
    delete pPLCioObj;
    delete pCSInfObj;
    delete pAgentInfObj;
    delete pSwayIO_Obj;
};
/*****************************************************************************/
/// <summary>
/// �o�̓o�b�t�@�̃A�h���X���Z�b�g�i�I�[�o�[���C�h�֐��j
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
int COteIF::set_outbuf(LPVOID pbuf) { 
    poutput = pbuf;return 0;
    return 0; 

}    //�o�̓o�b�t�@�Z�b�g
/*****************************************************************************/
/// <summary>
/// �����������i�I�[�o�[���C�h�֐��j
/// </summary>
/// <returns></returns>
int COteIF::init_proc() {
    // ���L�������擾
	{
		// �o�͗p���L�������擾
		out_size = sizeof(ST_OTE_IO);
		if (OK_SHMEM != pOteIOObj->create_smem(SMEM_OTE_IO_NAME, out_size, MUTEX_OTE_IO_NAME)) {
			mode |= OTE_IF_OTE_IO_MEM_NG;
		}
		set_outbuf(pOteIOObj->get_pMap());

		// ���͗p���L�������擾
		if (OK_SHMEM != pSimulationStatusObj->create_smem(SMEM_SIMULATION_STATUS_NAME, sizeof(ST_SIMULATION_STATUS), MUTEX_SIMULATION_STATUS_NAME)) {
			mode |= OTE_IF_SIM_MEM_NG;
		}

		if (OK_SHMEM != pCraneStatusObj->create_smem(SMEM_CRANE_STATUS_NAME, sizeof(ST_CRANE_STATUS), MUTEX_CRANE_STATUS_NAME)) {
			mode |= OTE_IF_CRANE_MEM_NG;
		}

		if (OK_SHMEM != pPLCioObj->create_smem(SMEM_PLC_IO_NAME, sizeof(ST_PLC_IO), MUTEX_PLC_IO_NAME)) {
			mode |= OTE_IF_PLC_MEM_NG;
		}

		if (OK_SHMEM != pCSInfObj->create_smem(SMEM_CS_INFO_NAME, sizeof(ST_CS_INFO), MUTEX_CS_INFO_NAME)) {
			mode |= OTE_IF_PLC_MEM_NG;
		}

		if (OK_SHMEM != pAgentInfObj->create_smem(SMEM_AGENT_INFO_NAME, sizeof(ST_AGENT_INFO), MUTEX_AGENT_INFO_NAME)) {
			mode |= OTE_IF_PLC_MEM_NG;
		}

		pCraneStat = (LPST_CRANE_STATUS)(pCraneStatusObj->get_pMap());
		pPLCio = (LPST_PLC_IO)(pPLCioObj->get_pMap());
		pOTEio = (LPST_OTE_IO)pOteIOObj->get_pMap();
		pCSInf = (LPST_CS_INFO)pCSInfObj->get_pMap();
		pAgentInf = (LPST_AGENT_INFO)pAgentInfObj->get_pMap();
		pSway_IO = (LPST_SWAY_IO)(pSwayIO_Obj->get_pMap());
	}
    
	//�ʐM�C�x���g�����p�E�B���h�E�I�[�v��
	hWnd_if = open_work_Wnd(hWnd_parent);					//�C�x���g�����E�B���hOPEN

	open_connect_Wnd(hWnd_if);
	open_auto_Wnd(hWnd_if);
	open_mode_Wnd(hWnd_if);
	open_fault_Wnd(hWnd_if);
	open_moment_Wnd(hWnd_if);

	//�T�u�E�B���h�E�@�ڑ��\��
	ote_io_workbuf.ote_in.sub_monitor_mode = OTE_INDEX_RADIO_CONNECT;
	InvalidateRect(hWnd_sub[OTE_INDEX_RADIO_CONNECT], NULL, TRUE);//�\���X�V
	ShowWindow(hWnd_sub[OTE_INDEX_RADIO_CONNECT], SW_SHOW);
	UpdateWindow(hWnd_sub[OTE_INDEX_RADIO_CONNECT]);
	

	//### �\�P�b�g�A�h���X�Z�b�g
	memset(&addrin_ote_u_pc	, 0, sizeof(SOCKADDR_IN));	memset(&addrin_pc_m_pc, 0, sizeof(SOCKADDR_IN));memset(&addrin_ote_m_pc, 0, sizeof(SOCKADDR_IN));
	memset(&addrin_pc_u_ote, 0, sizeof(SOCKADDR_IN));	memset(&addrin_pc_m_ote, 0, sizeof(SOCKADDR_IN)); memset(&addrin_ote_m_ote, 0, sizeof(SOCKADDR_IN));
	
	set_sock_addr(&addrin_ote_u_pc,			IP_ADDR_OTE_UNI_CAST_PC, OTE_IF_UNICAST_PORT_PC			);
	set_sock_addr(&addrin_pc_m_pc,			IP_ADDR_OTE_UNI_CAST_PC, OTE_IF_PC_MULTICAST_PORT_PC	);
	set_sock_addr(&addrin_ote_m_pc,			IP_ADDR_OTE_UNI_CAST_PC, OTE_IF_OTE_MULTICAST_PORT_PC	);
	set_sock_addr(&addrin_pc_u_ote,			IP_ADDR_OTE_UNI_CAST_PC, OTE_IF_UNICAST_PORT_OTE		);
	set_sock_addr(&addrin_pc_m_ote,			IP_ADDR_OTE_UNI_CAST_PC, OTE_IF_PC_MULTICAST_PORT_OTE	);
	set_sock_addr(&addrin_ote_m_ote,		IP_ADDR_OTE_UNI_CAST_PC, OTE_IF_OTE_MULTICAST_PORT_OTE	);

	set_sock_addr(&addrin_pc_m_pc_snd , IP_ADDR_OTE_UNI_CAST_PC, OTE_IF_PC_MULTICAST_PORT_PC);
	set_sock_addr(&addrin_pc_m_ote_snd , IP_ADDR_OTE_UNI_CAST_PC, OTE_IF_PC_MULTICAST_PORT_OTE);
	set_sock_addr(&addrin_ote_m_ote_snd , IP_ADDR_OTE_UNI_CAST_PC, OTE_IF_OTE_MULTICAST_PORT_OTE);

	//### �\�P�b�g�ݒ�

	pSockOteUniCastPc	= new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_OTE_UNI_PC);
	pSockPcMultiCastPc	= new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_PC_MULTI_PC);
	pSockOteMultiCastPc = new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_OTE_MULTI_PC);
	pSockPcUniCastOte	= new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_PC_UNI_OTE);
	pSockPcMultiCastOte = new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_PC_MULTI_OTE);
	pSockOteMultiCastOte= new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_OTE_MULTI_OTE);

	//### �I�u�W�F�N�g������
	if (pSockOteUniCastPc->Initialize()		!= S_OK) { msg_wos.str() = pSockOteUniCastPc->err_msg.str();	return NULL; }
	if (pSockPcMultiCastPc->Initialize()	!= S_OK) { msg_wos.str() = pSockPcMultiCastPc->err_msg.str();	return NULL; }
	if (pSockOteMultiCastPc->Initialize()	!= S_OK) { msg_wos.str() = pSockOteMultiCastPc->err_msg.str();	return NULL; }
	if (pSockPcUniCastOte->Initialize()		!= S_OK) { msg_wos.str() = pSockPcUniCastOte->err_msg.str();	return NULL; }
	if (pSockPcMultiCastOte->Initialize()	!= S_OK) { msg_wos.str() = pSockPcMultiCastOte->err_msg.str();	return NULL; }
	if (pSockOteMultiCastOte->Initialize()	!= S_OK) { msg_wos.str() = pSockOteMultiCastOte->err_msg.str(); return NULL; }

	//### �\�P�b�g�ݒ�
	//���j�L���X�g�p
	if (pSockOteUniCastPc->init_sock(hWnd_if,	addrin_ote_u_pc		)	!= S_OK) { msg_wos.str() = pSockOteUniCastPc->err_msg.str(); return NULL;}
	if (pSockPcUniCastOte->init_sock(hWnd_if, addrin_pc_u_ote) != S_OK) { msg_wos.str() = pSockPcUniCastOte->err_msg.str(); return NULL; }
	//�}���`�L���X�g�p
	SOCKADDR_IN addr_tmp;
	set_sock_addr(&addr_tmp, IP_ADDR_OTE_MULTI_CAST_PC, NULL);
	if (pSockPcMultiCastPc->init_sock_m(hWnd_if, addrin_pc_m_pc, addr_tmp) != S_OK) { msg_wos.str() = pSockPcMultiCastPc->err_msg.str(); return NULL; }
	if (pSockOteMultiCastPc->init_sock_m(hWnd_if, addrin_ote_m_pc, addr_tmp) != S_OK) { msg_wos.str() = pSockOteMultiCastPc->err_msg.str(); return NULL; }
	if (pSockPcMultiCastOte->init_sock_m(hWnd_if, addrin_pc_m_ote, addr_tmp) != S_OK) { msg_wos.str() = pSockPcMultiCastOte->err_msg.str(); return NULL; }
	if (pSockOteMultiCastOte->init_sock_m(hWnd_if, addrin_ote_m_ote, addr_tmp) != S_OK) { msg_wos.str() = pSockOteMultiCastOte->err_msg.str(); return NULL; }

	//���M���b�Z�[�W�w�b�_�ݒ�

	st_msg_pc_u_snd.head.addr = addrin_ote_u_pc;
	st_msg_pc_m_snd.head.addr = addrin_ote_m_pc;

	st_msg_ote_u_snd.head.addr = addrin_pc_u_ote;
	st_msg_ote_m_snd.head.addr = addrin_pc_m_ote;

	return 0; 
}
/*****************************************************************************/
/// <summary>
/// ���͏����i�I�[�o�[���C�h�֐��j
/// </summary>
/// <returns></returns>
int COteIF::input() {

      return 0; 
} 
/*****************************************************************************/
/// <summary>
///  ��͏����i�I�[�o�[���C�h�֐��j
/// </summary>
/// <returns></returns>
int COteIF::parse() { 
	//���j�L���X�g�ʐM�f�[�^�����[�N�G���A�ɃR�s�[
	memcpy_s(&ote_io_workbuf.ote_in, sizeof(ST_OTE_U_BODY), &st_msg_ote_u_snd.body, sizeof(ST_OTE_U_BODY));
	memcpy_s(&ote_io_workbuf.ote_out, sizeof(ST_PC_U_BODY), &st_msg_pc_u_snd.body, sizeof(ST_PC_U_BODY));
    return 0;
}    
/*****************************************************************************/
/// <summary>
/// �o�͉�͏����i�I�[�o�[���C�h�֐��j
/// </summary>
/// <returns></returns>
int COteIF::output() {                          //�o�͏���

   if (out_size) memcpy_s(poutput, out_size, &ote_io_workbuf, out_size);
   return 0; 
}
/*****************************************************************************/
/// <summary>
/// PC���j�L���X�g���b�Z�[�W���M�o�b�t�@�Z�b�g
/// </summary>
/// <returns></returns>
LPST_PC_U_MSG COteIF:: set_msg_pc_u() {
#if 0 
   //Header��
    if (mode) {
        ote_io_workbuf.ote_io.snd_msg_u.head.addr = addrin_u;
        ote_io_workbuf.ote_io.snd_msg_u.head.myid = pCraneStat->spec.device_code.no;
    }
    ote_io_workbuf.ote_io.snd_msg_u.head.tgid = ote_io_workbuf.id_connected_te;;
    ote_io_workbuf.ote_io.snd_msg_u.head.code = code;
    ote_io_workbuf.ote_io.snd_msg_u.head.status = ote_io_workbuf.status_connected_te;

    //Body��
    //�����v
    for (int i = 0;i < N_UI_LAMP;i++) ote_io_workbuf.ote_io.snd_msg_u.body.lamp[i] = pCSInf->ui_lamp[i];
    //�m�b�`�w��
    for (int i = 0;i < MOTION_ID_MAX;i++) ote_io_workbuf.ote_io.snd_msg_u.body.notch_pos[i] = pPLCio->status.notch_ref[i];
    //�e���ʒu
    for (int i = 0;i < MOTION_ID_MAX;i++) ote_io_workbuf.ote_io.snd_msg_u.body.pos[i] = (INT32)(pPLCio->status.pos[i] * 1000.0);
    //�e�����xFB
    for (int i = 0;i < MOTION_ID_MAX;i++) ote_io_workbuf.ote_io.snd_msg_u.body.v_fb[i] = (INT32)(pPLCio->status.v_fb[i] * 1000.0);
    //�e�����x�w��
    for (int i = 0;i < MOTION_ID_MAX;i++) ote_io_workbuf.ote_io.snd_msg_u.body.v_ref[i] = (INT32)(pPLCio->status.v_ref[i] * 1000.0);

    //�ݓ_�ʒu
    ote_io_workbuf.ote_io.snd_msg_u.body.hp_pos[0] = pCSInf->hunging_point_for_view[0];
    ote_io_workbuf.ote_io.snd_msg_u.body.hp_pos[1] = pCSInf->hunging_point_for_view[1];
    ote_io_workbuf.ote_io.snd_msg_u.body.hp_pos[2] = pCSInf->hunging_point_for_view[2];

    //�݉׈ʒu(�ݓ_�Ƃ̑��Έʒu�j
    ote_io_workbuf.ote_io.snd_msg_u.body.ld_pos[0] = (INT32)(pSway_IO->th[ID_SLEW]* 1000.0);
    ote_io_workbuf.ote_io.snd_msg_u.body.ld_pos[1] = (INT32)(pSway_IO->th[ID_BOOM_H] * 1000.0);
    ote_io_workbuf.ote_io.snd_msg_u.body.ld_pos[2] = (INT32)(pCraneStat->mh_l*1000.0);

    //�݉ב��x
    ote_io_workbuf.ote_io.snd_msg_u.body.ld_v_fb[0] = (INT32)(pSway_IO->dth[ID_SLEW] * 1000.0);
    ote_io_workbuf.ote_io.snd_msg_u.body.ld_v_fb[1] = (INT32)(pSway_IO->dth[ID_BOOM_H] * 1000.0);
    ote_io_workbuf.ote_io.snd_msg_u.body.ld_v_fb[2] = (INT32)(pPLCio->status.v_fb[ID_HOIST] * 1000.0);

    //�����ڕW�ʒu
    double tg_x_rad, tg_x_m, tg_y_rad, tg_y_m,h;

        h = pCSInf->ote_camera_height_m;
        tg_x_m = pCSInf->semi_auto_selected_target.pos[ID_BOOM_H] * cos(pCSInf->semi_auto_selected_target.pos[ID_SLEW]);
        tg_x_rad = tg_x_m / h;
        tg_y_m = pCSInf->semi_auto_selected_target.pos[ID_BOOM_H] * sin(pCSInf->semi_auto_selected_target.pos[ID_SLEW]);
        tg_y_rad = tg_y_m / h;

        ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos[0] = (INT32)(tg_x_rad * 1000.0);
        ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos[1] = (INT32)(tg_y_rad * 1000.0);
        ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos[2] = (INT32)(pCSInf->semi_auto_selected_target.pos[ID_HOIST] * 1000.0);
  
    //�������ڕW�ʒu
    for (int i = 0;i < 6;i++) {
        ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos_semi[i][0] = (INT32)(pCSInf->semi_auto_setting_target[i].pos[ID_BOOM_H] * 1000.0);
        ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos_semi[i][1] = (INT32)(pCSInf->semi_auto_setting_target[i].pos[ID_SLEW] * 1000.0);
        ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos_semi[i][2] = (INT32)(pCSInf->semi_auto_setting_target[i].pos[ID_HOIST] * 1000.0);
    }

    //VIEW�J�����Z�b�g����
    ote_io_workbuf.ote_io.snd_msg_u.body.cam_inf[ID_OTE_CAMERA_HEIGHT] = (INT16)(pCraneStat->spec.boom_high * 1000.0);

    ote_io_workbuf.ote_io.snd_msg_u.body.lamp[ID_LAMP_OTE_NOTCH_MODE] = ote_io_workbuf.ote_io.rcv_msg_u.body.pb[ID_LAMP_OTE_NOTCH_MODE];

    //PLC�f�[�^
    for (int i = 0;i < PLC_IO_MONT_WORD_NUM;i++) ote_io_workbuf.ote_io.snd_msg_u.body.plc_data[i] = pPLCio->plc_data[i];
#endif  
    return &st_msg_pc_u_snd;
} 
/*****************************************************************************/
/// <summary>
/// PC�}���`�L���X�g���b�Z�[�W���M�o�b�t�@�Z�b�g
/// </summary>
/// <returns></returns>
LPST_PC_M_MSG COteIF::set_msg_pc_m() {
#if 0
    //���M���b�Z�[�W�Z�b�g
    int n = sizeof(ST_MOTE_SND_MSG);
    nRtn = sendto(s_m_snd, reinterpret_cast<const char*> (&ote_io_workbuf.ote_io.snd_msg_m), n, 0, (LPSOCKADDR)&addrin_m_snd, sizeof(addrin_m_snd));
    nRtn = sendto(s_m_snd_dbg, reinterpret_cast<const char*> (&ote_io_workbuf.ote_io.snd_msg_m), n, 0, (LPSOCKADDR)&addrin_m_snd, sizeof(addrin_m_snd));
    woMSG.str(L"");
    if (nRtn == n) {
        nSnd_m++;
        lSnd_m = nRtn;
        woMSG << L"Rcv n:" << nRcv_u << L" l:" << lRcv_u << L"  Snd n:" << nSnd_u << L" l:" << lSnd_u;
        tweet2infMSG(woMSG.str(), ID_SOCK_CODE_CR); woMSG.str(L"");woMSG.clear();

        woMSG << L"ID:" << ote_io_workbuf.ote_io.snd_msg_m.head.myid << L" CD:" << ote_io_workbuf.ote_io.snd_msg_m.head.code;
        sockaddr_in* psockaddr = (sockaddr_in*)&ote_io_workbuf.ote_io.snd_msg_m.head.addr;
        woMSG << L" IP:" << psockaddr->sin_addr.S_un.S_un_b.s_b1 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b2 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b3 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b4;
        woMSG << L" PORT: " << htons(psockaddr->sin_port);
        woMSG << L" ST:" << ote_io_workbuf.ote_io.snd_msg_m.head.status << L" ID:" << ote_io_workbuf.ote_io.snd_msg_m.head.tgid;
        tweet2sndMSG(woMSG.str(), ID_SOCK_CODE_CR); woMSG.str(L"");woMSG.clear();

    }
    else if (nRtn == SOCKET_ERROR) {
        woMSG << L"ERR CODE ->" << WSAGetLastError();
        tweet2sndMSG(woMSG.str(), ID_SOCK_CODE_CR); woMSG.str(L"");woMSG.clear();
    }
    else {
        woMSG << L" sendto size ERROR ";
        tweet2sndMSG(woMSG.str(), ID_SOCK_CODE_CR); woMSG.str(L"");woMSG.clear();
    }
#endif
    return &st_msg_pc_m_snd;
}
/*****************************************************************************/
/// <summary>
/// OTE���j�L���X�g���b�Z�[�W���M�o�b�t�@�Z�b�g
/// </summary>
/// <returns></returns>
LPST_OTE_U_MSG COteIF::set_msg_ote_u() {
#if 0 
	//Header��
	if (mode) {
		ote_io_workbuf.ote_io.snd_msg_u.head.addr = addrin_u;
		ote_io_workbuf.ote_io.snd_msg_u.head.myid = pCraneStat->spec.device_code.no;
	}
	ote_io_workbuf.ote_io.snd_msg_u.head.tgid = ote_io_workbuf.id_connected_te;;
	ote_io_workbuf.ote_io.snd_msg_u.head.code = code;
	ote_io_workbuf.ote_io.snd_msg_u.head.status = ote_io_workbuf.status_connected_te;

	//Body��
	//�����v
	for (int i = 0; i < N_UI_LAMP; i++) ote_io_workbuf.ote_io.snd_msg_u.body.lamp[i] = pCSInf->ui_lamp[i];
	//�m�b�`�w��
	for (int i = 0; i < MOTION_ID_MAX; i++) ote_io_workbuf.ote_io.snd_msg_u.body.notch_pos[i] = pPLCio->status.notch_ref[i];
	//�e���ʒu
	for (int i = 0; i < MOTION_ID_MAX; i++) ote_io_workbuf.ote_io.snd_msg_u.body.pos[i] = (INT32)(pPLCio->status.pos[i] * 1000.0);
	//�e�����xFB
	for (int i = 0; i < MOTION_ID_MAX; i++) ote_io_workbuf.ote_io.snd_msg_u.body.v_fb[i] = (INT32)(pPLCio->status.v_fb[i] * 1000.0);
	//�e�����x�w��
	for (int i = 0; i < MOTION_ID_MAX; i++) ote_io_workbuf.ote_io.snd_msg_u.body.v_ref[i] = (INT32)(pPLCio->status.v_ref[i] * 1000.0);

	//�ݓ_�ʒu
	ote_io_workbuf.ote_io.snd_msg_u.body.hp_pos[0] = pCSInf->hunging_point_for_view[0];
	ote_io_workbuf.ote_io.snd_msg_u.body.hp_pos[1] = pCSInf->hunging_point_for_view[1];
	ote_io_workbuf.ote_io.snd_msg_u.body.hp_pos[2] = pCSInf->hunging_point_for_view[2];

	//�݉׈ʒu(�ݓ_�Ƃ̑��Έʒu�j
	ote_io_workbuf.ote_io.snd_msg_u.body.ld_pos[0] = (INT32)(pSway_IO->th[ID_SLEW] * 1000.0);
	ote_io_workbuf.ote_io.snd_msg_u.body.ld_pos[1] = (INT32)(pSway_IO->th[ID_BOOM_H] * 1000.0);
	ote_io_workbuf.ote_io.snd_msg_u.body.ld_pos[2] = (INT32)(pCraneStat->mh_l * 1000.0);

	//�݉ב��x
	ote_io_workbuf.ote_io.snd_msg_u.body.ld_v_fb[0] = (INT32)(pSway_IO->dth[ID_SLEW] * 1000.0);
	ote_io_workbuf.ote_io.snd_msg_u.body.ld_v_fb[1] = (INT32)(pSway_IO->dth[ID_BOOM_H] * 1000.0);
	ote_io_workbuf.ote_io.snd_msg_u.body.ld_v_fb[2] = (INT32)(pPLCio->status.v_fb[ID_HOIST] * 1000.0);

	//�����ڕW�ʒu
	double tg_x_rad, tg_x_m, tg_y_rad, tg_y_m, h;

	h = pCSInf->ote_camera_height_m;
	tg_x_m = pCSInf->semi_auto_selected_target.pos[ID_BOOM_H] * cos(pCSInf->semi_auto_selected_target.pos[ID_SLEW]);
	tg_x_rad = tg_x_m / h;
	tg_y_m = pCSInf->semi_auto_selected_target.pos[ID_BOOM_H] * sin(pCSInf->semi_auto_selected_target.pos[ID_SLEW]);
	tg_y_rad = tg_y_m / h;

	ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos[0] = (INT32)(tg_x_rad * 1000.0);
	ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos[1] = (INT32)(tg_y_rad * 1000.0);
	ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos[2] = (INT32)(pCSInf->semi_auto_selected_target.pos[ID_HOIST] * 1000.0);

	//�������ڕW�ʒu
	for (int i = 0; i < 6; i++) {
		ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos_semi[i][0] = (INT32)(pCSInf->semi_auto_setting_target[i].pos[ID_BOOM_H] * 1000.0);
		ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos_semi[i][1] = (INT32)(pCSInf->semi_auto_setting_target[i].pos[ID_SLEW] * 1000.0);
		ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos_semi[i][2] = (INT32)(pCSInf->semi_auto_setting_target[i].pos[ID_HOIST] * 1000.0);
	}

	//VIEW�J�����Z�b�g����
	ote_io_workbuf.ote_io.snd_msg_u.body.cam_inf[ID_OTE_CAMERA_HEIGHT] = (INT16)(pCraneStat->spec.boom_high * 1000.0);

	ote_io_workbuf.ote_io.snd_msg_u.body.lamp[ID_LAMP_OTE_NOTCH_MODE] = ote_io_workbuf.ote_io.rcv_msg_u.body.pb[ID_LAMP_OTE_NOTCH_MODE];

	//PLC�f�[�^
	for (int i = 0; i < PLC_IO_MONT_WORD_NUM; i++) ote_io_workbuf.ote_io.snd_msg_u.body.plc_data[i] = pPLCio->plc_data[i];
#endif  
	return &st_msg_ote_u_snd;
}
/*****************************************************************************/
/// <summary>
/// OTE�}���`�L���X�g���b�Z�[�W���M�o�b�t�@�Z�b�g
/// </summary>
/// <returns></returns>
LPST_OTE_M_MSG COteIF::set_msg_ote_m() {
#if 0
	//���M���b�Z�[�W�Z�b�g
	int n = sizeof(ST_MOTE_SND_MSG);
	nRtn = sendto(s_m_snd, reinterpret_cast<const char*> (&ote_io_workbuf.ote_io.snd_msg_m), n, 0, (LPSOCKADDR)&addrin_m_snd, sizeof(addrin_m_snd));
	nRtn = sendto(s_m_snd_dbg, reinterpret_cast<const char*> (&ote_io_workbuf.ote_io.snd_msg_m), n, 0, (LPSOCKADDR)&addrin_m_snd, sizeof(addrin_m_snd));
	woMSG.str(L"");
	if (nRtn == n) {
		nSnd_m++;
		lSnd_m = nRtn;
		woMSG << L"Rcv n:" << nRcv_u << L" l:" << lRcv_u << L"  Snd n:" << nSnd_u << L" l:" << lSnd_u;
		tweet2infMSG(woMSG.str(), ID_SOCK_CODE_CR); woMSG.str(L""); woMSG.clear();

		woMSG << L"ID:" << ote_io_workbuf.ote_io.snd_msg_m.head.myid << L" CD:" << ote_io_workbuf.ote_io.snd_msg_m.head.code;
		sockaddr_in* psockaddr = (sockaddr_in*)&ote_io_workbuf.ote_io.snd_msg_m.head.addr;
		woMSG << L" IP:" << psockaddr->sin_addr.S_un.S_un_b.s_b1 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b2 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b3 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b4;
		woMSG << L" PORT: " << htons(psockaddr->sin_port);
		woMSG << L" ST:" << ote_io_workbuf.ote_io.snd_msg_m.head.status << L" ID:" << ote_io_workbuf.ote_io.snd_msg_m.head.tgid;
		tweet2sndMSG(woMSG.str(), ID_SOCK_CODE_CR); woMSG.str(L""); woMSG.clear();

	}
	else if (nRtn == SOCKET_ERROR) {
		woMSG << L"ERR CODE ->" << WSAGetLastError();
		tweet2sndMSG(woMSG.str(), ID_SOCK_CODE_CR); woMSG.str(L""); woMSG.clear();
	}
	else {
		woMSG << L" sendto size ERROR ";
		tweet2sndMSG(woMSG.str(), ID_SOCK_CODE_CR); woMSG.str(L""); woMSG.clear();
	}
#endif
	return &st_msg_ote_m_snd;
}
//****************************************************************************�@
/// <summary>
/// �N���[�Y����
/// </summary>
/// <returns></returns>
HRESULT COteIF::close() {
	delete  pSockOteUniCastPc;		//PC���j�L���X�g�p�\�P�b�g
	delete  pSockPcMultiCastPc;		//PC�}���`�L���X�gPC��M�p�\�P�b�g
	delete  pSockOteMultiCastPc;	//OTE�}���`�L���X�gPC��M�p�\�P�b�g
	delete  pSockPcUniCastOte;		//OTE���j�L���X�g�p�\�P�b�g
	delete  pSockPcMultiCastOte;	//PC�}���`�L���X�gOTE��M�p�\�P�b�g
	delete  pSockOteMultiCastOte;	//OTE�}���`�L���X�gOTE��M�p�\�P�b�g
	KillTimer(hWnd_if, ID_OTE_MULTICAST_TIMER);
	KillTimer(hWnd_if, ID_OTE_UNICAST_TIMER);

	return S_OK;
}
//****************************************************************************
/// <summary>
/// 
/// </summary>
/// <param name="is_activate_req"></param>
void COteIF::activate_local_ote(bool is_activate_req) {
	if (is_activate_req) {
		//�}���`�L���X�g�^�C�}�N��
		SetTimer(hWnd_if, ID_OTE_MULTICAST_TIMER, OTE_MULTICAST_SCAN_MS, NULL);
		//���j�L���X�g�^�C�}�N��
		SetTimer(hWnd_if, ID_OTE_UNICAST_TIMER, OTE_UNICAST_SCAN_MS, NULL);
	}
	else {
		KillTimer(hWnd_if, ID_OTE_MULTICAST_TIMER);
		KillTimer(hWnd_if, ID_OTE_UNICAST_TIMER);
	}
	return;
}
//****************************************************************************
/// <summary>
/// ��M�����p�E�B���h�E�I�[�v��
/// </summary>
/// <param name="hwnd"></param>
/// <returns></returns>
HWND COteIF::open_work_Wnd(HWND hwnd) {

	InitCommonControls();//�R�����R���g���[��������
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(0);;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = TEXT("OTE IF");
	wcex.lpszClassName = TEXT("OTE IF");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//���C���E�B���h�E
	hWnd_if = CreateWindowW(TEXT("OTE IF"), TEXT("OTE IF"), WS_OVERLAPPEDWINDOW,
				OTE_WORK_WND_X, OTE_WORK_WND_Y, OTE_WORK_WND_W, OTE_WORK_WND_H,
				nullptr, nullptr, hInst, nullptr);

	RECT rc;
	GetClientRect(hWnd_if, &rc);
	st_work_wnd.area_w = rc.right - rc.left;
	st_work_wnd.area_h = rc.bottom - rc.top;

	//�\���t�H���g�ݒ�
	st_work_wnd.hfont_inftext = CreateFont(6, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));
	//�f�o�C�X�R���e�L�X�g
	HDC hdc = GetDC(hWnd_if);
	st_work_wnd.hBmap_mem0 = CreateCompatibleBitmap(hdc, st_work_wnd.area_w, st_work_wnd.area_h);
	st_work_wnd.hdc_mem0 = CreateCompatibleDC(hdc);
	SelectObject(st_work_wnd.hdc_mem0, st_work_wnd.hBmap_mem0);
	PatBlt(st_work_wnd.hdc_mem0, 0, 0, st_work_wnd.area_w, st_work_wnd.area_h, WHITENESS);

	st_work_wnd.hBmap_inf = CreateCompatibleBitmap(hdc, st_work_wnd.area_w, st_work_wnd.area_h);
	st_work_wnd.hdc_mem_inf = CreateCompatibleDC(hdc);
	SelectObject(st_work_wnd.hdc_mem_inf, st_work_wnd.hBmap_inf);
	PatBlt(st_work_wnd.hdc_mem_inf, 0, 0, st_work_wnd.area_w, st_work_wnd.area_h, WHITENESS);
	TextOutW(st_work_wnd.hdc_mem_inf, 0, 0, L"<<Information>>", 15);

	ReleaseDC(hWnd_if, hdc);

	InvalidateRect(hWnd_if, NULL, TRUE);//�\���X�V

	ShowWindow(hWnd_if, SW_SHOW);
	UpdateWindow(hWnd_if);

	return hWnd_if;
}
HWND COteIF::open_connect_Wnd(HWND hwnd) {

	InitCommonControls();//�R�����R���g���[��������
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndConnectProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(0);;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
	wcex.lpszMenuName = TEXT("OTE CONNECT");
	wcex.lpszClassName = TEXT("OTE CONNECT");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//���C���E�B���h�E
	hWnd_sub[OTE_INDEX_RADIO_CONNECT] = CreateWindowW(TEXT("OTE CONNECT"), TEXT("OTE CONNECT"), WS_POPUP | WS_BORDER,
		OTE_WORK_SUB_WND_X, OTE_WORK_SUB_WND_Y, OTE_WORK_SUB_WND_W, OTE_WORK_SUB_WND_H,
		nullptr, nullptr, hInst, nullptr);

	return hWnd_sub[OTE_INDEX_RADIO_CONNECT];
}
HWND COteIF::open_auto_Wnd(HWND hwnd) {

	InitCommonControls();//�R�����R���g���[��������
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndAutoProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(0);;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
	wcex.lpszMenuName = TEXT("OTE AUTO");
	wcex.lpszClassName = TEXT("OTE AUTO");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//���C���E�B���h�E
	hWnd_sub[OTE_INDEX_RADIO_AUTO] = CreateWindowW(TEXT("OTE AUTO"), TEXT("OTE AUTO"), WS_POPUP | WS_BORDER,//WS_OVERLAPPEDWINDOW,
		OTE_WORK_SUB_WND_X, OTE_WORK_SUB_WND_Y, OTE_WORK_SUB_WND_W, OTE_WORK_SUB_WND_H,
		nullptr, nullptr, hInst, nullptr);

	return hWnd_sub[OTE_INDEX_RADIO_AUTO];
}
HWND COteIF::open_mode_Wnd(HWND hwnd) {

	InitCommonControls();//�R�����R���g���[��������
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndModeProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(0);;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
	wcex.lpszMenuName = TEXT("OTE MODE");
	wcex.lpszClassName = TEXT("OTE MODE");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//���C���E�B���h�E
	hWnd_sub[OTE_INDEX_RADIO_MODE] = CreateWindowW(TEXT("OTE MODE"), TEXT("OTE MODE"), WS_POPUP | WS_BORDER,//WS_OVERLAPPEDWINDOW,
		OTE_WORK_SUB_WND_X, OTE_WORK_SUB_WND_Y, OTE_WORK_SUB_WND_W, OTE_WORK_SUB_WND_H,
		nullptr, nullptr, hInst, nullptr);

	return hWnd_sub[OTE_INDEX_RADIO_MODE];
}
HWND COteIF::open_fault_Wnd(HWND hwnd) {

	InitCommonControls();//�R�����R���g���[��������
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndFaultProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(0);;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
	wcex.lpszMenuName = TEXT("OTE FAULT");
	wcex.lpszClassName = TEXT("OTE FAULT");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//���C���E�B���h�E
	hWnd_sub[OTE_INDEX_RADIO_FAULT] = CreateWindowW(TEXT("OTE FAULT"), TEXT("OTE FAULT"), WS_POPUP | WS_BORDER,//WS_OVERLAPPEDWINDOW,
		OTE_WORK_SUB_WND_X, OTE_WORK_SUB_WND_Y, OTE_WORK_SUB_WND_W, OTE_WORK_SUB_WND_H,
		nullptr, nullptr, hInst, nullptr);

	return hWnd_sub[OTE_INDEX_RADIO_FAULT];
}
HWND COteIF::open_moment_Wnd(HWND hwnd) {

	InitCommonControls();//�R�����R���g���[��������
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndMomentProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(0);;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
	wcex.lpszMenuName = TEXT("OTE MOMENT");
	wcex.lpszClassName = TEXT("OTE MOMENT");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//���C���E�B���h�E
	hWnd_sub[OTE_INDEX_RADIO_MOMENT] = CreateWindowW(TEXT("OTE MOMENT"), TEXT("OTE MOMENT"), WS_POPUP | WS_BORDER,//WS_OVERLAPPEDWINDOW,
		OTE_WORK_SUB_WND_X, OTE_WORK_SUB_WND_Y, OTE_WORK_SUB_WND_W, OTE_WORK_SUB_WND_H,
		nullptr, nullptr, hInst, nullptr);

	return hWnd_sub[OTE_INDEX_RADIO_MOMENT];
}

HWND COteIF::open_ifchk_Wnd(HWND hwnd) {

	InitCommonControls();//�R�����R���g���[��������
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndIfChkProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(0);;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
	wcex.lpszMenuName = TEXT("OTE IF CHK");
	wcex.lpszClassName = TEXT("OTE IF CHK");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//���C���E�B���h�E
	hWnd_ifchk = CreateWindowW(TEXT("OTE IF CHK"), TEXT("OTE IF CHK"), WS_POPUP,
		OTE_IFCHK_WND_X, OTE_IFCHK_WND_Y, OTE_IFCHK_WND_W, OTE_IFCHK_WND_H,
		nullptr, nullptr, hInst, nullptr);

	InvalidateRect(hWnd_ifchk, NULL, TRUE);//�\���X�V

	ShowWindow(hWnd_ifchk, SW_SHOW);
	UpdateWindow(hWnd_ifchk);

	return hWnd_ifchk;
}

//*********************************************************************************************
/// <summary>
/// ���[�N�E�B���h�E�R�[���o�b�N�֐�
/// </summary>
/// <param name="hWnd"></param>
/// <param name="message"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
/// <returns></returns>
LRESULT CALLBACK COteIF::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	
	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		//�E�B���h�E�ɃR���g���[���ǉ�
		set_OTE_panel_objects(hWnd);
	}

	case WM_TIMER: {
		if (wParam == ID_OTE_UNICAST_TIMER) {

			if (is_my_ote_active) {
				if (S_OK == snd_ote_u_pc(set_msg_ote_u(), &addrin_ote_u_pc)) {//OTE->PC�փ��j�L���X�g���M
					cnt_snd_ote_u++;
				}
			}
		}
		if (wParam == ID_OTE_MULTICAST_TIMER) {

			if (S_OK == snd_pc_m_pc(set_msg_pc_m())) {//PC->OTE�փ}���`�L���X�g���M
				cnt_snd_pc_m_pc++;
			}

			if (S_OK == snd_pc_m_ote(set_msg_pc_m())) {//PC->OTE�փ}���`�L���X�g���M
				cnt_snd_pc_m_ote++;
			}

			if (is_my_ote_active) {
				if (S_OK == snd_ote_m_ote(set_msg_ote_m())) {
					cnt_snd_ote_m_ote++;
				}
			}
			disp_ip_inf();//IP���\���X�V
		}
		disp_msg_cnt();//�J�E���g�\���X�V

		for (int i = OTE_INDEX_PB_START; i <= OTE_INDEX_PB_END; i++) {
			if (st_msg_ote_u_snd.body.pb_ope[i] & OTE_PB_HOLDTIME_MASK)
				st_msg_ote_u_snd.body.pb_ope[i]--;
			else {
				if (Button_GetState(h_pb_ote[i]) == BST_PUSHED) {
					Button_SetState(h_pb_ote[i], FALSE);
				}
			}
		}

	}break;

	case WM_COMMAND:{
		int wmId = LOWORD(wParam);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
#pragma region notch
		//�努
		case OTE_ID_RADIO_HST_NOTCH - 5: st_msg_ote_u_snd.body.notch_pos[ID_HOIST] = -5; break;
		case OTE_ID_RADIO_HST_NOTCH - 4: st_msg_ote_u_snd.body.notch_pos[ID_HOIST] = -4; break;
		case OTE_ID_RADIO_HST_NOTCH - 3: st_msg_ote_u_snd.body.notch_pos[ID_HOIST] = -3; break;
		case OTE_ID_RADIO_HST_NOTCH - 2: st_msg_ote_u_snd.body.notch_pos[ID_HOIST] = -2; break;
		case OTE_ID_RADIO_HST_NOTCH - 1: st_msg_ote_u_snd.body.notch_pos[ID_HOIST] = -1; break;
		case OTE_ID_RADIO_HST_NOTCH + 0: st_msg_ote_u_snd.body.notch_pos[ID_HOIST] = 0; break;
		case OTE_ID_RADIO_HST_NOTCH + 1: st_msg_ote_u_snd.body.notch_pos[ID_HOIST] = 1; break;
		case OTE_ID_RADIO_HST_NOTCH + 2: st_msg_ote_u_snd.body.notch_pos[ID_HOIST] = 2; break;
		case OTE_ID_RADIO_HST_NOTCH + 3: st_msg_ote_u_snd.body.notch_pos[ID_HOIST] = 3; break;
		case OTE_ID_RADIO_HST_NOTCH + 4: st_msg_ote_u_snd.body.notch_pos[ID_HOIST] = 4; break;
		case OTE_ID_RADIO_HST_NOTCH + 5: st_msg_ote_u_snd.body.notch_pos[ID_HOIST] = 5; break;
			//�⊪
		case OTE_ID_RADIO_AH_NOTCH - 5: st_msg_ote_u_snd.body.notch_pos[ID_AHOIST] = -5; break;
		case OTE_ID_RADIO_AH_NOTCH - 4: st_msg_ote_u_snd.body.notch_pos[ID_AHOIST] = -4; break;
		case OTE_ID_RADIO_AH_NOTCH - 3: st_msg_ote_u_snd.body.notch_pos[ID_AHOIST] = -3; break;
		case OTE_ID_RADIO_AH_NOTCH - 2: st_msg_ote_u_snd.body.notch_pos[ID_AHOIST] = -2; break;
		case OTE_ID_RADIO_AH_NOTCH - 1: st_msg_ote_u_snd.body.notch_pos[ID_AHOIST] = -1; break;
		case OTE_ID_RADIO_AH_NOTCH + 0: st_msg_ote_u_snd.body.notch_pos[ID_AHOIST] = 0; break;
		case OTE_ID_RADIO_AH_NOTCH + 1: st_msg_ote_u_snd.body.notch_pos[ID_AHOIST] = 1; break;
		case OTE_ID_RADIO_AH_NOTCH + 2: st_msg_ote_u_snd.body.notch_pos[ID_AHOIST] = 2; break;
		case OTE_ID_RADIO_AH_NOTCH + 3: st_msg_ote_u_snd.body.notch_pos[ID_AHOIST] = 3; break;
		case OTE_ID_RADIO_AH_NOTCH + 4: st_msg_ote_u_snd.body.notch_pos[ID_AHOIST] = 4; break;
		case OTE_ID_RADIO_AH_NOTCH + 5: st_msg_ote_u_snd.body.notch_pos[ID_AHOIST] = 5; break;
			//���s
		case OTE_ID_RADIO_GNT_NOTCH - 5: st_msg_ote_u_snd.body.notch_pos[ID_GANTRY] = -5; break;
		case OTE_ID_RADIO_GNT_NOTCH - 4: st_msg_ote_u_snd.body.notch_pos[ID_GANTRY] = -4; break;
		case OTE_ID_RADIO_GNT_NOTCH - 3: st_msg_ote_u_snd.body.notch_pos[ID_GANTRY] = -3; break;
		case OTE_ID_RADIO_GNT_NOTCH - 2: st_msg_ote_u_snd.body.notch_pos[ID_GANTRY] = -2; break;
		case OTE_ID_RADIO_GNT_NOTCH - 1: st_msg_ote_u_snd.body.notch_pos[ID_GANTRY] = -1; break;
		case OTE_ID_RADIO_GNT_NOTCH + 0: st_msg_ote_u_snd.body.notch_pos[ID_GANTRY] = 0; break;
		case OTE_ID_RADIO_GNT_NOTCH + 1: st_msg_ote_u_snd.body.notch_pos[ID_GANTRY] = 1; break;
		case OTE_ID_RADIO_GNT_NOTCH + 2: st_msg_ote_u_snd.body.notch_pos[ID_GANTRY] = 2; break;
		case OTE_ID_RADIO_GNT_NOTCH + 3: st_msg_ote_u_snd.body.notch_pos[ID_GANTRY] = 3; break;
		case OTE_ID_RADIO_GNT_NOTCH + 4: st_msg_ote_u_snd.body.notch_pos[ID_GANTRY] = 4; break;
		case OTE_ID_RADIO_GNT_NOTCH + 5: st_msg_ote_u_snd.body.notch_pos[ID_GANTRY] = 5; break;
			//����
		case OTE_ID_RADIO_BH_NOTCH - 5: st_msg_ote_u_snd.body.notch_pos[ID_BOOM_H] = -5; break;
		case OTE_ID_RADIO_BH_NOTCH - 4: st_msg_ote_u_snd.body.notch_pos[ID_BOOM_H] = -4; break;
		case OTE_ID_RADIO_BH_NOTCH - 3: st_msg_ote_u_snd.body.notch_pos[ID_BOOM_H] = -3; break;
		case OTE_ID_RADIO_BH_NOTCH - 2: st_msg_ote_u_snd.body.notch_pos[ID_BOOM_H] = -2; break;
		case OTE_ID_RADIO_BH_NOTCH - 1: st_msg_ote_u_snd.body.notch_pos[ID_BOOM_H] = -1; break;
		case OTE_ID_RADIO_BH_NOTCH + 0: st_msg_ote_u_snd.body.notch_pos[ID_BOOM_H] = 0; break;
		case OTE_ID_RADIO_BH_NOTCH + 1: st_msg_ote_u_snd.body.notch_pos[ID_BOOM_H] = 1; break;
		case OTE_ID_RADIO_BH_NOTCH + 2: st_msg_ote_u_snd.body.notch_pos[ID_BOOM_H] = 2; break;
		case OTE_ID_RADIO_BH_NOTCH + 3: st_msg_ote_u_snd.body.notch_pos[ID_BOOM_H] = 3; break;
		case OTE_ID_RADIO_BH_NOTCH + 4: st_msg_ote_u_snd.body.notch_pos[ID_BOOM_H] = 4; break;
		case OTE_ID_RADIO_BH_NOTCH + 5: st_msg_ote_u_snd.body.notch_pos[ID_BOOM_H] = 5; break;
			//����
		case OTE_ID_RADIO_SLW_NOTCH - 5: st_msg_ote_u_snd.body.notch_pos[ID_SLEW] = -5; break;
		case OTE_ID_RADIO_SLW_NOTCH - 4: st_msg_ote_u_snd.body.notch_pos[ID_SLEW] = -4; break;
		case OTE_ID_RADIO_SLW_NOTCH - 3: st_msg_ote_u_snd.body.notch_pos[ID_SLEW] = -3; break;
		case OTE_ID_RADIO_SLW_NOTCH - 2: st_msg_ote_u_snd.body.notch_pos[ID_SLEW] = -2; break;
		case OTE_ID_RADIO_SLW_NOTCH - 1: st_msg_ote_u_snd.body.notch_pos[ID_SLEW] = -1; break;
		case OTE_ID_RADIO_SLW_NOTCH + 0: st_msg_ote_u_snd.body.notch_pos[ID_SLEW] = 0; break;
		case OTE_ID_RADIO_SLW_NOTCH + 1: st_msg_ote_u_snd.body.notch_pos[ID_SLEW] = 1; break;
		case OTE_ID_RADIO_SLW_NOTCH + 2: st_msg_ote_u_snd.body.notch_pos[ID_SLEW] = 2; break;
		case OTE_ID_RADIO_SLW_NOTCH + 3: st_msg_ote_u_snd.body.notch_pos[ID_SLEW] = 3; break;
		case OTE_ID_RADIO_SLW_NOTCH + 4: st_msg_ote_u_snd.body.notch_pos[ID_SLEW] = 4; break;
		case OTE_ID_RADIO_SLW_NOTCH + 5: st_msg_ote_u_snd.body.notch_pos[ID_SLEW] = 5; break;
#pragma endregion
		case OTE_ID_PB_CHK_RADIO + OTE_INDEX_RADIO_CONNECT: {
			ShowWindow(hWnd_sub[ote_io_workbuf.ote_in.sub_monitor_mode], SW_MINIMIZE);
			ote_io_workbuf.ote_in.sub_monitor_mode = OTE_INDEX_RADIO_CONNECT;
			InvalidateRect(hWnd_sub[OTE_INDEX_RADIO_CONNECT], NULL, TRUE);//�\���X�V
			ShowWindow(hWnd_sub[OTE_INDEX_RADIO_CONNECT], SW_SHOWDEFAULT);
			UpdateWindow(hWnd_sub[OTE_INDEX_RADIO_CONNECT]);
		}break;
		case OTE_ID_PB_CHK_RADIO + OTE_INDEX_RADIO_AUTO: {
			ShowWindow(hWnd_sub[ote_io_workbuf.ote_in.sub_monitor_mode], SW_MINIMIZE);
			ote_io_workbuf.ote_in.sub_monitor_mode = OTE_INDEX_RADIO_AUTO;
			InvalidateRect(hWnd_sub[OTE_INDEX_RADIO_AUTO], NULL, TRUE);//�\���X�V
			ShowWindow(hWnd_sub[OTE_INDEX_RADIO_AUTO], SW_SHOWDEFAULT);
			UpdateWindow(hWnd_sub[OTE_INDEX_RADIO_AUTO]);
		} break;
		case OTE_ID_PB_CHK_RADIO + OTE_INDEX_RADIO_MODE: { 
			ShowWindow(hWnd_sub[ote_io_workbuf.ote_in.sub_monitor_mode], SW_MINIMIZE);
			ote_io_workbuf.ote_in.sub_monitor_mode = OTE_INDEX_RADIO_MODE;
			InvalidateRect(hWnd_sub[OTE_INDEX_RADIO_MODE], NULL, TRUE);//�\���X�V
			ShowWindow(hWnd_sub[OTE_INDEX_RADIO_MODE], SW_SHOWDEFAULT);
			UpdateWindow(hWnd_sub[OTE_INDEX_RADIO_MODE]);
		} break;
		case OTE_ID_PB_CHK_RADIO + OTE_INDEX_RADIO_FAULT: {
			ShowWindow(hWnd_sub[ote_io_workbuf.ote_in.sub_monitor_mode], SW_MINIMIZE);
			ote_io_workbuf.ote_in.sub_monitor_mode = OTE_INDEX_RADIO_FAULT;
			InvalidateRect(hWnd_sub[OTE_INDEX_RADIO_FAULT], NULL, TRUE);//�\���X�V
			ShowWindow(hWnd_sub[OTE_INDEX_RADIO_FAULT], SW_SHOWDEFAULT);
			UpdateWindow(hWnd_sub[OTE_INDEX_RADIO_FAULT]);
		}break;
		case OTE_ID_PB_CHK_RADIO + OTE_INDEX_RADIO_MOMENT:{
			ShowWindow(hWnd_sub[ote_io_workbuf.ote_in.sub_monitor_mode], SW_MINIMIZE);
			ote_io_workbuf.ote_in.sub_monitor_mode = OTE_INDEX_RADIO_MOMENT;
			InvalidateRect(hWnd_sub[OTE_INDEX_RADIO_MOMENT], NULL, TRUE);//�\���X�V
			ShowWindow(hWnd_sub[OTE_INDEX_RADIO_MOMENT], SW_SHOWDEFAULT);
			UpdateWindow(hWnd_sub[OTE_INDEX_RADIO_MOMENT]);
		}break;
//PB
		case OTE_ID_PB_CHK_RADIO + OTE_INDEX_CHK_ESTOP: {
			if (BST_CHECKED == SendMessage(h_pb_ote[OTE_INDEX_CHK_ESTOP], BM_GETCHECK, 0, 0))
				st_msg_ote_u_snd.body.pb_ope[OTE_INDEX_CHK_ESTOP] = L_ON;
			else
				st_msg_ote_u_snd.body.pb_ope[OTE_INDEX_CHK_ESTOP] = L_OFF;
		}break;
		case OTE_ID_PB_CHK_RADIO + OTE_INDEX_PB_CTR_SOURCE: {
			st_msg_ote_u_snd.body.pb_ope[OTE_INDEX_PB_CTR_SOURCE] = OTE_PB_HOLDTIME_MASK;
			Button_SetState(h_pb_ote[OTE_INDEX_PB_CTR_SOURCE], TRUE);
			SetFocus(h_pb_ote[OTE_INDEX_CHK_ESTOP]);//�t�H�[�J�X�𑼂Ɉڂ��Ȃ��ƃZ�b�g��Ԃ���荞�܂�Ȃ��悤�Ȃ̂Œǉ�
		}break;
		case OTE_ID_PB_CHK_RADIO + OTE_INDEX_PB_FAULT_RESET: {
			st_msg_ote_u_snd.body.pb_ope[OTE_INDEX_PB_FAULT_RESET] = OTE_PB_HOLDTIME_MASK;
			Button_SetState(h_pb_ote[OTE_INDEX_PB_FAULT_RESET], TRUE);
			SetFocus(h_pb_ote[OTE_INDEX_CHK_ESTOP]);//�t�H�[�J�X�𑼂Ɉڂ��Ȃ��ƃZ�b�g��Ԃ���荞�܂�Ȃ��悤�Ȃ̂Œǉ�
		}break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}break;
	
	//�\�P�b�gIF
	case ID_SOCK_EVENT_OTE_UNI_PC: {
		int nEvent = WSAGETSELECTEVENT(lParam);
		switch (nEvent) {
		case FD_READ: {
			if (rcv_ote_u_pc(&st_msg_pc_u_rcv) == S_OK) {				//PC����̃��j�L���X�g���b�Z�[�W��M
				cnt_rcv_ote_u++;
				disp_msg_cnt();


				if (S_OK == snd_pc_u_ote(set_msg_pc_u(), &addrin_pc_u_ote)) {//PC->OTM�փ��j�L���X�g���M
					cnt_snd_pc_u++;
				}
			}
			else {
				msg_ws = L"ERROR : rcv_ote_u_pc()";	wstr_out_inf(msg_ws);
			}
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case ID_SOCK_EVENT_PC_MULTI_PC: {
		int nEvent = WSAGETSELECTEVENT(lParam);
		switch (nEvent) {
		case FD_READ: {
			if (rcv_pc_m_pc(&st_msg_pc_m_pc_rcv) == S_OK) {				//PC����̃��j�L���X�g���b�Z�[�W��M
				cnt_rcv_pc_m_pc++;
				disp_msg_cnt();
			}
			else {
				msg_ws = L"ERROR : rcv_ote_u_pc()";	wstr_out_inf(msg_ws);
			}
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case ID_SOCK_EVENT_OTE_MULTI_PC: {
		int nEvent = WSAGETSELECTEVENT(lParam);
		switch (nEvent) {
		case FD_READ: {
			if (rcv_ote_m_pc(&st_msg_ote_m_pc_rcv) == S_OK) {				//PC����̃��j�L���X�g���b�Z�[�W��M
				cnt_rcv_ote_m_pc++;
				disp_msg_cnt();
			}
			else {
				msg_ws = L"ERROR : rcv_ote_u_pc()";	wstr_out_inf(msg_ws);
			}
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case ID_SOCK_EVENT_PC_UNI_OTE: {
		int nEvent = WSAGETSELECTEVENT(lParam);
		switch (nEvent) {
		case FD_READ: {
			if (rcv_pc_u_ote(&st_msg_ote_u_rcv) == S_OK) {				//PC����̃��j�L���X�g���b�Z�[�W��M
				cnt_rcv_pc_u++;
				disp_msg_cnt();
			}
			else {
				msg_ws = L"ERROR : rcv_ote_u_pc()";	wstr_out_inf(msg_ws);
			}
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case ID_SOCK_EVENT_PC_MULTI_OTE: {
		int nEvent = WSAGETSELECTEVENT(lParam);
		switch (nEvent) {
		case FD_READ: {
			if (rcv_pc_m_ote(&st_msg_pc_m_ote_rcv) == S_OK) {				//PC����̃��j�L���X�g���b�Z�[�W��M
				cnt_rcv_pc_m_ote++;
				disp_msg_cnt();
			}
			else {
				msg_ws = L"ERROR : rcv_ote_u_pc()";	wstr_out_inf(msg_ws);
			}
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case ID_SOCK_EVENT_OTE_MULTI_OTE: {
		int nEvent = WSAGETSELECTEVENT(lParam);
		switch (nEvent) {
		case FD_READ: {
			if (rcv_ote_m_ote(&st_msg_ote_m_ote_rcv) == S_OK) {				//PC����̃��j�L���X�g���b�Z�[�W��M
				cnt_rcv_ote_m_ote++;
				disp_msg_cnt();
			}
			else {
				msg_ws = L"ERROR : rcv_ote_m_ote()";	wstr_out_inf(msg_ws);
			}
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case WM_PAINT:{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		BOOL brtn = TransparentBlt(	st_work_wnd.hdc_mem0, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H,		//DST
									st_work_wnd.hdc_mem_inf, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H,	//SORCE
									RGB(255, 255, 255));

		BitBlt(hdc, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H, st_work_wnd.hdc_mem0, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
	}break;

	case WM_DESTROY:{
		close();
		PostQuitMessage(0);
	}break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
}
//*********************************************************************************************
/// <summary>
/// ���[�N�E�B���h�E�R�[���o�b�N�֐�
/// </summary>
/// <param name="hWnd"></param>
/// <param name="message"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
/// <returns></returns>
LRESULT CALLBACK COteIF::WndConnectProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//�E�B���h�E�ɃR���g���[���ǉ�
		CreateWindowW(TEXT("STATIC"), L"CONNECT WINDOW", WS_CHILD | WS_VISIBLE | SS_LEFT,
			0, 0, 80, 20,hWnd, (HMENU)(OTE_ID_STATIC+OTE_INDEX_STATIC_COMMON_WND), hInst, NULL);

		//���u	
		h_pb_ote[OTE_INDEX_CHK_REMOTE] = CreateWindow(L"BUTTON", pb_text[OTE_INDEX_CHK_REMOTE], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
			ote_pb_chk_radio_loc[OTE_INDEX_CHK_REMOTE].x, ote_pb_chk_radio_loc[OTE_INDEX_CHK_REMOTE].y, ote_pb_chk_radio_size[OTE_INDEX_CHK_REMOTE].cx, ote_pb_chk_radio_size[OTE_INDEX_CHK_ESTOP].cy,
			hWnd, (HMENU)(OTE_ID_PB_CHK_RADIO + OTE_INDEX_CHK_REMOTE), hInst, NULL);

		//�ڑ�	
		h_pb_ote[OTE_INDEX_CHK_CONNECT] = CreateWindow(L"BUTTON", pb_text[OTE_INDEX_CHK_CONNECT], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
			ote_pb_chk_radio_loc[OTE_INDEX_CHK_CONNECT].x, ote_pb_chk_radio_loc[OTE_INDEX_CHK_CONNECT].y, ote_pb_chk_radio_size[OTE_INDEX_CHK_CONNECT].cx, ote_pb_chk_radio_size[OTE_INDEX_CHK_ESTOP].cy,
			hWnd, (HMENU)(OTE_ID_PB_CHK_RADIO + OTE_INDEX_CHK_CONNECT), hInst, NULL);
		//MONIT	
		h_pb_ote[OTE_INDEX_CHK_OTE_MON] = CreateWindow(L"BUTTON", pb_text[OTE_INDEX_CHK_OTE_MON], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
			ote_pb_chk_radio_loc[OTE_INDEX_CHK_OTE_MON].x, ote_pb_chk_radio_loc[OTE_INDEX_CHK_OTE_MON].y, ote_pb_chk_radio_size[OTE_INDEX_CHK_OTE_MON].cx, ote_pb_chk_radio_size[OTE_INDEX_CHK_ESTOP].cy,
			hWnd, (HMENU)(OTE_ID_PB_CHK_RADIO + OTE_INDEX_CHK_OTE_MON), hInst, NULL);

		//�ʐM�J�E���g�\��
		h_static_ote[OTE_INDEX_STATIC_CONNECT_CNT] = CreateWindowW(TEXT("STATIC"), L"SND CNT U: -", WS_CHILD | WS_VISIBLE | SS_LEFT,
			ote_static_loc[OTE_INDEX_STATIC_CONNECT_CNT].x, ote_static_loc[OTE_INDEX_STATIC_CONNECT_CNT].y,
			ote_static_size[OTE_INDEX_STATIC_CONNECT_CNT].cx, ote_static_size[OTE_INDEX_STATIC_CONNECT_CNT].cy,
			hWnd, (HMENU)(OTE_ID_STATIC + OTE_INDEX_STATIC_CONNECT_CNT), hInst, NULL);

		h_static_ote[OTE_INDEX_STATIC_OTE_IP_INF] = CreateWindowW(TEXT("STATIC"), L"IP INF: -", WS_CHILD | WS_VISIBLE | SS_LEFT,
			ote_static_loc[OTE_INDEX_STATIC_OTE_IP_INF].x, ote_static_loc[OTE_INDEX_STATIC_OTE_IP_INF].y,
			ote_static_size[OTE_INDEX_STATIC_OTE_IP_INF].cx, ote_static_size[OTE_INDEX_STATIC_OTE_IP_INF].cy,
			hWnd, (HMENU)(OTE_ID_STATIC + OTE_INDEX_STATIC_OTE_IP_INF), hInst, NULL);
	
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case OTE_ID_PB_CHK_RADIO + OTE_INDEX_CHK_REMOTE: {
			if (BST_CHECKED == SendMessage(h_pb_ote[OTE_INDEX_CHK_REMOTE], BM_GETCHECK, 0, 0)) {
				st_msg_ote_u_snd.body.ote_mode &= 0x0000000f;
				st_msg_ote_u_snd.body.ote_mode |= ID_OTE_MODE_REMOTE_ACTIVE;
			}
			else
				st_msg_ote_u_snd.body.ote_mode &= ~ID_OTE_MODE_REMOTE_ACTIVE;
		}break;
		case OTE_ID_PB_CHK_RADIO + OTE_INDEX_CHK_CONNECT: {
			if (BST_CHECKED == SendMessage(h_pb_ote[OTE_INDEX_CHK_CONNECT], BM_GETCHECK, 0, 0)) {
				st_msg_ote_u_snd.body.ote_mode &= 0x000000f0;
				st_msg_ote_u_snd.body.ote_mode |= ID_OTE_MODE_REMOTE_OPE;
				SendMessage(h_pb_ote[OTE_INDEX_CHK_OTE_MON], BM_SETCHECK, BST_UNCHECKED, 0L);
			}
			else
				st_msg_ote_u_snd.body.ote_mode &= ~ID_OTE_MODE_REMOTE_OPE;

		}break;
		case OTE_ID_PB_CHK_RADIO + OTE_INDEX_CHK_OTE_MON: {
			if (BST_CHECKED == SendMessage(h_pb_ote[OTE_INDEX_CHK_OTE_MON], BM_GETCHECK, 0, 0)) {
				st_msg_ote_u_snd.body.ote_mode &= 0x000000f0;
				st_msg_ote_u_snd.body.ote_mode |= ID_OTE_MODE_MONITOR;
				SendMessage(h_pb_ote[OTE_INDEX_CHK_CONNECT], BM_SETCHECK, BST_UNCHECKED, 0L);
			}
			else
				st_msg_ote_u_snd.body.ote_mode &= ~ID_OTE_MODE_MONITOR;
		}break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		BOOL brtn = TransparentBlt(st_work_wnd.hdc_mem0, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H,		//DST
			st_work_wnd.hdc_mem_inf, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H,	//SORCE
			RGB(255, 255, 255));

		BitBlt(hdc, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H, st_work_wnd.hdc_mem0, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		//PostQuitMessage(0);
	}break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
}
LRESULT CALLBACK COteIF::WndAutoProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//�E�B���h�E�ɃR���g���[���ǉ�
		CreateWindowW(TEXT("STATIC"), L"AUTO WINDOW", WS_CHILD | WS_VISIBLE | SS_LEFT,
			0, 0, 100, 20, hWnd, (HMENU)(OTE_ID_STATIC + OTE_INDEX_STATIC_COMMON_WND), hInst, NULL);

	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case OTE_ID_PB_CHK_RADIO + OTE_INDEX_PB_CTR_SOURCE: {

		}break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		BOOL brtn = TransparentBlt(st_work_wnd.hdc_mem0, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H,		//DST
			st_work_wnd.hdc_mem_inf, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H,	//SORCE
			RGB(255, 255, 255));

		BitBlt(hdc, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H, st_work_wnd.hdc_mem0, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
	}break;

	case WM_DESTROY: {
		//PostQuitMessage(0);
	}break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
}
LRESULT CALLBACK COteIF::WndModeProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//�E�B���h�E�ɃR���g���[���ǉ�
		CreateWindowW(TEXT("STATIC"), L"MODE WINDOW", WS_CHILD | WS_VISIBLE | SS_LEFT,
			0, 0, 100, 20, hWnd, (HMENU)(OTE_ID_STATIC + OTE_INDEX_STATIC_COMMON_WND), hInst, NULL);

	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case OTE_ID_PB_CHK_RADIO + OTE_INDEX_PB_CTR_SOURCE: {

		}break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		BOOL brtn = TransparentBlt(st_work_wnd.hdc_mem0, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H,		//DST
			st_work_wnd.hdc_mem_inf, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H,	//SORCE
			RGB(255, 255, 255));

		BitBlt(hdc, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H, st_work_wnd.hdc_mem0, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
	}break;

	case WM_DESTROY: {
		//PostQuitMessage(0);
	}break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
}
LRESULT CALLBACK COteIF::WndFaultProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//�E�B���h�E�ɃR���g���[���ǉ�
		CreateWindowW(TEXT("STATIC"), L"FAULT WINDOW", WS_CHILD | WS_VISIBLE | SS_LEFT,
			0, 0, 100, 20, hWnd, (HMENU)(OTE_ID_STATIC + OTE_INDEX_STATIC_COMMON_WND), hInst, NULL);

	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case OTE_ID_PB_CHK_RADIO + OTE_INDEX_PB_CTR_SOURCE: {

		}break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		BOOL brtn = TransparentBlt(st_work_wnd.hdc_mem0, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H,		//DST
			st_work_wnd.hdc_mem_inf, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H,	//SORCE
			RGB(255, 255, 255));

		BitBlt(hdc, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H, st_work_wnd.hdc_mem0, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
	}break;

	case WM_DESTROY: {
		//PostQuitMessage(0);
	}break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
}
LRESULT CALLBACK COteIF::WndMomentProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//�E�B���h�E�ɃR���g���[���ǉ�
		CreateWindowW(TEXT("STATIC"), L"MOMENT WINDOW", WS_CHILD | WS_VISIBLE | SS_LEFT,
			0, 0, 100, 20, hWnd, (HMENU)(OTE_ID_STATIC + OTE_INDEX_STATIC_COMMON_WND), hInst, NULL);

	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case OTE_ID_PB_CHK_RADIO + OTE_INDEX_PB_CTR_SOURCE: {

		}break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		BOOL brtn = TransparentBlt(st_work_wnd.hdc_mem0, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H,		//DST
			st_work_wnd.hdc_mem_inf, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H,	//SORCE
			RGB(255, 255, 255));

		BitBlt(hdc, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H, st_work_wnd.hdc_mem0, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
	}break;

	case WM_DESTROY: {
		//PostQuitMessage(0);
	}break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
}

static INT32 ifchk_disp_item = OTE_INDEX_RADIO_IFCHK_UNI;
#define ID_OTE_IF_CHK_TIMER 669
#define OTE_IF_CHK_UPDATE_MS 100

LRESULT CALLBACK COteIF::WndIfChkProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//�R�����R���g���[��������
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//�E�B���h�E�ɃR���g���[���ǉ�
		CreateWindowW(TEXT("STATIC"), L"OTE IF CHECK", WS_CHILD | WS_VISIBLE | SS_LEFT,
			5, 5, 75, 20, hWnd, (HMENU)(OTE_ID_STATIC + OTE_INDEX_STATIC_COMMON_WND), hInst, NULL);

		//���j�L���X�g	
		h_pb_ote[OTE_INDEX_RADIO_IFCHK_UNI] = CreateWindow(L"BUTTON", pb_text[OTE_INDEX_RADIO_IFCHK_UNI], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
			ote_pb_chk_radio_loc[OTE_INDEX_RADIO_IFCHK_UNI].x, ote_pb_chk_radio_loc[OTE_INDEX_RADIO_IFCHK_UNI].y, ote_pb_chk_radio_size[OTE_INDEX_RADIO_IFCHK_UNI].cx, ote_pb_chk_radio_size[OTE_INDEX_CHK_ESTOP].cy,
			hWnd, (HMENU)(OTE_ID_PB_CHK_RADIO + OTE_INDEX_RADIO_IFCHK_UNI), hInst, NULL);

		//PC�}���`�L���X�g	
		h_pb_ote[OTE_INDEX_RADIO_IFCHK_MPC] = CreateWindow(L"BUTTON", pb_text[OTE_INDEX_RADIO_IFCHK_MPC], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
			ote_pb_chk_radio_loc[OTE_INDEX_RADIO_IFCHK_MPC].x, ote_pb_chk_radio_loc[OTE_INDEX_RADIO_IFCHK_MPC].y, ote_pb_chk_radio_size[OTE_INDEX_RADIO_IFCHK_MPC].cx, ote_pb_chk_radio_size[OTE_INDEX_CHK_ESTOP].cy,
			hWnd, (HMENU)(OTE_ID_PB_CHK_RADIO + OTE_INDEX_RADIO_IFCHK_MPC), hInst, NULL);
		//OTE�}���`�L���X�g	
		h_pb_ote[OTE_INDEX_RADIO_IFCHK_MTE] = CreateWindow(L"BUTTON", pb_text[OTE_INDEX_RADIO_IFCHK_MTE], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
			ote_pb_chk_radio_loc[OTE_INDEX_RADIO_IFCHK_MTE].x, ote_pb_chk_radio_loc[OTE_INDEX_RADIO_IFCHK_MTE].y, ote_pb_chk_radio_size[OTE_INDEX_RADIO_IFCHK_MTE].cx, ote_pb_chk_radio_size[OTE_INDEX_CHK_ESTOP].cy,
			hWnd, (HMENU)(OTE_ID_PB_CHK_RADIO + OTE_INDEX_RADIO_IFCHK_MTE), hInst, NULL);

		ifchk_disp_item = OTE_INDEX_RADIO_IFCHK_UNI;
		SendMessage(h_pb_ote[OTE_INDEX_RADIO_IFCHK_UNI], BM_SETCHECK, BST_CHECKED, 0L);


		//���M�w�b�_�\��
		h_static_ote[OTE_INDEX_STATIC_IFCHK_MSG_HEAD_S] = CreateWindowW(TEXT("STATIC"), L"SND HEAD: -", WS_CHILD | WS_VISIBLE | SS_LEFT,
			ote_static_loc[OTE_INDEX_STATIC_IFCHK_MSG_HEAD_S].x, ote_static_loc[OTE_INDEX_STATIC_IFCHK_MSG_HEAD_S].y,
			ote_static_size[OTE_INDEX_STATIC_IFCHK_MSG_HEAD_S].cx, ote_static_size[OTE_INDEX_STATIC_IFCHK_MSG_HEAD_S].cy,
			hWnd, (HMENU)(OTE_ID_STATIC + OTE_INDEX_STATIC_IFCHK_MSG_HEAD_S), hInst, NULL);
		//���M�{�f�B�\��
		h_static_ote[OTE_INDEX_STATIC_IFCHK_MSG_BODY_S] = CreateWindowW(TEXT("STATIC"), L"SND BODY: -", WS_CHILD | WS_VISIBLE | SS_LEFT,
			ote_static_loc[OTE_INDEX_STATIC_IFCHK_MSG_BODY_S].x, ote_static_loc[OTE_INDEX_STATIC_IFCHK_MSG_BODY_S].y,
			ote_static_size[OTE_INDEX_STATIC_IFCHK_MSG_BODY_S].cx, ote_static_size[OTE_INDEX_STATIC_IFCHK_MSG_BODY_S].cy,
			hWnd, (HMENU)(OTE_ID_STATIC + OTE_INDEX_STATIC_IFCHK_MSG_BODY_S), hInst, NULL);
		//��M�w�b�_�\��
		h_static_ote[OTE_INDEX_STATIC_IFCHK_MSG_HEAD_R] = CreateWindowW(TEXT("STATIC"), L"RCV HEAD: -", WS_CHILD | WS_VISIBLE | SS_LEFT,
			ote_static_loc[OTE_INDEX_STATIC_IFCHK_MSG_HEAD_R].x, ote_static_loc[OTE_INDEX_STATIC_IFCHK_MSG_HEAD_R].y,
			ote_static_size[OTE_INDEX_STATIC_IFCHK_MSG_HEAD_R].cx, ote_static_size[OTE_INDEX_STATIC_IFCHK_MSG_HEAD_R].cy,
			hWnd, (HMENU)(OTE_ID_STATIC + OTE_INDEX_STATIC_IFCHK_MSG_HEAD_R), hInst, NULL);
		//��M�{�f�B�\��
		h_static_ote[OTE_INDEX_STATIC_IFCHK_MSG_BODY_R] = CreateWindowW(TEXT("STATIC"), L"RCV BODY: -", WS_CHILD | WS_VISIBLE | SS_LEFT,
			ote_static_loc[OTE_INDEX_STATIC_IFCHK_MSG_BODY_R].x, ote_static_loc[OTE_INDEX_STATIC_IFCHK_MSG_BODY_R].y,
			ote_static_size[OTE_INDEX_STATIC_IFCHK_MSG_BODY_R].cx, ote_static_size[OTE_INDEX_STATIC_IFCHK_MSG_BODY_R].cy,
			hWnd, (HMENU)(OTE_ID_STATIC + OTE_INDEX_STATIC_IFCHK_MSG_BODY_R), hInst, NULL);

		SetTimer(hWnd, ID_OTE_IF_CHK_TIMER, OTE_IF_CHK_UPDATE_MS,0);
	}
	case WM_TIMER: {
		msg_wos.str(L"");
		if (ifchk_disp_item == OTE_INDEX_RADIO_IFCHK_UNI) {

			//PC ���j�L���X�g���M�@HEAD��
			msg_wos.str(L"");
			msg_wos << L"SND HEAD:" << L" (PC)" << st_msg_pc_u_snd.head.myid << L" (EVENT)" << st_msg_pc_u_snd.head.code
				<< L" (IP)" << st_msg_pc_u_snd.head.addr.sin_addr.S_un.S_un_b.s_b1 << L"." << st_msg_pc_u_snd.head.addr.sin_addr.S_un.S_un_b.s_b2 << L"." << st_msg_pc_u_snd.head.addr.sin_addr.S_un.S_un_b.s_b3 << L"." << st_msg_pc_u_snd.head.addr.sin_addr.S_un.S_un_b.s_b4
				<< L" (PORT) " << htons(st_msg_pc_u_snd.head.addr.sin_port)
				<< L" (COM)" << st_msg_pc_u_snd.head.status << L" (�ڑ���OTE)" << st_msg_pc_u_snd.head.tgid;
			SetWindowText(h_static_ote[OTE_INDEX_STATIC_IFCHK_MSG_HEAD_S], msg_wos.str().c_str());

			msg_wos.str(L"");
			//PC ���j�L���X�g���M�@BODY��
			msg_wos << L"SND BODY:";
			SetWindowText(h_static_ote[OTE_INDEX_STATIC_IFCHK_MSG_BODY_S], msg_wos.str().c_str());

			//PC ���j�L���X�g��M�@HEAD��
			msg_wos.str(L"");
			msg_wos << L"RCV HEAD:" << L" (OTE)" << st_msg_pc_u_rcv.head.myid << L" (EVENT)" << st_msg_pc_u_rcv.head.code
				<< L" (IP)" << st_msg_pc_u_rcv.head.addr.sin_addr.S_un.S_un_b.s_b1 << L"." << st_msg_pc_u_rcv.head.addr.sin_addr.S_un.S_un_b.s_b2 << L"." << st_msg_pc_u_rcv.head.addr.sin_addr.S_un.S_un_b.s_b3 << L"." << st_msg_pc_u_rcv.head.addr.sin_addr.S_un.S_un_b.s_b4
				<< L" (PORT) " << htons(st_msg_pc_u_rcv.head.addr.sin_port)
				<< L" (COM)" << st_msg_pc_u_rcv.head.status << L" (�ڑ���PC)" << st_msg_pc_u_rcv.head.tgid;
			SetWindowText(h_static_ote[OTE_INDEX_STATIC_IFCHK_MSG_HEAD_R], msg_wos.str().c_str());

			msg_wos.str(L"");
			//PC ���j�L���X�g��M�@BODY��
			msg_wos << L"RCV BODY:" << L"#PB(�劲)" << st_msg_pc_u_rcv.body.pb_ope[OTE_INDEX_PB_CTR_SOURCE] << L"(���)" << st_msg_pc_u_rcv.body.pb_ope[OTE_INDEX_CHK_ESTOP]
					<< L" #ɯ�(MH)" << st_msg_pc_u_rcv.body.notch_pos[ID_HOIST] << L" (BH)" << st_msg_ote_u_snd.body.notch_pos[ID_BOOM_H] << L" (SL)" << st_msg_ote_u_snd.body.notch_pos[ID_SLEW];
			SetWindowText(h_static_ote[OTE_INDEX_STATIC_IFCHK_MSG_BODY_R], msg_wos.str().c_str());

		}
		else if (ifchk_disp_item == OTE_INDEX_RADIO_IFCHK_MPC) {
			//PC �}���`�L���X�g���M�@HEAD��
			msg_wos.str(L"");
			msg_wos << L"SND HEAD:" << L" (PC)" << st_msg_pc_m_snd.head.myid << L" (EVENT)" << st_msg_pc_m_snd.head.code
				<< L" (IP)" << st_msg_pc_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b1 << L"." << st_msg_pc_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b2 << L"." << st_msg_pc_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b3 << L"." << st_msg_pc_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b4
				<< L" (PORT) " << htons(st_msg_pc_m_snd.head.addr.sin_port)
				<< L" (COM)" << st_msg_pc_m_snd.head.status << L" (�ڑ���OTE)" << st_msg_pc_m_snd.head.tgid;
			SetWindowText(h_static_ote[OTE_INDEX_STATIC_IFCHK_MSG_HEAD_S], msg_wos.str().c_str());

			msg_wos.str(L"");
			//PC �}���`�L���X�g���M�@BODY��
			msg_wos << L"SND BODY:";
			SetWindowText(h_static_ote[OTE_INDEX_STATIC_IFCHK_MSG_BODY_S], msg_wos.str().c_str());

			//PC �}���`�L���X�g��M�@HEAD��
			msg_wos.str(L"");
			msg_wos << L"RCV HEAD:" << L" (PC)" << st_msg_pc_m_pc_rcv.head.myid << L" (EVENT)" << st_msg_pc_m_pc_rcv.head.code
				<< L" (IP)" << st_msg_pc_m_pc_rcv.head.addr.sin_addr.S_un.S_un_b.s_b1 << L"." << st_msg_pc_m_pc_rcv.head.addr.sin_addr.S_un.S_un_b.s_b2 << L"." << st_msg_pc_m_pc_rcv.head.addr.sin_addr.S_un.S_un_b.s_b3 << L"." << st_msg_pc_m_pc_rcv.head.addr.sin_addr.S_un.S_un_b.s_b4
				<< L" (PORT) " << htons(st_msg_pc_m_pc_rcv.head.addr.sin_port)
				<< L" (COM)" << st_msg_pc_m_pc_rcv.head.status << L" (�ڑ���PC)" << st_msg_pc_m_pc_rcv.head.tgid;
			SetWindowText(h_static_ote[OTE_INDEX_STATIC_IFCHK_MSG_HEAD_R], msg_wos.str().c_str());

			msg_wos.str(L"");
			//PC �}���`�L���X�g��M�@BODY��
			msg_wos << L"RCV BODY:" ;
		}
		else if (ifchk_disp_item == OTE_INDEX_RADIO_IFCHK_MTE) {
			//PC �}���`�L���X�g���M�@HEAD��
			msg_wos.str(L"");
			msg_wos << L"SND HEAD:" << L" (PC)" << st_msg_pc_m_snd.head.myid << L" (EVENT)" << st_msg_pc_m_snd.head.code
				<< L" (IP)" << st_msg_pc_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b1 << L"." << st_msg_pc_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b2 << L"." << st_msg_pc_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b3 << L"." << st_msg_pc_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b4
				<< L" (PORT) " << htons(st_msg_pc_m_snd.head.addr.sin_port)
				<< L" (COM)" << st_msg_pc_m_snd.head.status << L" (�ڑ���OTE)" << st_msg_pc_m_snd.head.tgid;
			SetWindowText(h_static_ote[OTE_INDEX_STATIC_IFCHK_MSG_HEAD_S], msg_wos.str().c_str());

			msg_wos.str(L"");
			//PC �}���`�L���X�g���M�@BODY��
			msg_wos << L"SND BODY:";
			SetWindowText(h_static_ote[OTE_INDEX_STATIC_IFCHK_MSG_BODY_S], msg_wos.str().c_str());

			//OTE �}���`�L���X�g��M�@HEAD��
			msg_wos.str(L"");
			msg_wos << L"RCV HEAD:" << L" (OTE)" << st_msg_ote_m_pc_rcv.head.myid << L" (EVENT)" << st_msg_ote_m_pc_rcv.head.code
				<< L" (IP)" << st_msg_ote_m_pc_rcv.head.addr.sin_addr.S_un.S_un_b.s_b1 << L"." << st_msg_ote_m_pc_rcv.head.addr.sin_addr.S_un.S_un_b.s_b2 << L"." << st_msg_ote_m_pc_rcv.head.addr.sin_addr.S_un.S_un_b.s_b3 << L"." << st_msg_ote_m_pc_rcv.head.addr.sin_addr.S_un.S_un_b.s_b4
				<< L" (PORT) " << htons(st_msg_ote_m_pc_rcv.head.addr.sin_port)
				<< L" (COM)" << st_msg_ote_m_pc_rcv.head.status << L" (�ڑ���PC)" << st_msg_ote_m_pc_rcv.head.tgid;
			SetWindowText(h_static_ote[OTE_INDEX_STATIC_IFCHK_MSG_HEAD_R], msg_wos.str().c_str());

			msg_wos.str(L"");
			//OTE �}���`�L���X�g��M�@BODY��
			msg_wos << L"RCV BODY:";
		}
		else;

	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case OTE_ID_PB_CHK_RADIO + OTE_INDEX_RADIO_IFCHK_UNI: {
			ifchk_disp_item = OTE_INDEX_RADIO_IFCHK_UNI;
		}break;
		case OTE_ID_PB_CHK_RADIO + OTE_INDEX_RADIO_IFCHK_MPC: {
			ifchk_disp_item = OTE_INDEX_RADIO_IFCHK_MPC;
		}break;
		case OTE_ID_PB_CHK_RADIO + OTE_INDEX_RADIO_IFCHK_MTE: {
			ifchk_disp_item = OTE_INDEX_RADIO_IFCHK_MTE;
		}break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		BOOL brtn = TransparentBlt(st_work_wnd.hdc_mem0, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H,		//DST
			st_work_wnd.hdc_mem_inf, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H,	//SORCE
			RGB(255, 255, 255));

		BitBlt(hdc, 0, 0, OTE_WORK_WND_W, OTE_WORK_WND_H, st_work_wnd.hdc_mem0, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		KillTimer(hWnd, ID_OTE_IF_CHK_TIMER);
		//PostQuitMessage(0);
	}break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
}
//*********************************************************************************************
/// <summary>
/// OTE�E�B���h�E��ɃI�u�W�F�N�g�z�u
/// </summary>
void COteIF::set_OTE_panel_objects(HWND hWnd) {

	HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

	//�ʐM�J�E���g�\��
	h_static_ote[OTE_INDEX_STATIC_MSG_CNT] = CreateWindowW(TEXT("STATIC"), L"SND CNT U: -", WS_CHILD | WS_VISIBLE | SS_LEFT,
		ote_static_loc[OTE_INDEX_STATIC_MSG_CNT].x, ote_static_loc[OTE_INDEX_STATIC_MSG_CNT].y, 
		ote_static_size[OTE_INDEX_STATIC_MSG_CNT].cx, ote_static_size[OTE_INDEX_STATIC_MSG_CNT].cy,
		hWnd, (HMENU)(OTE_ID_STATIC + OTE_INDEX_STATIC_MSG_CNT), hInst, NULL);

	//���[�h�ݒ胉�W�I�{�^��
	{
		for (LONGLONG i = 0; i < 5; i++) {
			msg_wos.str(L""); msg_wos << i;
			if (i)// | WS_GROUP ����
				h_pb_ote[i] = CreateWindow(L"BUTTON", pb_text[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					ote_pb_chk_radio_loc[i].x, ote_pb_chk_radio_loc[i].y, ote_pb_chk_radio_size[i].cx, ote_pb_chk_radio_size[i].cy,
					hWnd, (HMENU)(OTE_ID_PB_CHK_RADIO + i), hInst, NULL);

			else
				h_pb_ote[i] = CreateWindow(L"BUTTON", pb_text[i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					ote_pb_chk_radio_loc[i].x, ote_pb_chk_radio_loc[i].y, ote_pb_chk_radio_size[i].cx, ote_pb_chk_radio_size[i].cy,
					hWnd, (HMENU)(OTE_ID_PB_CHK_RADIO + i), hInst, NULL);
		}
	}
	//�m�b�`���W�I�{�^��
	//�努
	{
		
		//�e�L�X�g
		CreateWindowW(TEXT("STATIC"), L"MH", WS_CHILD | WS_VISIBLE | SS_CENTER,
		ote_notch_pb_loc[ID_HOIST].x, ote_notch_pb_loc[ID_HOIST].y - ote_notch_radio_pb_size.cy * (OTE_N_NOTCH+1)-5, 30, 20,
		hWnd, (HMENU)(OTE_ID_RADIO_HST_NOTCH - OTE_N_NOTCH -1), hInst, NULL);
		//���W�I�{�^��
		for (int i = -OTE_N_NOTCH; i <= OTE_N_NOTCH; i++) {
			msg_wos.str(L""); msg_wos << i;
			if(i== -OTE_N_NOTCH)// | WS_GROUP ����
				h_rdio_ote_notch[ID_HOIST][i + OTE_N_NOTCH] = CreateWindow(L"BUTTON", msg_wos.str().c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					ote_notch_pb_loc[ID_HOIST].x, ote_notch_pb_loc[ID_HOIST].y - ote_notch_radio_pb_size.cy * i, ote_notch_radio_pb_size.cx, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_RADIO_HST_NOTCH + (LONG64)i), hInst, NULL);
			else
				h_rdio_ote_notch[ID_HOIST][i + OTE_N_NOTCH] = CreateWindow(L"BUTTON", msg_wos.str().c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					ote_notch_pb_loc[ID_HOIST].x, ote_notch_pb_loc[ID_HOIST].y - ote_notch_radio_pb_size.cy * i, ote_notch_radio_pb_size.cx, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_RADIO_HST_NOTCH + (LONG64)i), hInst, NULL);

			if(i==0)
			h_static_ote_notch[ID_HOIST][i + OTE_N_NOTCH] = CreateWindowW(TEXT("STATIC"), L"*", WS_CHILD | WS_VISIBLE | SS_LEFT,
				ote_notch_pb_loc[ID_HOIST].x + ote_notch_radio_pb_size.cx +2, ote_notch_pb_loc[ID_HOIST].y - ote_notch_radio_pb_size.cy * i, 10, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_STATIC_HST_NOTCH - OTE_N_NOTCH - 1), hInst, NULL);
			else
				h_static_ote_notch[ID_HOIST][i + OTE_N_NOTCH] = CreateWindowW(TEXT("STATIC"), L" ", WS_CHILD | WS_VISIBLE | SS_LEFT,
					ote_notch_pb_loc[ID_HOIST].x + ote_notch_radio_pb_size.cx + 2, ote_notch_pb_loc[ID_HOIST].y - ote_notch_radio_pb_size.cy * i, 10, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_STATIC_HST_NOTCH - OTE_N_NOTCH - 1), hInst, NULL);

			SendMessage(h_rdio_ote_notch[ID_HOIST][OTE_N_NOTCH], BM_SETCHECK, BST_CHECKED, 0L);
			ote_io_workbuf.ote_in.notch_pos[ID_HOIST] = 0;
		}
	}
	//���s
	{
		//�e�L�X�g
		CreateWindowW(TEXT("STATIC"), L"GT", WS_CHILD | WS_VISIBLE | SS_CENTER,
			ote_notch_pb_loc[ID_GANTRY].x - ote_notch_radio_pb_size.cx * (OTE_N_NOTCH + 1) - 5, ote_notch_pb_loc[ID_GANTRY].y , 30, 20,
			hWnd, (HMENU)(OTE_ID_RADIO_GNT_NOTCH - OTE_N_NOTCH - 1), hInst, NULL);
		//���W�I�{�^��
		for (int i = -OTE_N_NOTCH; i <= OTE_N_NOTCH; i++) {
			msg_wos.str(L""); msg_wos << i;
			if (i == -OTE_N_NOTCH)// | WS_GROUP ����
				h_rdio_ote_notch[ID_GANTRY][i + OTE_N_NOTCH] = CreateWindow(L"BUTTON", msg_wos.str().c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					ote_notch_pb_loc[ID_GANTRY].x + ote_notch_radio_pb_size.cx * i, ote_notch_pb_loc[ID_GANTRY].y, ote_notch_radio_pb_size.cx, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_RADIO_GNT_NOTCH + (LONG64)i), hInst, NULL);
			else
				h_rdio_ote_notch[ID_GANTRY][i + OTE_N_NOTCH] = CreateWindow(L"BUTTON", msg_wos.str().c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					ote_notch_pb_loc[ID_GANTRY].x + ote_notch_radio_pb_size.cx * i, ote_notch_pb_loc[ID_GANTRY].y , ote_notch_radio_pb_size.cx, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_RADIO_GNT_NOTCH + (LONG64)i), hInst, NULL);

			if (i == 0)
				h_static_ote_notch[ID_GANTRY][i + OTE_N_NOTCH] = CreateWindowW(TEXT("STATIC"), L"*", WS_CHILD | WS_VISIBLE | SS_CENTER,
					ote_notch_pb_loc[ID_GANTRY].x + ote_notch_radio_pb_size.cx * i, ote_notch_pb_loc[ID_GANTRY].y + ote_notch_radio_pb_size.cy, ote_notch_radio_pb_size.cx, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_STATIC_GNT_NOTCH - OTE_N_NOTCH - 1), hInst, NULL);
			else
				h_static_ote_notch[ID_GANTRY][i + OTE_N_NOTCH] = CreateWindowW(TEXT("STATIC"), L" ", WS_CHILD | WS_VISIBLE | SS_CENTER,
					ote_notch_pb_loc[ID_GANTRY].x + ote_notch_radio_pb_size.cx * i, ote_notch_pb_loc[ID_GANTRY].y + ote_notch_radio_pb_size.cy, ote_notch_radio_pb_size.cx, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_STATIC_GNT_NOTCH - OTE_N_NOTCH - 1), hInst, NULL);

			SendMessage(h_rdio_ote_notch[ID_GANTRY][OTE_N_NOTCH], BM_SETCHECK, BST_CHECKED, 0L);
			ote_io_workbuf.ote_in.notch_pos[ID_GANTRY] = 0;
		}
	}
	//�⊪
	{
		//�e�L�X�g
		CreateWindowW(TEXT("STATIC"), L"AH", WS_CHILD | WS_VISIBLE | SS_CENTER,
			ote_notch_pb_loc[ID_AHOIST].x, ote_notch_pb_loc[ID_AHOIST].y - ote_notch_radio_pb_size.cy * (OTE_N_NOTCH + 1) - 5, 30, 20,
			hWnd, (HMENU)(OTE_ID_RADIO_AH_NOTCH - OTE_N_NOTCH - 1), hInst, NULL);
		//���W�I�{�^��
		for (int i = -OTE_N_NOTCH; i <= OTE_N_NOTCH; i++) {
			msg_wos.str(L""); msg_wos << i;
			if (i == -OTE_N_NOTCH)// | WS_GROUP ����
				h_rdio_ote_notch[ID_AHOIST][i + OTE_N_NOTCH] = CreateWindow(L"BUTTON", msg_wos.str().c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					ote_notch_pb_loc[ID_AHOIST].x, ote_notch_pb_loc[ID_AHOIST].y - ote_notch_radio_pb_size.cy * i, ote_notch_radio_pb_size.cx, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_RADIO_AH_NOTCH + (LONG64)i), hInst, NULL);
			else
				h_rdio_ote_notch[ID_AHOIST][i + OTE_N_NOTCH] = CreateWindow(L"BUTTON", msg_wos.str().c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					ote_notch_pb_loc[ID_AHOIST].x, ote_notch_pb_loc[ID_AHOIST].y - ote_notch_radio_pb_size.cy * i, ote_notch_radio_pb_size.cx, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_RADIO_AH_NOTCH + (LONG64)i), hInst, NULL);

			if (i == 0)
				h_static_ote_notch[ID_AHOIST][i + OTE_N_NOTCH] = CreateWindowW(TEXT("STATIC"), L"*", WS_CHILD | WS_VISIBLE | SS_LEFT,
					ote_notch_pb_loc[ID_AHOIST].x + ote_notch_radio_pb_size.cx + 2, ote_notch_pb_loc[ID_AHOIST].y - ote_notch_radio_pb_size.cy * i, 10, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_STATIC_AH_NOTCH - OTE_N_NOTCH - 1), hInst, NULL);
			else
				h_static_ote_notch[ID_AHOIST][i + OTE_N_NOTCH] = CreateWindowW(TEXT("STATIC"), L" ", WS_CHILD | WS_VISIBLE | SS_LEFT,
					ote_notch_pb_loc[ID_AHOIST].x + ote_notch_radio_pb_size.cx + 2, ote_notch_pb_loc[ID_AHOIST].y - ote_notch_radio_pb_size.cy * i, 10, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_STATIC_AH_NOTCH - OTE_N_NOTCH - 1), hInst, NULL);

			SendMessage(h_rdio_ote_notch[ID_AHOIST][OTE_N_NOTCH], BM_SETCHECK, BST_CHECKED, 0L);
			ote_io_workbuf.ote_in.notch_pos[ID_AHOIST] = 0;
		}
	}
	//����
	{
		//�e�L�X�g
		CreateWindowW(TEXT("STATIC"), L"BH", WS_CHILD | WS_VISIBLE | SS_CENTER,
			ote_notch_pb_loc[ID_BOOM_H].x, ote_notch_pb_loc[ID_BOOM_H].y - ote_notch_radio_pb_size.cy * (OTE_N_NOTCH + 1) - 5, 30, 20,
			hWnd, (HMENU)(OTE_ID_RADIO_BH_NOTCH - OTE_N_NOTCH - 1), hInst, NULL);
		//���W�I�{�^��
		for (int i = -OTE_N_NOTCH; i <= OTE_N_NOTCH; i++) {
			msg_wos.str(L""); msg_wos << i;
			if (i == -OTE_N_NOTCH)// | WS_GROUP ����
				h_rdio_ote_notch[ID_BOOM_H][i + OTE_N_NOTCH] = CreateWindow(L"BUTTON", msg_wos.str().c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					ote_notch_pb_loc[ID_BOOM_H].x, ote_notch_pb_loc[ID_BOOM_H].y - ote_notch_radio_pb_size.cy * i, ote_notch_radio_pb_size.cx, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_RADIO_BH_NOTCH + (LONG64)i), hInst, NULL);
			else
				h_rdio_ote_notch[ID_BOOM_H][i + OTE_N_NOTCH] = CreateWindow(L"BUTTON", msg_wos.str().c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					ote_notch_pb_loc[ID_BOOM_H].x, ote_notch_pb_loc[ID_BOOM_H].y - ote_notch_radio_pb_size.cy * i, ote_notch_radio_pb_size.cx, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_RADIO_BH_NOTCH + (LONG64)i), hInst, NULL);

			if (i == 0)
				h_static_ote_notch[ID_BOOM_H][i + OTE_N_NOTCH] = CreateWindowW(TEXT("STATIC"), L"*", WS_CHILD | WS_VISIBLE | SS_LEFT,
					ote_notch_pb_loc[ID_BOOM_H].x + ote_notch_radio_pb_size.cx + 2, ote_notch_pb_loc[ID_BOOM_H].y - ote_notch_radio_pb_size.cy * i, 10, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_STATIC_BH_NOTCH - OTE_N_NOTCH - 1), hInst, NULL);
			else
				h_static_ote_notch[ID_BOOM_H][i + OTE_N_NOTCH] = CreateWindowW(TEXT("STATIC"), L" ", WS_CHILD | WS_VISIBLE | SS_LEFT,
					ote_notch_pb_loc[ID_BOOM_H].x + ote_notch_radio_pb_size.cx + 2, ote_notch_pb_loc[ID_BOOM_H].y - ote_notch_radio_pb_size.cy * i, 10, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_STATIC_AH_NOTCH - OTE_N_NOTCH - 1), hInst, NULL);

			SendMessage(h_rdio_ote_notch[ID_BOOM_H][OTE_N_NOTCH], BM_SETCHECK, BST_CHECKED, 0L);
			ote_io_workbuf.ote_in.notch_pos[ID_BOOM_H] = 0;
		}
	}
	//����
	{
		//�e�L�X�g
		CreateWindowW(TEXT("STATIC"), L"SL", WS_CHILD | WS_VISIBLE | SS_CENTER,
			ote_notch_pb_loc[ID_SLEW].x - ote_notch_radio_pb_size.cx * (OTE_N_NOTCH + 1) - 5, ote_notch_pb_loc[ID_SLEW].y, 30, 20,
			hWnd, (HMENU)(OTE_ID_RADIO_SLW_NOTCH - OTE_N_NOTCH - 1), hInst, NULL);
		//���W�I�{�^��
		for (int i = -OTE_N_NOTCH; i <= OTE_N_NOTCH; i++) {
			msg_wos.str(L""); msg_wos << i;
			if (i == -OTE_N_NOTCH)// | WS_GROUP ����
				h_rdio_ote_notch[ID_SLEW][i + OTE_N_NOTCH] = CreateWindow(L"BUTTON", msg_wos.str().c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					ote_notch_pb_loc[ID_SLEW].x + ote_notch_radio_pb_size.cx * i, ote_notch_pb_loc[ID_SLEW].y, ote_notch_radio_pb_size.cx, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_RADIO_SLW_NOTCH + (LONG64)i), hInst, NULL);
			else
				h_rdio_ote_notch[ID_SLEW][i + OTE_N_NOTCH] = CreateWindow(L"BUTTON", msg_wos.str().c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					ote_notch_pb_loc[ID_SLEW].x + ote_notch_radio_pb_size.cx * i, ote_notch_pb_loc[ID_SLEW].y, ote_notch_radio_pb_size.cx, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_RADIO_SLW_NOTCH + (LONG64)i), hInst, NULL);

			if (i == 0)
				h_static_ote_notch[ID_SLEW][i + OTE_N_NOTCH] = CreateWindowW(TEXT("STATIC"), L"*", WS_CHILD | WS_VISIBLE | SS_CENTER,
					ote_notch_pb_loc[ID_SLEW].x + ote_notch_radio_pb_size.cx * i, ote_notch_pb_loc[ID_SLEW].y + ote_notch_radio_pb_size.cy, ote_notch_radio_pb_size.cx, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_STATIC_SLW_NOTCH - OTE_N_NOTCH - 1), hInst, NULL);
			else
				h_static_ote_notch[ID_SLEW][i + OTE_N_NOTCH] = CreateWindowW(TEXT("STATIC"), L" ", WS_CHILD | WS_VISIBLE | SS_CENTER,
					ote_notch_pb_loc[ID_SLEW].x + ote_notch_radio_pb_size.cx * i, ote_notch_pb_loc[ID_SLEW].y + ote_notch_radio_pb_size.cy, ote_notch_radio_pb_size.cx, ote_notch_radio_pb_size.cy,
					hWnd, (HMENU)(OTE_ID_STATIC_SLW_NOTCH - OTE_N_NOTCH - 1), hInst, NULL);

			SendMessage(h_rdio_ote_notch[ID_SLEW][OTE_N_NOTCH], BM_SETCHECK, BST_CHECKED, 0L);
			ote_io_workbuf.ote_in.notch_pos[ID_SLEW] = 0;
		}
	}
	//�{�^��
	{
		//����~	
		h_pb_ote[OTE_INDEX_CHK_ESTOP] = CreateWindow(L"BUTTON", pb_text[OTE_INDEX_CHK_ESTOP], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
			ote_pb_chk_radio_loc[OTE_INDEX_CHK_ESTOP].x, ote_pb_chk_radio_loc[OTE_INDEX_CHK_ESTOP].y, ote_pb_chk_radio_size[OTE_INDEX_CHK_ESTOP].cx, ote_pb_chk_radio_size[OTE_INDEX_CHK_ESTOP].cy,
			hWnd, (HMENU)(OTE_ID_PB_CHK_RADIO + OTE_INDEX_CHK_ESTOP), hInst, NULL);
		//�劲	
		h_pb_ote[OTE_INDEX_PB_CTR_SOURCE] = CreateWindow(L"BUTTON", pb_text[OTE_INDEX_PB_CTR_SOURCE], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			ote_pb_chk_radio_loc[OTE_INDEX_PB_CTR_SOURCE].x, ote_pb_chk_radio_loc[OTE_INDEX_PB_CTR_SOURCE].y, ote_pb_chk_radio_size[OTE_INDEX_PB_CTR_SOURCE].cx, ote_pb_chk_radio_size[OTE_INDEX_PB_CTR_SOURCE].cy,
			hWnd, (HMENU)(OTE_ID_PB_CHK_RADIO + OTE_INDEX_PB_CTR_SOURCE), hInst, NULL);
		//�̏჊�Z�b�g	
		h_pb_ote[OTE_INDEX_PB_FAULT_RESET] = CreateWindow(L"BUTTON", pb_text[OTE_INDEX_PB_FAULT_RESET], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			ote_pb_chk_radio_loc[OTE_INDEX_PB_FAULT_RESET].x, ote_pb_chk_radio_loc[OTE_INDEX_PB_FAULT_RESET].y, ote_pb_chk_radio_size[OTE_INDEX_PB_FAULT_RESET].cx, ote_pb_chk_radio_size[OTE_INDEX_PB_FAULT_RESET].cy,
			hWnd, (HMENU)(OTE_ID_PB_CHK_RADIO + OTE_INDEX_PB_FAULT_RESET), hInst, NULL);
	}

	return;
}
//*********************************************************************************************
/// <summary>
/// 
/// </summary>
/// <param name="srcw"></param>
void COteIF::wstr_out_inf(const std::wstring& srcw) {
	return;
}
//****************************************************************************
/// <summary>
/// �ʐM����M�J�E���^�̕\��
/// </summary>
void COteIF::disp_msg_cnt() {
	msg_wos.str(L"");
	msg_wos << L"CNT PC#U�y��:" << std::dec << cnt_rcv_ote_u <<L" ��:" << cnt_snd_pc_u << L"�z"
		<< L"]  #M�y��O:" << cnt_snd_pc_m_ote << L"��P:" << cnt_snd_pc_m_pc << L" ��P:" << cnt_rcv_pc_m_pc << L"�z";

	SetWindowText(h_static_ote[OTE_INDEX_STATIC_MSG_CNT], msg_wos.str().c_str());

	msg_wos.str(L"");
	msg_wos << L"#U�y��:" << std::dec << cnt_snd_ote_u << L" ��:" << cnt_rcv_pc_u
		<< L"�z  \n#M�y��:" << cnt_snd_ote_m_ote << L" ��M:" << cnt_rcv_ote_m_ote <<  L" ��P:" << cnt_rcv_pc_m_ote << L"�z";

	SetWindowText(h_static_ote[OTE_INDEX_STATIC_CONNECT_CNT], msg_wos.str().c_str());
}
//****************************************************************************
/// <summary>
/// �ʐMIP���̕\��
/// </summary>
void COteIF::disp_ip_inf() {
	sockaddr_in* psockaddr = (sockaddr_in*)&addrin_pc_u_ote;
	msg_wos.str(L"");
	msg_wos << L" U0   :" << psockaddr->sin_addr.S_un.S_un_b.s_b1 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b2 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b3 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b4
			<< L" : " << htons(psockaddr->sin_port) << L" \n";
	psockaddr = (sockaddr_in*)&addrin_pc_m_ote;
	msg_wos << L" M0   :" << psockaddr->sin_addr.S_un.S_un_b.s_b1 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b2 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b3 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b4
			<< L" : " << htons(psockaddr->sin_port) << L" \n";
	psockaddr = (sockaddr_in*)&addrin_ote_m_ote;
	msg_wos << L" M1   :" << psockaddr->sin_addr.S_un.S_un_b.s_b1 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b2 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b3 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b4
		<< L" : " << htons(psockaddr->sin_port) << L" \n";

	psockaddr = (sockaddr_in*)&(pSockPcUniCastOte->addr_in_from);
	msg_wos << L" PP��  :" << psockaddr->sin_addr.S_un.S_un_b.s_b1 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b2 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b3 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b4
		<< L" : " << htons(psockaddr->sin_port) << L" \n";
	psockaddr = (sockaddr_in*)&(pSockPcMultiCastOte->addr_in_from);
	msg_wos << L" PM�� :" << psockaddr->sin_addr.S_un.S_un_b.s_b1 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b2 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b3 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b4
		<< L" : " << htons(psockaddr->sin_port) << L" \n";
	psockaddr = (sockaddr_in*)&(pSockOteMultiCastOte->addr_in_from);
	msg_wos << L" MM��:" << psockaddr->sin_addr.S_un.S_un_b.s_b1 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b2 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b3 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b4
		<< L" : " << htons(psockaddr->sin_port) << L" \n";

	SetWindowText(h_static_ote[OTE_INDEX_STATIC_OTE_IP_INF], msg_wos.str().c_str());
}
//*********************************************************************************************
/// <summary>
///  OTE��PC�@Unicast�@�yOTE��PC����̃��j�L���X�g��M�\�P�b�g�ő��M�z
/// </summary>
/// <param name="pbuf"></param>
/// <param name="pto_addrin"></param>
/// <returns></returns>
HRESULT COteIF::snd_ote_u_pc(LPST_OTE_U_MSG pbuf, SOCKADDR_IN* p_addrin_to) {
	if (pSockPcUniCastOte->snd_udp_msg((char*)pbuf, sizeof(ST_OTE_U_MSG), *p_addrin_to) == SOCKET_ERROR) {
		msg_wos.str() = pSockPcUniCastOte->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}
//*********************************************************************************************
/// <summary>
/// PC��OTE�@Unicast�@�yPC��OTE����̃��j�L���X�g��M�\�P�b�g�ő��M�z
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COteIF::snd_pc_u_ote(LPST_PC_U_MSG pbuf, SOCKADDR_IN* p_addrin_to) {
	if (pSockOteUniCastPc->snd_udp_msg((char*)pbuf, sizeof(ST_PC_M_MSG), *p_addrin_to) == SOCKET_ERROR) {
		msg_wos.str() = pSockOteUniCastPc->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}
//*********************************************************************************************
/// <summary>
/// PC->PC �}���`�L���X�g�@�yPC��PC����̃}���`�L���X�g��M�\�P�b�g�ő��M�z
/// </summary>
/// <param name="pbuf"></param>
/// <param name="pto_addrin"></param>
/// <returns></returns>
HRESULT COteIF::snd_pc_m_pc(LPST_PC_M_MSG pbuf) {
	if (pSockPcMultiCastPc->snd_udp_msg((char*)pbuf, sizeof(ST_PC_M_MSG), addrin_pc_m_pc_snd) == SOCKET_ERROR) {
		msg_wos.str() = pSockPcMultiCastOte->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}
//*********************************************************************************************
/// <summary>
/// OTE->PC �}���`�L���X�g�@�yOTE��PC����̃}���`�L���X�g��M�\�P�b�g�ő��M�z
/// </summary>
/// <param name="pbuf"></param>
/// <param name="pto_addrin"></param>
/// <returns></returns>
HRESULT COteIF::snd_ote_m_pc(LPST_OTE_M_MSG pbuf) {
	if (pSockPcMultiCastOte->snd_udp_msg((char*)pbuf, sizeof(ST_OTE_M_MSG), addrin_ote_m_pc) == SOCKET_ERROR) {
		msg_wos.str() = pSockPcMultiCastOte->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}//OTE���j�L���X�g���M����
//*********************************************************************************************
/// <summary>
/// OTE->OTE �}���`�L���X�g�@�yOTE��OTE����̃}���`�L���X�g��M�\�P�b�g�ő��M�z
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COteIF::snd_ote_m_ote(LPST_OTE_M_MSG pbuf) {
	if (pSockOteMultiCastOte->snd_udp_msg((char*)pbuf, sizeof(ST_OTE_M_MSG), addrin_ote_m_ote_snd) == SOCKET_ERROR) {
		msg_wos.str() = pSockOteMultiCastOte->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}//OTE�}���`�L���X�g���M����
//*********************************************************************************************
/// <summary>
/// PC->OTE �}���`�L���X�g�@�yPC��OTE����̃}���`�L���X�g��M�\�P�b�g�ő��M�z
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COteIF::snd_pc_m_ote(LPST_PC_M_MSG pbuf) {
	if (pSockOteMultiCastPc->snd_udp_msg((char*)pbuf, sizeof(ST_PC_M_MSG), addrin_pc_m_ote_snd) == SOCKET_ERROR) {
		msg_wos.str() = pSockOteMultiCastPc->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}//OTE�}���`�L���X�g���M����
//��M�����p
//*********************************************************************************************
/// <summary>
///PC���j�L���X�g�d����M���� (OTE���j�L���X�g���b�Z�[�W����M�j
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COteIF::rcv_ote_u_pc(LPST_OTE_U_MSG pbuf) {
	int nRtn = pSockOteUniCastPc->rcv_udp_msg((char*)pbuf, sizeof(ST_OTE_U_MSG));
	if (nRtn == SOCKET_ERROR) {
		msg_wos.str() = pSockOteUniCastPc->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}
//*********************************************************************************************
/// <summary>
///PC�}���`�L���X�g�d����M����  (PC�}���`�L���X�g���b�Z�[�W����M�j
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COteIF::rcv_pc_m_pc(LPST_PC_M_MSG pbuf) {
	int nRtn = pSockPcMultiCastPc->rcv_udp_msg((char*)pbuf, sizeof(ST_PC_M_MSG));
	if (nRtn == SOCKET_ERROR) {
		msg_wos.str() = pSockPcMultiCastPc->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}
//*********************************************************************************************
/// <summary>
/// OTE���j�L���X�g�d����M����(PC���j�L���X�g���b�Z�[�W����M�j
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COteIF::rcv_ote_m_pc(LPST_OTE_M_MSG pbuf) {
	int nRtn = pSockOteMultiCastPc->rcv_udp_msg((char*)pbuf, sizeof(ST_PC_U_MSG));
	if (nRtn == SOCKET_ERROR) {
		msg_wos.str() = pSockOteUniCastPc->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}
//*********************************************************************************************
/// <summary>
/// OTE�}���`�L���X�g�d����M�����iOTE�}���`�L���X�g���b�Z�[�W����M�j
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COteIF::rcv_pc_u_ote(LPST_PC_U_MSG pbuf) {
	int nRtn = pSockPcUniCastOte->rcv_udp_msg((char*)pbuf, sizeof(ST_PC_U_MSG));
	if (nRtn == SOCKET_ERROR) {
		msg_wos.str() = pSockPcUniCastOte->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}
//*********************************************************************************************
/// <summary>
/// OTE�}���`�L���X�g�d����M�����iOTE�}���`�L���X�g���b�Z�[�W����M�j
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COteIF::rcv_ote_m_ote(LPST_OTE_M_MSG pbuf) {
	int nRtn = pSockOteMultiCastOte->rcv_udp_msg((char*)pbuf, sizeof(ST_OTE_M_MSG));
	if (nRtn == SOCKET_ERROR) {
		msg_wos.str() = pSockOteMultiCastOte->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}
//*********************************************************************************************
/// <summary>
/// OTE�}���`�L���X�g�d����M�����iOTE�}���`�L���X�g���b�Z�[�W����M�j
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COteIF::rcv_pc_m_ote(LPST_PC_M_MSG pbuf) {
	int nRtn = pSockPcMultiCastOte->rcv_udp_msg((char*)pbuf, sizeof(ST_PC_M_MSG));
	if (nRtn == SOCKET_ERROR) {
		msg_wos.str() = pSockPcMultiCastOte->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}




