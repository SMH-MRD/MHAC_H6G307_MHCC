#include "framework.h"
#include "OTE0.h"
#include <windowsx.h>       //# �R�����R���g���[��
#include <winsock2.h>

#include <iostream>
#include <iomanip>
#include <sstream>

/*
std::wstring COte::msg_ws;
std::wostringstream COte::msg_wos;

HWND COte::hWnd_parent;				//�e�E�B���h�E�̃n���h��
HWND COte::hWnd_work;				//����[�����C���E�B���h�E�n���h��
HWND COte::hWnd_sub[OTE0_N_SUB_WND];	//�ʐM�C�x���g�����p�E�B���h�E�n���h��
HWND COte::hwnd_current_subwnd;		//�\�����̃T�u�E�B���h�n���h��

ST_OTE_WORK_WND	COte::st_work_wnd;

CSockAlpha* COte::pSockPcUniCastOte;	//OTE���j�L���X�g�p�\�P�b�g
CSockAlpha* COte::pSockPcMultiCastOte;	//PC�}���`�L���X�gOTE��M�p�\�P�b�g
CSockAlpha* COte::pSockOteMultiCastOte;	//OTE�}���`�L���X�gOTE��M�p�\�P�b�g

SOCKADDR_IN COte::addrin_pc_u_ote;		//OTE���j�L���X�g�p�A�h���X(OTE��M�p)
SOCKADDR_IN COte::addrin_pc_m_ote;		//PC�}���`�L���X�g��M�A�h���X(OTE��M�p)
SOCKADDR_IN COte::addrin_ote_m_ote;		//OTE�}���`�L���X�g��M�A�h���X(OTE��M�p)

SOCKADDR_IN COte::addrin_ote_m_snd;		//OTE�}���`�L���X�g���M��A�h���X
SOCKADDR_IN COte::addrin_ote_u_snd;		//OTE�}���`�L���X�g��M�A�h���X

SOCKADDR_IN COte::addrin_pc_u_from;	//OTE���j�L���X�g���M���A�h���X�iOTE�p)
SOCKADDR_IN COte::addrin_pc_m_from;	//PC�}���`�L���X�g���M���A�h���X�iOTE�p)
SOCKADDR_IN COte::addrin_ote_m_from;	//OTE�}���`�L���X�g���M���A�h���X�iOTE�p)


LONG COte::cnt_snd_ote_u, COte::cnt_snd_ote_m;
LONG COte::cnt_rcv_pc_u, COte::cnt_rcv_pc_m, COte::cnt_rcv_ote_m;

ST_OTE_U_MSG COte::st_msg_ote_u_snd;
ST_OTE_M_MSG COte::st_msg_ote_m_snd;

ST_PC_M_MSG COte::st_msg_pc_m_ote_rcv;
ST_OTE_M_MSG COte::st_msg_ote_m_ote_rcv;
ST_PC_U_MSG COte::st_msg_ote_u_rcv;

*/

/*****************************************************************************/
/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="hWnd">�e�E�B���h�E�̃n���h��</param>
COte::COte(HWND hWnd) {
	hWnd_parent = hWnd;
	cnt_snd_ote_u = cnt_snd_ote_m = 0;
	cnt_rcv_pc_u = cnt_rcv_pc_m = cnt_rcv_ote_m = 0;
};
/*****************************************************************************/
/// <summary>
/// �f�X�g���N�^
/// </summary>
COte::~COte() {
};
/*****************************************************************************/
/// <summary>
/// �����������i�I�[�o�[���C�h�֐��j
/// </summary>
/// <returns></returns>
int COte::init_proc() {

	//### �\�P�b�g�A�h���X�Z�b�g
	memset(&addrin_pc_u_ote, 0, sizeof(SOCKADDR_IN));	memset(&addrin_pc_m_ote, 0, sizeof(SOCKADDR_IN)); memset(&addrin_ote_m_ote, 0, sizeof(SOCKADDR_IN));
	memset(&addrin_ote_m_snd, 0, sizeof(SOCKADDR_IN));	memset(&addrin_ote_u_snd, 0, sizeof(SOCKADDR_IN));

	set_sock_addr(&addrin_pc_u_ote, OTE_IF_UNICAST_IP_OTE0, OTE_IF_UNICAST_PORT_OTE);
	set_sock_addr(&addrin_ote_m_ote, OTE_IF_UNICAST_IP_OTE0, OTE_IF_MULTICAST_PORT_OTE);
	set_sock_addr(&addrin_pc_m_ote, OTE_IF_UNICAST_IP_OTE0, OTE_IF_MULTICAST_PORT_PC);

	set_sock_addr(&addrin_ote_u_snd, OTE_IF_UNICAST_IP_PC, OTE_IF_UNICAST_PORT_PC);
	set_sock_addr(&addrin_ote_m_snd, OTE_IF_MULTICAST_IP_OTE, OTE_IF_MULTICAST_PORT_OTE);

	//### �\�P�b�g�ݒ�
	pSockPcUniCastOte = new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_PC_UNI_OTE);
	pSockPcMultiCastOte = new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_PC_MULTI_OTE);
	pSockOteMultiCastOte = new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_OTE_MULTI_OTE);

	//### �I�u�W�F�N�g������

	if (pSockPcUniCastOte->Initialize() != S_OK) { msg_wos.str() = pSockPcUniCastOte->err_msg.str();	return NULL; }
	if (pSockPcMultiCastOte->Initialize() != S_OK) { msg_wos.str() = pSockPcMultiCastOte->err_msg.str();	return NULL; }
	if (pSockOteMultiCastOte->Initialize() != S_OK) { msg_wos.str() = pSockOteMultiCastOte->err_msg.str(); return NULL; }

	//### �\�P�b�g�ݒ�
	//���j�L���X�g�p
	if (pSockPcUniCastOte->init_sock(hWnd_parent, addrin_pc_u_ote) != S_OK) { msg_wos.str() = pSockPcUniCastOte->err_msg.str(); return NULL; }

	//�}���`�L���X�g�p
	SOCKADDR_IN addr_tmp;
	set_sock_addr(&addr_tmp, OTE_IF_MULTICAST_IP_OTE, NULL);
	if (pSockPcMultiCastOte->init_sock_m(hWnd_parent, addrin_pc_m_ote, addr_tmp) != S_OK) { msg_wos.str() = pSockPcMultiCastOte->err_msg.str(); return NULL; }
	if (pSockOteMultiCastOte->init_sock_m(hWnd_parent, addrin_ote_m_ote, addr_tmp) != S_OK) { msg_wos.str() = pSockOteMultiCastOte->err_msg.str(); return NULL; }

	//���M���b�Z�[�W�w�b�_�ݒ�i���M����M�A�h���X�F��M��̐܂�Ԃ��p�j
	st_msg_ote_u_snd.head.addr = addrin_pc_u_ote;
	st_msg_ote_m_snd.head.addr = addrin_ote_m_ote;

	return 0;
}

/*****************************************************************************/
/// <summary>
///  ��͏����i�\���X�V�����@WM_TIMER�ŌĂяo���j
/// </summary>
/// <returns></returns>
int COte::parse() {
	return 0;
}
/*****************************************************************************/
/// <summary>
/// OTE���j�L���X�g���b�Z�[�W���M�o�b�t�@�Z�b�g
/// </summary>
/// <returns></returns>
LPST_OTE_U_MSG COte::set_msg_ote_u() {
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
LPST_OTE_M_MSG COte::set_msg_ote_m() {
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
HRESULT COte::close() {

	delete pSockPcUniCastOte;		//PC->OTE���j�L���X�g��M�p�\�P�b�g
	delete pSockPcMultiCastOte;		//PC->OTE�}���`�L���X�g��M�p�\�P�b�g
	delete pSockOteMultiCastOte;	//OTE��OTE�}���`�L���X�g��M�p�\�P�b�g



	return S_OK;
}

//*********************************************************************************************
/// <summary>
/// OTE�}���`�L���X�g�d����M�����iOTE�}���`�L���X�g���b�Z�[�W����M�j
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COte::rcv_pc_u_ote(LPST_PC_U_MSG pbuf) {
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
HRESULT COte::rcv_ote_m_ote(LPST_OTE_M_MSG pbuf) {
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
HRESULT COte::rcv_pc_m_ote(LPST_PC_M_MSG pbuf) {
	int nRtn = pSockPcMultiCastOte->rcv_udp_msg((char*)pbuf, sizeof(ST_PC_M_MSG));
	if (nRtn == SOCKET_ERROR) {
		msg_wos.str() = pSockPcMultiCastOte->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}

//*********************************************************************************************
/// <summary>
///  OTE��PC�@Unicast�@�yOTE��PC����̃��j�L���X�g��M�\�P�b�g�ő��M�z
/// </summary>
/// <param name="pbuf"></param>
/// <param name="pto_addrin"></param>
/// <returns></returns>
HRESULT COte::snd_ote_u_pc(LPST_OTE_U_MSG pbuf, SOCKADDR_IN* p_addrin_to) {
	if (pSockPcUniCastOte->snd_udp_msg((char*)pbuf, sizeof(ST_OTE_U_MSG), *p_addrin_to) == SOCKET_ERROR) {
		msg_wos.str() = pSockPcUniCastOte->err_msg.str();
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
HRESULT COte::snd_ote_m_pc(LPST_OTE_M_MSG pbuf) {
	if (pSockPcMultiCastOte->snd_udp_msg((char*)pbuf, sizeof(ST_OTE_M_MSG), addrin_ote_m_snd) == SOCKET_ERROR) {
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
HRESULT COte::snd_ote_m_ote(LPST_OTE_M_MSG pbuf) {
	if (pSockOteMultiCastOte->snd_udp_msg((char*)pbuf, sizeof(ST_OTE_M_MSG), addrin_ote_m_snd) == SOCKET_ERROR) {
		msg_wos.str() = pSockOteMultiCastOte->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}//OTE�}���`�L���X�g���M����
//*********************************************************************************************
/// <summary>
/// 
/// </summary>
/// <param name="srcw"></param>
void COte::wstr_out_inf(const std::wstring& srcw) {
	return;
}
