#pragma once

#include "resource.h"
// #Touch�^�b�`�|�C���g
//Windows�^�b�`
#ifndef WINVER                  // Specifies that the minimum required platform is Windows 7.
#define WINVER 0x0601           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows 7.
#define _WIN32_WINNT 0x0601     // Change this to the appropriate value to target other versions of Windows.
#endif     

#ifndef _WIN32_WINDOWS          // Specifies that the minimum required platform is Windows 98.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE                       // Specifies that the minimum required platform is Internet Explorer 7.0.
#define _WIN32_IE 0x0700        // Change this to the appropriate value to target other versions of IE.
#endif

#include <windows.h>    // included for Windows Touch
#include <windowsx.h>   // included for point conversion

#define MAXPOINTS 10

// You will use this array to track touch points
int points[MAXPOINTS][2];

// You will use this array to switch the color / track ids
int idLookup[MAXPOINTS];


// You can make the touch points larger
// by changing this radius value
static int radius = 50;

// There should be at least as many colors
// as there can be touch points so that you
// can have different colors for each point
COLORREF colors[] = { RGB(153,255,51),
					  RGB(153,0,0),
					  RGB(0,153,0),
					  RGB(255,255,0),
					  RGB(255,51,204),
					  RGB(0,0,0),
					  RGB(0,153,0),
					  RGB(153, 255, 255),
					  RGB(153,153,255),
					  RGB(0,51,153)
};

// #Touch�^�b�`�|�C���g

///# �x�[�X�ݒ�

//-�^�X�N�ݒ�
#define TARGET_RESOLUTION			1		//�}���`���f�B�A�^�C�}�[����\ msec
#define SYSTEM_TICK_ms				25		//���C���X���b�h���� msec

//�^�C�}�[ID
#define ID_MAIN_WINDOW_UPDATE_TIMER				10702
#define ID_MAIN_WINDOW_UPDATE_TICK_ms			1000


//-Main Window�̔z�u�ݒ�
#define MAIN_WND_INIT_SIZE_W		380		//-Main Window�̏����T�C�Y�@W
#define MAIN_WND_INIT_SIZE_H		195		//-Main Window�̏����T�C�Y�@H
#define MAIN_WND_INIT_POS_X			670		//-Main Window�̏����ʒu�ݒ�@X
#define MAIN_WND_INIT_POS_Y			20		//-Main Window�̏����ʒu�ݒ�@Y

//-ID��` Main�X���b�h�p�@2000 + 300 +��
#define ID_STATUS					2400
#define IDC_STATIC_0				2401
#define IDC_PB_EXIT					2402
#define IDC_PB_DEBUG				2403
#define IDC_PB_COMWIN				2404
#define IDC_PB_SIM_OTE				2405

#define IDC_STATIC_1				2415
#define IDC_PB_SENSOR_1				2416
#define IDC_PB_SENSOR_2				2417
#define IDC_PB_0SET_CAMERA			2418
#define IDC_PB_0SET_TILT			2419
#define IDC_PB_RESET_CAMERA			2420
#define IDC_PB_RESET_TILT			2421
#define IDC_PB_PC_RESET				2422
#define IDC_PB_SCREEN_SHOT			2424

#define ID_CHECK_SWAY_CAL_NO_OFFSET 2424
#define ID_CHECK_SWAY_CAL_NO_TILT   2425

#define MSG_SENSOR_SOCK				2410	//WinSock�C�x���g���b�Z�[�W


//���C���E�B���h�E�Ǘ��\����
typedef struct stMainWndTag {
	HWND hWnd_status_bar = NULL;	//�X�e�[�^�X�o�[�̃n���h��
	HWND h_static0 = NULL;			//�X�^�e�B�b�N�I�u�W�F�N�g�̃n���h��
	HWND h_pb_exit = NULL;			//�{�^���I�u�W�F�N�g�̃n���h��
	HWND h_pb_debug = NULL;			//�{�^���I�u�W�F�N�g�̃n���h��
	HWND h_pb_comwin = NULL;		//�{�^���I�u�W�F�N�g�̃n���h��

	HWND h_static1 = NULL;			//�X�^�e�B�b�N�I�u�W�F�N�g�̃n���h��
	HWND h_pb_sel_sensor1 = NULL;	//�{�^���I�u�W�F�N�g�̃n���h��
	HWND h_pb_sel_sensor2 = NULL;	//�{�^���I�u�W�F�N�g�̃n���h��
	HWND h_pb_0set_sensor = NULL;	//�{�^���I�u�W�F�N�g�̃n���h��
	HWND h_pb_0set_tilt = NULL;		//�{�^���I�u�W�F�N�g�̃n���h��
	HWND h_pb_reset_sensor = NULL;	//�{�^���I�u�W�F�N�g�̃n���h��
	HWND h_pb_reset_tilt = NULL;	//�{�^���I�u�W�F�N�g�̃n���h��
	HWND h_pb_pc_reset = NULL;		//�{�^���I�u�W�F�N�g�̃n���h��
	HWND h_pb_img_save = NULL;		//�{�^���I�u�W�F�N�g�̃n���h��


	HWND hWorkWnd = NULL;			//���[�N�E�B���h�E�̃n���h��

}ST_MAIN_WND, * LPST_MAIN_WND;

//�}���`���f�B�A�^�C�}�[�Ǘ��\����
#define TARGET_RESOLUTION			1		//�}���`���f�B�A�^�C�}�[����\ msec
#define SYSTEM_TICK_ms				25		//���C���X���b�h���� msec
#define INITIAL_TASK_STACK_SIZE		16384	//�^�X�N�I�u�W�F�N�g�X���b�h�p�X�^�b�N�T�C�Y

typedef struct stKnlManageSetTag {
	WORD mmt_resolution = TARGET_RESOLUTION;			//�}���`���f�B�A�^�C�}�[�̕���\
	unsigned int cycle_base = SYSTEM_TICK_ms;			//�}���`���f�B�A�^�C�}�[�̕���\
	WORD KnlTick_TimerID = 0;							//�}���`���f�B�A�^�C�}�[��ID
	unsigned int num_of_task = 0;						//�A�v���P�[�V�����ŗ��p����X���b�h��
	unsigned long sys_counter = 0;						//�}���`���f�B�A�N���^�C�}�J�E���^
	SYSTEMTIME Knl_Time = { 0,0,0,0,0,0,0,0 };			//�A�v���P�[�V�����J�n����̌o�ߎ���
	unsigned int stackSize = INITIAL_TASK_STACK_SIZE;	//�^�X�N�̏����X�^�b�N�T�C�Y
}ST_KNL_MANAGE_SET, * LPST_KNL_MANAGE_SET;