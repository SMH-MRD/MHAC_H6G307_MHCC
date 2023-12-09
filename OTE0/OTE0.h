#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "resource.h"
#include <windows.h>
//#include <ws2tcpip.h>
#include "COMMON_DEF.H"
#include <commctrl.h>
#include <time.h>
#include <string>
#include "CSockAlpha.h"
#include "OTE_DEF.h"
#include "OTE0panel.H"


//�^�C�}�[
#define ID_OTE_MULTICAST_TIMER				199
#define ID_OTE_UNICAST_TIMER				198
#define OTE_MULTICAST_SCAN_MS				1000	    // �}���`�L���X�g IF���M����
#define OTE_UNICAST_SCAN_MS					100			// ���j�L���X�g IF���M�����@UI�X�V����

//�\�P�b�g�C�x���gID
#define ID_SOCK_EVENT_PC_UNI_OTE 		    10653		//OTE��M�\�P�b�g�C�x���g�@PC UNICAST���b�Z�[�W
#define ID_SOCK_EVENT_PC_MULTI_OTE 		    10654		//OTE��M�\�P�b�g�C�x���g�@PC MULTICAST���b�Z�[�W
#define ID_SOCK_EVENT_OTE_MULTI_OTE 		10655		//OTE��M�\�P�b�g�C�x���g�@PC MULTICAST���b�Z�[�W

#define OTE0_WND_X				0			//OTE���C���p�l���\���ʒuX
#define OTE0_WND_Y				560			//OTE���C���p�l���\���ʒuY0
#define OTE0_WND_W				850		//OTE���C���p�l��WINDOW��0
#define OTE0_WND_H				600			//OTE���C���p�l��WINDOW����0

#define OTE0_SUB_WND_X			5			//SUB�p�l���\���ʒuX
#define OTE0_SUB_WND_Y			5			//SUB�p�l���\���ʒuY
#define OTE0_SUB_WND_W			260			//SUB�p�l��WINDOW��
#define OTE0_SUB_WND_H			240			//SUB�p�l��WINDOW����

#define OTE0_SWY_WND_X			270			//�U��p�l���\���ʒuX
#define OTE0_SWY_WND_Y			5			//�U��p�l���\���ʒuY
#define OTE0_SWY_WND_W			280			//�U��p�l��WINDOW��
#define OTE0_SWY_WND_H			240			//�U��p�l��WINDOW����


#define OTE0_N_SUB_WND			6
#define ID_OTE0_SUB_WND_CONNECT	0
#define ID_OTE0_SUB_WND_MODE	1
#define ID_OTE0_SUB_WND_AUTO	2
#define ID_OTE0_SUB_WND_FAULT	3
#define ID_OTE0_SUB_WND_MOMENT	4
#define ID_OTE0_SWY_WND	        5

class COte
{
public:

	COte(HWND hWnd);
	~COte();

	int init_proc();            //����������

	HWND hWnd_parent;
	HWND hwnd_current_subwnd;		//�\�����̃T�u�E�B���h�n���h��

	ST_OTE_WORK_WND	st_work_wnd;	//�p�l���E�B���h�E�����p�\����

	WORD helthy_cnt = 0;

	std::wstring msg_ws;
	std::wostringstream msg_wos;

	CSockAlpha* pSockPcUniCastOte;		//PC->OTE���j�L���X�g��M�p�\�P�b�g
	CSockAlpha* pSockPcMultiCastOte;	//PC->OTE�}���`�L���X�g��M�p�\�P�b�g
	CSockAlpha* pSockOteMultiCastOte;	//OTE��OTE�}���`�L���X�g��M�p�\�P�b�g

	SOCKADDR_IN addrin_pc_u_ote;			//PC���j�L���X�g��M�A�h���X�iOTE�p)
	SOCKADDR_IN addrin_pc_m_ote;			//PC�}���`�L���X�g��M�A�h���X�iOTE�p)
	SOCKADDR_IN addrin_ote_m_ote;		//OTE�}���`�L���X�g��M�A�h���X�iOTE�p)

	SOCKADDR_IN addrin_ote_m_snd;		//OTE�}���`�L���X�g���M�A�h���X�iOTE�p)
	SOCKADDR_IN addrin_ote_u_snd;		//OTE���j�L���X�g���M��A�h���X�iOTE�p)

	SOCKADDR_IN addrin_pc_u_from;	//OTE���j�L���X�g���M���A�h���X�iOTE�p)
	SOCKADDR_IN addrin_pc_m_from;	//PC�}���`�L���X�g���M���A�h���X�iOTE�p)
	SOCKADDR_IN addrin_ote_m_from;	//OTE�}���`�L���X�g���M���A�h���X�iOTE�p)

	//����M�J�E���g
	LONG cnt_snd_ote_u, cnt_snd_ote_m;
	LONG cnt_rcv_pc_u, cnt_rcv_pc_m, cnt_rcv_ote_m;

	ST_OTE_U_MSG st_msg_ote_u_snd;
	ST_OTE_M_MSG st_msg_ote_m_snd;

	ST_PC_U_MSG st_msg_ote_u_rcv;
	ST_PC_M_MSG st_msg_pc_m_ote_rcv;
	ST_OTE_M_MSG st_msg_ote_m_ote_rcv;

	HRESULT close();
	int parse();//�[���\���X�V����

	LPST_OTE_U_MSG set_msg_ote_u();
	LPST_OTE_M_MSG set_msg_ote_m();

	HRESULT snd_ote_u_pc(LPST_OTE_U_MSG pbuf, SOCKADDR_IN* p_addrin_to);	//OTE��PC�@ ���j�L���X�g���M����
	HRESULT snd_ote_m_pc(LPST_OTE_M_MSG pbuf);								//OTE->PC	�}���`�L���X�g���M����
	HRESULT snd_ote_m_ote(LPST_OTE_M_MSG pbuf);								//OTE->OTE	�}���`�L���X�g���M����

	//��M�����p
	HRESULT rcv_pc_u_ote(LPST_PC_U_MSG pbuf);								//PC��OTE�@	���j�L���X�g��M����
	HRESULT rcv_ote_m_ote(LPST_OTE_M_MSG pbuf);								//OTE->OTE	�}���`�L���X�g��M����
	HRESULT rcv_pc_m_ote(LPST_PC_M_MSG pbuf);								//PC->OTE	�}���`�L���X�g��M����

	void wstr_out_inf(const std::wstring& srcw);
	void set_sock_addr(SOCKADDR_IN* paddr, PCSTR ip, USHORT port) {				//�\�P�b�g�A�h���X�Z�b�g�֐�
		paddr->sin_family = AF_INET;
		paddr->sin_port = htons(port);
		inet_pton(AF_INET, ip, &(paddr->sin_addr.s_addr));
	};
};

