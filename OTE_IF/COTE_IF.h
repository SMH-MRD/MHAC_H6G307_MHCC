#pragma once

#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>
#include "COMMON_DEF.H"
#include "CSharedMem.h"	    //# ���L�������N���X
#include "Spec.h"
#include "Opeterminal.h"

#include <commctrl.h>
#include <time.h>
#include <string>

#define OTE_IF_OTE_IO_MEM_NG              0x8000
#define OTE_IF_CRANE_MEM_NG                0x4000
#define OTE_IF_SIM_MEM_NG                  0x2000
#define OTE_IF_PLC_MEM_NG                  0x1000
#define OTE_IF_DBG_MODE                0x00000010

#define ID_SOCK_CODE_U  			        0
#define ID_SOCK_CODE_TE			            1
#define ID_SOCK_CODE_CR			            2

#define ID_UDP_EVENT_U  			        10604
#define ID_UDP_EVENT_M_TE			        10605
#define ID_UDP_EVENT_M_CR			        10606

#define ID_STATIC_OTE_IF_LABEL_COM          10607

#define ID_STATIC_OTE_IF_VIEW_STAT_U        10609
#define ID_STATIC_OTE_IF_VIEW_RCV_U         10609
#define ID_STATIC_OTE_IF_VIEW_SND_U         10610
#define ID_STATIC_OTE_IF_VIEW_INF_U         10611

#define ID_STATIC_OTE_IF_VIEW_STAT_TE       10611
#define ID_STATIC_OTE_IF_VIEW_RCV_TE        10612
#define ID_STATIC_OTE_IF_VIEW_SND_TE        10613
#define ID_STATIC_OTE_IF_VIEW_INF_TE        10614

#define ID_STATIC_OTE_IF_VIEW_STAT_CR       10615
#define ID_STATIC_OTE_IF_VIEW_RCV_CR        10616
#define ID_STATIC_OTE_IF_VIEW_SND_CR        10617
#define ID_STATIC_OTE_IF_VIEW_INF_CR        10618


//�N���^�C�}�[ID
#define ID_WORK_WND_TIMER					100
#define OTE_IO_TIME_CYCLE_MS    		    100		    // ����������^�C�}�lmsec
#define MULTI_SND_SCAN_TIME_MS				5000	    // �}���`�L���X�g IF���M����
#define TE_CONNECT_TIMEOVER_MS				500		    // ����[���ڑ��f����lmsec


#define WORK_WND_X							1050		//�����e�p�l���\���ʒuX
#define WORK_WND_Y							20			//�����e�p�l���\���ʒuY
#define WORK_WND_W							540		    //�����e�p�l��WINDOW��
#define WORK_WND_H							370			//�����e�p�l��WINDOW����


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

class COteIF :  public CBasicControl
{
private:

    //# �o�͗p���L�������I�u�W�F�N�g�|�C���^:
  CSharedMem* pOteIOObj;
    //# ���͗p���L�������I�u�W�F�N�g�|�C���^:
  CSharedMem* pCraneStatusObj;
  CSharedMem* pSimulationStatusObj;
  CSharedMem* pPLCioObj;
  CSharedMem* pCSInfObj;
  CSharedMem* pAgentInfObj;
  CSharedMem* pSwayIO_Obj;

    static LPST_OTE_IO pOTEio;
    static LPST_CRANE_STATUS pCraneStat;
    static LPST_PLC_IO pPLCio;
    static LPST_CS_INFO pCSInf;
    static LPST_AGENT_INFO pAgentInf;
    static LPST_SWAY_IO pSway_IO;

    HINSTANCE hInst;

    void init_rcv_msg();

    static void tweet2rcvMSG(const std::wstring& srcw,int code);
    static void tweet2sndMSG(const std::wstring& srcw,int code);
    static void tweet2infMSG(const std::wstring& srcw, int code);
    static void tweet2statusMSG(const std::wstring& srcw, int code);

    //IF�p�\�P�b�g
    static WSADATA wsaData;
    static SOCKET s_u;                              //���j�L���X�g��M�\�P�b�g
    static SOCKET s_m_te, s_m_cr;                   //�}���`�L���X�g��M�\�P�b�g
    static SOCKET s_m_snd,s_m_snd_dbg;              //�}���`�L���X�g���M�\�P�b�g
    static SOCKADDR_IN addrin_u;                    //���j�L���X�g��M�A�h���X
    static SOCKADDR_IN addrin_ote_u;                //���j�L���X�g���M�A�h���X
    static SOCKADDR_IN addrin_m_te, addrin_m_cr;    //�}���`�L���X�g��M�A�h���X
    static SOCKADDR_IN addrin_m_snd;                //�}���`�L���X�g���M�A�h���X
    static u_short port_u;                          //���j�L���X�g��M�|�[�g
    static u_short port_m_te, port_m_cr ;           //�}���`�L���X�g��M�|�[�g

    static ST_OTE_IO_WORK ote_io_workbuf;

    static int send_msg_u();   //���j�L���X�g���M
    static int send_msg_m();   //�}���`�L���X�g���M

    static int set_msg_u(int mode, INT32 code);                                  //���j�L���X�g���M���b�Z�[�W�Z�b�g(�������p�j
    static int set_msg_m_cr(int mode, INT32 code);                               //�}���`�L���X�g���M���b�Z�[�W�Z�b�g(�������p�j

public:
    COteIF();
    ~COteIF();

    static HWND hWorkWnd;
    WORD helthy_cnt = 0;
 
    //Work Window�\���p
    static HWND hwndSTAT_U,hwndRCVMSG_U,hwndSNDMSG_U,hwndINFMSG_U;
    static HWND hwndSTAT_M_TE,hwndRCVMSG_M_TE,hwndSNDMSG_M_TE,hwndINFMSG_M_TE;
    static HWND hwndSTAT_M_CR,hwndRCVMSG_M_CR,hwndSNDMSG_M_CR,hwndINFMSG_M_CR;

    void set_debug_mode(int id) {
        if (id) mode |= OTE_IF_DBG_MODE;
        else    mode &= ~OTE_IF_DBG_MODE;
    }

    int is_debug_mode() { return(mode & OTE_IF_DBG_MODE); }

    //�I�[�o�[���C�h
    int set_outbuf(LPVOID);     //�o�̓o�b�t�@�Z�b�g
    int init_proc();            //����������
    int input();                //���͏���
    int parse();                //���C������
    int output();               //�o�͏���

    virtual HWND open_WorkWnd(HWND hwnd_parent);
    static LRESULT CALLBACK WorkWndProc(HWND, UINT, WPARAM, LPARAM);
    static int close_WorkWnd();
    static int init_sock_u(HWND hwnd);
    static int init_sock_m_te(HWND hwnd);
    static int init_sock_m_cr(HWND hwnd);

    static std::wostringstream woMSG;
    static std::wstring wsMSG;

};
