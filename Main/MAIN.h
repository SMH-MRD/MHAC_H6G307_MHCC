#pragma once

#include "resource.h"
#include "CTaskObj.h"
#include "CEnvironment.h"
#include "CClientService.h"
#include "CPolicy.h"
#include "CAgent.h"
#include "CSCADA.h"

///# �x�[�X�ݒ�

//-�^�X�N�ݒ�
#define TARGET_RESOLUTION			1		//�}���`���f�B�A�^�C�}�[����\ msec
#define SYSTEM_TICK_ms				25		//���C���X���b�h���� msec
#define MAX_APP_TASK				8		//�^�X�N�I�u�W�F�N�g�X���b�h�ő吔
#define INITIAL_TASK_STACK_SIZE		16384	//�^�X�N�I�u�W�F�N�g�X���b�h�p�X�^�b�N�T�C�Y
#define TASK_NUM					5		//���̃A�v���P�[�V�����̃^�X�N��

//�^�X�N�X�L�����^�C��msec
#define AGENT_SCAN_MS				25
#define ENV_SCAN_MS					25
#define POLICY_SCAN_MS				50
#define SCADA_SCAN_MS				25
#define CS_SCAN_MS					200

//-Main Window��
#define MAIN_WND_INIT_SIZE_W		620		//-Main Window�̏����T�C�Y�@W
#define MAIN_WND_INIT_SIZE_H		420		//-Main Window�̏����T�C�Y�@H
#define MAIN_WND_INIT_POS_X			20		//-Main Window�̏����ʒu�ݒ�@X
#define MAIN_WND_INIT_POS_Y			20		//-Main Window�̏����ʒu�ݒ�@Y

//-Tweet Message�\���pStatic Window�̃T�C�Y�ݒ�l
#define MSG_WND_H					20		//Height of tweet window
#define MSG_WND_W					590		//Width of  tweet window
#define MSG_WND_ORG_X				35		//Origin X of tweet window position 
#define MSG_WND_ORG_Y				10		//Origin Y of tweet window position 
#define MSG_WND_Y_SPACE				12		//Space between tweet windows Y direction 
#define TWEET_IMG_ORG_X				5		//Origin X of bitmap tweet icon position

//-�e�^�X�N�A�C�R���p�C���[�W���X�g�ݒ�l
#define ICON_IMG_W					32		//�A�C�R���C���[�W��
#define ICON_IMG_H					32		//�A�C�R���C���[�W����

//-�^�X�N�ݒ�^�u�z�u�ݒ�
#define TAB_DIALOG_W				620
#define TAB_DIALOG_H				300
#define TAB_POS_X					10
#define TAB_POS_Y					MSG_WND_ORG_Y+(MSG_WND_H+MSG_WND_Y_SPACE)*TASK_NUM+10
#define TAB_SIZE_H					35

//-ID��` Main�X���b�h�p�@WM_USER + 1000 + ��
#define ID_STATUS					WM_USER + 1001
#define ID_STATIC_MAIN				WM_USER + 1002
#define ID_TASK_SET_TAB				WM_USER + 1003


//-.ini �֘A�ݒ�


//#define PATH_OF_INIFILE					L"./app.ini"	//�I�v�V���i���p�X

#define NAME_OF_INIFILE					L"app"			//ini�t�@�C���t�@�C����
#define EXT_OF_INIFILE					L"ini"			//ini�t�@�C���g���q
#define PATH_OF_INIFILE					pszInifile		//ini�t�@�C���p�X


#define OPATH_SECT_OF_INIFILE			L"PATH"			//�I�v�V���i���p�X�Z�N�V����
#define OBJ_NAME_SECT_OF_INIFILE		L"OBJECT_NAME"	//�^�X�N�I�u�W�F�N�g�t���l�[���Z�N�V����
#define OBJ_SNAME_SECT_OF_INIFILE		L"OBJECT_SNAME"	//�^�X�N�I�u�W�F�N�g���̃Z�N�V����
#define NETWORK_SECT_OF_INIFILE			L"NW_PARAM"		//Communicator�^�X�N�Z�N�V����
#define SMEM_SECT_OF_INIFILE			L"SMEM"			//���L�������Z�N�V����
#define SYSTEM_SECT_OF_INIFILE			L"SYSTEM"		//�V�X�e���ݒ�Z�N�V����


//�e�Z�N�V�����̊e�^�X�N�p�̏����Q�Ƃ��邽�߂̃L�[
#define MAIN_KEY_OF_INIFILE				L"MAIN"
#define CLIENT_SERVICE_KEY_OF_INIFILE	L"CLIENT"
#define SCADA_KEY_OF_INIFILE			L"SCADA"
#define ENVIRONMENT_KEY_OF_INIFILE		L"ENV"
#define POLICY_KEY_OF_INIFILE			L"POLICY"
#define AGENT_KEY_OF_INIFILE			L"AGENT"
#define DEFAULT_KEY_OF_INIFILE			L"DEFAULT"
#define HELPER_KEY_OF_INIFILE			L"HELPER"
#define DUMMY_KEY_OF_INIFILE			L"DUMMY"
#define PRODUCT_MODE_KEY_OF_INIFILE		L"PRODUCT_MODE"

///# �^�X�N�X���b�h�N���Ǘ��p�\����
typedef struct stKnlManageSetTag {
	WORD mmt_resolution = TARGET_RESOLUTION;			//�}���`���f�B�A�^�C�}�[�̕���\
	unsigned int cycle_base = SYSTEM_TICK_ms;			//�}���`���f�B�A�^�C�}�[�̕���\
	WORD KnlTick_TimerID = 0;							//�}���`���f�B�A�^�C�}�[��ID
	unsigned int num_of_task = 0;						//�A�v���P�[�V�����ŗ��p����X���b�h��
	unsigned long sys_counter = 0;						//�}���`���f�B�A�N���^�C�}�J�E���^
	SYSTEMTIME Knl_Time = {0,0,0,0,0,0,0,0};			//�A�v���P�[�V�����J�n����̌o�ߎ���
	unsigned int stackSize = INITIAL_TASK_STACK_SIZE;	//�^�X�N�̏����X�^�b�N�T�C�Y
}ST_KNL_MANAGE_SET, * LPST_KNL_MANAGE_SET;
