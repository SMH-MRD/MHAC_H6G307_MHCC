#include "framework.h"
#include "COTE_IF.h"
#include <windowsx.h>       //# �R�����R���g���[��
#include <winsock2.h>

#include <iostream>
#include <iomanip>
#include <sstream>

#include "OTE0panel.h"

ST_OTE_WORK_WND st_ote_work;

std::wstring COteIF::msg_ws;
std::wostringstream COteIF::msg_wos, COteIF::msg_wos2;

ST_OTE_IO COteIF::ote_io_workbuf;

LPST_OTE_IO COteIF::pOTEio;
LPST_CRANE_STATUS COteIF::pCraneStat;
LPST_PLC_IO COteIF::pPLCio;
LPST_CS_INFO COteIF::pCSInf;
LPST_AGENT_INFO COteIF::pAgentInf;
LPST_SWAY_IO COteIF::pSway_IO;

HWND COteIF::hWnd_parent;					//�e�E�B���h�E�̃n���h��
HWND COteIF::hWnd_work; 					//�ʐM�C�x���g�����p�E�B���h�E�n���h��

ST_OTEIF_WORK_WND COteIF::st_work_wnd;

CSockAlpha* COteIF::pSockOteUniCastPc;		//PC���j�L���X�g�p�\�P�b�g
CSockAlpha* COteIF::pSockPcMultiCastPc;		//PC�}���`�L���X�gPC��M�p�\�P�b�g
CSockAlpha* COteIF::pSockOteMultiCastPc;	//OTE�}���`�L���X�gPC��M�p�\�P�b�g

SOCKADDR_IN COteIF::addrin_ote_u_pc;		//PC���j�L���X�g�p�A�h���X(PC��M�p)
SOCKADDR_IN COteIF::addrin_pc_m_pc;			//PC�}���`�L���X�g��M�A�h���X(PC��M�p)
SOCKADDR_IN COteIF::addrin_ote_m_pc;		//OTE�}���`�L���X�g��M�A�h���X(PC��M�p)

SOCKADDR_IN COteIF::addrin_pc_u_snd;		//PC���j�L���X�g���M��A�h���X�iOTE��M�p�j
SOCKADDR_IN COteIF::addrin_pc_m_ote_snd;	//PC�}���`�L���X�g���M��A�h���X�iOTE��M�p�j
SOCKADDR_IN COteIF::addrin_pc_m_pc_snd;		//PC�}���`�L���X�g���M��A�h���X�iPC��M�p�j

SOCKADDR_IN COteIF::addrin_ote_u_from;	//OTE���j�L���X�g���M���A�h���X�iPC�p)
SOCKADDR_IN COteIF::addrin_pc_m_from;	//PC�}���`�L���X�g���M���A�h���X�iPC�p)
SOCKADDR_IN COteIF::addrin_ote_m_from;	//OTE�}���`�L���X�g���M���A�h���X�iPC�p)


SOCKADDR_IN COteIF::addr_active_ote;		//����M�����L���Ȓ[���̃A�h���X

LONG COteIF::cnt_snd_pc_u, COteIF::cnt_snd_pc_m_pc, COteIF::cnt_snd_pc_m_ote;
LONG COteIF::cnt_rcv_ote_u, COteIF::cnt_rcv_ote_m,COteIF::cnt_rcv_pc_m;

ST_PC_U_MSG COteIF::st_msg_pc_u_snd;
ST_PC_M_MSG COteIF::st_msg_pc_m_snd;

ST_PC_M_MSG COteIF::st_msg_pc_m_pc_rcv;
ST_PC_M_MSG COteIF::st_msg_pc_m_ote_rcv;

ST_OTE_M_MSG COteIF::st_msg_ote_m_pc_rcv;
ST_OTE_M_MSG COteIF::st_msg_ote_m_ote_rcv;
ST_OTE_U_MSG COteIF::st_msg_ote_u_rcv;

ST_OTE_U_MSG COteIF::st_ote_active_msg;	//����M�����L���Ȍ����b�Z�[�W

/*****************************************************************************/
/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="hWnd">�e�E�B���h�E�̃n���h��</param>
COteIF::COteIF(HWND hWnd) {
    hWnd_parent = hWnd;
    // ���L�������I�u�W�F�N�g�̃C���X�^���X��
    pOteIOObj				= new CSharedMem;
    pCraneStatusObj			= new CSharedMem;
    pSimulationStatusObj	= new CSharedMem;
    pPLCioObj				= new CSharedMem;
    pCSInfObj				= new CSharedMem;
    pAgentInfObj			= new CSharedMem;
    pSwayIO_Obj				= new CSharedMem;

	cnt_snd_pc_u = cnt_snd_pc_m_pc = cnt_snd_pc_m_ote = 0;
	cnt_rcv_ote_u = cnt_rcv_ote_m = cnt_rcv_pc_m = 0;
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

		if (OK_SHMEM != pSwayIO_Obj->create_smem(SMEM_SWAY_IO_NAME, sizeof(ST_SWAY_IO), MUTEX_SWAY_IO_NAME)) {
			mode |= OTE_IF_PLC_MEM_NG;
		}

		pCraneStat = (LPST_CRANE_STATUS)(pCraneStatusObj->get_pMap());
		pPLCio = (LPST_PLC_IO)(pPLCioObj->get_pMap());
		pOTEio = (LPST_OTE_IO)pOteIOObj->get_pMap();
		pCSInf = (LPST_CS_INFO)pCSInfObj->get_pMap();
		pAgentInf = (LPST_AGENT_INFO)pAgentInfObj->get_pMap();
		pSway_IO = (LPST_SWAY_IO)(pSwayIO_Obj->get_pMap());
	}
    
	//����[���A�ʐM�C�x���g�����p�E�B���h�E�I�[�v��
	hWnd_work = open_work_Wnd(hWnd_parent);					//����[���E�B���hOPEN

	//### �\�P�b�g�A�h���X�Z�b�g
	memset(&addrin_ote_u_pc	, 0, sizeof(SOCKADDR_IN));	memset(&addrin_pc_m_pc, 0, sizeof(SOCKADDR_IN));memset(&addrin_ote_m_pc, 0, sizeof(SOCKADDR_IN));
	memset(&addrin_pc_m_pc_snd, 0, sizeof(SOCKADDR_IN));memset(&addrin_pc_m_ote_snd, 0, sizeof(SOCKADDR_IN)); 
	
	//��M�A�h���X
	set_sock_addr(&addrin_ote_u_pc, OTE_IF_UNICAST_IP_PC, OTE_IF_UNICAST_PORT_PC);
	set_sock_addr(&addrin_ote_m_pc, OTE_IF_UNICAST_IP_PC, OTE_IF_MULTICAST_PORT_OTE2PC);
	set_sock_addr(&addrin_pc_m_pc,	OTE_IF_UNICAST_IP_PC, OTE_IF_MULTICAST_PORT_PC2PC);

	//���M��A�h���X
	set_sock_addr(&addrin_pc_m_pc_snd , OTE_IF_MULTICAST_IP_PC, OTE_IF_MULTICAST_PORT_PC2PC);
	set_sock_addr(&addrin_pc_m_ote_snd, OTE_IF_MULTICAST_IP_PC, OTE_IF_MULTICAST_PORT_PC2OTE);
	 	
	//### �\�P�b�g�ݒ�
	pSockOteUniCastPc	= new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_SERVER, ID_SOCK_EVENT_OTE_UNI_PC);
	pSockPcMultiCastPc	= new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_PC_MULTI_PC);
	pSockOteMultiCastPc = new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_OTE_MULTI_PC);

	//### �I�u�W�F�N�g������
	if (pSockOteUniCastPc->Initialize()		!= S_OK) { msg_wos.str() = pSockOteUniCastPc->err_msg.str();	return NULL; }
	if (pSockPcMultiCastPc->Initialize()	!= S_OK) { msg_wos.str() = pSockPcMultiCastPc->err_msg.str();	return NULL; }
	if (pSockOteMultiCastPc->Initialize()	!= S_OK) { msg_wos.str() = pSockOteMultiCastPc->err_msg.str();	return NULL; }

	//### �\�P�b�g�ݒ�
	//���j�L���X�g�p init_sock():bind()���񓯊����܂Ŏ��{
	if (pSockOteUniCastPc->init_sock(hWnd_work,	addrin_ote_u_pc	)	!= S_OK) { msg_wos.str() = pSockOteUniCastPc->err_msg.str(); return NULL;}

	//�}���`�L���X�g�p init_sock_m():bind()�܂Ŏ��{ + �}���`�L���X�g�O���[�v�֓o�^
	SOCKADDR_IN addr_tmp;
	set_sock_addr(&addr_tmp, OTE_IF_MULTICAST_IP_PC, NULL);//PC�}���`�L���X�g��MIP�Z�b�g,PORT�̓l�b�g���[�N�ݒ�i��2�����j�̃|�[�g
	if (pSockPcMultiCastPc->init_sock_m(hWnd_work, addrin_pc_m_pc, addr_tmp) != S_OK) { msg_wos.str() = pSockPcMultiCastPc->err_msg.str(); return NULL; }

	set_sock_addr(&addr_tmp, OTE_IF_MULTICAST_IP_OTE, NULL);//OTE�}���`�L���X�g��MIP�Z�b�g,PORT�̓l�b�g���[�N�ݒ�i��2�����j�̃|�[�g
	if (pSockOteMultiCastPc->init_sock_m(hWnd_work, addrin_ote_m_pc, addr_tmp) != S_OK) { msg_wos.str() = pSockOteMultiCastPc->err_msg.str(); return NULL; }

	//���M���b�Z�[�W�w�b�_�ݒ�i���M����M�A�h���X�F��M��̐܂�Ԃ��p�j
	st_msg_pc_u_snd.head.addr = addrin_ote_u_pc;
	st_msg_pc_m_snd.head.addr = addrin_ote_m_pc;

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
	
	pOTEio->ote_u_silent_cnt++;//���b�Z�[�W��M�C�x���g�ŃN���A

	bool bgrip_enable;

	//�O���b�v�X�C�b�`�L������@2�ڂ̏����̓f�o�b�O�p
	if((pOTEio->ote_umsg_in.body.grip_status & OTE_GRIP_ENABLE)|| (pOTEio->ote_umsg_in.body.grip_status & OTE_GRIP_DBG_ENABLE))
		bgrip_enable = true;
	else
		bgrip_enable = false;

	//�ً}��~�@ON/OFF
	if(!bgrip_enable && pOTEio->ote_umsg_in.body.grip_status & OTE_GRIP_ESTP)  //GURIP����؂�@����~�M��NORMAL�ŗL���r�b�gOFF
		pOTEio->ote_estop = L_ON;
	else if (bgrip_enable && !(pOTEio->ote_umsg_in.body.grip_status & OTE_GRIP_ESTP))
		pOTEio->ote_estop = L_ON;
	else if ((pOTEio->ote_umsg_in.body.grip_status & OTE_GRIP_ESTP)||!bgrip_enable)
		pOTEio->ote_estop = L_OFF;
	else														
		pOTEio->ote_estop = L_ON;

	//�O���b�v�@ON/OFF
	if (pOTEio->ote_umsg_in.body.grip_status & OTE_GRIP_ACTIVE)		pOTEio->ote_grip = L_ON;
	else															pOTEio->ote_grip = L_OFF;

	//GAME PAD MODE
	if (pOTEio->ote_umsg_in.body.ope_mode & OTE_ID_OPE_MODE_GPAD)	pOTEio->ote_padmode = L_ON;
	else															pOTEio->ote_padmode = L_OFF;


    return 0;
}    
/*****************************************************************************/
/// <summary>
/// �o�͉�͏����i�I�[�o�[���C�h�֐��j
/// </summary>
/// <returns></returns>
int COteIF::output() {                          //�o�͏���
	//�ʐM���b�Z�[�W�̓C�x���g�����ŏo��
	//if (out_size) memcpy_s(poutput, out_size, &ote_io_workbuf, out_size);
   return 0; 
}
/*****************************************************************************/
/// <summary>
/// PC���j�L���X�g���b�Z�[�W���M�o�b�t�@�Z�b�g
/// </summary>
/// <returns></returns>
LPST_PC_U_MSG COteIF:: set_msg_pc_u() {

   //#Header��
	st_msg_pc_u_snd.head.addr = addrin_ote_u_pc;
	st_msg_pc_u_snd.head.myid = pCraneStat->spec.device_code.no;
	st_msg_pc_u_snd.head.tgid = st_msg_ote_u_rcv.head.myid;
	st_msg_pc_u_snd.head.code = st_msg_ote_u_rcv.head.code;
	st_msg_pc_u_snd.head.status = CODE_ITE_RES_ACK;

	//#Body��
	//����[�����v�w��
	memcpy(st_msg_pc_u_snd.body.pb_lamp, pCSInf->ote_pb_lamp, sizeof(ST_OTE_LAMP_COM) * N_OTE_PNL_PB);
	memcpy(st_msg_pc_u_snd.body.notch_lamp, pCSInf->ote_notch_lamp, sizeof(ST_OTE_LAMP_COM) * N_OTE_PNL_NOTCH);
	//PLC���̓f�[�^
	memcpy(&st_msg_pc_u_snd.body.plc_in, &(pPLCio->input),sizeof(st_msg_pc_u_snd.body.plc_in));

	for (int i = 0; i < MOTION_ID_MAX; i++) {
		st_msg_pc_u_snd.body.pos[i] = pPLCio->pos[i];
	}

	//�U��Z���T�J����
	st_msg_pc_u_snd.body.swy_cam_pix[OTE_ID_LOAD_MH][OTE_ID_SWY_AXIS_X] = pSway_IO->cam_pix[SID_LOAD_MH][SID_CAM_X];
	st_msg_pc_u_snd.body.swy_cam_pix[OTE_ID_LOAD_MH][OTE_ID_SWY_AXIS_Y] = pSway_IO->cam_pix[SID_LOAD_MH][SID_CAM_Y];
	st_msg_pc_u_snd.body.swy_cam_pix[OTE_ID_LOAD_AH][OTE_ID_SWY_AXIS_X] = pSway_IO->cam_pix[SID_LOAD_AH][SID_CAM_X];
	st_msg_pc_u_snd.body.swy_cam_pix[OTE_ID_LOAD_AH][OTE_ID_SWY_AXIS_Y] = pSway_IO->cam_pix[SID_LOAD_AH][SID_CAM_Y];
	
	
	return &st_msg_pc_u_snd;
} 
//*********************************************************************************************
/// <summary>
/// PC��OTE�@Unicast�@�yPC��OTE����̃��j�L���X�g��M�\�P�b�g�ő��M�z
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COteIF::snd_pc_u_ote(LPST_PC_U_MSG pbuf, SOCKADDR_IN* p_addrin_to) {

	if (pSockOteUniCastPc->snd_udp_msg((char*)pbuf, sizeof(ST_PC_U_MSG), *p_addrin_to) == SOCKET_ERROR) {
		msg_wos.str() = pSockOteUniCastPc->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
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
//*********************************************************************************************
/// <summary>
/// PC->PC �}���`�L���X�g�@�yPC��PC����̃}���`�L���X�g��M�\�P�b�g�ő��M�z
/// </summary>
/// <param name="pbuf"></param>
/// <param name="pto_addrin"></param>
/// <returns></returns>
HRESULT COteIF::snd_pc_m_pc(LPST_PC_M_MSG pbuf) {
	if (pSockPcMultiCastPc->snd_udp_msg((char*)pbuf, sizeof(ST_PC_M_MSG), addrin_pc_m_pc_snd) == SOCKET_ERROR) {
		msg_wos.str() = pSockPcMultiCastPc->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}

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
/// <summary>
/// �ʐM��ԕ\���e�L�X�g�X�V
/// </summary>
/// <param name="is_msg"></param>




//****************************************************************************�@
/// <summary>
/// �N���[�Y����
/// </summary>
/// <returns></returns>
HRESULT COteIF::close() {
	delete  pSockOteUniCastPc;		//PC���j�L���X�g�p�\�P�b�g
	delete  pSockPcMultiCastPc;		//PC�}���`�L���X�gPC��M�p�\�P�b�g
	delete  pSockOteMultiCastPc;	//OTE�}���`�L���X�gPC��M�p�\�P�b�g
	return S_OK;
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
	hWnd_work = CreateWindowW(TEXT("OTE IF"), TEXT("OTE IF"), WS_OVERLAPPEDWINDOW,
				OTEIF_WORK_WND_X, OTEIF_WORK_WND_Y, OTEIF_WORK_WND_W0, OTEIF_WORK_WND_H0,
				hwnd, nullptr, hInst, nullptr);

	RECT rc;
	GetClientRect(hWnd_work, &rc);
	st_work_wnd.area_w = rc.right - rc.left;
	st_work_wnd.area_h = rc.bottom - rc.top;

	//�\���t�H���g�ݒ�
	st_work_wnd.hfont_inftext = CreateFont(6, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));

	InvalidateRect(hWnd_work, NULL, TRUE);//�\���X�V

	ShowWindow(hWnd_work, SW_SHOW);
	UpdateWindow(hWnd_work);

	return hWnd_work;
}

BOOL COteIF::show_if_wnd() {
	return SetWindowPos(hWnd_work, HWND_TOP, OTEIF_WORK_WND_X, OTEIF_WORK_WND_Y, OTEIF_WORK_WND_W0, OTEIF_WORK_WND_H0, SWP_SHOWWINDOW);
}
BOOL COteIF::hide_if_wnd() {
	return SetWindowPos(hWnd_work, HWND_TOP, OTEIF_WORK_WND_X, OTEIF_WORK_WND_Y, OTEIF_WORK_WND_W0, OTEIF_WORK_WND_H0, SWP_HIDEWINDOW);
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
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//�E�B���h�E�ɃR���g���[���ǉ�
		set_OTEIF_panel_objects(hWnd);

		//�}���`�L���X�g�^�C�}�N�� 
		SetTimer(hWnd, ID_PC_MULTICAST_TIMER, PC_MULTICAST_SCAN_MS, NULL);

		break;
	}
	case WM_TIMER: {
		if (wParam == ID_PC_MULTICAST_TIMER) {
			if (S_OK == snd_pc_m_pc(set_msg_pc_m())) {//OTE�}���`�L���X�g���M
				cnt_snd_pc_m_pc++;
			}
			if (S_OK == snd_pc_m_ote(set_msg_pc_m())) {//OTE�}���`�L���X�g���M
				cnt_snd_pc_m_ote++;
			}
			if_disp_update();
		}
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{

		case BASE_ID_OTEIF_PB + ID_OTEIF_CHK_HOLD: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_HOLD], BM_GETCHECK, 0, 0)) st_work_wnd.is_hold_disp = true;
			else st_work_wnd.is_hold_disp = false;
		}break;

		case BASE_ID_OTEIF_PB + ID_OTEIF_RADIO_PCM: {
			st_work_wnd.id_disp_item = ID_OTEIF_RADIO_PCM;
		}break;

		case BASE_ID_OTEIF_PB + ID_OTEIF_RADIO_UNI: {
			st_work_wnd.id_disp_item = ID_OTEIF_RADIO_UNI;
		}break;

		case BASE_ID_OTEIF_PB + ID_OTEIF_RADIO_TEM: {
			st_work_wnd.id_disp_item = ID_OTEIF_RADIO_TEM;
		}break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}break;

	case ID_SOCK_EVENT_OTE_UNI_PC: {
		int nEvent = WSAGETSELECTEVENT(lParam);
		switch (nEvent) {
		case FD_READ: {
			
			//	if (rcv_ote_u_pc(&st_msg_ote_u_rcv) == S_OK) {				//OTE����̃��j�L���X�g���b�Z�[�W��M
			if (rcv_ote_u_pc(&pOTEio->ote_umsg_in) == S_OK) {				//OTE����̃��j�L���X�g���b�Z�[�W��M
				cnt_rcv_ote_u++;
				pOTEio->ote_u_silent_cnt = 0;
				addrin_ote_u_from = pSockOteUniCastPc->addr_in_from;
				set_sock_addr(&addrin_pc_u_snd, OTE_IF_UNICAST_IP_OTE0, OTE_IF_UNICAST_PORT_OTE);
				if(snd_pc_u_ote(set_msg_pc_u(), &addrin_pc_u_snd)==S_OK)cnt_snd_pc_u++;;
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

	case ID_SOCK_EVENT_PC_MULTI_PC: {
		int nEvent = WSAGETSELECTEVENT(lParam);
		switch (nEvent) {
		case FD_READ: {
			if (rcv_pc_m_pc(&st_msg_pc_m_pc_rcv) == S_OK) {				//PC����̃��j�L���X�g���b�Z�[�W��M
				addrin_pc_m_from = pSockPcMultiCastPc->addr_in_from;
				cnt_rcv_pc_m++;
				disp_msg_cnt();
			}
			else {
				msg_ws = L"ERROR : rcv_pc_m_pc()";	wstr_out_inf(msg_ws);
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
				cnt_rcv_ote_m++;
				addrin_ote_m_from = pSockOteMultiCastPc->addr_in_from;
				disp_msg_cnt();
			}
			else {
				msg_ws = L"ERROR : rcv_ote_m_pc()";	wstr_out_inf(msg_ws);
			}
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		KillTimer(hWnd, ID_PC_MULTICAST_TIMER);
		PostQuitMessage(0);
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
void COteIF::set_OTEIF_panel_objects(HWND hWnd) {

	HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

	//IF STATIC
	{
		for (int i = ID_OTEIF_INF_CNT_IP0; i <= ID_OTEIF_INF_BODYR; i++) {
			st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][i] = CreateWindowW(TEXT("STATIC"),
				st_work_wnd.ctrl_text[ID_OTEIF_CTRL_STATIC][i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_STATIC][i].x, st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_STATIC][i].y,
				st_work_wnd.size_ctrl[ID_OTEIF_CTRL_STATIC][i].cx, st_work_wnd.size_ctrl[ID_OTEIF_CTRL_STATIC][i].cy,
				hWnd, (HMENU)(BASE_ID_OTEIF_STATIC + i), hInst, NULL);
		}

	}

	
	//CHK BOX
	{
		st_work_wnd.hctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_HOLD] = CreateWindow(L"BUTTON",
			st_work_wnd.ctrl_text[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_HOLD], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_HOLD].x, st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_HOLD].y,
			st_work_wnd.size_ctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_HOLD].cx, st_work_wnd.size_ctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_HOLD].cy,
			hWnd, (HMENU)(BASE_ID_OTEIF_STATIC + ID_OTEIF_CHK_HOLD), hInst, NULL);

		st_work_wnd.is_hold_disp = false;
		SendMessage(st_work_wnd.hctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_HOLD], BM_SETCHECK, BST_UNCHECKED, 0L);
	}

	//RADIO BUTTON
	{
		for (int i = ID_OTEIF_RADIO_UNI; i <= ID_OTEIF_RADIO_TEM; i++) {
			if (i == ID_OTEIF_RADIO_UNI)// | WS_GROUP ����
				st_work_wnd.hctrl[ID_OTEIF_CTRL_PB][i] = CreateWindow(L"BUTTON",
					st_work_wnd.ctrl_text[ID_OTEIF_CTRL_PB][i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_PB][i].x, st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_PB][i].y,
					st_work_wnd.size_ctrl[ID_OTEIF_CTRL_PB][i].cx, st_work_wnd.size_ctrl[ID_OTEIF_CTRL_PB][i].cy,
					hWnd, (HMENU)(BASE_ID_OTEIF_PB + (LONG64)i), hInst, NULL);
			else
				st_work_wnd.hctrl[ID_OTEIF_CTRL_PB][i] = CreateWindow(L"BUTTON",
					st_work_wnd.ctrl_text[ID_OTEIF_CTRL_PB][i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_PB][i].x, st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_PB][i].y,
					st_work_wnd.size_ctrl[ID_OTEIF_CTRL_PB][i].cx, st_work_wnd.size_ctrl[ID_OTEIF_CTRL_PB][i].cy,
					hWnd, (HMENU)(BASE_ID_OTEIF_PB + (LONG64)i), hInst, NULL);
		}
		SendMessage(st_work_wnd.hctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_RADIO_UNI], BM_SETCHECK, BST_CHECKED, 0L);
		st_work_wnd.id_disp_item = ID_OTEIF_RADIO_UNI;
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
void COteIF::if_disp_update() {
	
	SOCKADDR_IN* pmy_addr= &addrin_ote_u_pc, * psnd_addr = &addrin_ote_u_pc, * pfrom_addr = &addrin_ote_u_pc;
	LPST_OTE_HEAD phead_snd= &st_msg_pc_u_snd.head,phead_rcv= &st_msg_pc_u_snd.head;
	LONG* pcnt_snd= &cnt_snd_pc_u, * pcnt_rcv = &cnt_snd_pc_u;

	//�\���z�[���h�͍X�V����
	if (st_work_wnd.is_hold_disp)return;

	switch (st_work_wnd.id_disp_item) {
	case ID_OTEIF_RADIO_UNI: {
		pmy_addr = &addrin_ote_u_pc; psnd_addr = &addrin_pc_u_snd; pfrom_addr=&addrin_ote_u_from;
		phead_snd = &st_msg_pc_u_snd.head; pcnt_snd = &cnt_snd_pc_u;
		phead_rcv=&pOTEio->ote_umsg_in.head; pcnt_rcv = &cnt_rcv_ote_u;
		
	}break;
	case ID_OTEIF_RADIO_PCM: {
		pmy_addr = &addrin_pc_m_pc; psnd_addr = &addrin_pc_m_pc_snd; pfrom_addr = &addrin_pc_m_from;
		phead_snd = &st_msg_pc_m_snd.head; phead_rcv = &st_msg_pc_m_pc_rcv.head;
		pcnt_snd = &cnt_snd_pc_m_pc; pcnt_rcv = &cnt_rcv_ote_m;
	}break;
	case ID_OTEIF_RADIO_TEM: {
		pmy_addr = &addrin_ote_m_pc; psnd_addr = &addrin_pc_m_ote_snd; pfrom_addr = &addrin_ote_m_from;
		phead_snd = &st_msg_pc_m_snd.head; phead_rcv = &st_msg_pc_m_pc_rcv.head;
		pcnt_snd = &cnt_snd_pc_m_ote; pcnt_rcv = &cnt_rcv_ote_m;
	}break;
	}

	//�ʐM�J�E���g�@��M�A�h���X
	msg_wos.str(L"");
	msg_wos <<L"CNT S:"<< *pcnt_snd << L" R:" << *pcnt_rcv << L"    MyIP:" << pmy_addr->sin_addr.S_un.S_un_b.s_b1 << L"." << pmy_addr->sin_addr.S_un.S_un_b.s_b2 << L"." << pmy_addr->sin_addr.S_un.S_un_b.s_b3 << L"." << pmy_addr->sin_addr.S_un.S_un_b.s_b4 << L": "
		<< htons(pmy_addr->sin_port);

	SetWindowText(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_CNT_IP0], msg_wos.str().c_str());

	//���M��A���M���A�h���X
	msg_wos.str(L"");
	msg_wos << L"AddrTo:" << psnd_addr->sin_addr.S_un.S_un_b.s_b1 << L"." << psnd_addr->sin_addr.S_un.S_un_b.s_b2 << L"." << psnd_addr->sin_addr.S_un.S_un_b.s_b3 << L"." << psnd_addr->sin_addr.S_un.S_un_b.s_b4 << L": "
		<< htons(psnd_addr->sin_port);
	msg_wos << L"  From:" << pfrom_addr->sin_addr.S_un.S_un_b.s_b1 << L"." << pfrom_addr->sin_addr.S_un.S_un_b.s_b2 << L"." << pfrom_addr->sin_addr.S_un.S_un_b.s_b3 << L"." << pfrom_addr->sin_addr.S_un.S_un_b.s_b4 << L": "
		<< htons(pfrom_addr->sin_port);

	SetWindowText(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_IP_SND_FROM], msg_wos.str().c_str());

	//�w�b�_�����ǉ�
	msg_wos.str(L"");
	msg_wos << L" HEAD(S):(PC)" << phead_snd->myid << L" (EVENT)" << phead_snd->code
			<< L" (IP)" << phead_snd->addr.sin_addr.S_un.S_un_b.s_b1 << L"." << phead_snd->addr.sin_addr.S_un.S_un_b.s_b2 << L"." <<  phead_snd->addr.sin_addr.S_un.S_un_b.s_b3 << L"." << phead_snd->addr.sin_addr.S_un.S_un_b.s_b4
			<< L" (PORT) " << htons(phead_snd->addr.sin_port)
			<< L" (COM)" << phead_snd->status << L" \n(�ڑ���OTE)" << phead_snd->tgid;
	SetWindowText(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_HEADS], msg_wos.str().c_str());
	
	msg_wos.str(L"");
	msg_wos << L" HEAD(R):(OTE)" << phead_rcv->myid << L" (EVENT)" << phead_rcv->code
		<< L" (IP)" << phead_rcv->addr.sin_addr.S_un.S_un_b.s_b1 << L"." << phead_rcv->addr.sin_addr.S_un.S_un_b.s_b2 << L"." << phead_rcv->addr.sin_addr.S_un.S_un_b.s_b3 << L"." << phead_rcv->addr.sin_addr.S_un.S_un_b.s_b4
		<< L" (PORT) " << htons(phead_rcv->addr.sin_port)
		<< L" (COM)" << phead_rcv->status << L" \n(�ڑ���PC)" << phead_rcv->tgid;
	SetWindowText(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_HEADR], msg_wos.str().c_str());
			
	msg_wos.str(L""); msg_wos2.str(L"");
	switch (st_work_wnd.id_disp_item) {
	case ID_OTEIF_RADIO_UNI: {
		LPST_PC_U_BODY pbody_s = &st_msg_pc_u_snd.body;
		msg_wos << L"BODYS ���"<<pbody_s->pb_lamp[ID_OTE_PB_HIJYOU].color<< L"�劲" << pbody_s->pb_lamp[ID_OTE_PB_SYUKAN].color;
		msg_wos << L" Notch MH" << pbody_s->notch_lamp[0].color << pbody_s->notch_lamp[1].color << pbody_s->notch_lamp[2].color << pbody_s->notch_lamp[3].color << pbody_s->notch_lamp[4].color << pbody_s->notch_lamp[5].color << pbody_s->notch_lamp[6].color << pbody_s->notch_lamp[7].color << pbody_s->notch_lamp[8].color;

		//PB���͏��
		LPST_OTE_U_BODY pbody_r = &pOTEio->ote_umsg_in.body;
		msg_wos2 << L"BODYR PB:  ";
		for (int i = ID_OTE_PB_TEISHI; i <= ID_OTE_CHK_N3; i++) {
			msg_wos2 << st_ote_work.ctrl_text[ID_OTE_CTRL_PB][i] << L":" << pbody_r->pb_ope[i] << L" ";
		} 
		for (int i = ID_OTE_RADIO_MHSPD_7; i <= ID_OTE_CHK_LOAD_SWY; i++) {
			msg_wos2 << st_ote_work.ctrl_text[ID_OTE_CTRL_PB][i] << L":" << pbody_r->pb_ope[i] << L" ";
		}

		msg_wos2 << L" " << st_ote_work.ctrl_text[ID_OTE_CTRL_PB][ID_OTE_PB_FLT_RESET] << L":" << pbody_r->pb_ope[ID_OTE_PB_FLT_RESET] << L" ";
		msg_wos2 << st_ote_work.ctrl_text[ID_OTE_CTRL_PB][ID_OTE_CHK_IL_BYPASS] << L":" << pbody_r->pb_ope[ID_OTE_CHK_IL_BYPASS] << L" ";

		msg_wos2 << L"\n";
		msg_wos2 << L" Notch HOLD  ";
		for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
			msg_wos2  << pbody_r->notch_pos[ID_OTE_NOTCH_POS_HOLD][i] << L":" ;
		}
//		msg_wos2 << L"\n";
		msg_wos2 << L" Notch TRIG  ";
		for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
			msg_wos2 << pbody_r->notch_pos[ID_OTE_NOTCH_POS_TRIG][i] << L":" ;
		}

	}break;
	case ID_OTEIF_RADIO_PCM: {
		LPST_PC_M_BODY pbody_s = &(st_msg_pc_m_snd.body);
		msg_wos << L"BODYS MH:" << pbody_s->pos[ID_HOIST] << L"GT:" << pbody_s->pos[ID_GANTRY] << L"BH:" << pbody_s->pos[ID_BOOM_H];
		msg_wos << L"SL:" << pbody_s->pos[ID_SLEW] << L"AH:" << pbody_s->pos[ID_AHOIST] ;

		LPST_PC_M_BODY pbody_r = &(st_msg_pc_m_pc_rcv.body);
		msg_wos2 << L"BODYR MH:" << pbody_r->pos[ID_HOIST] << L"GT:" << pbody_r->pos[ID_GANTRY] << L"BH:" << pbody_r->pos[ID_BOOM_H];
		msg_wos2 << L"SL:" << pbody_r->pos[ID_SLEW] << L"AH:" << pbody_r->pos[ID_AHOIST];

	}break;
	case ID_OTEIF_RADIO_TEM: {
		msg_wos << L"BODYS -" ;

		LPST_OTE_M_BODY pbody_r = &(st_msg_ote_m_pc_rcv.body);
		msg_wos2 << L"BODYR  OKPC:";
		for(int i=0;i<32;i++) msg_wos2 << pbody_r->pc_enable[i] << L",";
		msg_wos2 << L"\n RMT CNT:" << pbody_r->n_remote_wait << L" SEQ B:" << pbody_r->onbord_seqno << L" SEQ RMT:" << pbody_r->remote_seqno << L" MY SEQ:" << pbody_r->my_seqno;
	}break;
	}
	SetWindowText(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_BODYS], msg_wos.str().c_str());
	SetWindowText(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_BODYR], msg_wos2.str().c_str());

	return;
}
void COteIF::disp_msg_cnt() {
	msg_wos.str(L"");
	msg_wos << L"OTEIF     " << L"SPU:" << cnt_snd_pc_u << L"  ROU:" << cnt_rcv_ote_u << L"  SPMO:" << cnt_snd_pc_m_ote << L"  ROM:" << cnt_rcv_ote_m << L"  SPMP:" << cnt_snd_pc_m_pc << L"  RPM:" << cnt_rcv_pc_m;
	SetWindowText(hWnd_work, msg_wos.str().c_str());

	return;
}


