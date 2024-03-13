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

	for (int i = 0; i < SEMI_AUTO_TARGET_MAX; i++) {
		CS_workbuf.semi_auto_setting_target[i].pos[ID_HOIST] = spec.semi_target[i][ID_HOIST];
		CS_workbuf.semi_auto_setting_target[i].pos[ID_BOOM_H] = spec.semi_target[i][ID_BOOM_H];
		CS_workbuf.semi_auto_setting_target[i].pos[ID_SLEW] = spec.semi_target[i][ID_SLEW];
	}

	for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
		if (i == ID_OTE_GRIP_SWITCH)continue;//	�O���b�v�X�C�b�`�͑ΏۊO
		CS_workbuf.notch_pos[ID_OTE_NOTCH_POS_HOLD][i] = CS_workbuf.notch_pos[ID_OTE_NOTCH_POS_TRIG][i] = ID_OTE_0NOTCH_POS;	//0�m�b�`�ŏ�����
		//�����v�w��
		CS_workbuf.ote_notch_lamp[i * 10 + ID_OTE_0NOTCH_POS].com = OTE_LAMP_COM_ON;
		CS_workbuf.ote_notch_lamp[i * 10 + ID_OTE_0NOTCH_POS].color = OTE0_GREEN;
	}

	CS_workbuf.ote_remote_status &= ~CS_CODE_OTE_REMOTE_ENABLE; //�����[�g������N���A

	return;
};

/****************************************************************************/
/*   �^�X�N���������                                                       */
/* �@�^�X�N�X���b�h�Ŗ��������s�����֐�			�@                      */
/****************************************************************************/
void CClientService::routine_work(void* param) {
	input();
	main_proc();
	output();
};

//# ����������菇1�@�O���M������
/***  JOB�֘A���͐M�������i�����J�nPB)   ***/
/****************************************************************************/
/*  ���͏���																*/
/****************************************************************************/

void CClientService::input() {

	//PLC���͏���
	parce_onboard_input(CS_NORMAL_OPERATION_MODE);
	return;
};


//���[�h�Z�b�g�@�����ڕW�ʒu
int CClientService::parce_onboard_input(int mode) {

	/*### ���[�h�Ǘ� ###*/
	//�U��~�߃��[�h�Z�b�g
	INT16* pUIpb;
	INT16* pUIpb_semiauto;
#if 0
	if (pCraneStat->operation_mode & OPERATION_MODE_REMOTE) {
		if (pOTE_IO->rcv_msg_u.body.pb[ID_PB_ESTOP]) CS_workbuf.estop_active = L_ON;
		else CS_workbuf.estop_active = L_OFF;


		if ((pOTE_IO->rcv_msg_u.body.pb[ID_PB_ANTISWAY_ON]) && (as_pb_last == 0)) { // PB���͗����オ��
			if (CS_workbuf.antisway_mode == L_OFF)
				CS_workbuf.antisway_mode = L_ON;
			else
				CS_workbuf.antisway_mode = L_OFF;
		}
		else;

		as_pb_last = pOTE_IO->rcv_msg_u.body.pb[ID_PB_ANTISWAY_ON];

		pUIpb = pOTE_IO->rcv_msg_u.body.pb;
		pUIpb_semiauto = pOTE_IO->ui.PBsemiauto;
	}
	else {

		if (pPLC_IO->ui.PB[ID_PB_ESTOP]) CS_workbuf.estop_active = L_ON;
		else CS_workbuf.estop_active = L_OFF;
		
		if ((pPLC_IO->ui.PB[ID_PB_ANTISWAY_ON]) && (as_pb_last == 0)) { // PB���͗����オ��
			if (CS_workbuf.antisway_mode == L_OFF)
				CS_workbuf.antisway_mode = L_ON;
			else
				CS_workbuf.antisway_mode = L_OFF;
		}
		else;

		as_pb_last = pPLC_IO->ui.PB[ID_PB_ANTISWAY_ON];

		pUIpb = pPLC_IO->ui.PB;
		pUIpb_semiauto = pPLC_IO->ui.PBsemiauto;
	}

	//�������[�h�Z�b�g
	
	if ((pUIpb[ID_PB_AUTO_MODE]) && (auto_pb_last == 0)) { // PB���͗����オ��
		if (CS_workbuf.auto_mode == L_OFF)
			CS_workbuf.auto_mode = L_ON;
		else
			CS_workbuf.auto_mode = L_OFF;
	}
	auto_pb_last = pUIpb[ID_PB_AUTO_MODE];


	if (CS_workbuf.estop_active == L_ON) {

		CS_workbuf.antisway_mode = L_OFF;
		CS_workbuf.auto_mode = L_OFF;
	}

	//�������I��ݒ�,�ڕW�ʒu�ݒ�
	if (CS_workbuf.auto_mode == L_ON) {

		//�������ڕW�ݒ�
		//�ڕW�ʒu���m�莞�̂ݍX�V�\
		if ((CS_workbuf.target_set_z != CS_SEMIAUTO_TG_SEL_FIXED) && (CS_workbuf.target_set_xy != CS_SEMIAUTO_TG_SEL_FIXED)) {
			for (int i = 0; i < SEMI_AUTO_TARGET_MAX; i++) {		//�������ݒ�{�^���̏��

				//PB ON���ԃJ�E���g ���Ԃɉ����đI��ڕW�ʒu�ւ̐ݒ�A�o�^�ڕW�ʒu�̍X�V����
				if (pUIpb_semiauto[i] <= 0) CS_workbuf.semiauto_pb[i] = 0;
				else if (CS_workbuf.semiauto_pb[i] < SEMI_AUTO_TG_RESET_TIME) CS_workbuf.semiauto_pb[i]++;
				else;

				//�ڕW�ݒ�
				if (CS_workbuf.semiauto_pb[i] == SEMI_AUTO_TG_RESET_TIME) {//�������ڕW�ʒu�ݒ�l�X�V
					CS_workbuf.semi_auto_setting_target[i].pos[ID_HOIST] = pPLC_IO->status.pos[ID_HOIST];
					CS_workbuf.semi_auto_setting_target[i].pos[ID_BOOM_H] = pPLC_IO->status.pos[ID_BOOM_H];
					CS_workbuf.semi_auto_setting_target[i].pos[ID_SLEW] = pPLC_IO->status.pos[ID_SLEW];

					CS_workbuf.semi_auto_selected_target.pos[ID_HOIST] = CS_workbuf.semi_auto_setting_target[i].pos[ID_HOIST];
					CS_workbuf.semi_auto_selected_target.pos[ID_BOOM_H] = CS_workbuf.semi_auto_setting_target[i].pos[ID_BOOM_H];
					CS_workbuf.semi_auto_selected_target.pos[ID_SLEW] = CS_workbuf.semi_auto_setting_target[i].pos[ID_SLEW];
					//OTE�p�ϊ����W�Z�b�g
					set_selected_target_for_view();
					CS_workbuf.tg_sel_trigger_z = L_ON, CS_workbuf.tg_sel_trigger_xy = L_ON;
				}
				else if (CS_workbuf.semiauto_pb[i] == SEMI_AUTO_TG_SELECT_TIME) {						//�������ڕW�ݒ�
					if (i == CS_workbuf.semi_auto_selected) {											//�ݒ蒆�̃{�^���������������
						CS_workbuf.semi_auto_selected = SEMI_AUTO_TG_CLR;
						CS_workbuf.semi_auto_selected_target.pos[ID_HOIST] = pPLC_IO->status.pos[ID_HOIST];
						CS_workbuf.semi_auto_selected_target.pos[ID_BOOM_H] = pPLC_IO->status.pos[ID_BOOM_H];
						CS_workbuf.semi_auto_selected_target.pos[ID_SLEW] = pPLC_IO->status.pos[ID_SLEW];
						//OTE�p�ϊ����W�Z�b�g
						set_selected_target_for_view();
						CS_workbuf.tg_sel_trigger_z = L_OFF, CS_workbuf.tg_sel_trigger_xy = L_OFF;
					}
					else {																				//�������I���{�^����荞��
						CS_workbuf.semi_auto_selected = i;
						CS_workbuf.semi_auto_selected_target.pos[ID_HOIST] = CS_workbuf.semi_auto_setting_target[i].pos[ID_HOIST];
						CS_workbuf.semi_auto_selected_target.pos[ID_BOOM_H] = CS_workbuf.semi_auto_setting_target[i].pos[ID_BOOM_H];
						CS_workbuf.semi_auto_selected_target.pos[ID_SLEW] = CS_workbuf.semi_auto_setting_target[i].pos[ID_SLEW];
						//OTE�p�ϊ����W�Z�b�g
						set_selected_target_for_view();
						CS_workbuf.tg_sel_trigger_z = L_ON, CS_workbuf.tg_sel_trigger_xy = L_ON;
					}
				}
				else;
			}

			if (pCraneStat->operation_mode & OPERATION_MODE_REMOTE) {//���u�[�����[�h
				if (CS_workbuf.semi_auto_selected >= SEMI_AUTO_TG_CLR) { //�o�^�ݒ�{�^���I���ȊO�̎�
					if (chk_trig_ote_touch_pos_target()) {
						update_ote_touch_pos_tg();
						CS_workbuf.semi_auto_selected = SEMI_AUTO_TOUCH_POS;
						CS_workbuf.semi_auto_selected_target.pos[ID_HOIST] = CS_workbuf.semi_auto_setting_target[SEMI_AUTO_TOUCH_POS].pos[ID_HOIST];
						CS_workbuf.semi_auto_selected_target.pos[ID_BOOM_H] = CS_workbuf.semi_auto_setting_target[SEMI_AUTO_TOUCH_POS].pos[ID_BOOM_H];
						CS_workbuf.semi_auto_selected_target.pos[ID_SLEW] = CS_workbuf.semi_auto_setting_target[SEMI_AUTO_TOUCH_POS].pos[ID_SLEW];
						//OTE�p�ϊ����W�Z�b�g
						set_selected_target_for_view();
						CS_workbuf.tg_sel_trigger_z = L_ON, CS_workbuf.tg_sel_trigger_xy = L_ON;
					}
					else if (chk_trig_ote_touch_dist_target()) {
	
						//OTE �^�b�`�ڕW�ړ��������[�h
						CS_workbuf.ote_notch_dist_mode = pOTE_IO->rcv_msg_u.body.pb[ID_LAMP_OTE_NOTCH_MODE];
						//OTE �^�b�`�ڕW�ړ�����
						CS_workbuf.semi_auto_setting_target[SEMI_AUTO_TOUCH_DIST].pos[ID_HOIST] = pPLC_IO->pos[ID_HOIST] + (double)pOTE_IO->rcv_msg_u.body.tg_dist1[0] / 1000.0;
						CS_workbuf.semi_auto_setting_target[SEMI_AUTO_TOUCH_DIST].pos[ID_BOOM_H] = pPLC_IO->pos[ID_BOOM_H] + (double)pOTE_IO->rcv_msg_u.body.tg_dist1[1] / 1000.0;
						CS_workbuf.semi_auto_setting_target[SEMI_AUTO_TOUCH_DIST].pos[ID_SLEW] = pPLC_IO->pos[ID_SLEW] + (double)pOTE_IO->rcv_msg_u.body.tg_dist1[2] / 1000.0;

						CS_workbuf.semi_auto_selected = SEMI_AUTO_TOUCH_DIST;

						CS_workbuf.semi_auto_selected_target.pos[ID_HOIST] = CS_workbuf.semi_auto_setting_target[SEMI_AUTO_TOUCH_DIST].pos[ID_HOIST];
						CS_workbuf.semi_auto_selected_target.pos[ID_BOOM_H] = CS_workbuf.semi_auto_setting_target[SEMI_AUTO_TOUCH_DIST].pos[ID_BOOM_H];
						CS_workbuf.semi_auto_selected_target.pos[ID_SLEW] = CS_workbuf.semi_auto_setting_target[SEMI_AUTO_TOUCH_DIST].pos[ID_SLEW];
						//OTE�p�ϊ����W�Z�b�g
						set_selected_target_for_view();
						CS_workbuf.tg_sel_trigger_z = L_ON, CS_workbuf.tg_sel_trigger_xy = L_ON;
					}
					else;
				}
			}
		}
		//Z�ڕW�ʒu�␳�@�Ή����ڕW���m�莞�̂ݍX�V�\
		if (CS_workbuf.target_set_z != CS_SEMIAUTO_TG_SEL_FIXED) {//�ڕW�m��ݒ�OFF���␳�\
			if ((pUIpb[ID_PB_MH_P1]) && (mhp1_pb_last == 0)) {
				CS_workbuf.semi_auto_selected_target.pos[ID_HOIST] += AUTO_TG_ADJUST_100mm;
				CS_workbuf.tg_sel_trigger_z = L_ON;
			}
			if ((pUIpb[ID_PB_MH_P2]) && (mhp2_pb_last == 0)) {
				CS_workbuf.semi_auto_selected_target.pos[ID_HOIST] += AUTO_TG_ADJUST_1000mm;
				CS_workbuf.tg_sel_trigger_z = L_ON;
			}
			if ((pUIpb[ID_PB_MH_M1]) && (mhm1_pb_last == 0)) {
				CS_workbuf.semi_auto_selected_target.pos[ID_HOIST] -= AUTO_TG_ADJUST_100mm;
				CS_workbuf.tg_sel_trigger_z = L_ON;
			}
			if ((pUIpb[ID_PB_MH_M2]) && (mhm2_pb_last == 0)) {
				CS_workbuf.semi_auto_selected_target.pos[ID_HOIST] -= AUTO_TG_ADJUST_1000mm;
				CS_workbuf.tg_sel_trigger_z = L_ON;
			}
		}
		//XY�ڕW�ʒu�␳�@�Ή����ڕW���m�莞�̂ݍX�V�\
		if (CS_workbuf.target_set_xy != CS_SEMIAUTO_TG_SEL_FIXED) {//�ڕW�m��ݒ�OFF���␳�\
			if ((pUIpb[ID_PB_BH_P1]) && (bhp1_pb_last == 0)) {
				CS_workbuf.semi_auto_selected_target.pos[ID_BOOM_H] += AUTO_TG_ADJUST_100mm;
				CS_workbuf.tg_sel_trigger_xy = L_ON;
			}
			if ((pUIpb[ID_PB_BH_P2]) && (bhp2_pb_last == 0)) {
				CS_workbuf.semi_auto_selected_target.pos[ID_BOOM_H] += AUTO_TG_ADJUST_1000mm;
				CS_workbuf.tg_sel_trigger_xy = L_ON;
			}
			if ((pUIpb[ID_PB_BH_M1]) && (bhm1_pb_last == 0)) {
				CS_workbuf.semi_auto_selected_target.pos[ID_BOOM_H] -= AUTO_TG_ADJUST_100mm;
				CS_workbuf.tg_sel_trigger_xy = L_ON;
			}
			if ((pUIpb[ID_PB_BH_M2]) && (bhm2_pb_last == 0)) {
				CS_workbuf.semi_auto_selected_target.pos[ID_BOOM_H] -= AUTO_TG_ADJUST_1000mm;
				CS_workbuf.tg_sel_trigger_xy = L_ON;
			}

			//����͊p�x�ɕϊ�
			if ((pUIpb[ID_PB_SL_P1]) && (slp1_pb_last == 0)) {
				double rad = AUTO_TG_ADJUST_100mm / pCraneStat->R;
				CS_workbuf.semi_auto_selected_target.pos[ID_SLEW] += rad;
				CS_workbuf.tg_sel_trigger_xy = L_ON;
			}
			if ((pUIpb[ID_PB_SL_P2]) && (slp2_pb_last == 0)) {
				double rad = AUTO_TG_ADJUST_1000mm / pCraneStat->R;
				CS_workbuf.semi_auto_selected_target.pos[ID_SLEW] += rad;
				CS_workbuf.tg_sel_trigger_xy = L_ON;
			}
			if ((pUIpb[ID_PB_SL_M1]) && (slm1_pb_last == 0)) {
				double rad = AUTO_TG_ADJUST_100mm / pCraneStat->R;
				CS_workbuf.semi_auto_selected_target.pos[ID_SLEW] -= rad;
				CS_workbuf.tg_sel_trigger_xy = L_ON;
			}
			if ((pUIpb[ID_PB_SL_M2]) && (slm2_pb_last == 0)) {
				double rad = AUTO_TG_ADJUST_1000mm / pCraneStat->R;
				CS_workbuf.semi_auto_selected_target.pos[ID_SLEW] -= rad;
				CS_workbuf.tg_sel_trigger_xy = L_ON;
			}
		}

		//�����������{�^�����͂Őݒ�N���A
		if (pUIpb[ID_PB_AUTO_RESET]) {
			CS_workbuf.semi_auto_selected = SEMI_AUTO_TG_CLR;
		}


		//�������ڕW�ݒ���͌��o��ԃZ�b�g�i��ʃN���b�N���́j
		if (CS_workbuf.tg_sel_trigger_z) {
			if (CS_workbuf.target_set_z == CS_SEMIAUTO_TG_SEL_DEFAULT) CS_workbuf.target_set_z = CS_SEMIAUTO_TG_SEL_ACTIVE;
		}
		if (CS_workbuf.tg_sel_trigger_xy) {
			if (CS_workbuf.target_set_xy == CS_SEMIAUTO_TG_SEL_DEFAULT) CS_workbuf.target_set_xy = CS_SEMIAUTO_TG_SEL_ACTIVE;
		}

		//�ڕW�ʒu�m��Z�b�g
		//#### �W���u�o�^�C�x���g�Z�b�g
		job_set_event_last = CS_workbuf.job_set_event;

		//Z����
		if ((pUIpb[ID_PB_AUTO_SET_Z]) && (set_z_pb_last == 0)) {				// PB���͗����オ��
			if (CS_workbuf.target_set_z & CS_SEMIAUTO_TG_SEL_FIXED) {			//�ڕW�m��ς���̓��͂́A�m�����
				CS_workbuf.target_set_z = CS_SEMIAUTO_TG_SEL_DEFAULT;
				CS_workbuf.job_set_event = CS_JOBSET_EVENT_SEMI_SEL_CLEAR;
			}
			else {
				//�ڕW�m��ς���̓��͂́A�ڕW�I���ς���̓��͂͊m���
				if (CS_workbuf.target_set_z == CS_SEMIAUTO_TG_SEL_ACTIVE) {
					CS_workbuf.target_set_z = CS_SEMIAUTO_TG_SEL_FIXED;

					if (CS_workbuf.target_set_xy & CS_SEMIAUTO_TG_SEL_FIXED) {		//XY�����m��ςȂ��JOB�Z�b�g�\
						CS_workbuf.job_set_event = CS_JOBSET_EVENT_SEMI_STANDBY;
					}
				}
			}
		}
		//XY����
		if ((pUIpb[ID_PB_AUTO_SET_XY]) && (set_xy_pb_last == 0)) {				// PB���͗����オ��
			if (CS_workbuf.target_set_xy & CS_SEMIAUTO_TG_SEL_FIXED) {			//�ڕW�m��ς���̓��͂́A�m�����
				CS_workbuf.target_set_xy = CS_SEMIAUTO_TG_SEL_DEFAULT;
				CS_workbuf.job_set_event = CS_JOBSET_EVENT_SEMI_SEL_CLEAR;
			}
			else {
				if (CS_workbuf.target_set_xy == CS_SEMIAUTO_TG_SEL_ACTIVE) CS_workbuf.target_set_xy = CS_SEMIAUTO_TG_SEL_FIXED;

				if (CS_workbuf.target_set_z & CS_SEMIAUTO_TG_SEL_FIXED) {		//Z�����m��ςȂ��JOB�Z�b�g�\
					CS_workbuf.job_set_event = CS_JOBSET_EVENT_SEMI_STANDBY;
				}
			}
		}

		if ((semi_auto_selected_last != SEMI_AUTO_TG_CLR) && (CS_workbuf.semi_auto_selected == SEMI_AUTO_TG_CLR)) {
			CS_workbuf.tg_sel_trigger_z = L_OFF, CS_workbuf.tg_sel_trigger_xy = L_OFF;
			CS_workbuf.target_set_z = CS_SEMIAUTO_TG_SEL_DEFAULT;
			CS_workbuf.target_set_xy = CS_SEMIAUTO_TG_SEL_DEFAULT;
			CS_workbuf.job_set_event = CS_JOBSET_EVENT_SEMI_SEL_CLEAR;
		}

		//������JOB�����ŖڕW�m��N���A
		if ((job_set_event_last == CS_JOBSET_EVENT_SEMI_STANDBY) && (CS_workbuf.job_set_event != CS_JOBSET_EVENT_SEMI_STANDBY)) {
			CS_workbuf.tg_sel_trigger_z = L_OFF, CS_workbuf.tg_sel_trigger_xy = L_OFF;
			CS_workbuf.target_set_z = CS_SEMIAUTO_TG_SEL_DEFAULT;
			CS_workbuf.target_set_xy = CS_SEMIAUTO_TG_SEL_DEFAULT;
		}

		//�����R�}���h�iPICK GRND PARK�j�I��ݒ�
		if ((pUIpb[ID_PB_PARK]) && (park_pb_last == 0)) { // PB���͗����オ��
			if (CS_workbuf.command_type == COM_TYPE_PARK)
				CS_workbuf.command_type = COM_TYPE_NON;
			else
				CS_workbuf.command_type = COM_TYPE_PARK;
		}
		park_pb_last = pUIpb[ID_PB_PARK];

		if ((pUIpb[ID_PB_PICK]) && (pick_pb_last == 0)) { // PB���͗����オ��
			if (CS_workbuf.command_type == COM_TYPE_PICK)
				CS_workbuf.command_type = COM_TYPE_NON;
			else
				CS_workbuf.command_type = COM_TYPE_PICK;
		}
		pick_pb_last = pUIpb[ID_PB_PICK];

		if ((pUIpb[ID_PB_GRND]) && (grnd_pb_last == 0)) { // PB���͗����オ��K
			if (CS_workbuf.command_type == COM_TYPE_GRND)
				CS_workbuf.command_type = COM_TYPE_NON;
			else
				CS_workbuf.command_type = COM_TYPE_GRND;
		}
		grnd_pb_last = pUIpb[ID_PB_GRND];

		//�����N��PB
		if (pUIpb[ID_PB_AUTO_START])CS_workbuf.ui_pb[ID_PB_AUTO_START]++;
		else CS_workbuf.ui_pb[ID_PB_AUTO_START] = 0;
		//JOB�N������
		//JOB STATUS��STAT_TRIGED�̏�Ԃ�AGENT����`�F�b�N����POLICY�Ń��V�s���Z�b�g���Ď��s�̗���
		if (CS_workbuf.ui_pb[ID_PB_AUTO_START] == AUTO_START_CHECK_TIME) {
			//���������X�^���o�C���
			if (pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status == STAT_STANDBY) {
				pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_TRIGED;
			}
			//JOB���X�^���o�C���
			else if (pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot].status == STAT_STANDBY) {
				pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot].status = STAT_TRIGED;
			}
			else;
		}
	}
	else {
		//�������[�h�łȂ��Ƃ��͔������ݒ�N���A
		CS_workbuf.tg_sel_trigger_z = L_OFF;
		CS_workbuf.tg_sel_trigger_xy = L_OFF;
		CS_workbuf.target_set_z = CS_SEMIAUTO_TG_SEL_DEFAULT;
		CS_workbuf.target_set_xy = CS_SEMIAUTO_TG_SEL_DEFAULT;
		CS_workbuf.semi_auto_selected = SEMI_AUTO_TG_CLR;

		CS_workbuf.semi_auto_selected_target.pos[ID_HOIST] = pPLC_IO->status.pos[ID_HOIST];
		CS_workbuf.semi_auto_selected_target.pos[ID_BOOM_H] = pPLC_IO->status.pos[ID_BOOM_H];
		CS_workbuf.semi_auto_selected_target.pos[ID_SLEW] = pPLC_IO->status.pos[ID_SLEW];

		//OTE�p�ϊ����W�Z�b�g
		set_selected_target_for_view();


		if ((semi_auto_selected_last != SEMI_AUTO_TG_CLR) && (CS_workbuf.semi_auto_selected == SEMI_AUTO_TG_CLR)) {
			CS_workbuf.tg_sel_trigger_z = L_OFF, CS_workbuf.tg_sel_trigger_xy = L_OFF;
			CS_workbuf.target_set_z = CS_SEMIAUTO_TG_SEL_DEFAULT;
			CS_workbuf.target_set_xy = CS_SEMIAUTO_TG_SEL_DEFAULT;
			CS_workbuf.job_set_event = CS_JOBSET_EVENT_SEMI_SEL_CLEAR;
		}
	}


	//�O��l�ێ�
	set_z_pb_last = pUIpb[ID_PB_AUTO_SET_Z];
	set_xy_pb_last = pUIpb[ID_PB_AUTO_SET_XY];
	mhp1_pb_last = pUIpb[ID_PB_MH_P1];
	mhp2_pb_last = pUIpb[ID_PB_MH_P2];
	mhm1_pb_last = pUIpb[ID_PB_MH_M1];
	mhm2_pb_last = pUIpb[ID_PB_MH_M2];
	bhp1_pb_last = pUIpb[ID_PB_BH_P1];
	bhp2_pb_last = pUIpb[ID_PB_BH_P2];
	bhm1_pb_last = pUIpb[ID_PB_BH_M1];
	bhm2_pb_last = pUIpb[ID_PB_BH_M2];
	slp1_pb_last = pUIpb[ID_PB_SL_P1];
	slp2_pb_last = pUIpb[ID_PB_SL_P2];
	slm1_pb_last = pUIpb[ID_PB_SL_M1];
	slm2_pb_last = pUIpb[ID_PB_SL_M2];
	semi_auto_selected_last = CS_workbuf.semi_auto_selected;
#endif
	return 0;
}

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

UINT16 pb_ope_last[N_OTE_PNL_PB];

int CClientService::ote_handle_proc() {         //����[������
	if (can_ote_activate()) {

		//�O���b�v�X�C�b�`
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

		//����X�C�b�`
		{

			//��~�@PBL
			CS_workbuf.ote_pb_lamp[ID_OTE_PB_TEISHI].com = CS_workbuf.ote_pb_lamp[ID_OTE_PB_KIDOU].com = OTE_LAMP_COM_ON;
			if (pOTE_IO->ote_umsg_in.body.pb_ope[ID_OTE_PB_TEISHI]) CS_workbuf.ote_pb_lamp[ID_OTE_PB_TEISHI].color = OTE0_GREEN;
			else													CS_workbuf.ote_pb_lamp[ID_OTE_PB_TEISHI].color = OTE0_GLAY;
			
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

			//�������@PBL
			for (int i = ID_OTE_CHK_S1; i <= ID_OTE_CHK_N3; i++) {
				if (pOTE_IO->ote_umsg_in.body.pb_ope[i]) {
					CS_workbuf.semi_auto_selected = i;
					break;
				}
			}

			for (int i = ID_OTE_CHK_S1; i <= ID_OTE_CHK_N3; i++) {
				if (i == CS_workbuf.semi_auto_selected) {
					if (pOTE_IO->ote_umsg_in.body.pb_ope[i] > SEMI_AUTO_TG_RESET_TIME) {
						CS_workbuf.ote_pb_lamp[i].com = OTE_LAMP_COM_ON;
						CS_workbuf.ote_pb_lamp[i].color = OTE0_MAZENDA;
					}
					else if (pOTE_IO->ote_umsg_in.body.pb_ope[i] > OTE0_PB_OFF_DELAY_COUNT) {
						CS_workbuf.ote_pb_lamp[i].com = OTE_LAMP_COM_FLICK;
						CS_workbuf.ote_pb_lamp[i].color = OTE0_ORANGE;
					}
					else {
						CS_workbuf.ote_pb_lamp[i].com = OTE_LAMP_COM_ON;
						CS_workbuf.ote_pb_lamp[i].color = OTE0_BLUE;
					}
				}
				else CS_workbuf.ote_pb_lamp[i].com = OTE_LAMP_COM_OFF;
			}
		}

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

		//����PB�O��l�ێ�
		for (int i = ID_OTE_PB_TEISHI; i < ID_OTE_CHK_N3; i++) {
			pb_ope_last[i] = pOTE_IO->ote_umsg_in.body.pb_ope[i];
		}

		//�m�b�`�����v
		for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
			if (i == ID_OTE_GRIP_SWITCH)continue;//	�O���b�v�X�C�b�`�͑ΏۊO

			if (CS_workbuf.notch_pos[ID_OTE_NOTCH_POS_HOLD][i] != pOTE_IO->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][i]) {
				for (int j = 0; j < 9; j++) {
					if (j == pOTE_IO->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][i]) {
						CS_workbuf.ote_notch_lamp[i*10+j].com = OTE_LAMP_COM_ON;
						
						if(j== ID_OTE_0NOTCH_POS)	CS_workbuf.ote_notch_lamp[i * 10 + j].color = OTE0_GREEN;
						else						CS_workbuf.ote_notch_lamp[i * 10 + j].color = OTE0_RED;
					}
					else {
						CS_workbuf.ote_notch_lamp[i * 10 + j].com = OTE_LAMP_COM_OFF;
					}
				}

			}
			//�m�b�`�ʒu�O��l�ێ�
			CS_workbuf.notch_pos[ID_OTE_NOTCH_POS_HOLD][i] = pOTE_IO->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][i];
		}


	}
	else {//�����vOFF
		//���색���v
		for (int i = ID_OTE_PB_TEISHI; i < ID_OTE_CHK_N3; i++) {
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

void CClientService::main_proc() {

	//����[������

	ote_handle_proc();
	
	//�������W���u�C�x���g����
	//�������o�^����

	if (CS_workbuf.auto_mode == L_OFF) {
		//�������[�hOFF�ŃW���u�z�[���h���N���A
		pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job = pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job = 0;
		CS_workbuf.semi_auto_selected = 0;
	}

	//�C�x���g����
	switch (CS_workbuf.job_set_event) {
	case CS_JOBSET_EVENT_SEMI_SEL_CLEAR: {
		int i_job = pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot;
		LPST_JOB_SET p_job = &pJob_IO->job_list[ID_JOBTYPE_SEMI].job[i_job];
		//		if (p_job->status == STAT_STANDBY) p_job->status = STAT_REQ_WAIT;
		p_job->status = STAT_REQ_WAIT;
		pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job--;
		if (pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job < 0) pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job = 0;

		//�C�x���g�N���A
		CS_workbuf.job_set_event = CS_JOBSET_EVENT_CLEAR;
	}break;
	case CS_JOBSET_EVENT_SEMI_STANDBY: {										//������STANBY��ԓ���
		//*�@�������́A����JOB�̎��O�o�^�Ȃ�
				//���݂�JOB�o�b�t�@
		int i_job = pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot;
		LPST_JOB_SET p_job = &pJob_IO->job_list[ID_JOBTYPE_SEMI].job[i_job];

		if (p_job->status != STAT_STANDBY) {//���݂�JOB �N���҂��łȂ�
			//���̃o�b�t�@��
			i_job = ++pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot;
			p_job = &pJob_IO->job_list[ID_JOBTYPE_SEMI].job[i_job];
			
			if (i_job >= JOB_HOLD_MAX) {
				pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot = i_job = 0;
				p_job->status = STAT_ABOTED;
			}
			else {
				p_job->status = STAT_STANDBY;
			}

			//JOB LIST�X�V
			pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job++;	//���Z�́APOLICY�Ăяo����update_job_status()�̊����񍐂Ŏ��s
		}
		else {//���݂�JOB���ɋN���҂����X�e�[�^�X�z�[���h
			//���o�b�t�@���X�V
			//i_job = ++CS_workbuf.job_list[ID_JOBTYPE_SEMI].i_job_hot;
			//p_job = &CS_workbuf.job_list[ID_JOBTYPE_SEMI].job[i_job];
			p_job->status = STAT_STANDBY;
		}

		 //JOB SET���e�Z�b�g
		p_job->list_id = ID_JOBTYPE_SEMI;
		p_job->id = i_job;
		set_semi_recipe(p_job);							//JOB���V�s�Z�b�g
	
		//�C�x���g�N���A
		CS_workbuf.job_set_event = CS_JOBSET_EVENT_CLEAR;
	}break;
	case CS_JOBSET_EVENT_JOB_STANDBY: {							//JOB�@STANDBY��ԓ��� JOB��CLIENT�����JOB��M����STANDBY
		//*�@JOB�́A����JOB�̎��O�o�^�\ 
		if (pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job < JOB_REGIST_MAX - 1) {
			//�o�b�t�@�ւ̒ǉ��ꏊ��]��
			int i_job = pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot + pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job;
			if (i_job > JOB_REGIST_MAX) i_job = i_job % JOB_REGIST_MAX;
			LPST_JOB_SET p_job = &pJob_IO->job_list[ID_JOBTYPE_JOB].job[i_job];


			//JOB LIST�X�V
			pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job++;	//���Z�́APOLICY�Ăяo����update_job_status()�̊����񍐂Ŏ��s

			//JOB SET���e�Z�b�g
			p_job->status = STAT_STANDBY;
			p_job->list_id = ID_JOBTYPE_SEMI;
			p_job->id = i_job;
			set_job_recipe(p_job);							//JOB���V�s�Z�b�g

			CS_workbuf.job_set_event = CS_JOBSET_EVENT_CLEAR;
		}
		else {//�o�^�������z���́A�����B�x��\���K�v
			CS_workbuf.job_set_event = CS_JOBSET_EVENT_JOB_OVERFLOW;
		}
	}break;
	case CS_JOBSET_EVENT_SEMI_TRIG: {							//�������N��PB�g���K����
		int i_job = pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot;
		LPST_JOB_SET p_job = &pJob_IO->job_list[ID_JOBTYPE_SEMI].job[i_job];

		if (p_job->status == STAT_STANDBY) {//JOB�N���҂����
			p_job->status = STAT_TRIGED;	//JOB�g���K��ԂɃX�e�[�^�X�X�V(STANDBY�Ń��V�s�ݒ�ρ@AGENT���s�҂�
		}
	}break;
	case CS_JOBSET_EVENT_JOB_TRIG: {
		int i_job = pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot;
		LPST_JOB_SET p_job = &pJob_IO->job_list[ID_JOBTYPE_SEMI].job[i_job];

		if (p_job->status == STAT_STANDBY) {//JOB�N���҂����
			p_job->status = STAT_TRIGED;	//JOB�g���K��ԂɃX�e�[�^�X�X�V(STANDBY�Ń��V�s�ݒ�ρ@AGENT���s�҂�
		}
	}break;
	default:break;
	}

	//���݃A�N�e�B�u��JOB
	if ((pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job > 0) &&
		(pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot].status != STAT_ACTIVE)){
		CS_workbuf.p_active_job = &pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot];
	}
	else if ((pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job > 0) &&
			 (pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status != STAT_ACTIVE)) {
		CS_workbuf.p_active_job = &pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot];
	}
	else
		CS_workbuf.p_active_job = NULL;


	return;
}

//�W���u�̃��V�s�Z�b�g
LPST_JOB_SET CClientService::set_job_recipe(LPST_JOB_SET pjob_set) {
	LPST_JOB_SET pjob = NULL;
	pjob_set->type = ID_JOBTYPE_JOB;
	return pjob;
}

//�������̃��V�s�Z�b�g
LPST_JOB_SET CClientService::set_semi_recipe(LPST_JOB_SET pjob_set) {
	//JOB�̃R�}���h���@�������͂P
	pjob_set->n_com = 1;
	pjob_set->type = ID_JOBTYPE_SEMI;
	//�ڕW�ʒu�Z�b�g
	pjob_set->recipe[0].target.pos[ID_HOIST] = CS_workbuf.semi_auto_selected_target.pos[ID_HOIST];
	pjob_set->recipe[0].target.pos[ID_BOOM_H] = CS_workbuf.semi_auto_selected_target.pos[ID_BOOM_H];
	pjob_set->recipe[0].target.pos[ID_SLEW] = CS_workbuf.semi_auto_selected_target.pos[ID_SLEW];

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

/*### �����֘A�����v�\���@###*/
#if 0

	//�N�������v
	//�z�b�g�ȃW���u���A�N�e�B�u���_��
	if ((pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot].status & STAT_ACTIVE)||
		(pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status & STAT_ACTIVE)){
		CS_workbuf.ui_lamp[ID_PB_AUTO_START] = L_ON;
	}
	//�z�b�g�ȃW���u���X�^���o�C���_��
	else if((pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status & STAT_STANDBY)||
		(pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot].status & STAT_STANDBY)) {
		if (inf.total_act % LAMP_FLICKER_BASE_COUNT > LAMP_FLICKER_CHANGE_COUNT)
			CS_workbuf.ui_lamp[ID_PB_AUTO_START] = L_ON;
		else
			CS_workbuf.ui_lamp[ID_PB_AUTO_START] = L_OFF;
	}
	//�z�b�g�ȃW���u���T�X�y���h���_��
	else if ((pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot].status & STAT_SUSPENDED) ||
		(pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status & STAT_SUSPENDED)) {
		if (inf.total_act % LAMP_FLICKER_BASE_COUNT > LAMP_FLICKER_CHANGE_COUNT)
			CS_workbuf.ui_lamp[ID_PB_AUTO_START] = L_ON;
		else
			CS_workbuf.ui_lamp[ID_PB_AUTO_START] = L_OFF;
	}
	else {
		CS_workbuf.ui_lamp[ID_PB_AUTO_START] = L_OFF;
	}

	//�ڕW�ݒ胉���v�@�ڕW�ݒ�m�胉���v

	if (CS_workbuf.target_set_z == CS_SEMIAUTO_TG_SEL_FIXED) {
		CS_workbuf.ui_lamp[ID_PB_AUTO_SET_Z] = L_ON;
		CS_workbuf.ui_lamp[ID_PB_AUTO_SET_Z_FIXED] = L_ON;
	}
	else if (CS_workbuf.target_set_z == CS_SEMIAUTO_TG_SEL_ACTIVE) {
		if (inf.total_act % LAMP_FLICKER_BASE_COUNT > LAMP_FLICKER_CHANGE_COUNT)
			CS_workbuf.ui_lamp[ID_PB_AUTO_SET_Z] = L_ON;
		else
			CS_workbuf.ui_lamp[ID_PB_AUTO_SET_Z] = L_OFF;

		CS_workbuf.ui_lamp[ID_PB_AUTO_SET_Z_FIXED] = L_OFF;
	}
	else {
		CS_workbuf.ui_lamp[ID_PB_AUTO_SET_Z] = L_OFF;
		CS_workbuf.ui_lamp[ID_PB_AUTO_SET_Z_FIXED] = L_OFF;
	}

	if (CS_workbuf.target_set_xy == CS_SEMIAUTO_TG_SEL_FIXED) {
		CS_workbuf.ui_lamp[ID_PB_AUTO_SET_XY] = L_ON;
		CS_workbuf.ui_lamp[ID_PB_AUTO_SET_XY_FIXED] = L_ON;
	}
	else if (CS_workbuf.target_set_xy == CS_SEMIAUTO_TG_SEL_ACTIVE) {
		if (inf.total_act % LAMP_FLICKER_BASE_COUNT > LAMP_FLICKER_CHANGE_COUNT)
			CS_workbuf.ui_lamp[ID_PB_AUTO_SET_XY] = L_ON;
		else
			CS_workbuf.ui_lamp[ID_PB_AUTO_SET_XY] = L_OFF;
		CS_workbuf.ui_lamp[ID_PB_AUTO_SET_XY_FIXED] = L_OFF;
	}
	else {
		CS_workbuf.ui_lamp[ID_PB_AUTO_SET_XY] = L_OFF;
		CS_workbuf.ui_lamp[ID_PB_AUTO_SET_XY_FIXED] = L_OFF;
	}
	//�����R�}���h�����v
	if (CS_workbuf.command_type == COM_TYPE_PICK) {
		CS_workbuf.ui_lamp[ID_PB_PICK] = L_ON;
		CS_workbuf.ui_lamp[ID_PB_GRND] = L_OFF;
		CS_workbuf.ui_lamp[ID_PB_PARK] = L_OFF;
	}
	else if (CS_workbuf.command_type == COM_TYPE_GRND) {
		CS_workbuf.ui_lamp[ID_PB_PICK] = L_OFF;
		CS_workbuf.ui_lamp[ID_PB_GRND] = L_ON;
		CS_workbuf.ui_lamp[ID_PB_PARK] = L_OFF;
	}
	else if (CS_workbuf.command_type == COM_TYPE_PARK) {
		CS_workbuf.ui_lamp[ID_PB_PICK] = L_OFF;
		CS_workbuf.ui_lamp[ID_PB_GRND] = L_OFF;
		CS_workbuf.ui_lamp[ID_PB_PARK] = L_ON;
	}
	else {
		CS_workbuf.ui_lamp[ID_PB_PICK] = L_OFF;
		CS_workbuf.ui_lamp[ID_PB_GRND] = L_OFF;
		CS_workbuf.ui_lamp[ID_PB_PARK] = L_OFF;
	}

	//�����������v
	for (int i = 0;i < SEMI_AUTO_TG_CLR;i++) {
		if (i == CS_workbuf.semi_auto_selected) {	//�������I�𒆂�PB
			if(CS_workbuf.semiauto_pb[i]){
				if ((CS_workbuf.semiauto_pb[i] >= SEMI_AUTO_TG_RESET_TIME)||(CS_workbuf.semiauto_pb[i]< SEMI_AUTO_TG_SELECT_TIME * 4))
					CS_workbuf.semiauto_lamp[i] = L_ON;
				else if ((CS_workbuf.semiauto_pb[i] % (SEMI_AUTO_TG_SELECT_TIME*2)) > SEMI_AUTO_TG_SELECT_TIME)
					CS_workbuf.semiauto_lamp[i] = L_ON;
				else
					CS_workbuf.semiauto_lamp[i] = L_OFF;
			}
			else {//�ڕW�ʒu�m��œ_��
				CS_workbuf.semiauto_lamp[i] = L_ON;
			}
		}
		else {	//�������I�𒆂łȂ�PB
			CS_workbuf.semiauto_lamp[i] = L_OFF;
		}

		CS_workbuf.ui_lamp[ID_PB_SEMI_AUTO_S1 + i] = CS_workbuf.semiauto_lamp[i];//�\�������v�o�b�t�@�փR�s�[
	}


	//�u���[�L���
	for (int i = 0;i < MOTION_ID_MAX;i++) {
		CS_workbuf.ui_lamp[ID_LAMP_HST_BRK + i] = pPLC_IO->status.brk[i];
	}

	//�劲�����v�@���u���[�h
	CS_workbuf.ui_lamp[ID_PB_CRANE_MODE]		= pPLC_IO->ui.LAMP[ID_PB_CRANE_MODE];
	CS_workbuf.ui_lamp[ID_PB_REMOTE_MODE]		= pPLC_IO->ui.LAMP[ID_PB_REMOTE_MODE];
	CS_workbuf.ui_lamp[ID_PB_CTRL_SOURCE_ON]	= pPLC_IO->ui.LAMP[ID_PB_CTRL_SOURCE_ON];
	CS_workbuf.ui_lamp[ID_PB_CTRL_SOURCE_OFF]	= pPLC_IO->ui.LAMP[ID_PB_CTRL_SOURCE_OFF];
	CS_workbuf.ui_lamp[ID_PB_CTRL_SOURCE2_ON]	= pPLC_IO->ui.LAMP[ID_PB_CTRL_SOURCE2_ON];
	CS_workbuf.ui_lamp[ID_PB_CTRL_SOURCE2_OFF]	= pPLC_IO->ui.LAMP[ID_PB_CTRL_SOURCE2_OFF];

	//����[���\���p���Z�b�g
	CS_workbuf.ui_lamp[ID_LAMP_OTE_NOTCH_MODE] = CS_workbuf.ote_notch_dist_mode;		//�ړ��ڕW�ݒ胂�[�h
	set_hp_pos_for_view();																//�ݓ_�ʒu���W�Z�b�g



#endif

	//���L�������o��
	memcpy_s(pCSinf, sizeof(ST_CS_INFO), &CS_workbuf, sizeof(ST_CS_INFO));
	//�^�X�N�p�l���\���o��
	{
		wostrs << L" AS=" << CS_workbuf.antisway_mode << L",AUTO=" << CS_workbuf.auto_mode;

		int status;
		if (CS_workbuf.p_active_job != NULL) {
			status = CS_workbuf.p_active_job->status;
			wostrs << L",JOB TYPE=" << CS_workbuf.p_active_job->list_id << L", id=" << CS_workbuf.p_active_job->id;
		}
		else status = STAT_NA;

		if ((pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job <= 0) && (pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job <= 0)) {
			wostrs << L" >JOB: 0 hold";
		}
		else if ((CS_workbuf.p_active_job != NULL) &&
			(CS_workbuf.p_active_job->list_id == ID_JOBTYPE_JOB) &&
			(pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job > 0)) {
			wostrs << L" >JOB: " << pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job << L" iHOT-> ";
			if (status & STAT_STANDBY)		wostrs << L"STANDBY";
			else if (status & STAT_ACTIVE)	wostrs << L"ACTIVE";
			else if (status & STAT_TRIGED)	wostrs << L"TRIGED";
			else if (status & STAT_SUSPENDED)	wostrs << L"SUSPEND";
			else if (status & STAT_REQ_WAIT)	wostrs << L"WAIT REQ";
			else wostrs << L"OUT OF SERV";

		}
		else if ((CS_workbuf.p_active_job != NULL) &&
			(CS_workbuf.p_active_job->list_id == ID_JOBTYPE_SEMI) &&
			(pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job > 0)) {
			wostrs << L" >SEMIAUTO: " << pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job << L" iHOT-> ";
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
//### AGENT����̎��s�҂�JOB�₢���킹����
LPST_JOB_SET CClientService::get_next_job() {
	
	//HOT�@JOB�̃X�e�[�^�X���g���K��Ԃ̂��̂�ԐM
	
	//�������`�F�b�N
	int job_status = pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status;

	switch(job_status){
	case STAT_TRIGED:
		//���V�s���Z�b�g���ă|�C���^��Ԃ�
		 return &(pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot]);
	
	case STAT_ACTIVE:
	case STAT_SUSPENDED:
	case STAT_STANDBY:
	case STAT_ABOTED:
	case STAT_END:
	case STAT_REQ_WAIT:
		//���s�҂�(TRIGGER�j��ԈȊO�̓X���[
		break;
	}

	//JOB�`�F�b�N
	job_status = pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot].status;

	switch (job_status) {
	case STAT_TRIGED:
		//���V�s���Z�b�g���ă|�C���^��Ԃ�
		return &(pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot]);

	case STAT_ACTIVE:
	case STAT_SUSPENDED:
	case STAT_STANDBY:
	case STAT_ABOTED:
	case STAT_END:
	case STAT_REQ_WAIT:
		//���s�҂�(TRIGGER�j��ԈȊO�̓X���[
		break;
	}
	//���s�҂�������΃k�����^�[��
	return NULL;
}

//### POLICY�����JOB Status�X�V�˗�
int CClientService::update_job_status(LPST_JOB_SET pjobset, int fb_code) {

	if (pjobset->list_id == ID_JOBTYPE_JOB) {
		switch (fb_code) {
		case STAT_END: {
			pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job--;
			pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_END;

			job_report2client(pjobset, STAT_END);

			return STAT_ACK;
			break;
		}
		case STAT_ABNORMAL_END: {
			pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job--;
			pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_END;

			job_report2client(pjobset, STAT_END);

			return STAT_ACK;
		}break;
		case STAT_ACTIVE :break;
		case STAT_ABOTED:break;
		default:break;
		}
	}
	else if (pjobset->list_id == ID_JOBTYPE_SEMI) {
		switch (fb_code) {
		case STAT_END: {
			//���튮����JOB�̃z�[���h����0�N���A
			pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job = 0;
			pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_END;

	//		CS_workbuf.semi_auto_selected = SEMI_AUTO_TG_CLR;		//�������ݒ�N���A
			CS_workbuf.target_set_z = CS_SEMIAUTO_TG_SEL_CLEAR;	//�ڕW�m�����
			CS_workbuf.target_set_xy = CS_SEMIAUTO_TG_SEL_CLEAR;	//�ڕW�m�����

			job_report2client(pjobset, STAT_END);

			return STAT_ACK;
		}break;
		case STAT_ABNORMAL_END: {
			//�ُ튮����JOB�̃z�[���h����0�N���A
			pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job = 0;
			pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_ABNORMAL_END;

	//		CS_workbuf.semi_auto_selected = SEMI_AUTO_TG_CLR;		//�������ݒ�N���A
			CS_workbuf.target_set_z = CS_SEMIAUTO_TG_SEL_CLEAR;	//�ڕW�m�����
			CS_workbuf.target_set_xy = CS_SEMIAUTO_TG_SEL_CLEAR;	//�ڕW�m�����

			job_report2client(pjobset, STAT_ABNORMAL_END);

			return STAT_ACK;
		}break;
		case STAT_ACTIVE: {
			//�W���u���s�񍐂́A�X�e�[�^�X�̍X�V�̂�
			pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_ACTIVE;

			job_report2client(pjobset, STAT_ACTIVE);

			return STAT_ACK;
		}break;
		case STAT_ABOTED: {
			//�W���u�L�����Z�����i����OFF���jJOB�̃z�[���h����0�N���A
			pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job = 0;
			pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_ABOTED;

			return STAT_ACK;
		}break;
		case STAT_SUSPENDED: {
			//���f���i�蓮����j�̏ꍇ�́A�X�e�[�^�X���X�^���o�C��Ԃɖ߂�
			pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_STANDBY;

			return STAT_ACK;
		}break;
		default:return STAT_LOGICAL_ERROR;
		}
	}

	return STAT_LOGICAL_ERROR;
}

int CClientService::job_report2client(LPST_JOB_SET pjobset, int fb_code) {       //Job�̎��s�󋵕�

	//��������@CLIENT�ʐM�A���T�o�b�N,���O�L�^��
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
		}break;
		case STAT_ABNORMAL_END: {
		}break;
		case STAT_ACTIVE: {
		}break;
		case STAT_ABOTED: {
		}break;
		case STAT_SUSPENDED: {
		}break;
		default:return STAT_LOGICAL_ERROR;
		}
	}

	return STAT_SUCCEED;
	return STAT_NA;
}


/****************************************************************************/
/*   �������֘A����															*/
/****************************************************************************/


/****************************************************************************/
/*   JOB�֘A																*/
/****************************************************************************/

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
