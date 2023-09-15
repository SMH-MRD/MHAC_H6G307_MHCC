#pragma once

#include <Windows.h>
#include <commctrl.h>
#include <time.h>
#include <string>
#include "CSockAlpha.h"
#include <winsock2.h>
#include <ws2tcpip.h>

#include "OTE_DEF.h"

#define DEVICE_TYPE_PC					0		//�f�o�C�X�^�C�v����PC
#define DEVICE_TYPE_OTE					1		//�f�o�C�X�^�C�v�[��

#define OTE_SOCKET_TYPE_UNICAST				0//�\�P�b�g�^�C�v ���j�L���X�g
#define OTE_SOCKET_TYPE_PC_MULTICAST		1//�\�P�b�g�^�C�v �}���`�L���X�g
#define OTE_SOCKET_TYPE_OTE_MULTICAST		2//�\�P�b�g�^�C�v �}���`�L���X�g

#define IP_ADDR_OTE_SERVER_DEF	"127.0.0.1"
#define IP_ADDR_OTE_CLIENT_DEF	"127.254.254.254"
#define IP_ADDR_MULTI_CAST_DEF	"239.1.0.1"

//WorkWindow�p��`
//�\�P�b�g�C�x���g��MID
#define ID_SOCK_UNICAST			OTE_IF_UNICAST_PORT
#define ID_SOCK_OTE_MULTICAST	OTE_IF_OTE_MULTICAST_PORT
#define ID_SOCK_PC_MULTICAST	OTE_IF_PC_MULTICAST_PORT

#define ID_OTE_MULTICAST_UPDATE_TIMER	10900	//TIMER ID
#define OTE_TIMER_PERIOD				3000	//TIMER����

#define WORK_WND_INIT_SIZE_W			530		//-Main Window�̏����T�C�Y�@W
#define WORK_WND_INIT_SIZE_H			300		//-Main Window�̏����T�C�Y�@H
#define WORK_WND_INIT_POS_X				600		//-Main Window�̏����ʒu�ݒ�@X
#define WORK_WND_INIT_POS_Y				20		//-Main Window�̏����ʒu�ݒ�@Y

#define OTE_PRM_MEM0_POS_Y           150
#define OTE_PRM_MEM0_POS_H           150
#define OTE_PRM_INFMEM_POS_Y         300
#define OTE_PRM_INFMEM_POS_H         300

#define N_CREATE_PEN                8
#define N_CREATE_BRUSH              8
#define CMON_RED_PEN                0
#define CMON_BLUE_PEN               1
#define CMON_GREEN_PEN              2
#define CMON_GLAY_PEN               3
#define CMON_YELLOW_PEN             4
#define CMON_MAZENDA_PEN            5 
#define CMON_MAZENDA_PEN2           6

#define CMON_RED_BRUSH              0
#define CMON_BLUE_BRUSH             1
#define CMON_GREEN_BRUSH            2
#define CMON_BG_BRUSH               3

#define IDC_OTE_STATIC_SND_CNT_U		30100
#define IDC_OTE_STATIC_RCV_CNT_U		30101
#define IDC_OTE_STATIC_RCV_SUB_CNT_U	30102
#define IDC_OTE_STATIC_SND_CNT_M		30103
#define IDC_OTE_STATIC_RCV_CNT_M		30104
#define IDC_OTE_STATIC_SOCK_INF			30105

#define IDC_OTE_STATIC_SND_U			30106
#define IDC_OTE_STATIC_RCV_U			30107
#define IDC_OTE_STATIC_RCV_SUB_U		30108
#define IDC_OTE_STATIC_SND_M			30109
#define IDC_OTE_STATIC_RCV_M			30110

#define IDC_CHK_DISP_SOCK				30120
#define IDC_CHK_IS_SLOW_MODE			30121
#define IDC_CHK_INF						30122
#define IDC_CHK_MSG						30123


//�ʐM�p�E�B���h�E�\����
typedef struct _stOTEifwnd {
	HWND hWnd_parent;
	HWND hWnd;
	HINSTANCE hInst;

	LONG count_snd_m = 0, count_snd_u = 0, count_rcv_m = 0, count_rcv_u = 0, count_rcv_sub_u = 0;
	UINT32 cnt_res_w = 0, cnt_res_r = 0;

	int disp_item = 0;                                                      //�\������
	int area_x = 0, area_y = 0, area_w = 0, area_h = 0;                     //���C���E�B���h�E��̕\���G���A
	int inf_area_x = 0, inf_area_y = 0, inf_area_w = 0, inf_area_h = 0;     //���C���E�B���h�E��̕\���G���A
	int bmp_w = 0, bmp_h = 0;                               //�O���t�B�b�N�r�b�g�}�b�v�T�C�Y

	HBITMAP hBmap_mem0 = NULL;
	HBITMAP hBmap_inf = NULL;
	HDC hdc_mem0 = NULL;						            //������ʃ������f�o�C�X�R���e�L�X�g
	HDC hdc_mem_inf = NULL;					                //������ʃ������f�o�C�X�R���e�L�X�g

	HFONT hfont_inftext = NULL;				                //�e�L�X�g�p�t�H���g
	BLENDFUNCTION bf = { 0,0,0,0 };					        //�����ߐݒ�\����

	HPEN hpen[N_CREATE_PEN] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };
	HBRUSH hbrush[N_CREATE_BRUSH] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };

	HWND h_static_snd_msg_u, h_static_snd_msg_m, h_static_rcv_msg_u, h_static_rcv_sub_msg_u, h_static_rcv_msg_m, h_socket_inf;
	HWND h_static_snd_cnt_u, h_static_snd_cnt_m, h_static_rcv_cnt_u, h_static_rcv_sub_cnt_u, h_static_rcv_cnt_m;
	HWND h_static_res_w, h_static_res_r;
	HWND h_pb_read, h_pb_write;;
	HWND h_chkSlow, h_chkSockinf, h_chk_inf, h_chk_msg;

	bool is_slowmode = false, disp_sock_info = false, disp_msg = false, disp_infomation = true;
	wostringstream	wos;
	ostringstream	os;
	wstring			wstr;
	string			str;

}ST_OTE_IF_WND, * LPST_OTE_IF_WND;


class COTEProtocol
{
public:
	COTEProtocol();		
	~COTEProtocol();	
	
	static int device_type;				//PC/OTE��ʃt���O
	static int socket_type;				//�\�P�b�g��ʃt���O

	static SOCKADDR_IN addrin;			//��M�p�\�P�b�g�A�h���X
	static SOCKADDR_IN addrfrom;		//���M���\�P�b�g�A�h���X
	static SOCKADDR_IN addrin_to;		//���M��A�h���X

	static SOCKADDR_IN addr_active_ote;				//����M�����L���Ȓ[���̃A�h���X
	static ST_OTE_U_MSG st_ote_active_msg;			//����M�����L���Ȍ����b�Z�[�W

	static UINT32 ote_com_status;		//OTE����M���L������ԃt���O

	static std::wostringstream msg_wos;

	static CSockAlpha* pOTESock;
	static ST_OTE_IF_WND st_work_wnd;
	static HWND open_work_Wnd(HWND hwnd);
	
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static void wstr_out_inf(const std::wstring& srcw);

	HWND Initialize(HWND hwnd, SOCKADDR_IN* paddrin, SOCKADDR_IN* paddrin_to,int sock_type, int device);
	HRESULT close();

	void set_buf_pc_m_snd(LPST_PC_M_MSG pbuf) { pbuf_pc_m_snd = pbuf; return; };
	void set_buf_pc_m_rcv(LPST_PC_M_MSG pbuf) { pbuf_pc_m_rcv = pbuf; return; };
	void set_buf_ote_m_snd(LPST_OTE_M_MSG pbuf) { pbuf_ote_m_snd = pbuf; return; };
	void set_buf_ote_m_rcv(LPST_OTE_M_MSG pbuf) { pbuf_ote_m_rcv = pbuf; return; };

	void set_buf_pc_u_snd(LPST_PC_U_MSG pbuf) { pbuf_pc_u_snd = pbuf; return; };
	void set_buf_pc_u_rcv(LPST_PC_U_MSG pbuf) { pbuf_pc_u_rcv = pbuf; return; };
	void set_buf_pc_u_rcv_sub(LPST_OTE_U_MSG pbuf) { pbuf_ote_u_rcv_sub = pbuf; return; };
	void set_buf_ote_u_snd(LPST_OTE_U_MSG pbuf) { pbuf_ote_u_snd = pbuf; return; };
	void set_buf_ote_u_rcv(LPST_OTE_U_MSG pbuf) { pbuf_ote_u_rcv = pbuf; return; };

private:
	static HRESULT snd_pc_u(LPST_PC_U_MSG pbuf,SOCKADDR_IN *pto_addrinc);	//PC���j�L���X�g���M����
	static HRESULT snd_pc_m(LPST_PC_M_MSG pbuf);							//PC�}���`�L���X�g���M����
	static HRESULT snd_ote_u(LPST_OTE_U_MSG pbuf, SOCKADDR_IN *pto_addrin);	//OTE���j�L���X�g���M����
	static HRESULT snd_ote_m(LPST_OTE_M_MSG pbuf);							//OTE�}���`�L���X�g���M����
	
	//��M�����p
	static HRESULT rcv_pc_m(LPST_PC_M_MSG pbuf);						//PC�}���`�L���X�g�d����M����
	static HRESULT rcv_pc_u(LPST_PC_U_MSG pbuf);						//PC���j�L���X�g�d����M����
	static HRESULT rcv_ote_m(LPST_OTE_M_MSG pbuf);						//OTE�}���`�L���X�g�d����M����
	static HRESULT rcv_ote_u(LPST_OTE_U_MSG pbuf);						//OTE���j�L���X�g�d����M����

	static bool is_umsg_res_completed;
	static LPST_PC_M_MSG	pbuf_pc_m_snd,	pbuf_pc_m_rcv;
	static LPST_OTE_U_MSG	pbuf_ote_u_snd,	pbuf_ote_u_rcv, pbuf_ote_u_rcv_sub;
	static LPST_OTE_M_MSG	pbuf_ote_m_snd,	pbuf_ote_m_rcv;
	static LPST_PC_U_MSG	pbuf_pc_u_snd,	pbuf_pc_u_rcv;

};