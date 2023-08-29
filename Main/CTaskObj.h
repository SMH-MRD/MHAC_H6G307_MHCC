#pragma once

#include "CHelper.h"
#include <process.h>
#include <typeinfo>
#include "resource.h"

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

//�^�X�N�����ݒ蕔
#define DEFAUT_TASK_CYCLE			25		//������N���f�t�H���g�l msec
#define TASK_EVENT_MAX				8		//�^�X�N�N���C�x���g�I�u�W�F�N�g�ő吔

//inf.thread_com�p�@�X���b�h���[�v����p
#define REPEAT_INFINIT				0		//�i�v���[�v
#define TERMINATE_THREAD			1		//�X���b�h���f
#define REPEAT_ONCE					-1		//��������

//�X���b�h���s��run()�֐����̎��s�֐��I��p
#define THREAD_WORK_IDLE			0		//�A�C�h�����O����
#define THREAD_WORK_ROUTINE			1		//�ʏ폈��
#define THREAD_WORK_OPTION1			2		//�I�v�V��������1
#define THREAD_WORK_OPTION2			3		//�I�v�V��������2

#define MSG_LIST_MAX				14

#define ID_TIMER_EVENT				0	//�^�C�}�[�p�C�x���g�z��̈ʒu

//***********************************************
//  �^�X�N�I�u�W�F�N�g�C���f�b�N�X�\����    
//***********************************************
typedef struct stiTask {
	int environment;
	int scada;
	int agent;
	int policy;
	int client;
	int spare1;
	int spare2;
	int spare3;
}ST_iTask, * PST_iTask;

/***********************************************************************
�^�X�N�I�u�W�F�N�g�̌ʊǗ����\����
************************************************************************/
typedef struct st_thread_info {
	//-�I�u�W�F�N�g���ʏ��
	TCHAR			name[24];						//�I�u�W�F�N�g��
	TCHAR			sname[8];						//�I�u�W�F�N�g����
	HBITMAP			hBmp;							//�I�u�W�F�N�g���ʗp�r�b�g�}�b�v

	//-�X���b�h�ݒ���e
	int				index=0;						//�X���b�hIndex
	unsigned int	ID=0;							//�X���b�hID
	HANDLE			hndl=NULL;						//�X���b�h�n���h��
	HANDLE			hevents[TASK_EVENT_MAX];		//�C�x���g�n���h��
	int				n_active_events = 1;			//�L���ȃC�x���g��
	int				event_triggered=0;				//���������C�x���g�̔ԍ�
	unsigned int	cycle_ms=0;						//�X���b�h���s�ݒ����
	unsigned int	cycle_count=0;					//�X���b�h���s�ݒ�����@Tick count�ims/system tick)
	int				trigger_type=0;					//�X���b�h�N�������@�����or�C�x���g
	int				priority=0;						//�X���b�h�̃v���C�I���e�B
	int				thread_com=0;					//�X���b�h����t���O �@0:�J��Ԃ��@1:���f�@����ȊO�F��������:

	//-�X���b�h���j�^���
	DWORD			start_time=0;					//���X�L�����̃X���b�h�J�n����
	DWORD			act_time=0;						//1�X�L�����̃X���b�h����������
	DWORD			period=0;						//�X���b�h���[�v��������
	DWORD			act_count=0;					//�X���b�h���[�v�J�E���^
	DWORD			total_act=0;					//�N���ώZ�J�E���^
	unsigned int	time_over_count=0;				//�\��������I�[�o�[������

	//-�֘A�E�B���h�E�n���h��
	HWND			hWnd_parent = NULL;				//�e�E�B���h�E�̃n���h��
	HWND			hWnd_msgStatics = NULL;			//�e�E�B���h�E���b�Z�[�W�\���p�E�B���h�E�ւ̃n���h��
	HWND			hWnd_opepane = NULL;			//�����C���E�B���h�E�̃n���h���i���C���t���[����ɔz�u�j
	HWND			hWnd_msgList = NULL;			//�����C���E�B���h�E�̃��b�Z�[�W�\���p���X�g�R���g���[���ւ̃n���h��
	HWND			hWnd_work = NULL;				//����p��Ɨp�E�B���h�E�̃n���h��

	HINSTANCE		hInstance;

	//-����p�l���֘A
	int				cnt_PNLlist_msg = 0;			//�p�l�����b�Z�[�W���X�g�̃J�E���g��
	int				panel_func_id = 1;				//�p�l��function�{�^���̑I����e
	int				panel_type_id = 1;				//�p�l��type�{�^���̑I����e

	//-�O���C���^�[�t�F�[�X
	unsigned long* psys_counter;					//���C���V�X�e���J�E���^�[�̎Q�Ɛ�|�C���^
	unsigned		work_select = 0;				//�X���b�h���s�̊֐��̎�ނ��w��

	bool is_init_complete = false;

}ST_THREAD_INFO, * PST_THREAD_INFO;

/***********************************************************************
�^�X�N�X���b�h�I�u�W�F�N�g�x�[�X�N���X
************************************************************************/
class CTaskObj {
public:
	ST_THREAD_INFO inf;

	CTaskObj();
	virtual ~CTaskObj();//�f�X�g���N�^

	virtual void init_task(void* pobj);
	unsigned __stdcall run(void* param);//�X���b�h���s�Ώۊ֐�

	//�X���b�h���s�Ώۊ֐����ŌĂ΂�鏈�� run()���ł����ꂩ���I�����{�����
	// set_work()�ł���������{���邩�ݒ肷�� 
	virtual void routine_work(void* param);						//�ʏ폈��
	virtual void optional_work1(void* param);					//�I�v�V���������P
	virtual void optional_work2(void* param);					//�I�v�V���������Q
	virtual void default_work(void* param) { Sleep(1000); };	//�A�C�h������

	//����������̎��s�֐��I��
	unsigned int set_work(int work_id) { inf.work_select = work_id; return(inf.work_select); };

	//���N���X�^���̕\��
	virtual void output_name() { const type_info& t = typeid(this); std::cout << "My type is " << t.name() << std::endl; }

	//���^�X�N�ŗL�E�B���h�E����
	virtual HWND CreateOwnWindow(HWND h_parent_wnd);

	//���^�X�N�ݒ�^�u�p�l���E�B���h�E�����֐�
	virtual LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
	//�^�u�p�l����Static�e�L�X�g��ݒ�
	virtual void set_panel_tip_txt();
	//�^�u�p�l����Function�{�^����Static�e�L�X�g��ݒ�
	virtual void set_panel_pb_txt() { return; }
	//�p�����[�^�����\���l�ݒ�
	virtual void set_PNLparam_value(float p1, float p2, float p3, float p4, float p5, float p6);
	//�^�u�p�l����Function�{�^���̃��Z�b�g
	virtual void reset_panel_func_pb(HWND hDlg);
	virtual void reset_panel_item_pb(HWND hDlg);

protected:
	CHelper tool;
	ostringstream ostrs;		//ostringstream  �ꎞ������o�b�t�@
	wostringstream wostrs;		//wostringstream �ꎞ������o�b�t�@
	wstring wstr;				//wstring �ꎞ������o�b�t�@
	string  str;				//string �ꎞ������o�b�t�@

	//���C���E�B���h�E�ւ̃V���[�g���b�Z�[�W�\��
	void tweet2owner(const std::string& src);
	//���C���E�B���h�E�ւ̃V���[�g���b�Z�[�W�\��
	void tweet2owner(const std::wstring& srcw); 
	//���X�g�R���g���[���ւ̃V���[�g���b�Z�[�W�\��
	void txout2msg_listbox(const std::wstring str);
	//���X�g�R���g���[���ւ̃V���[�g���b�Z�[�W�\��
	void txout2msg_listbox(const std::string str);
};
