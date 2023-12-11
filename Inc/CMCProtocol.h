#pragma once


#include <Windows.h>
#include <commctrl.h>
#include <time.h>
#include <string>
#include "CSockAlpha.h"

#include <ws2tcpip.h>


#define IP_ADDR_MC_CLIENT_DEF	"127.0.0.1"
#define IP_ADDR_MC_SERVER_DEF	"127.254.254.254"

#define PORT_MC_CLIENT		3020
#define PORT_MC_SERVER		3010
//#define PORT_MC_SERVER		1025

#define ID_SOCK_MC_CLIENT   PORT_MC_CLIENT
#define ID_SOCK_MC_SERVER   PORT_MC_SERVER

#define MC_ADDR_W_READ		10300
#define MC_SIZE_W_READ		100
#define MC_ADDR_W_WRITE		10200
#define MC_SIZE_W_WRITE		100

#define CODE_3E_FORMAT		0x50
#define CODE_4E_FORMAT		0x54
#define CODE_3E_NULL		0x00
#define CODE_NW				0x00
#define CODE_PC				0xFF

#define CODE_UIO			0x3FF
#define CODE_UIO_L			0xFF
#define CODE_UIO_H			0x03
#define CODE_UNODE			0x00

#define CODE_MON_TIMER		0x0001
#define CODE_DEVICE_D		0xA8

#define CODE_COMMAND_READ	0x0401
#define CODE_SUBCOM_READ	0x00	//WORD�P�ʓǂݏo��

#define CODE_COMMAND_WRITE	0x1401
#define CODE_SUBCOM_WRITE	0x00	//WORD�P�ʏ�������

#define MAX_MC_DATA			1024

#define MC_RES_WRITE		2		//������M�^�C�v�@��������
#define MC_RES_READ			3		//������M�^�C�v�@��������
#define MC_RES_ERR			0		//������M�^�C�v�@�G���[

//�v�����M�d���t�H�[�}�b�g
typedef struct _stXEMsgReq {
	UINT16	subcode;				//�T�u�w�b�_
	UINT16  serial;					//�V���A��
	UINT16	blank;					//��
	UINT8	nNW;					//�l�b�g���[�N�ԍ�
	UINT8	nPC;					//PC�ԍ�
	UINT16	nUIO;					//�v�����j�b�gIO�ԍ�
	UINT8	nUcode;					//�v�����j�b�g�ǔ�
	UINT16	len;					//�f�[�^��
	UINT16	timer;					//�Ď��^�C�}
	UINT16	com;					//�R�}���h
	UINT16	scom;					//�T�u�R�}���h
	UINT16	d_no;					//�f�o�C�X�ԍ�(3byte�\����2byte�̂ݗ��p���鎖�ɂ���j
	UINT8	d_no0;					//�f�o�C�X�ԍ�(3byte�\����2byte�̂ݗ��p���鎖�ɂ���j
	UINT8	d_code;					//�f�o�C�X�R�[�h
	UINT16	n_device;				//�ǂ�/�����f�o�C�X�_��
	UINT16	req_data[MAX_MC_DATA];	//�������݃f�[�^
}ST_XE_REQ, * LPST_XE_REQ;
//��M�d���t�H�[�}�b�g
typedef struct _stXEMsgRes {
	UINT16	subcode;				//�T�u�w�b�_
	UINT16  serial;					//�V���A��
	UINT16	blank;					//��
	UINT8	nNW;					//�l�b�g���[�N�ԍ�
	UINT8	nPC;					//PC�ԍ�
	UINT16	nUIO;					//�v�����j�b�gIO�ԍ�
	UINT8	nUcode;					//�v�����j�b�g�ǔ�
	UINT16	len;					//�f�[�^��
	UINT16	endcode;				//�I���R�[�h
	UINT16	res_data[MAX_MC_DATA];	//�ǂݍ��݃f�[�^
}ST_XE_RES, * LPST_XE_RES;


//WorkWindow�p��`

#define ID_UPDATE_TIMER				10800	//TIMER ID
#define MC_TIMER_PERIOD				25		//�v�����M����

#define WORK_WND_INIT_SIZE_W		530		//-Main Window�̏����T�C�Y�@W
#define WORK_WND_INIT_SIZE_H		600		//-Main Window�̏����T�C�Y�@H
#define WORK_WND_INIT_POS_X			860		//-Main Window�̏����ʒu�ݒ�@X
#define WORK_WND_INIT_POS_Y			0		//-Main Window�̏����ʒu�ݒ�@Y

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

#define IDC_MC_STATIC_SND0	        30000
#define IDC_MC_STATIC_RCV0		    30001
#define IDC_MC_STATIC_SND	        30002
#define IDC_MC_STATIC_RCV	        30003
#define IDC_MC_STATIC_MCCBUF        30004
#define IDC_MC_STATIC_PLCBUF        30005
#define IDC_MC_STATIC_SOCK_INF      30006
#define IDC_MC_STATIC_INF			30007
#define IDC_MC_STATIC_DW		    30008
#define IDC_MC_STATIC_DR		    30009

#define IDC_CHK_DISP_SOCK			30020
#define IDC_CHK_IS_SLOW_MODE        30021
#define IDC_CHK_INF					30022
#define IDC_CHK_MSG					30023
#define IDC_PB_DW_PLUS				30024
#define IDC_PB_DW_MINUS				30025
#define IDC_PB_DR_PLUS				30026
#define IDC_PB_DR_MINUS				30027
#define IDC_MC_INF_CHK_HEX			30028

#define CODE_SERIAL_REQ_R           0x1
#define CODE_SERIAL_REQ_W           0x2

#define MC_PRM_MEM0_POS_Y           150
#define MC_PRM_MEM0_POS_H           150
#define MC_PRM_INFMEM_POS_Y         300
#define MC_PRM_INFMEM_POS_H         300

#define MC_PRM_INFTXT_ORG_Y         25
#define MC_PRM_MEM0TXT_ORG_Y        25
#define MC_PRM_INFMEM_POS_H         300


//�ʐM�p�E�B���h�E�\����
typedef struct _stMCwnd {
	HWND hWnd_parent;
	HWND hWnd;
	HINSTANCE hInst;

	SOCKADDR_IN addrinc, addrins, addrfrom;	//addrinc:�N���C�A���g�y��M�z�A�h���X,addrins:�T�[�o�yPLC�F���M��z�A�h���X

	LONG count_snd_r = 0, count_snd_w = 0, count_rcv_r = 0, count_rcv_w = 0, count_rcv_err = 0;
	UINT32 cnt_res_w = 0, cnt_res_r = 0;
	INT32 i_dr_disp = 0, i_dw_disp = 0;

	int disp_item = 0;                                                      //�\������
	int area_x = 0, area_y = 0, area_w = 0, area_h = 0;                     //���C���E�B���h�E��̕\���G���A
	int inf_area_x = 0, inf_area_y = 0, inf_area_w = 0, inf_area_h = 0;     //���C���E�B���h�E��̕\���G���A
										//�O���t�B�b�N�r�b�g�}�b�v�T�C�Y

	int w_ptr = MC_PRM_MEM0TXT_ORG_Y, w_ptr_inf = MC_PRM_INFTXT_ORG_Y;
	int x_org = 0;
	int n_max_row = 10, n_max_row_inf = 10;
	int h_row = 20;

	HBITMAP hBmap_mem0 = NULL;
	HBITMAP hBmap_inf = NULL;
	HDC hdc_mem0 = NULL;						            //������ʃ������f�o�C�X�R���e�L�X�g
	HDC hdc_mem_inf = NULL;					                //������ʃ������f�o�C�X�R���e�L�X�g

	HFONT hfont_inftext = NULL;				                //�e�L�X�g�p�t�H���g
	BLENDFUNCTION bf = { 0,0,0,0 };					        //�����ߐݒ�\����

	HPEN hpen[N_CREATE_PEN] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };
	HBRUSH hbrush[N_CREATE_BRUSH] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };

	HWND h_static_snd_msg_w, h_static_snd_msg_r, h_static_rcv_msg_w, h_static_rcv_msg_r, h_socket_inf;
	HWND h_static_snd_cnt_w,h_static_snd_cnt_r,h_static_rcv_cnt_w,h_static_rcv_cnt_r, h_static_rcv_cnt_err;
	HWND h_static_res_w, h_static_res_r;
	HWND h_static_inf, h_static_Dr,h_static_Dw;
	HWND h_pb_dw_plus, h_pb_dw_minus, h_pb_dr_plus, h_pb_dr_minus;
	HWND h_chkSlow, h_chkSockinf, h_chk_inf, h_chk_msg,h_chk_hex;

}ST_MC_WND, * LPST_MC_WND;

class CMCProtocol
{
public:
	CMCProtocol();		
	~CMCProtocol();	
	
	static UINT8 snd_buf[MAX_MC_DATA * 2];
	static UINT8 read_req_snd_buf[MAX_MC_DATA * 2];//�ǂݍ��ݗv�����M�p�o�b�t�@
	static UINT8 write_req_snd_buf[MAX_MC_DATA * 2];//�����ݗv�����M�p�o�b�t�@
	static UINT8 rcv_buf[MAX_MC_DATA * 2];

	static std::wostringstream msg_wos;
	static ST_XE_REQ mc_req_msg_r, mc_req_msg_w;
	static ST_XE_RES mc_res_msg_r, mc_res_msg_w, mc_res_msg_err;
	static CSockAlpha* pMCSock;
	static ST_MC_WND st_work_wnd;
	static HWND open_work_Wnd(HWND hwnd);
	static UINT16 D_no_r, D_no_w, n_D_read, n_D_write;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void set_access_D_w(UINT16 no, UINT16 num) { D_no_w = no; n_D_write = num; return; };
	static void set_access_D_r(UINT16 no, UINT16 num) { D_no_r = no; n_D_read = num; return; };

	static void wstr_out_inf(const std::wstring& srcw);

	HWND Initialize(HWND hwnd,SOCKADDR_IN *paddrinc, SOCKADDR_IN *paddrins, UINT16 no_r, UINT16 num_r, UINT16 no_w, UINT16 num_w);
	HRESULT close();

	HRESULT set_sndbuf_read_D_3E();				//3E�@D�f�o�C�X�ǂݍ��ݗp���M�t�H�[�}�b�g�Z�b�g
	HRESULT set_sndbuf_write_D_3E();			//3E�@D�f�o�C�X�������ݗp���M�t�H�[�}�b�g�Z�b�g

	static HRESULT send_read_req_D_3E();				//3E�t�H�[�}�b�g W�f�o�C�X�ǂݏo���v�����M
	static HRESULT send_write_req_D_3E(void* p_data);	//3E�t�H�[�}�b�g W�f�o�C�X�������ݗv�����M
		
	static HRESULT send_read_req(UINT16 d_no, UINT16 n_read);									//3E�t�H�[�}�b�g
	static HRESULT send_write_req(UINT16 d_no, UINT16 n_write, UINT16* pdata);					//3E�t�H�[�}�b�g
	
	static UINT rcv_msg_3E();

	//��M������ʗp
	static HRESULT rcv_res(LPST_XE_RES pbuf);
	static HRESULT parse_rcv_buf(UINT8* p8, LPST_XE_RES pbuf, int nrcv);

	//�f�o�b�O,�V�~�����[�^�p
	static UINT16 snd_responce(ST_XE_REQ st_com, UINT16* pdata);
	static HRESULT parse_snd_buf(UINT8* p8,LPST_XE_REQ pbuf);

};