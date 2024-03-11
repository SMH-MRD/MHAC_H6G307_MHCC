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

#include "CPsaMain.H"


//�J����
#define OTE_CAMERA_PTZ0_IP "192.168.1.202"
#define OTE_CAMERA_FISH0_IP "192.168.1.201"
#define OTE_CAMERA_HOOK0_IP "192.168.1.205"
#define OTE_CAMERA_DOOM0_IP "192.168.1.204"

#define OTE_CAMERA_USER "SHIMH"
#define OTE_CAMERA_PASS "Shimh001"
#define OTE_CAMERA_FORMAT_JPEG 0
#define OTE_CAMERA_FORMAT_H264 3
#define OTE_CAMERA_FORMAT_H265 6


#define OTE0_N_IP_CAMERA					8//�ڑ�����IP�J�����̑䐔
#define OTE_CAMERA_ID_PTZ0					0//�^�]��PTZ�J����
#define OTE_CAMERA_ID_FISH0					1//�^�]���S���ʃJ����
#define OTE_CAMERA_ID_DOOM0					2//�ڑ�����IP�J�����̑䐔
#define OTE_CAMERA_ID_HOOK0					3//�ڑ�����IP�J�����̑䐔
#define OTE_CAMERA_ID_NA					-1//�ڑ�����IP�J�����̑䐔

#define OTE_CAMERA_WND_ID_BASE				0//�J�����\��BASE�E�B���h�E
#define OTE_CAMERA_WND_ID_OPT1				1//�J�����\���I�v�V�����P�E�B���h�E
#define OTE_CAMERA_WND_ID_OPT2				2//�J�����\���I�v�V�����P�E�B���h�E
#define OTE_CAMERA_WND_ID_OPT3				3//�J�����\���I�v�V�����P�E�B���h�E
#define OTE_CAMERA_WND_ID_OPT4				4//�J�����\���I�v�V�����P�E�B���h�E
#define OTE_CAMERA_WND_ID_OPT5				5//�J�����\���I�v�V�����P�E�B���h�E
#define OTE_CAMERA_WND_ID_OPT6				6//�J�����\���I�v�V�����P�E�B���h�E
#define OTE_CAMERA_WND_ID_OPT7				7//�J�����\���I�v�V�����P�E�B���h�E

//�^�C�}�[
#define ID_OTE_MULTICAST_TIMER				199
#define ID_OTE_UNICAST_TIMER				198
#define ID_OTE_CAMERA_TIMER					197
#define OTE_MULTICAST_SCAN_MS				1000	    // �}���`�L���X�g IF���M����
#define OTE_UNICAST_SCAN_MS					100			// ���j�L���X�g IF���M�����@UI�X�V����
#define OTE_CAMERA_SCAN_MS					100			// �J�����w�ߍX�V�p�^�C�}

//�\�P�b�g�C�x���gID
#define ID_SOCK_EVENT_PC_UNI_OTE 		    10653		//OTE��M�\�P�b�g�C�x���g�@PC UNICAST���b�Z�[�W
#define ID_SOCK_EVENT_PC_MULTI_OTE 		    10654		//OTE��M�\�P�b�g�C�x���g�@PC MULTICAST���b�Z�[�W
#define ID_SOCK_EVENT_OTE_MULTI_OTE 		10655		//OTE��M�\�P�b�g�C�x���g�@PC MULTICAST���b�Z�[�W

//�C�x���gID
#define ID_OTE0_STATIC_UPDATE				10656

#define OTE0_WND_X				0			//OTE���C���p�l���\���ʒuX
#define OTE0_WND_Y				560			//OTE���C���p�l���\���ʒuY0
#define OTE0_WND_W				960			//OTE���C���p�l��WINDOW��0
#define OTE0_WND_H				600			//OTE���C���p�l��WINDOW����0

#define OTE0_SUB_WND_X			5			//SUB�p�l���\���ʒuX
#define OTE0_SUB_WND_Y			5			//SUB�p�l���\���ʒuY
#define OTE0_SUB_WND_W			280			//SUB�p�l��WINDOW��
#define OTE0_SUB_WND_H			260			//SUB�p�l��WINDOW����

#define OTE0_SWY_WND_X			5			//�U��p�l���\���ʒuX
#define OTE0_SWY_WND_Y			5			//�U��p�l���\���ʒuY
#define OTE0_SWY_WND_W			280			//�U��p�l��WINDOW��
#define OTE0_SWY_WND_H			260			//�U��p�l��WINDOW����


#define OTE0_CAM_WND_X			0			//IP CAMERA �\���ʒuX
#define OTE0_CAM_WND_Y			-860		//IP CAMERA �\���ʒuY
#define OTE0_CAM_WND_W			1300		//IP CAMERA WINDOW��
#define OTE0_CAM_WND_H			750			//IP CAMERA WINDOW����

#define OTE0_CAM2_WND_X			OTE0_WND_X + OTE0_SUB_WND_W + 63	//IP CAMERA2 �\���ʒuX
#define OTE0_CAM2_WND_Y			5 									//IP CAMERA �\���ʒuY
#define OTE0_CAM2_WND_W			360									//IP CAMERA WINDOW��
#define OTE0_CAM2_WND_H			270									//IP CAMERA WINDOW����

#define OTE0_CAM_WND_TG_X		300			//��ʐ؂���X�ʒu
#define OTE0_CAM_WND_TG_Y		300			//��ʐ؂���Y�ʒu

#define OTE0_MSG_SWICH_CAMERA   WM_USER + 1000

#define OTE0_N_SUB_WND			6
#define ID_OTE0_SUB_WND_CONNECT	0
#define ID_OTE0_SUB_WND_MODE	1
#define ID_OTE0_SUB_WND_AUTO	2
#define ID_OTE0_SUB_WND_FAULT	3
#define ID_OTE0_SUB_WND_MOMENT	4
#define ID_OTE0_SWY_WND	        5

#define OTE_CODE_FLICKER_FREQ		0x0040
#define OTE_CODE_SUB_STATIC_FREQ	0x0001

#define OTE_N_TARGET		8
#define OTE_ID_HOT_TARGET	0
#define OTE_ID_S1_TARGET	1
#define OTE_ID_S2_TARGET	2
#define OTE_ID_S3_TARGET	3
#define OTE_ID_N1_TARGET	4
#define OTE_ID_N2_TARGET	5
#define OTE_ID_N3_TARGET	6

#define OTE_ID_AUTOSTAT_OFF		0
#define OTE_ID_AUTOSTAT_STANDBY	1
#define OTE_ID_AUTOSTAT_ACTIVE	2

//���C���E�B���h�E�Ǘ��\����
typedef struct stOte0DataTag {
	double	pos[MOTION_ID_MAX];
	double	load[MOTION_ID_MAX];
	double	v_fb[MOTION_ID_MAX];
	double	v_ref[MOTION_ID_MAX];
	double	deg_sl;				//����p��
	double	deg_bh;				//�N���p��
	POINT	pt_tgpos[OTE_N_TARGET][OTE0_N_AREA_GR];
	double	d_tgpos[OTE_N_TARGET][MOTION_ID_MAX];
	INT gpad_mode = L_OFF;
	INT auto_mode = OTE_ID_AUTOSTAT_OFF;
	INT anti_sway_mode = OTE_ID_AUTOSTAT_OFF;
	INT auto_sel[MOTION_ID_MAX] = { OTE_ID_AUTOSTAT_OFF ,OTE_ID_AUTOSTAT_OFF ,OTE_ID_AUTOSTAT_OFF ,OTE_ID_AUTOSTAT_OFF ,OTE_ID_AUTOSTAT_OFF ,OTE_ID_AUTOSTAT_OFF ,OTE_ID_AUTOSTAT_OFF ,OTE_ID_AUTOSTAT_OFF };

}ST_OTE0_DATA, * LPST_OTE0_DATA;

//�J�����f�������p�\����
typedef struct stIPCamSet {
	HWND hwnd = NULL; 
	int icam = OTE_CAMERA_ID_NA;
	CPsaMain* pPSA = NULL;	//PSApi�����p�I�u�W�F�N�g
}ST_IPCAM_SET, * LPST_IPCAM_SET;

class COte
{
public:

	COte(HWND hWnd);
	~COte();

	int init_proc(LPST_OTE_WORK_WND pst);            //����������

	HWND hWnd_parent;
	HWND hwnd_current_subwnd;		//�\�����̃T�u�E�B���h�n���h��

	LPST_OTE_WORK_WND	pst_work_wnd;	//�p�l���E�B���h�E�����p�\����

	WORD helthy_cnt = 0;

	ST_OTE0_DATA data;

	CSockAlpha* pSockPcUniCastOte;		//PC->OTE���j�L���X�g��M�p�\�P�b�g
	CSockAlpha* pSockPcMultiCastOte;	//PC->OTE�}���`�L���X�g��M�p�\�P�b�g
	CSockAlpha* pSockOteMultiCastOte;	//OTE��OTE�}���`�L���X�g��M�p�\�P�b�g

	SOCKADDR_IN addrin_pc_u_ote;		//PC���j�L���X�g��M�A�h���X�iOTE�p)
	SOCKADDR_IN addrin_pc_m_ote;		//PC�}���`�L���X�g��M�A�h���X�iOTE�p)
	SOCKADDR_IN addrin_ote_m_ote;		//OTE�}���`�L���X�g��M�A�h���X�iOTE�p)

	SOCKADDR_IN addrin_ote_m_ote_snd;	//OTE�}���`�L���X�g���M�A�h���X�iOTE�p)
	SOCKADDR_IN addrin_ote_m_pc_snd;	//OTE�}���`�L���X�g���M�A�h���X�iPC�p)
	SOCKADDR_IN addrin_ote_u_snd;		//OTE���j�L���X�g���M��A�h���X�iOTE�p)

	SOCKADDR_IN addrin_pc_u_from;	//OTE���j�L���X�g���M���A�h���X�iOTE�p)
	SOCKADDR_IN addrin_pc_m_from;	//PC�}���`�L���X�g���M���A�h���X�iOTE�p)
	SOCKADDR_IN addrin_ote_m_from;	//OTE�}���`�L���X�g���M���A�h���X�iOTE�p)

	SOCKADDR_IN addrin_pc_m_rcv;	//OTE�}���`�L���X�g��M�A�h���X
	SOCKADDR_IN addrin_ote_m_rcv;	//PC�}���`�L���X�g��M�A�h���X

	//����M�J�E���g
	LONG cnt_snd_ote_u, cnt_snd_ote_m_ote, cnt_snd_ote_m_pc;
	LONG cnt_rcv_pc_u, cnt_rcv_pc_m, cnt_rcv_ote_m;

	ST_OTE_U_MSG st_msg_ote_u_snd;
	ST_OTE_M_MSG st_msg_ote_m_snd;

	ST_PC_U_MSG st_msg_pc_u_rcv;
	ST_PC_M_MSG st_msg_pc_m_ote_rcv;
	ST_OTE_M_MSG st_msg_ote_m_ote_rcv;

	HRESULT close();
	int parse();//��M�f�[�^�W�J����

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

	void parse_auto_status();
	void update_auto_target_touch(int area, int x, int y);
	POINT cal_gr_pos_from_d_pos(int motion, double dpos1,double dpos2);	//���̈ʒu����O���t�B�b�N��̃|�C���g�����߂� �����A�����dpos1,dpos2���K�v�A��⊪��dpos1�̂�

	std::wstring msg_ws;
	std::wostringstream msg_wos;

	//long PlayStatus;
};

