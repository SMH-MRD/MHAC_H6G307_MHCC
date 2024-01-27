#pragma once
#include "resource.h"
#include <windows.h>    // included for Windows Touch
#include <windowsx.h>   // included for point conversion

///# �x�[�X�ݒ�

//�^�C�}�[ID
#define ID_MAIN_WINDOW_UPDATE_TIMER				10702
#define ID_MAIN_WINDOW_UPDATE_TICK_ms			1000

//-Main Window�̔z�u�ݒ�
#define MAIN_WND_INIT_SIZE_W		220		//-Main Window�̏����T�C�Y�@W
#define MAIN_WND_INIT_SIZE_H		180		//-Main Window�̏����T�C�Y�@H
#define MAIN_WND_INIT_POS_X			648	//-Main Window�̏����ʒu�ݒ�@X
#define MAIN_WND_INIT_POS_Y			180		//-Main Window�̏����ʒu�ݒ�@Y

//-ID��` Main�X���b�h�p�@2000 + 400 +��
#define ID_STATUS					2400
#define IDC_STATIC_0				2401
#define IDC_PB_EXIT					2402
#define IDC_PB_DEBUG				2403
#define IDC_CHK_IFCHK				2404
#define IDC_CHK_OTE					2405
#define IDC_RADIO_LOCAL_OTE			2406	//���j�^�̂݁@PLC�������ݖ���
#define IDC_RADIO_REMOTE_OTE		2407	//���u���[�h�@�^�]������֘A�̂ݏ�������


#define MSG_SENSOR_SOCK				2410	//WinSock�C�x���g���b�Z�[�W


//���C���E�B���h�E�Ǘ��\����
typedef struct stMainWndTag {
	HWND hWnd_status_bar = NULL;	//�X�e�[�^�X�o�[�̃n���h��
	HWND h_static0 = NULL;			//�X�^�e�B�b�N�I�u�W�F�N�g�̃n���h��
	HWND h_pb_exit = NULL;			//�{�^���I�u�W�F�N�g�̃n���h��
	HWND h_chk_if = NULL;			//�`�F�b�N�{�b�N�X�I�u�W�F�N�g�̃n���h��
	HWND h_chk_local_ote = NULL;	//�`�F�b�N�{�b�N�X�I�u�W�F�N�g�̃n���h��

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
#define SYSTEM_TICK_ms				100		//���C���X���b�h���� msec
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