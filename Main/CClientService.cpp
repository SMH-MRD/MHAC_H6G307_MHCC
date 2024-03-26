#include "CClientService.h"
#include "OTE0panel.h"
#include "PLC_DEF.h"


//-���L�������I�u�W�F�N�g�|�C���^:
extern CSharedMem* pCraneStatusObj;
extern CSharedMem* pSwayStatusObj;
extern CSharedMem* pPLCioObj;
extern CSharedMem* pSwayIO_Obj;
extern CSharedMem* pOTEioObj;
extern CSharedMem* pCSInfObj;
extern CSharedMem* pPolicyInfsObj;
extern CSharedMem* pAgentInfObj;
extern CSharedMem* pJobIO_Obj;

extern vector<void*>	VectpCTaskObj;	//�^�X�N�I�u�W�F�N�g�̃|�C���^
extern ST_iTask g_itask;
extern ST_SPEC spec;

ERMPLC_BOUT_MAP plc_bo_map;	//PLC���͐M����͗p
CABPLC_BOUT_MAP plc_bi_map;	//PLC���͐M����͗p

/****************************************************************************/
/*   �R���X�g���N�^�@�f�X�g���N�^                                           */
/****************************************************************************/
CClientService::CClientService() {
	pPLC_IO = NULL;
	pCraneStat = NULL;
}

CClientService::~CClientService() {

}
/****************************************************************************/
/*   �^�X�N����������                                                       */
/* �@���C���X���b�h�ŃC���X�^���X��������ɌĂт܂��B                       */
/****************************************************************************/
//static BOOL PLC_PBs_last[N_PLC_PB];

void CClientService::init_task(void* pobj) {

	//���L�������\���̂̃|�C���^�Z�b�g
	pPLC_IO = (LPST_PLC_IO)(pPLCioObj->get_pMap());
	pCraneStat = (LPST_CRANE_STATUS)(pCraneStatusObj->get_pMap()); 
	pCSinf = (LPST_CS_INFO)(pCSInfObj->get_pMap());
	pAgent_Inf = (LPST_AGENT_INFO)(pAgentInfObj->get_pMap());
	pOTE_IO = (LPST_OTE_IO)(pOTEioObj->get_pMap());
	pJob_IO = (LPST_JOB_IO)(pJobIO_Obj->get_pMap());

	for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
		if (i == ID_OTE_GRIP_SWITCH)continue;//	�O���b�v�X�C�b�`�͑ΏۊO
		CS_workbuf.notch_pos[ID_OTE_NOTCH_POS_HOLD][i] = CS_workbuf.notch_pos[ID_OTE_NOTCH_POS_TRIG][i] = ID_OTE_0NOTCH_POS;	//0�m�b�`�ŏ�����
		//�����v�w��
		CS_workbuf.ote_notch_lamp[i * 10 + ID_OTE_0NOTCH_POS].com = OTE_LAMP_COM_ON;
		CS_workbuf.ote_notch_lamp[i * 10 + ID_OTE_0NOTCH_POS].color = OTE0_GREEN;
	}

	CS_workbuf.ote_remote_status &= ~CS_CODE_OTE_REMOTE_ENABLE; //�����[�g������N���A

	for (int i = ID_HOIST; i <= ID_AHOIST; i++) CS_workbuf.auto_status[i] = STAT_MANUAL;

	return;
};

/****************************************************************************/
/*   �^�X�N���������                                                       */
/* �@�^�X�N�X���b�h�Ŗ��������s�����֐�			�@                      */
/****************************************************************************/

static UINT16 pb_ope_last[N_OTE_PNL_PB];
static INT32 ote_grip_last;

void CClientService::routine_work(void* param) {
	input();
	main_proc();
	output();

	//����PB�O��l�ێ�
	for (int i = ID_OTE_PB_TEISHI; i <= ID_OTE_PB_FUREDOME; i++) {
		pb_ope_last[i] = pOTE_IO->ote_umsg_in.body.pb_ope[i];
	}
	//�O���b�v�X�C�b�`�O��l�ێ�
	INT32 ote_grip_last = pOTE_IO->ote_grip;
};

//# ����������菇1�@�O���M������
/***  JOB�֘A���͐M�������i�����J�nPB)   ***/
/****************************************************************************/
/*  ���͏���																*/
/****************************************************************************/

void CClientService::input() {

	return;
};

//# OTE���͑���[���L�����f
int CClientService::can_ote_activate() {
	if ((pOTE_IO->ote_u_silent_cnt < CS_OTE_U_MSG_TIMEOUT)&&(pOTE_IO->ote_umsg_in.body.ope_mode & OTE_ID_OPE_MODE_COMMAND)) {
		CS_workbuf.ote_remote_status |= CS_CODE_OTE_REMOTE_ENABLE;
		return L_ON;
	}
	CS_workbuf.ote_remote_status &= ~CS_CODE_OTE_REMOTE_ENABLE;
	return L_OFF;
}
/****************************************************************************/
/*  ���C������																*/
/****************************************************************************/
int CClientService::ote_handle_proc() {         //����[������
	if (can_ote_activate()) {
		//�O���b�v�X�C�b�`
		{
			CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_RMT].com = CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_PAD_MODE].com = CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_ESTOP].com = CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_NOTCH].com = OTE_LAMP_COM_ON;

			if (pOTE_IO->ote_estop == L_OFF)
				CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_ESTOP].color = OTE0_GREEN;
			else
				CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_ESTOP].color = OTE0_RED;

			if (pOTE_IO->ote_grip == L_ON)
				CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_NOTCH].color = OTE0_ORANGE;
			else
				CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_NOTCH].color = OTE0_GREEN;

			if (pOTE_IO->ote_padmode == L_ON)
				CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_PAD_MODE].color = OTE0_ORANGE;
			else
				CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_PAD_MODE].color = OTE0_GLAY;

			if (pOTE_IO->ote_umsg_in.body.ope_mode & OTE_ID_OPE_MODE_COMMAND)
				CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_RMT].color = OTE0_ORANGE;
			else
				CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_RMT].color = OTE0_GLAY;
		}
		//����X�C�b�`
		{
			//��~�@PBL
			CS_workbuf.ote_pb_lamp[ID_OTE_PB_TEISHI].com = CS_workbuf.ote_pb_lamp[ID_OTE_PB_KIDOU].com = OTE_LAMP_COM_ON;
			if (pOTE_IO->ote_umsg_in.body.pb_ope[ID_OTE_PB_TEISHI]) CS_workbuf.ote_pb_lamp[ID_OTE_PB_TEISHI].color = OTE0_GREEN;
			else													CS_workbuf.ote_pb_lamp[ID_OTE_PB_TEISHI].color = OTE0_GLAY;
			//�N�� PBL
			if (pOTE_IO->ote_umsg_in.body.pb_ope[ID_OTE_PB_KIDOU]) CS_workbuf.ote_pb_lamp[ID_OTE_PB_KIDOU].color = OTE0_RED;
			else													CS_workbuf.ote_pb_lamp[ID_OTE_PB_KIDOU].color = OTE0_GLAY;

			//�����@PBL
			if ((pOTE_IO->ote_umsg_in.body.pb_ope[ID_OTE_PB_AUTO]) && !(pb_ope_last[ID_OTE_PB_AUTO])) {
				if (CS_workbuf.auto_mode)	CS_workbuf.auto_mode = L_OFF;
				else 						CS_workbuf.auto_mode = L_ON;
			}
			CS_workbuf.ote_pb_lamp[ID_OTE_PB_AUTO].com = OTE_LAMP_COM_ON;
			if (CS_workbuf.auto_mode)	CS_workbuf.ote_pb_lamp[ID_OTE_PB_AUTO].color = OTE0_ORANGE;
			else 						CS_workbuf.ote_pb_lamp[ID_OTE_PB_AUTO].color = OTE0_GREEN;

			//�U��~�߁@PBL
			if ((pOTE_IO->ote_umsg_in.body.pb_ope[ID_OTE_PB_FUREDOME]) && !(pb_ope_last[ID_OTE_PB_FUREDOME])) {
				if (CS_workbuf.antisway_mode)	CS_workbuf.antisway_mode = L_OFF;
				else							CS_workbuf.antisway_mode = L_ON;
			}
			CS_workbuf.ote_pb_lamp[ID_OTE_PB_FUREDOME].com = OTE_LAMP_COM_ON;
			if (CS_workbuf.antisway_mode)	CS_workbuf.ote_pb_lamp[ID_OTE_PB_FUREDOME].color = OTE0_ORANGE;
			else							CS_workbuf.ote_pb_lamp[ID_OTE_PB_FUREDOME].color = OTE0_GREEN;

			//�����I�����
			for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
				CS_workbuf.auto_status[i] = pOTE_IO->ote_umsg_in.body.auto_sel[i];
			}

		}
		//�����v
		{
			//�劲�����v�@PB�󂯕t��������PLC IF
			CS_workbuf.ote_pb_lamp[ID_OTE_PB_SYUKAN].com = OTE_LAMP_COM_ON;
			if (pPLC_IO->input.rbuf.erm_bo[plc_bo_map.ctrl_source_mc_ok.x] & plc_bo_map.ctrl_source_mc_ok.y) {

				CS_workbuf.ote_pb_lamp[ID_OTE_PB_SYUKAN].color = OTE0_RED;
			}
			else {
				CS_workbuf.ote_pb_lamp[ID_OTE_PB_SYUKAN].color = OTE0_GREEN;
			}
			//����~�����v�@PB�󂯕t��������PLC IF
			CS_workbuf.ote_pb_lamp[ID_OTE_PB_HIJYOU].com = OTE_LAMP_COM_ON;
			if (pPLC_IO->input.rbuf.cab_bi[plc_bi_map.cab_estp.x] & plc_bi_map.cab_estp.y) {

				CS_workbuf.ote_pb_lamp[ID_OTE_PB_HIJYOU].color = OTE0_GREEN;
			}
			else {
				CS_workbuf.ote_pb_lamp[ID_OTE_PB_HIJYOU].color = OTE0_RED;
			}



			//�m�b�`�����v
			for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
				if (i == ID_OTE_GRIP_SWITCH)continue;//	�O���b�v�X�C�b�`�͑ΏۊO
				for (int j = 0; j < 9; j++) {
					CS_workbuf.ote_notch_lamp[i * 10 + j].com = OTE_LAMP_COM_ON;

					if (j == pOTE_IO->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][i]) {
						CS_workbuf.ote_notch_lamp[i * 10 + j].com = OTE_LAMP_COM_ON;

						if (j == ID_OTE_0NOTCH_POS)	CS_workbuf.ote_notch_lamp[i * 10 + j].color = OTE0_GREEN;
						else						CS_workbuf.ote_notch_lamp[i * 10 + j].color = OTE0_ORANGE;
					}
					else {

						CS_workbuf.ote_notch_lamp[i * 10 + j].color = OTE0_GLAY;
					}
				}


				for (int i = ID_HOIST; i <= ID_AHOIST; i++) {

					if (i == ID_OTE_GRIP_SWITCH) { continue; }//	�O���b�v�X�C�b�`�͑ΏۊO

					//�S�N���A
					for (int j = 0; j < 9; j++) {
						CS_workbuf.ote_notch_lamp[i * 10 + j].com = OTE_LAMP_COM_ON;
						CS_workbuf.ote_notch_lamp[i * 10 + j].color = OTE0_GLAY;
					}

					int index_base = i * 10 + NOTCH_4;	//�m�b�`��10�̔z��@0�m�b�`��4�̈ʒu

					//�m�b�`�w�ߕ\��
					if (pPLC_IO->v_com_notch[i] != 0)	//0ɯ��ȊO
						CS_workbuf.ote_notch_lamp[pPLC_IO->v_com_notch[i] + index_base].color = OTE0_ORANGE;
					//�m�b�`���xFB�\��
					CS_workbuf.ote_notch_lamp[pPLC_IO->v_fb_notch[i] + index_base].color = OTE0_RED;
					//0�m�b�`�\��
					if(pPLC_IO->v_com_notch[i] == 0) CS_workbuf.ote_notch_lamp[index_base].color = OTE0_GREEN;
				}
			}
		}
	}
	else {//�����vOFF
		//���색���v
		for (int i = ID_OTE_PB_TEISHI; i <= ID_OTE_PB_FUREDOME; i++) {
			CS_workbuf.ote_pb_lamp[i].com = OTE_LAMP_COM_OFF;
		}
		//�m�b�`�����v
		for (int i = 0; i < N_OTE_PNL_NOTCH; i++) {
			CS_workbuf.ote_notch_lamp[i].com = OTE_LAMP_COM_OFF;
		}

		for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
			if (i == ID_OTE_GRIP_SWITCH)continue;//	�O���b�v�X�C�b�`�͑ΏۊO
			CS_workbuf.notch_pos[ID_OTE_NOTCH_POS_HOLD][i] = CS_workbuf.notch_pos[ID_OTE_NOTCH_POS_TRIG][i] = ID_OTE_0NOTCH_POS;	//0�m�b�`�ŏ�����
			//�����v�w��
			CS_workbuf.ote_notch_lamp[i * 10 + ID_OTE_0NOTCH_POS].com = OTE_LAMP_COM_ON;
			CS_workbuf.ote_notch_lamp[i * 10 + ID_OTE_0NOTCH_POS].color = OTE0_GREEN;
		}
	}

	return 0; 
}

static UINT32 ote_target_seq_last;
void CClientService::main_proc() {

	//����[������
	ote_handle_proc();
	
	//�������W���u�C�x���g����
	//�������o�^����

	LPST_JOB_SET p_job;

	if ((CS_workbuf.auto_mode == L_OFF) &&(CS_workbuf.job_control_status != CS_JOBSET_STATUS_DISABLE)){
		pJob_IO->job_list[ID_JOBTYPE_SEMI].n_job		= pJob_IO->job_list[ID_JOBTYPE_JOB].n_job		= 0;
		pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot	= pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot	= 0;
		CS_workbuf.job_control_status = CS_JOBSET_STATUS_DISABLE;
	}

	//�C�x���g����
	switch (CS_workbuf.job_control_status) {

	case CS_JOBSET_STATUS_DISABLE: {
		if(CS_workbuf.auto_mode==L_ON) CS_workbuf.job_control_status = CS_JOBSET_STATUS_IDLE;
		break;
	}
	case CS_JOBSET_STATUS_IDLE:		//�W���u����
	case CS_JOBSET_STATUS_STANDBY:	//�W���u�L��
	{
		p_job = &pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot];

		//�������R�}���h�󂯕t������
		if ((pOTE_IO->ote_grip == L_ON) && (ote_grip_last == L_OFF)) {//�O���b�v�g���K
			//�ڕW�ʒu�V�[�P���X�ԍ��X�V����΃R�}���h��t�@JOB�o�^
			if (pOTE_IO->ote_umsg_in.body.target_seq_no != ote_target_seq_last) {
				//JOB LIST����
				pJob_IO->job_list[ID_JOBTYPE_SEMI].n_job		= 1;
				pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot	= 0;//�������̓o�b�t�@�Œ�
				pJob_IO->job_list[ID_JOBTYPE_SEMI].status[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot]	= CS_JOBSET_STATUS_STANDBY;
				
				//JOB SET����
				p_job->status		= STAT_TRIGED;
				p_job->list_id		= ID_JOBTYPE_SEMI;
				p_job->n_com		= 1;//JOB�̃R�}���h���@�������͂P	
				p_job->job_id		= 0;

				p_job->type			= ID_JOBTYPE_SEMI;
				p_job->code			= pOTE_IO->ote_umsg_in.body.target_seq_no;
				p_job->com_type[0]	= ID_JOBIO_COMTYPE_PARK;
				//�ڕW�ʒu�Z�b�g
				p_job->targets[0].pos[ID_HOIST]		= pOTE_IO->ote_umsg_in.body.auto_tg_pos[ID_HOIST];
				p_job->targets[0].pos[ID_BOOM_H]	= pOTE_IO->ote_umsg_in.body.auto_tg_pos[ID_BOOM_H];
				p_job->targets[0].pos[ID_SLEW]		= pOTE_IO->ote_umsg_in.body.auto_tg_pos[ID_SLEW];
				p_job->targets[0].pos[ID_AHOIST]	= pOTE_IO->ote_umsg_in.body.auto_tg_pos[ID_AHOIST];

				CS_workbuf.job_control_status = CS_JOBSET_STATUS_STANDBY;

				//�N���C�A���g�֕�
				wostrs.str(L"");
				wostrs << L"�ړ��R�}���h�󂯕t���܂����@No.=" << pOTE_IO->ote_umsg_in.body.target_seq_no;
				txout2msg_listbox(wostrs.str());

				ote_target_seq_last = pOTE_IO->ote_umsg_in.body.target_seq_no;
			}
		}

		//�W���u���������Ă�����IDLE���
		if ((CS_workbuf.job_control_status == CS_JOBSET_STATUS_STANDBY) && (p_job->status == STAT_END)) {
			CS_workbuf.job_control_status = CS_JOBSET_STATUS_IDLE;
			//p_job�̃t���O�ނ�POLICY�̃X�e�[�^�X�X�V�Ăяo���ōX�V
		}

	}break;

	default:break;
	}

	
	//���݃A�N�e�B�u��JOB
	if (CS_workbuf.auto_mode == L_OFF) {
		CS_workbuf.p_active_job = NULL;
	}
	else if (pJob_IO->job_list[ID_JOBTYPE_SEMI].n_job != 0) {
		p_job = &(pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot]);
		switch (p_job->status) {
		case STAT_TRIGED://�N���҂�
		case STAT_ACTIVE:
		case STAT_STANDBY:
			CS_workbuf.p_active_job = p_job;
			break;

		case STAT_SUSPENDED:
		case STAT_ABOTED:
		case STAT_END:
		case STAT_ABNORMAL_END:
			CS_workbuf.p_active_job = NULL;
			break;
		}
	}
	else if (pJob_IO->job_list[ID_JOBTYPE_JOB].n_job != 0) {
		p_job = &(pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot]);
		switch (p_job->status) {
		case STAT_TRIGED://�N���҂�
		case STAT_ACTIVE:
		case STAT_STANDBY:
			CS_workbuf.p_active_job = p_job;
			break;

		case STAT_SUSPENDED:
		case STAT_ABOTED:
		case STAT_END:
		case STAT_REQ_WAIT:
			CS_workbuf.p_active_job = NULL;
			break;
		}
	}
	else {
		CS_workbuf.p_active_job = NULL;
	}

	//�O���b�v�X�C�b�`��Ԃ̔���iAGENT�p�jJOB�����̏�Ԃ�n������
	grip_status = pOTE_IO->ote_grip;

	return;
}

//�W���u�̃��V�s�Z�b�g
LPST_JOB_SET CClientService::set_job_seq(LPST_JOB_SET pjob_set) {
	LPST_JOB_SET pjob = NULL;
	return pjob;
}

//�������̃��V�s�Z�b�g
LPST_JOB_SET CClientService::set_semi_seq(LPST_JOB_SET pjob_set) {

	return pjob_set;
}

int CClientService::perce_client_message(LPST_CLIENT_COM_RCV_MSG pmsg) {
	return 0;
}

/****************************************************************************/
/*  �o�͏���																*/
/*  �W���u�֘A�����v�\����													*/
/****************************************************************************/
void CClientService::output() {
	
	//���L�������o��
	memcpy_s(pCSinf, sizeof(ST_CS_INFO), &CS_workbuf, sizeof(ST_CS_INFO));
	//�^�X�N�p�l���\���o��
	{
		
		wostrs.str(L"");
		wostrs << L" AS=" << CS_workbuf.antisway_mode << L",AUTO=" << CS_workbuf.auto_mode;

		int status;
		if (CS_workbuf.p_active_job != NULL) {
			status = CS_workbuf.p_active_job->status;
			wostrs << L",JOB TYPE=" << CS_workbuf.p_active_job->list_id << L", id=" << CS_workbuf.p_active_job->job_id;
		}
		else status = STAT_NA;

		if ((pJob_IO->job_list[ID_JOBTYPE_JOB].n_job <= 0) && (pJob_IO->job_list[ID_JOBTYPE_SEMI].n_job <= 0)) {
			wostrs << L" >JOB: 0 hold";
		}
		else if ((CS_workbuf.p_active_job != NULL) &&
			(CS_workbuf.p_active_job->list_id == ID_JOBTYPE_JOB) &&
			(pJob_IO->job_list[ID_JOBTYPE_JOB].n_job > 0)) {
			wostrs << L" >JOB: " << pJob_IO->job_list[ID_JOBTYPE_JOB].n_job << L" iHOT-> ";
			if (status & STAT_STANDBY)			wostrs << L"STANDBY";
			else if (status & STAT_ACTIVE)		wostrs << L"ACTIVE";
			else if (status & STAT_TRIGED)		wostrs << L"TRIGED";
			else if (status & STAT_SUSPENDED)	wostrs << L"SUSPEND";
			else if (status & STAT_REQ_WAIT)	wostrs << L"WAIT REQ";
			else wostrs << L"OUT OF SERV";

		}
		else if ((CS_workbuf.p_active_job != NULL) &&
			(CS_workbuf.p_active_job->list_id == ID_JOBTYPE_SEMI) &&
			(pJob_IO->job_list[ID_JOBTYPE_SEMI].n_job > 0)) {
			wostrs << L" >SEMIAUTO: " << pJob_IO->job_list[ID_JOBTYPE_SEMI].n_job << L" iHOT-> ";
			if (status & STAT_STANDBY)		wostrs << L"STANDBY";
			else if (status & STAT_ACTIVE)	wostrs << L"ACTIVE";
			else if (status & STAT_TRIGED)	wostrs << L"TRIGED";
			else if (status & STAT_SUSPENDED)	wostrs << L"SUSPEND";
			else if (status & STAT_REQ_WAIT)	wostrs << L"WAIT REQ";
			else wostrs << L"OUT OF SERV";
		}
		else  wostrs << L" >NO JOB REQUEST ";

		wostrs << L" >OTEU " << pOTE_IO->ote_u_silent_cnt;

		wostrs << L" --Scan " << inf.period;

		tweet2owner(wostrs.str()); wostrs.str(L""); wostrs.clear();
	}

	return;

};

/****************************************************************************/
/*   ���^�X�N����̃A�N�Z�X�֐�												*/
/****************************************************************************/

/// <summary>
/// AGENT����̎��s�҂�JOB�₢���킹����
/// </summary>
/// <returns>LPST_JOB_SET : �W���u�Z�b�g�\���̂̃|�C���^</returns>
LPST_JOB_SET CClientService::get_next_job() {
	
	return CS_workbuf.p_active_job;
}

//### POLICY�����JOB Status�X�V�˗�
int CClientService::update_job_status(LPST_JOB_SET pjobset, int fb_code) {

	if (pjobset->list_id == ID_JOBTYPE_JOB) {
		switch (fb_code) {
		case STAT_END: {
			pJob_IO->job_list[ID_JOBTYPE_JOB].n_job--;
			pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_END;

			job_report2client(pjobset, STAT_END);

			return STAT_ACK;
			break;
		}
		case STAT_ABNORMAL_END: {
			pJob_IO->job_list[ID_JOBTYPE_JOB].n_job--;
			pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_END;

			job_report2client(pjobset, STAT_ABNORMAL_END);

			return STAT_ACK;
		}break;
		case STAT_ACTIVE :break;
		case STAT_ABOTED:break;
		default:break;
		}
	}
	else if (pjobset->list_id == ID_JOBTYPE_SEMI) {//�������ł̓R�}���h�������W���u����
		switch (fb_code) {
		case STAT_END: {
			//���튮����JOB�̃z�[���h����0�N���A
			pJob_IO->job_list[ID_JOBTYPE_SEMI].n_job = 0;
			pjobset->status = STAT_END;

			//���s���W���u����
			CS_workbuf.p_active_job = NULL;

			job_report2client(pjobset, STAT_END);

			return STAT_ACK;
		}break;
		case STAT_ABNORMAL_END: {
			//�ُ튮����JOB�̃z�[���h����0�N���A
			pJob_IO->job_list[ID_JOBTYPE_SEMI].n_job = 0;
			pjobset->status = STAT_ABNORMAL_END;

			//���s���W���u����
			CS_workbuf.p_active_job = NULL;

			job_report2client(pjobset, STAT_ABNORMAL_END);

			return STAT_ACK;
		}break;

		case STAT_ABOTED: {
			//�W���u�L�����Z�����i����OFF,�O���b�v�X�C�b�`OFF���jJOB�̃z�[���h����0�N���A
			pJob_IO->job_list[ID_JOBTYPE_SEMI].n_job = 0;
			pjobset->status = STAT_ABOTED;

			//���s���W���u����
			CS_workbuf.p_active_job = NULL;

			job_report2client(pjobset, STAT_ABOTED);

			return STAT_ACK;
		}break;

		case STAT_STANDBY: {
			//�W���u���s�񍐂́A�X�e�[�^�X�̍X�V�̂�
			pjobset->status = STAT_STANDBY;

			job_report2client(pjobset, STAT_STANDBY);

			return STAT_ACK;
		}break;

		case STAT_ACTIVE: {
			//�W���u���s�񍐂́A�X�e�[�^�X�̍X�V�̂�
			pjobset->status = STAT_ACTIVE;

			job_report2client(pjobset, STAT_ACTIVE);

			return STAT_ACK;
		}break;		
		
		case STAT_SUSPENDED: {
			//���f���i�蓮����j�̏ꍇ�́A�X�e�[�^�X���X�^���o�C��Ԃɖ߂�
			pjobset->status = STAT_SUSPENDED;

			job_report2client(pjobset, STAT_SUSPENDED);

			return STAT_ACK;
		}break;
		default:return STAT_LOGICAL_ERROR;
		}
	}

	return STAT_LOGICAL_ERROR;
}



/****************************************************************************/
/*   JOB����															�@�@*/
/****************************************************************************/

//�N���C�A���g�ւ̃t�B�[�h�o�b�N
int CClientService::job_report2client(LPST_JOB_SET pjobset, int fb_code) {       //Job�̎��s�󋵕�

	if (pjobset == NULL)return 0;

	//�N���C�A���g�֕�
	wostrs.clear(); wostrs.str(L"");
	wostrs << L"�R�}���hNo.=" << pjobset->code << L":";

	if (pjobset->list_id == ID_JOBTYPE_JOB) {
		switch (fb_code) {
		case STAT_END: {
		}break;
		case STAT_ABNORMAL_END:break;
		case STAT_ACTIVE:break;
		case STAT_ABOTED:break;
		default:break;
		}
	}
	else if (pjobset->list_id == ID_JOBTYPE_SEMI) {
		switch (fb_code) {
		case STAT_END: {
			wostrs << L"���튮��";
		}break;
		case STAT_ABNORMAL_END: {
			wostrs << L"�ُ튮��";
		}break;
		case STAT_STANDBY: {
			wostrs << L"�R�}���h" << pjobset->i_hot_com << L"������";
		}break;
		case STAT_ACTIVE: {
			wostrs << L"�R�}���h" << pjobset->i_hot_com << L"�N��";
		}break;
		case STAT_ABOTED: {
			wostrs << L"�L�����Z��";
		}break;
		case STAT_SUSPENDED: {
			wostrs << L"���f��";
		}break;
		default:break;
		}
	}
	txout2msg_listbox(wostrs.str());

	return STAT_SUCCEED;
	return STAT_NA;
}


void CClientService::init_job() {
	return;
}

/****************************************************************************/
/*   �^�X�N�ݒ�^�u�p�l���E�B���h�E�̃R�[���o�b�N�֐�                       */
/****************************************************************************/
LRESULT CALLBACK CClientService::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_TASK_FUNC_RADIO1:
		case IDC_TASK_FUNC_RADIO2:
		case IDC_TASK_FUNC_RADIO3:
		case IDC_TASK_FUNC_RADIO4:
		case IDC_TASK_FUNC_RADIO5:
		case IDC_TASK_FUNC_RADIO6:
			inf.panel_func_id = LOWORD(wp); set_panel_tip_txt(); set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0); 
			reset_panel_item_pb(hDlg);
			break;

		case IDC_TASK_ITEM_RADIO1:
		case IDC_TASK_ITEM_RADIO2:
		case IDC_TASK_ITEM_RADIO3:
		case IDC_TASK_ITEM_RADIO4:
		case IDC_TASK_ITEM_RADIO5:
		case IDC_TASK_ITEM_RADIO6:
			inf.panel_type_id = LOWORD(wp);set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hWnd_opepane, IDC_TASK_EDIT1));
			if (inf.panel_func_id == IDC_TASK_FUNC_RADIO6) {
				if (inf.panel_type_id == IDC_TASK_ITEM_RADIO1) {
					;
				}
			}
			break;
		case IDSET: {
			wstring wstr, wstr_tmp;

			//�T���v���Ƃ��Ă��낢��Ȍ^�œǂݍ���ŕ\�����Ă���
			wstr += L"Param 1(d):";
			int n = GetDlgItemText(hDlg, IDC_TASK_EDIT1, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stod(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 2(i):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT2, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stoi(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 3(f):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT3, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stof(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 4(l):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT4, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stol(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 5(c):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT5, (LPTSTR)wstr_tmp.c_str(), 128);
			wstr += wstr_tmp.c_str();

			wstr += L",   Param 6(c):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT6, (LPTSTR)wstr_tmp.c_str(), 128);
			wstr += wstr_tmp.c_str();

			txout2msg_listbox(wstr);


		}break;
		case IDRESET: {
			set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			reset_panel_func_pb(hDlg);
		}break;

		case IDC_TASK_OPTION_CHECK1:
			SendMessage(GetDlgItem(hDlg, IDC_TASK_OPTION_CHECK2), BM_SETCHECK, BST_UNCHECKED, 0L);
			if (IsDlgButtonChecked(hDlg, IDC_TASK_OPTION_CHECK1) == BST_CHECKED) inf.work_select = THREAD_WORK_OPTION1;
			else inf.work_select = THREAD_WORK_ROUTINE;
			break;

		case IDC_TASK_OPTION_CHECK2:
			SendMessage(GetDlgItem(hDlg, IDC_TASK_OPTION_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);
			if (IsDlgButtonChecked(hDlg, IDC_TASK_OPTION_CHECK2) == BST_CHECKED) inf.work_select = THREAD_WORK_OPTION2;
			else inf.work_select = THREAD_WORK_ROUTINE;
			break;
		}
	}
	return 0;
};

/****************************************************************************/
/*   �^�X�N�ݒ�p�l���̑���{�^�������e�L�X�g�ݒ�֐�                       */
/****************************************************************************/
void CClientService::set_panel_tip_txt()
{
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO1: {
		wstr = L"Type for Func1 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO2: {
		wstr = L"Type of Func2 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO3: {
		wstr = L"Type for Func3 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO4: {
		wstr = L"Type for Func4 \n\r 1:VP 2 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO5: {
		wstr = L"Type for Func5 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO6: {
		wstr = L"Func6(Debug) \n\r 1:SIM 2:PLC 3:SWAY 4:RIO 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	default: {
		wstr = L"Type for Func? \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		wstr_type += L"(Param of type?) \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
	}break;
	}

	SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_STATIC_TASKSET2), wstr.c_str());
	SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_STATIC_TASKSET3), wstr_type.c_str());
}

/****************************************************************************/
/*�@�@�^�X�N�ݒ�p�l���{�^���̃e�L�X�g�Z�b�g					            */
/****************************************************************************/
void CClientService::set_panel_pb_txt() {

	//WCHAR str_func06[] = L"DEBUG";

	//SetDlgItemText(inf.hWnd_opepane, IDC_TASK_FUNC_RADIO6, (LPCWSTR)str_func06);

	return;
};
