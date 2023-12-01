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
#include "OTEIFpanel.h"

//�^�C�}�[
#define PC_MULTICAST_SCAN_MS				1000	    // �}���`�L���X�g IF���M����

//���L�������ُ�t���O
#define OTE_IF_OTE_IO_MEM_NG				0x8000
#define OTE_IF_CRANE_MEM_NG					0x4000
#define OTE_IF_SIM_MEM_NG					0x2000
#define OTE_IF_PLC_MEM_NG					0x1000

//�\�P�b�g�C�x���gID
#define ID_SOCK_EVENT_PC_MULTI_PC 		    10603		//PC��M�\�P�b�g�C�x���g�@PC MULTICAST���b�Z�[�W
#define ID_SOCK_EVENT_OTE_UNI_PC	        10604		//PC��M�\�P�b�g�C�x���g�@OTE UNICAST���b�Z�[�W
#define ID_SOCK_EVENT_OTE_MULTI_PC	        10605		//PC��M�\�P�b�g�C�x���g�@PC UNICAST���b�Z�[�W

#define OTEIF_WORK_WND_X					1300				//WORK WINDOW �\���ʒuX
#define OTEIF_WORK_WND_Y					10					//WORK WINDOW �\���ʒuY
#define OTEIF_WORK_WND_W0					600					//WORK WINDOW WINDOW��(MSG���j
#define OTEIF_WORK_WND_H0					400					//WORK WINDOW WINDOW����(MSG�L�j
#define OTEIF_WORK_WND_W1					1200				//WORK WINDOW WINDOW��(MSG���j
#define OTEIF_WORK_WND_H1					400					//WORK WINDOW WINDOW����(MSG�L�j

class COteIF :  public CBasicControl
{
public:

    COteIF(HWND hWnd);
   ~COteIF();

   static ST_OTE_IO ote_io_workbuf;
   static HWND hWnd_parent;		//�e�E�B���h�E�̃n���h��
   static HWND hWnd_work;
   static ST_OTEIF_WORK_WND st_work_wnd;

   WORD helthy_cnt = 0;

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
   static HRESULT close();

   //�I�[�o�[���C�h
   int set_outbuf(LPVOID);     //�o�̓o�b�t�@�Z�b�g
   int init_proc();            //����������
   int input();                //���͏���
   int parse();                //���C������
   int output();               //�o�͏���

   static LPST_OTE_IO		pOTEio;
   static LPST_CRANE_STATUS	pCraneStat;
   static LPST_PLC_IO		pPLCio;
   static LPST_CS_INFO		pCSInf;
   static LPST_AGENT_INFO	pAgentInf;
   static LPST_SWAY_IO		pSway_IO;

   static std::wstring			msg_ws;
   static std::wostringstream	msg_wos;

   static CSockAlpha* pSockOteUniCastPc;		//OTE��PC���j�L���X�g��M�p�\�P�b�g
   static CSockAlpha* pSockPcMultiCastPc;		//PC->PC�}���`�L���X�g��M�p�\�P�b�g
   static CSockAlpha* pSockOteMultiCastPc;		//OTE��PC�}���`�L���X�g��M�p�\�P�b�g
    
   static SOCKADDR_IN addrin_ote_u_pc;			//OTE��PC PC���j�L���X�g��M�A�h���X�iPC�p)
   static SOCKADDR_IN addrin_pc_m_pc;			//PC->PC PC�}���`�L���X�g��M�A�h���X�iPC�p)
   static SOCKADDR_IN addrin_ote_m_pc;			//OTE��PC OTE�}���`�L���X�g��M�A�h���X�iPC�p)
   
   static SOCKADDR_IN addrin_pc_u_snd;		    //PC->OTE PC���j�`�L���X�g���M��A�h���X�iPC�p)
   static SOCKADDR_IN addrin_pc_m_snd;			//PC->PC PC�}���`�L���X�g���M��A�h���X�iPC�p)

   static SOCKADDR_IN addrin_ote_u_from;		//OTE���j�L���X�g���M���A�h���X�iPC�p)
   static SOCKADDR_IN addrin_pc_m_from;		    //PC�}���`�L���X�g���M���A�h���X�iPC�p)
   static SOCKADDR_IN addrin_ote_m_from;		//OTE�}���`�L���X�g���M���A�h���X�iPC�p)
      
   static SOCKADDR_IN addr_active_ote;			//����M�����L���Ȓ[���̃A�h���X


   static LONG cnt_snd_pc_u, cnt_snd_pc_m;
   static LONG cnt_rcv_ote_u, cnt_rcv_ote_m, cnt_rcv_pc_m;

   static ST_PC_U_MSG st_msg_pc_u_snd;
   static ST_PC_M_MSG st_msg_pc_m_snd;
   
   static ST_PC_M_MSG st_msg_pc_m_pc_rcv;
   static ST_PC_M_MSG st_msg_pc_m_ote_rcv;
  
   static ST_OTE_M_MSG st_msg_ote_m_pc_rcv;
   static ST_OTE_M_MSG st_msg_ote_m_ote_rcv;
   static ST_OTE_U_MSG st_msg_ote_u_rcv;
   
   static ST_OTE_U_MSG st_ote_active_msg;	//����M�����L���Ȍ����b�Z�[�W

   static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
     
   static void wstr_out_inf(const std::wstring& srcw);

   static LPST_PC_U_MSG set_msg_pc_u();
   static LPST_PC_M_MSG set_msg_pc_m();
 
   static HRESULT snd_pc_m_pc(LPST_PC_M_MSG pbuf);								//PC��PC�@	�}���`�L���X�g���M����
   static HRESULT snd_pc_u_ote(LPST_PC_U_MSG pbuf, SOCKADDR_IN* p_addrin_to);	//PC��OTE�@	���j�L���X�g���M����
   static HRESULT snd_pc_m_ote(LPST_PC_M_MSG pbuf);								//PC->OTE	�}���`�L���X�g���M����

   //��M�����p
   static HRESULT rcv_ote_u_pc(LPST_OTE_U_MSG pbuf);							//OTE��PC�@ ���j�L���X�g��M����
   static HRESULT rcv_pc_m_pc(LPST_PC_M_MSG pbuf);								//PC��PC�@	�}���`�L���X�g��M����
   static HRESULT rcv_ote_m_pc(LPST_OTE_M_MSG pbuf);							//OTE->PC	�}���`�L���X�g��M����
   
   static void disp_update_spu(bool is_hold, bool is_msg, bool is_body);
   static void disp_update_rou(bool is_hold, bool is_msg, bool is_body);
   static void disp_update_spm(bool is_hold, bool is_msg, bool is_body);
   static void disp_update_rpm(bool is_hold, bool is_msg, bool is_body);
   static void disp_update_rom(bool is_hold, bool is_msg, bool is_body);
   
   static void set_OTEIF_panel_objects(HWND hWnd);
   void set_sock_addr(SOCKADDR_IN *paddr, PCSTR ip,USHORT port){
	   paddr->sin_family = AF_INET;
	   paddr->sin_port = htons(port);
	   inet_pton(AF_INET, ip, &(paddr->sin_addr.s_addr));
   }
 };
