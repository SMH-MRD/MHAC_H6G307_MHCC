#pragma once

#include "resource.h"
///# �x�[�X�ݒ�

//-Main Window�̔z�u�ݒ�
#define MAIN_WND_INIT_SIZE_W		380					//-Main Window�̏����T�C�Y�@W
#define MAIN_WND_INIT_SIZE_H		192					//-Main Window�̏����T�C�Y�@H
#define MAIN_WND_INIT_POS_X			1050				//-Main Window�̏����ʒu�ݒ�@X
#define MAIN_WND_INIT_POS_Y			875					//-Main Window�̏����ʒu�ݒ�@Y

//-ID��` Main�X���b�h�p�@2000 + 100 +��
#define ID_STATUS					2100
#define IDC_STATIC_0				2101
#define IDC_PB_EXIT					2102
#define IDC_PB_ACTIVE				2103
#define IDC_PB_PACKET_MODE			2104

//���C���E�B���h�E�Ǘ��\����
typedef struct stMainWndTag {
	HWND hWnd_status_bar;								//�X�e�[�^�X�o�[�̃n���h��
	HWND h_static0;										//�X�^�e�B�b�N�I�u�W�F�N�g�̃n���h��
	HWND h_pb_exit;										//�{�^���I�u�W�F�N�g�̃n���h��
	HWND h_pb_debug;									//�{�^���I�u�W�F�N�g�̃n���h��
	HWND h_chk_packetout;								//�U��Z���T�̃p�P�b�g�𑗐M����

	bool is_sway_packet_send;							//�U��Z���T�̃p�P�b�g�𑗐M����

}ST_MAIN_WND, * LPST_MAIN_WND;

//�}���`���f�B�A�^�C�}�[�Ǘ��\����
#define TARGET_RESOLUTION			1					//�}���`���f�B�A�^�C�}�[����\ msec
#define SYSTEM_TICK_ms				10					//���C���X���b�h���� msec
#define INITIAL_TASK_STACK_SIZE		16384				//�^�X�N�I�u�W�F�N�g�X���b�h�p�X�^�b�N�T�C�Y

typedef struct stKnlManageSetTag {
	WORD mmt_resolution = TARGET_RESOLUTION;			//�}���`���f�B�A�^�C�}�[�̕���\
	unsigned int cycle_base = SYSTEM_TICK_ms;			//�}���`���f�B�A�^�C�}�[�̕���\
	WORD KnlTick_TimerID = 0;							//�}���`���f�B�A�^�C�}�[��ID
	unsigned int num_of_task = 0;						//�A�v���P�[�V�����ŗ��p����X���b�h��
	unsigned long sys_counter = 0;						//�}���`���f�B�A�N���^�C�}�J�E���^
	SYSTEMTIME Knl_Time = { 0,0,0,0,0,0,0,0 };			//�A�v���P�[�V�����J�n����̌o�ߎ���
	unsigned int stackSize = INITIAL_TASK_STACK_SIZE;	//�^�X�N�̏����X�^�b�N�T�C�Y
}ST_KNL_MANAGE_SET, * LPST_KNL_MANAGE_SET;