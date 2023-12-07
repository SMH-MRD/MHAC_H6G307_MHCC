#pragma once

#include <windows.h>

//���[�N�E�B���h�E�I�u�W�F�N�gID
#define BASE_ID_OTEIF_STATIC				10630
#define ID_OTEIF_LABEL_0					0		//���^�C�g���@CNT�@IP�@PORT�@MSG
#define ID_OTEIF_LABEL_1					1		//�\������
#define ID_OTEIF_INF_SPU					2
#define ID_OTEIF_INF_ROU					3
#define ID_OTEIF_INF_SPM					4
#define ID_OTEIF_INF_RPM					5
#define ID_OTEIF_INF_ROM					6
#define ID_OTEIF_INF_BODY					7
			
#define BASE_ID_OTEIF_PB					10650
#define ID_OTEIF_CHK_MSG					0		//���b�Z�[�W�\���I��
#define ID_OTEIF_CHK_HOLD					1		//�\���X�V��~
#define ID_OTEIF_RADIO_SPU					2
#define ID_OTEIF_RADIO_ROU					3
#define ID_OTEIF_RADIO_SPM					4
#define ID_OTEIF_RADIO_RPM					5
#define ID_OTEIF_RADIO_ROM					6

#define N_OTEIF_CTRL_TYPE					2
#define ID_OTEIF_CTRL_STATIC				0
#define ID_OTEIF_CTRL_PB					1

#define N_OTEIF_PNL_ITEMS					16
#define N_OTEIF_PNL_STATIC					16
#define N_OTEIF_PNL_PB						16
#define N_OTEIF_PEN							8
#define N_OTEIF_BRUSH						8

#define OTEIF_X0							10
#define OTEIF_X1							20
#define OTEIF_Y0							5
#define OTEIF_Y1							20
#define OTEIF_W0							300
#define OTEIF_W1							600
#define OTEIF_W2							100
#define OTEIF_W3							10

#define OTEIF_H0							20
#define OTEIF_H1							25

//����[���E�B���h�E�\����
typedef struct _stOTEifWorkWnd {
	BOOL is_panel_disp	= false;		//�E�B���h�E��\��
	BOOL is_disp_msg	= false;	//���b�Z�[�W��\��
	BOOL is_hold_disp	= false;	//�\���X�V�z�[���h

	UINT32 id_disp_body;

	int area_x = 0, area_y = 0, area_w = 0, area_h = 0;     //���C���E�B���h�E��̕\���G���A
	int bmp_w = 0, bmp_h = 0;                               //�O���t�B�b�N�r�b�g�}�b�v�T�C�Y

	HBITMAP hBmap_mem0 = NULL, hBmap_inf = NULL;            //�r�b�g�}�b�v�n���h��
	HDC		hdc_mem0 = NULL,hdc_mem_inf = NULL;				//�������f�o�C�X�R���e�L�X�g
	HPEN	hpen[N_OTEIF_PEN]			= { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };
	HBRUSH	hbrush[N_OTEIF_BRUSH]	= { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };
	HWND	hctrl[N_OTEIF_CTRL_TYPE][N_OTEIF_PNL_STATIC];
	HFONT	hfont_inftext = NULL;				                //�e�L�X�g�p�t�H���g
	BLENDFUNCTION bf = { 0,0,0,0 };					        //�����ߐݒ�\����
	
	POINT pt_ctrl[N_OTEIF_CTRL_TYPE][N_OTEIF_PNL_ITEMS] = {
	//#STATIC
		//LABEL
		OTEIF_X0,OTEIF_Y0,  OTEIF_X0,OTEIF_Y0 + OTEIF_H1 * 6,
		//MSG(HEAD)
		OTEIF_X0,OTEIF_Y0 + OTEIF_H1,  OTEIF_X0,OTEIF_Y0 + OTEIF_H1*2,  OTEIF_X0,OTEIF_Y0 + OTEIF_H1 * 3,  OTEIF_X0,OTEIF_Y0 + OTEIF_H1 * 4,  OTEIF_X0,OTEIF_Y0 + OTEIF_H1 * 5,
		//MSG(BODY)
		OTEIF_X0,OTEIF_Y0 + OTEIF_H1 * 8,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	//#PB
		OTEIF_X0 + 300,OTEIF_Y0, OTEIF_X0 + 380,OTEIF_Y0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};

	SIZE size_ctrl[N_OTEIF_CTRL_TYPE][N_OTEIF_PNL_STATIC] = {
		//STATIC
		OTEIF_W1,OTEIF_H0, OTEIF_W1,OTEIF_H0 *2,
		OTEIF_W1,OTEIF_H0, OTEIF_W1,OTEIF_H0,  OTEIF_W1,OTEIF_H0,  OTEIF_W1,OTEIF_H0,  OTEIF_W1,OTEIF_H0,
		OTEIF_W1,OTEIF_H0*7,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		//PB
		OTEIF_W1,OTEIF_H0,  OTEIF_W2,OTEIF_H0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};

	WCHAR ctrl_text[N_OTEIF_CTRL_TYPE][N_OTEIF_PNL_STATIC][128] = {
	//STATIC
	L"HEADER��CNT�@:�@IP PORT�@:�@MSG",L"(S:SND R:RCV P:PC O:OTE U:UNI M:MULTI\nBODY",
	L"SPU #",L"ROU #",L"SPM #",L"RPM #",L"ROM #",
	L"BODY",L"",L"",L"",L"",L"",L"",L"",L"",
	//PB
	L"MSG",L"HOLD",L"",L"",L"",L"",L"",L"",
	L"",L"",L"",L"",L"",L"",L"",L""
	};

}ST_OTEIF_WORK_WND, * LPST_OTEIF_WORK_WND;
