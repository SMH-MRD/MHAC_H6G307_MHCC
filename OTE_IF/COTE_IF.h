#pragma once

#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>
#include "COMMON_DEF.H"
#include "CSharedMem.h"	    //# ���L�������N���X
#include "Spec.h"
#include <commctrl.h>
#include <time.h>
#include <string>
#include "OTE_DEF.h"

//IP�A�h���X
#define IP_ADDR_OTE_UNI_CAST_PC				"192.168.1.100"
#define IP_ADDR_PC_UNI_CAST_OTE				"192.168.1.100"
//#define IP_ADDR_OTE_UNI_CAST_OTE			"127.0.0.1"
#define IP_ADDR_OTE_MULTI_CAST_PC			"239.1.0.2"
#define IP_ADDR_OTE_MULTI_CAST_OTE			"239.1.0.1"

//�^�C�}�[
#define ID_OTE_MULTICAST_TIMER				199
#define ID_OTE_UNICAST_TIMER				198
#define OTE_MULTICAST_SCAN_MS				1000	    // �}���`�L���X�g IF���M����
#define OTE_UNICAST_SCAN_MS					100			// ���j�L���X�g IF���M����


#define OTE_IF_OTE_IO_MEM_NG				0x8000
#define OTE_IF_CRANE_MEM_NG					0x4000
#define OTE_IF_SIM_MEM_NG					0x2000
#define OTE_IF_PLC_MEM_NG					0x1000

#define OTE_IF_DBG_MODE						0x00000010

//�\�P�b�g�C�x���gID
#define ID_SOCK_EVENT_OTE_UNI_PC 		    10603
#define ID_SOCK_EVENT_PC_MULTI_PC 		    10604
#define ID_SOCK_EVENT_OTE_MULTI_PC 		    10605
#define ID_SOCK_EVENT_PC_UNI_OTE	        10606
#define ID_SOCK_EVENT_PC_MULTI_OTE	        10607
#define ID_SOCK_EVENT_OTE_MULTI_OTE	        10608


//���[�N�E�B���h�E�I�u�W�F�N�gID
#define ID_STATIC_OTE_IF_COM_CNT            10650


#define ID_STATIC_OTE_IF_LABEL_COM          10607

#define ID_STATIC_OTE_IF_VIEW_STAT_U        10609
#define ID_STATIC_OTE_IF_VIEW_RCV_U         10610
#define ID_STATIC_OTE_IF_VIEW_SND_U         10611
#define ID_STATIC_OTE_IF_VIEW_INF_U         10612

#define ID_STATIC_OTE_IF_VIEW_STAT_TE       10611
#define ID_STATIC_OTE_IF_VIEW_RCV_TE        10612
#define ID_STATIC_OTE_IF_VIEW_SND_TE        10613
#define ID_STATIC_OTE_IF_VIEW_INF_TE        10614

#define ID_STATIC_OTE_IF_VIEW_STAT_CR       10615
#define ID_STATIC_OTE_IF_VIEW_RCV_CR        10616
#define ID_STATIC_OTE_IF_VIEW_SND_CR        10617
#define ID_STATIC_OTE_IF_VIEW_INF_CR        10618

#define ID_OTE_EVENT_CODE_CONST             0
#define ID_OTE_EVENT_CODE_STAT_REPORT       1
#define ID_OTE_EVENT_CODE_CONNECTED         1

#define ID_TE_CONNECT_STATUS_OFF_LINE       0
#define ID_TE_CONNECT_STATUS_STANDBY        3
#define ID_TE_CONNECT_STATUS_ACTIVE         1
#define ID_TE_CONNECT_STATUS_WAITING        2

#define N_CREATE_PEN                8
#define N_CREATE_BRUSH              8
#define CMON_RED_PEN                0
#define CMON_BLUE_PEN               1
#define CMON_GREEN_PEN              2
#define CMON_GLAY_PEN               3
#define CMON_YELLOW_PEN             4
#define CMON_MAZENDA_PEN            5 
#define CMON_MAZENDA_PEN2           6

#define OTE_WORK_WND_X						10					//OTE���C���p�l���\���ʒuX
#define OTE_WORK_WND_Y						10					//OTE���C���p�l���\���ʒuY
#define OTE_WORK_WND_W						800					//OTE���C���p�l��WINDOW��
#define OTE_WORK_WND_H						600					//OTE���C���p�l��WINDOW����

#define OTE_WORK_SUB_WND_X					OTE_WORK_WND_X+10		//�����e�p�l���\���ʒuX
#define OTE_WORK_SUB_WND_Y					OTE_WORK_WND_Y+30	//�����e�p�l���\���ʒuY
#define OTE_WORK_SUB_WND_W					250					//�����e�p�l��WINDOW��
#define OTE_WORK_SUB_WND_H					270					//�����e�p�l��WINDOW����

#define OTE_IFCHK_WND_X						OTE_WORK_WND_X							//IF CHECK WINDOW �\���ʒuX
#define OTE_IFCHK_WND_Y						OTE_WORK_WND_Y + OTE_WORK_WND_H +5		//IF CHECK WINDOW �\���ʒuY
#define OTE_IFCHK_WND_W						OTE_WORK_WND_W							//IF CHECK WINDOW WINDOW��
#define OTE_IFCHK_WND_H						400										//IF CHECK WINDOW WINDOW����

#define OTE_N_SUB_WND						5

#define ID_MSG_SET_MODE_INIT                1
#define ID_MSG_SET_MODE_CONST               0


typedef struct stOteIOWork {
    ST_OTE_IO   ote_io;
    SOCKADDR_IN addr_connected_te;
    INT32       id_connected_te;
    INT32       status_connected_te;
    INT32       te_connect_chk_counter;
    INT32       te_connect_time_limit;
    INT32       te_multi_snd_cycle;
    INT32       te_multi_snd_chk_counter;
}ST_OTE_IO_WORK, * LPST_OTE_IO_WORK;

//����[���E�B���h�E�\����
typedef struct _stOTEifwnd {

	UINT32 cnt_res_w = 0, cnt_res_r = 0;

	int disp_item = 0;                                                      //�\������
	int area_x = 0, area_y = 0, area_w = 0, area_h = 0;                     //���C���E�B���h�E��̕\���G���A
	int inf_area_x = 0, inf_area_y = 0, inf_area_w = 0, inf_area_h = 0;     //���C���E�B���h�E��̕\���G���A
	int bmp_w = 0, bmp_h = 0;                               //�O���t�B�b�N�r�b�g�}�b�v�T�C�Y

	HBITMAP hBmap_mem0 = NULL;
	HBITMAP hBmap_inf = NULL;
	HDC hdc_mem0 = NULL;						            //������ʃ������f�o�C�X�R���e�L�X�g
	HDC hdc_mem_inf = NULL;					                //������ʃ������f�o�C�X�R���e�L�X�g
	HDC hdc_pb_ctr_source;

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

}ST_OTE_IF_WND, * LPST_OTE_IF_WND;


class COteIF :  public CBasicControl
{
public:

    COteIF(HWND hWnd);
   ~COteIF();

   static ST_OTE_IO ote_io_workbuf;

   static bool is_my_ote_active;
   static HWND hWnd_parent;		//�e�E�B���h�E�̃n���h��
   static HWND hWnd_if;
   static HWND hWnd_sub[OTE_N_SUB_WND];			//�ʐM�C�x���g�����p�E�B���h�E�n���h��
   static HWND hWnd_ifchk;		//�ʐM���j�^�E�B���h�E�n���h��

   WORD helthy_cnt = 0;

   void set_debug_mode(int id) { if (id) mode |= OTE_IF_DBG_MODE; else    mode &= ~OTE_IF_DBG_MODE; return; }
   int is_debug_mode() { return (mode & OTE_IF_DBG_MODE); }

   //# �o�͗p���L�������I�u�W�F�N�g�|�C���^:
   CSharedMem* pOteIOObj;
   //# ���͗p���L�������I�u�W�F�N�g�|�C���^:
   CSharedMem* pCraneStatusObj;
   CSharedMem* pSimulationStatusObj;
   CSharedMem* pPLCioObj;
   CSharedMem* pCSInfObj;
   CSharedMem* pAgentInfObj;
   CSharedMem* pSwayIO_Obj;

   static HWND open_work_Wnd(HWND hwnd);
   static HWND open_connect_Wnd(HWND hwnd);
   static HWND open_mode_Wnd(HWND hwnd);
   static HWND open_fault_Wnd(HWND hwnd);
   static HWND open_moment_Wnd(HWND hwnd);
   static HWND open_auto_Wnd(HWND hwnd);

   static HWND open_ifchk_Wnd(HWND hwnd);
  
   static HRESULT close();

   //�I�[�o�[���C�h
   int set_outbuf(LPVOID);     //�o�̓o�b�t�@�Z�b�g
   int init_proc();            //����������
   int input();                //���͏���
   int parse();                //���C������
   int output();               //�o�͏���

   static LPST_OTE_IO pOTEio;
   static LPST_CRANE_STATUS pCraneStat;
   static LPST_PLC_IO pPLCio;
   static LPST_CS_INFO pCSInf;
   static LPST_AGENT_INFO pAgentInf;
   static LPST_SWAY_IO pSway_IO;

   static std::wstring msg_ws;
   static std::wostringstream msg_wos;

   static ST_OTE_IF_WND st_work_wnd;

   static CSockAlpha* pSockOteUniCastPc;		//OTE��PC���j�L���X�g��M�p�\�P�b�g
   static CSockAlpha* pSockPcMultiCastPc;		//PC->PC�}���`�L���X�g��M�p�\�P�b�g
   static CSockAlpha* pSockOteMultiCastPc;		//OTE��PC�}���`�L���X�g��M�p�\�P�b�g
   static CSockAlpha* pSockPcUniCastOte;		//PC->OTE���j�L���X�g��M�p�\�P�b�g
   static CSockAlpha* pSockPcMultiCastOte;		//PC->OTE�}���`�L���X�g��M�p�\�P�b�g
   static CSockAlpha* pSockOteMultiCastOte;		//OTE��OTE�}���`�L���X�g��M�p�\�P�b�g�g
 
   static SOCKADDR_IN addrin_ote_u_pc;			//OTE���j�L���X�g��M�A�h���X�iPC�p)
   static SOCKADDR_IN addrin_pc_m_pc;			//PC�}���`�L���X�g��M�A�h���X�iPC�p)
   static SOCKADDR_IN addrin_ote_m_pc;			//OTE�}���`�L���X�g��M�A�h���X�iPC�p)
   static SOCKADDR_IN addrin_pc_u_ote;			//PC���j�L���X�g��M�A�h���X�iOTE�p)
   static SOCKADDR_IN addrin_pc_m_ote;		//PC�}���`�L���X�g��M�A�h���X�iOTE�p)
   static SOCKADDR_IN addrin_ote_m_ote;		//OTE�}���`�L���X�g��M�A�h���X�iOTE�p)

   static SOCKADDR_IN addrin_pc_m_pc_snd;
   static SOCKADDR_IN addrin_pc_m_ote_snd;
   static SOCKADDR_IN addrin_ote_m_ote_snd;
 
   static SOCKADDR_IN addr_active_ote;			//����M�����L���Ȓ[���̃A�h���X

   static LONG cnt_snd_pc_u, cnt_snd_pc_m_pc, cnt_snd_pc_m_ote, cnt_snd_ote_u, cnt_snd_ote_m_pc, cnt_snd_ote_m_ote;
   static LONG cnt_rcv_pc_u, cnt_rcv_pc_m_pc, cnt_rcv_pc_m_ote, cnt_rcv_ote_u, cnt_rcv_ote_m_pc, cnt_rcv_ote_m_ote;

   static ST_PC_U_MSG st_msg_pc_u_snd;
   static ST_PC_M_MSG st_msg_pc_m_snd;
   
   static ST_OTE_U_MSG st_msg_ote_u_snd;
   static ST_OTE_M_MSG st_msg_ote_m_snd;
 
   static ST_PC_M_MSG st_msg_pc_m_pc_rcv;
   static ST_PC_M_MSG st_msg_pc_m_ote_rcv;
   static ST_OTE_U_MSG st_msg_pc_u_rcv;

   static ST_OTE_M_MSG st_msg_ote_m_pc_rcv;
   static ST_OTE_M_MSG st_msg_ote_m_ote_rcv;
   static ST_PC_U_MSG st_msg_ote_u_rcv;
   
   static ST_OTE_U_MSG st_ote_active_msg;	//����M�����L���Ȍ����b�Z�[�W

   static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
   static LRESULT CALLBACK WndConnectProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
   static LRESULT CALLBACK WndAutoProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
   static LRESULT CALLBACK WndModeProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
   static LRESULT CALLBACK WndFaultProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
   static LRESULT CALLBACK WndMomentProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
   
   static LRESULT CALLBACK WndIfChkProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  
   static void wstr_out_inf(const std::wstring& srcw);

   static LPST_PC_U_MSG set_msg_pc_u();
   static LPST_PC_M_MSG set_msg_pc_m();
   static LPST_OTE_U_MSG set_msg_ote_u();
   static LPST_OTE_M_MSG set_msg_ote_m();

   static HRESULT snd_ote_u_pc(LPST_OTE_U_MSG pbuf, SOCKADDR_IN* p_addrin_to);	//OTE��PC�@ ���j�L���X�g���M����
   static HRESULT snd_pc_m_pc(LPST_PC_M_MSG pbuf);								//PC��PC�@	�}���`�L���X�g���M����
   static HRESULT snd_ote_m_pc(LPST_OTE_M_MSG pbuf);							//OTE->PC	�}���`�L���X�g���M����
   static HRESULT snd_pc_u_ote(LPST_PC_U_MSG pbuf, SOCKADDR_IN* p_addrin_to);	//PC��OTE�@	���j�L���X�g���M����
   static HRESULT snd_ote_m_ote(LPST_OTE_M_MSG pbuf);							//OTE->OTE	�}���`�L���X�g���M����
   static HRESULT snd_pc_m_ote(LPST_PC_M_MSG pbuf);								//PC->OTE	�}���`�L���X�g���M����

   //��M�����p
   static HRESULT rcv_ote_u_pc(LPST_OTE_U_MSG pbuf);							//OTE��PC�@ ���j�L���X�g��M����
   static HRESULT rcv_pc_m_pc(LPST_PC_M_MSG pbuf);								//PC��PC�@	�}���`�L���X�g��M����
   static HRESULT rcv_ote_m_pc(LPST_OTE_M_MSG pbuf);							//OTE->PC	�}���`�L���X�g��M����
   static HRESULT rcv_pc_u_ote(LPST_PC_U_MSG pbuf);								//PC��OTE�@	���j�L���X�g��M����
   static HRESULT rcv_ote_m_ote(LPST_OTE_M_MSG pbuf);							//OTE->OTE	�}���`�L���X�g��M����
   static HRESULT rcv_pc_m_ote(LPST_PC_M_MSG pbuf);								//PC->OTE	�}���`�L���X�g��M����

   static void disp_msg_cnt();
   static void disp_ip_inf();

   static void set_OTE_panel_objects(HWND hWnd);
   static void draw_pb();

   void activate_local_ote(bool is_activate_req);

   void set_sock_addr(SOCKADDR_IN *paddr, PCSTR ip,USHORT port){
	   paddr->sin_family = AF_INET;
	   paddr->sin_port = htons(port);
	   inet_pton(AF_INET, ip, &(paddr->sin_addr.s_addr));
   }
 };
