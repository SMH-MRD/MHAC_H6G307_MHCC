#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <ws2tcpip.h>
#include "COMMON_DEF.H"
#include <commctrl.h>
#include <time.h>
#include <string>
#include "CSockAlpha.h"
#include "OTE_DEF.h"
#include "OTEpanel.H"

//�^�C�}�[
#define ID_OTE_MULTICAST_TIMER				199
#define ID_OTE_UNICAST_TIMER				198
#define OTE_MULTICAST_SCAN_MS				1000	    // �}���`�L���X�g IF���M����
#define OTE_UNICAST_SCAN_MS					100			// ���j�L���X�g IF���M����

//�\�P�b�g�C�x���gID
#define ID_SOCK_EVENT_PC_UNI_OTE 		    10653		//OTE��M�\�P�b�g�C�x���g�@PC UNICAST���b�Z�[�W
#define ID_SOCK_EVENT_PC_MULTI_OTE 		    10654		//OTE��M�\�P�b�g�C�x���g�@PC MULTICAST���b�Z�[�W
#define ID_SOCK_EVENT_OTE_MULTI_OTE 		10655		//OTE��M�\�P�b�g�C�x���g�@PC MULTICAST���b�Z�[�W

#define OTE0_WND_X				10			//OTE���C���p�l���\���ʒuX
#define OTE0_WND_Y				60			//OTE���C���p�l���\���ʒuY0
#define OTE0_WND_W				800			//OTE���C���p�l��WINDOW��0
#define OTE0_WND_H				600			//OTE���C���p�l��WINDOW����0

#define OTE0_SUB_WND_X			0			//SUB�p�l���\���ʒuX
#define OTE0_SUB_WND_Y			0			//SUB�p�l���\���ʒuY
#define OTE0_SUB_WND_W			200			//SUB�p�l��WINDOW��
#define OTE0_SUB_WND_H			270			//SUB�p�l��WINDOW����

#define OTE0_SWY_WND_X			250			//�U��p�l���\���ʒuX
#define OTE0_SWY_WND_Y			0			//�U��p�l���\���ʒuY
#define OTE0_SWY_WND_W			250			//�U��p�l��WINDOW��
#define OTE0_SWY_WND_H			270			//�U��p�l��WINDOW����


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

	static HWND hWnd_parent;				//�e�E�B���h�E�̃n���h��
	static HWND hWnd_work;					//����[�����C���E�B���h�E�n���h��
	static HWND hWnd_sub[OTE0_N_SUB_WND];	//�T�u�E�B���h�E�n���h��
	static HWND hWnd_swy;					//�U��E�B���h�E�n���h��
	static HWND hwnd_current_subwnd;		//�\�����̃T�u�E�B���h�n���h��

	static ST_OTE_WORK_WND	st_work_wnd;	//�p�l���E�B���h�E�����p�\����

	WORD helthy_cnt = 0;

	static std::wstring msg_ws;
	static std::wostringstream msg_wos;

	static CSockAlpha* pSockPcUniCastOte;		//PC->OTE���j�L���X�g��M�p�\�P�b�g
	static CSockAlpha* pSockPcMultiCastOte;		//PC->OTE�}���`�L���X�g��M�p�\�P�b�g
	static CSockAlpha* pSockOteMultiCastOte;	//OTE��OTE�}���`�L���X�g��M�p�\�P�b�g

	static SOCKADDR_IN addrin_pc_u_ote;			//PC���j�L���X�g��M�A�h���X�iOTE�p)
	static SOCKADDR_IN addrin_pc_m_ote;			//PC�}���`�L���X�g��M�A�h���X�iOTE�p)
	static SOCKADDR_IN addrin_ote_m_ote;		//OTE�}���`�L���X�g��M�A�h���X�iOTE�p)

	static SOCKADDR_IN addrin_ote_m_snd;		//OTE�}���`�L���X�g���M�A�h���X�iOTE�p)
	static SOCKADDR_IN addrin_ote_u_snd;		//OTE���j�L���X�g���M��A�h���X�iOTE�p)
		
	//����M�J�E���g
	static LONG cnt_snd_ote_u, cnt_snd_ote_m;
	static LONG cnt_rcv_pc_u, cnt_rcv_pc_m, cnt_rcv_ote_m;

	static ST_OTE_U_MSG st_msg_ote_u_snd;
	static ST_OTE_M_MSG st_msg_ote_m_snd;

	static ST_PC_U_MSG st_msg_ote_u_rcv;
	static ST_PC_M_MSG st_msg_pc_m_ote_rcv;
	static ST_OTE_M_MSG st_msg_ote_m_ote_rcv;

	static HWND open_work_Wnd(HWND hwnd);
	static HWND open_connect_Wnd(HWND hwnd);
	static HWND open_mode_Wnd(HWND hwnd);
	static HWND open_fault_Wnd(HWND hwnd);
	static HWND open_moment_Wnd(HWND hwnd);
	static HWND open_auto_Wnd(HWND hwnd);
	static HWND open_swy_Wnd(HWND hwnd);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndConnectProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndAutoProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndModeProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndFaultProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndMomentProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndSwyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static HRESULT close();
	static int parse();//�[���\���X�V����

	static LPST_OTE_U_MSG set_msg_ote_u();
	static LPST_OTE_M_MSG set_msg_ote_m();

	static HRESULT snd_ote_u_pc(LPST_OTE_U_MSG pbuf, SOCKADDR_IN* p_addrin_to);	//OTE��PC�@ ���j�L���X�g���M����
	static HRESULT snd_ote_m_pc(LPST_OTE_M_MSG pbuf);							//OTE->PC	�}���`�L���X�g���M����
	static HRESULT snd_ote_m_ote(LPST_OTE_M_MSG pbuf);							//OTE->OTE	�}���`�L���X�g���M����
	
	//��M�����p
	static HRESULT rcv_pc_u_ote(LPST_PC_U_MSG pbuf);							//PC��OTE�@	���j�L���X�g��M����
	static HRESULT rcv_ote_m_ote(LPST_OTE_M_MSG pbuf);							//OTE->OTE	�}���`�L���X�g��M����
	static HRESULT rcv_pc_m_ote(LPST_PC_M_MSG pbuf);							//PC->OTE	�}���`�L���X�g��M����

	static void wstr_out_inf(const std::wstring& srcw);
	static void disp_msg_cnt();
	static void disp_ip_inf();

	static void set_OTE_panel_objects(HWND hWnd);								//OTE�E�B���h�E��փR���g���[���z�u
	static void draw_pb();														//�I�[�i�h���[PB�`��

	void activate_ote(bool is_activate_req);									//���s�^�C�}�[�N���A��~

	void set_sock_addr(SOCKADDR_IN* paddr, PCSTR ip, USHORT port) {				//�\�P�b�g�A�h���X�Z�b�g�֐�
		paddr->sin_family = AF_INET;
		paddr->sin_port = htons(port);
		inet_pton(AF_INET, ip, &(paddr->sin_addr.s_addr));
	}
};


