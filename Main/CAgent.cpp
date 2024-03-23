#include "CAgent.h"
#include "CPolicy.h"
#include "CEnvironment.h"
#include "CClientService.h"
#include "CHelper.h"

//-���L�������I�u�W�F�N�g�|�C���^:
extern CSharedMem* pCraneStatusObj;
extern CSharedMem* pSwayStatusObj;
extern CSharedMem* pPLCioObj;
extern CSharedMem* pSwayIO_Obj;
extern CSharedMem* pRemoteIO_Obj;
extern CSharedMem* pCSInfObj;
extern CSharedMem* pPolicyInfObj;
extern CSharedMem* pAgentInfObj;
extern CSharedMem* pJobIO_Obj;
extern CSharedMem* pOTEioObj;

extern vector<void*>	VectpCTaskObj;	//�^�X�N�I�u�W�F�N�g�̃|�C���^
extern ST_iTask g_itask;

static CClientService* pCS;
static CPolicy* pPolicy;
static CEnvironment* pEnv;

/****************************************************************************/
/*   �R���X�g���N�^�@�f�X�g���N�^                                           */
/****************************************************************************/
CAgent::CAgent() {
	pPolicyInf = NULL;
	pPLC_IO = NULL;
	pCraneStat = NULL;
	pAgentInf = NULL;
	for(int i=0;i<8;i++)dbg_mont[i] = 0;
}

CAgent::~CAgent() {

}

/****************************************************************************/
/*   �^�X�N����������                                                       */
/* �@���C���X���b�h�ŃC���X�^���X��������ɌĂт܂��B                       */
/****************************************************************************/

void CAgent::init_task(void* pobj) {

	//���L�������\���̂̃|�C���^�Z�b�g
	pPolicyInf = (LPST_POLICY_INFO)(pPolicyInfObj->get_pMap());
	pCSInf = (LPST_CS_INFO)(pCSInfObj->get_pMap());
	pAgentInf = (LPST_AGENT_INFO)(pAgentInfObj->get_pMap());
	pPLC_IO = (LPST_PLC_IO)(pPLCioObj->get_pMap());
	pCraneStat = (LPST_CRANE_STATUS)(pCraneStatusObj->get_pMap());
	pSway_IO = (LPST_SWAY_IO)(pSwayIO_Obj->get_pMap());
	pJob_IO = (LPST_JOB_IO)(pJobIO_Obj->get_pMap());
	pOTE_IO = (LPST_OTE_IO)(pOTEioObj->get_pMap());

	pPolicy = (CPolicy*)VectpCTaskObj[g_itask.policy];
	pEnv = (CEnvironment*)VectpCTaskObj[g_itask.environment];
	pCS = (CClientService*)VectpCTaskObj[g_itask.client];

	pCom_as = &(pAgentInf->st_as_comset);
	pCom_hot = NULL;
	pjob_active = NULL;

	AgentInf_workbuf.auto_on_going = AUTO_TYPE_MANUAL;
	pjob_active = NULL;

	//���X�e�[�^�X�N���A
	for (int i = 0;i < MOTION_ID_MAX;i++) {
		AgentInf_workbuf.axis_status[i] = 0;
		AgentInf_workbuf.as_count[i] = 0;
	}
	AgentInf_workbuf.command_count = 0;

	set_panel_tip_txt();

	AgentInf_workbuf.st_as_comset.com_code.i_list = ID_JOBTYPE_ANTISWAY;//�R�}���h�Z�b�g�̃^�C�v��U��~�߂ɐݒ�

	inf.is_init_complete = true;
	return;
};

/****************************************************************************/
/*   �^�X�N���������                                                       */
/* �@�^�X�N�X���b�h�Ŗ��������s�����֐�			�@                      */
/****************************************************************************/
void CAgent::routine_work(void* param) {
	input();
	main_proc();
	output();
};

/****************************************************************************/
/*  ����������菇1 �M�����́@�O���M�����͉��H�����@���[�h�Ǘ�				*/
/****************************************************************************/
void CAgent::input() {
	return;
};

/****************************************************************************/
/*   ����������菇2�@���C������											*/
/****************************************************************************/

static int auto_on_going_last = AUTO_TYPE_MANUAL;
static INT32 notch0_last;
static LPST_COMMAND_SET pCom_hot_last;

void CAgent::main_proc() {
	//###################################################	
	//�e�������ݒ�axis_status�iFB0,AUTO_ENABLE...)
	// AgentInf_workbuf.axis_status
	// 	�U��~�ߎ������s�ݒ�
	// 	���x0��Ԑݒ�
	//  PC�w�߉ݒ�
	//  �������s�ݒ�
	// 
	// AgentInf_workbuf.pc_ctrl_mode
	//  �����I���Z�b�g 
	//###################################################
	{
		//�U��~�ߎ������s�ݒ�
		AgentInf_workbuf.axis_status[ID_BOOM_H] |= AG_AXIS_STAT_ANTISWAY_ENABLE;
		AgentInf_workbuf.axis_status[ID_SLEW] |= AG_AXIS_STAT_ANTISWAY_ENABLE;

		//���x0��Ԑݒ�
		if (pEnv->is_speed_0(ID_HOIST))	AgentInf_workbuf.axis_status[ID_HOIST] |= AG_AXIS_STAT_FB0;
		else							AgentInf_workbuf.axis_status[ID_HOIST] &= ~AG_AXIS_STAT_FB0;
		if (pEnv->is_speed_0(ID_BOOM_H))AgentInf_workbuf.axis_status[ID_BOOM_H] |= AG_AXIS_STAT_FB0;
		else							AgentInf_workbuf.axis_status[ID_BOOM_H] &= ~AG_AXIS_STAT_FB0;
		if (pEnv->is_speed_0(ID_SLEW))	AgentInf_workbuf.axis_status[ID_SLEW] |= AG_AXIS_STAT_FB0;
		else							AgentInf_workbuf.axis_status[ID_SLEW] &= ~AG_AXIS_STAT_FB0;
		if (pEnv->is_speed_0(ID_AHOIST))AgentInf_workbuf.axis_status[ID_AHOIST] |= AG_AXIS_STAT_FB0;
		else							AgentInf_workbuf.axis_status[ID_AHOIST] &= ~AG_AXIS_STAT_FB0;

		//PC�w�߉ݒ�
		for (int i = 0; i < MOTION_ID_MAX; i++) {
			AgentInf_workbuf.axis_status[i] |= AG_AXIS_STAT_PC_ENABLE;
		}

		//�������s�ݒ�
		AgentInf_workbuf.axis_status[ID_HOIST]	|= AG_AXIS_STAT_AUTO_ENABLE;
		AgentInf_workbuf.axis_status[ID_BOOM_H] |= AG_AXIS_STAT_AUTO_ENABLE;
		AgentInf_workbuf.axis_status[ID_SLEW]	|= AG_AXIS_STAT_AUTO_ENABLE;
		AgentInf_workbuf.axis_status[ID_AHOIST] |= AG_AXIS_STAT_AUTO_ENABLE;

		//�����I���Z�b�g AgentInf_workbuf.pc_ctrl_mode

		//OTE�̓��͏�Ԃ���Z�b�g
		if (pCSInf->auto_status[ID_HOIST])	AgentInf_workbuf.pc_ctrl_mode |= BIT_SEL_HST;
		else								AgentInf_workbuf.pc_ctrl_mode &= ~BIT_SEL_HST;

		if (pCSInf->auto_status[ID_GANTRY]) AgentInf_workbuf.pc_ctrl_mode |= BIT_SEL_GNT;
		else								AgentInf_workbuf.pc_ctrl_mode &= ~BIT_SEL_GNT;

		if (pCSInf->auto_status[ID_BOOM_H]) AgentInf_workbuf.pc_ctrl_mode |= BIT_SEL_BH;
		else								AgentInf_workbuf.pc_ctrl_mode &= ~BIT_SEL_BH;

		if (pCSInf->auto_status[ID_SLEW])	AgentInf_workbuf.pc_ctrl_mode |= BIT_SEL_SLW;
		else								AgentInf_workbuf.pc_ctrl_mode &= ~BIT_SEL_SLW;

		if (pCSInf->auto_status[ID_AHOIST])	AgentInf_workbuf.pc_ctrl_mode |= BIT_SEL_AH;
		else								AgentInf_workbuf.pc_ctrl_mode &= ~BIT_SEL_AH;

	}

	//###################################################
	//# JOB�R�}���h�ݒ�
	//# �R�}���h�X�e�[�^�X�X�V
	//###################################################

	{
		if (pCSInf->auto_mode == L_ON) {	//�������[�h
			if (pjob_active == NULL) {		//�W���u���s���łȂ�
	
				pjob_active = pCS->get_next_job();								//CS�ɃW���u�₢���킹
				if (pjob_active != NULL) {										//CS�ɃW���u�L
					pCom_hot = pPolicy->req_command(pjob_active);				//POLICY�ɃR�}���h�W�J�˗� pjob NULL�Ȃ��NULL���A���Ă���
					if (pCom_hot != NULL) {
						init_comset(pCom_hot);									//�R�}���h�̃t���O������
						pPolicy->update_command_status(pCom_hot, STAT_ACTIVE);	//�R�}���h���s�J�n��
						AgentInf_workbuf.command_count++;						//�R�}���h���V�s�쐬�J�E���g ���j�^�p
					}
					else {
						pjob_active = NULL;
					}
				}
				else {//�W���u����
					pCom_hot = NULL;
				}
			}
			else {//�L���W���u�L
				if (pCom_hot != NULL) {	//�L���R�}���h�L

					//�O���b�v�X�C�b�`ABOT����
					if (pCS->get_gurip_status() == L_OFF) {//�O���b�v�X�C�b�`OFF�ŃR�}���h�N���A
						comset_abot_end(pCom_hot);//�R�}���hABOT�@END�I��
					}
					
					//�R�}���h��Ԋm�F
					int com_complete = STAT_END;
					for (int i = 0; i < MOTION_ID_MAX; i++) {
							if (!(pCom_hot->seq[i].seq_status & STAT_END)) {	//��������L�iSTAT_END�@bit OFF)
								com_complete = STAT_ACTIVE;
							}
							else {
								if (pCom_hot->seq[i].seq_status != STAT_END) {	//���튮���ł͂Ȃ�
									com_complete = pCom_hot->seq[i].seq_status;
								}
							}
					}

					//�����̎�POLICY�փX�e�[�^�X��
					if (com_complete != STAT_ACTIVE) {							//�R�}���h����
						AgentInf_workbuf.auto_on_going &= ~AUTO_TYPE_JOB_MASK;	//��������JOB TYPE�iJOB,������,�N���[������j�N���A
						pPolicy->update_command_status(pCom_hot, com_complete);	//�R�}���h���s������
						pCom_hot = NULL;										//�R�}���h�N���A
					}
				}
				else {//�W���u���s���ŃR�}���h����
					if (pCS->get_gurip_status() == L_ON) {//�O���b�v�X�C�b�`ON�̎��A�R�}���h�ėv��
						pCom_hot = pPolicy->req_command(pjob_active);				//POLICY�ɃR�}���h�W�J�˗� pjob NULL�Ȃ��NULL���A���Ă���
						if (pCom_hot != NULL) {										//���̃R�}���h�L
							init_comset(pCom_hot);
							pPolicy->update_command_status(pCom_hot, STAT_ACTIVE);	//�R�}���h���s�J�n��
							AgentInf_workbuf.command_count++;						//�R�}���h���V�s�쐬�J�E���g ���j�^�p
							//�������䃂�[�h�̃Z�b�g
						}
						else {
							pjob_active = NULL;
						}
					}
				}
			}

		}
		else {//�������[�hOFF �R�}���h���s���ł����ABORT��	
			if (pCom_hot != NULL) {
				comset_abot_end(pCom_hot);//�R�}���hABOT�@END�I��
				pPolicy->update_command_status(pCom_hot, STAT_ABOTED);	//�R�}���hABOTE��
			}
			pCom_hot = NULL;
			pjob_active = NULL;
		}
		AgentInf_workbuf.pCom_hot = pCom_hot;//SCADA�\���p
		AgentInf_workbuf.pJob_hot = pjob_active;//SCADA�\���p
	}

	//###################################################
	//# ���䃂�[�h�Z�b�g auto_on_going,antisway_on_going
	//###################################################
	{
		//�������䃂�[�h�̃Z�b�g
		if ((pCSInf->auto_mode == L_ON) && (pjob_active != NULL)) {//�������䃂�[�hON�Ŏ��s��JOB�L
			if (pjob_active->type == ID_JOBTYPE_SEMI)		AgentInf_workbuf.auto_on_going |= AUTO_TYPE_SEMIAUTO;
			else if (pjob_active->type == ID_JOBTYPE_JOB)	AgentInf_workbuf.auto_on_going |= AUTO_TYPE_JOB;
			else;
		}
		else {
			AgentInf_workbuf.auto_on_going &= ~AUTO_TYPE_SEMIAUTO;
			AgentInf_workbuf.auto_on_going &= ~AUTO_TYPE_JOB;
		}
		
		//�U��~�߃��[�h�Z�b�g
		if (pCSInf->antisway_mode == L_OFF) {
			//auto_on_going
			AgentInf_workbuf.auto_on_going &= ~AUTO_TYPE_FB_ANTI_SWAY;
			//antisway_on_going
			AgentInf_workbuf.antisway_on_going = ANTISWAY_ALL_MANUAL;
		}
		else {
			AgentInf_workbuf.auto_on_going |= AUTO_TYPE_FB_ANTI_SWAY;

			if (pCraneStat->notch0 & BIT_SEL_BH) {							//0�m�b�`
				AgentInf_workbuf.antisway_on_going |= ANTISWAY_BH_ACTIVE;
				AgentInf_workbuf.antisway_on_going &= ~ANTISWAY_BH_PAUSED;
			}
			else {															//�m�b�`����
				AgentInf_workbuf.antisway_on_going |= ANTISWAY_BH_PAUSED;
				AgentInf_workbuf.antisway_on_going &= ~ANTISWAY_BH_ACTIVE;
			}

			if (pCraneStat->notch0 & BIT_SEL_SLW) {							//0�m�b�`
				AgentInf_workbuf.antisway_on_going |= ANTISWAY_SLEW_ACTIVE;
				AgentInf_workbuf.antisway_on_going &= ~ANTISWAY_SLEW_PAUSED;
			}
			else {															//�m�b�`����
				AgentInf_workbuf.antisway_on_going |= ANTISWAY_SLEW_PAUSED;
				AgentInf_workbuf.antisway_on_going &= ~ANTISWAY_SLEW_ACTIVE;
			}
		}
	}

	//###################################################
	//# �U��~�ߊ�����ԃZ�b�g�@antisway_on_going
	//###################################################
	{
		double tmp_amp2, tmp_dist;
		//����
		//�U��U���A�ʒu����Ƃ��Ɋ������背�x���ȓ�
		tmp_amp2 = pEnv->cal_sway_r_amp2_m();
		tmp_dist = pEnv->cal_dist4target(ID_BOOM_H, true);
		//�U���A�ʒu�Ƃ����䊮�����x���ȓ�
		if ((tmp_amp2 < pCraneStat->spec.as_m2_level[ID_BOOM_H][ID_LV_COMPLE])
			&& (tmp_dist < pCraneStat->spec.as_pos_level[ID_BOOM_H][ID_LV_COMPLE])) {

			AgentInf_workbuf.antisway_on_going |= ANTISWAY_BH_COMPLETE;
		}
		//�U��~�ߊ������
		else if (pAgentInf->antisway_on_going & ANTISWAY_BH_COMPLETE) {	//�U��~�ߊ����t���OON
			//�U���܂��͈ʒu���U��~�ߋN�����x���z�Ŋ����t���O�N���A
			if ((pEnv->cal_sway_r_amp2_m() > pCraneStat->spec.as_m2_level[ID_BOOM_H][ID_LV_TRIGGER])
				|| (pEnv->cal_dist4target(ID_BOOM_H, true) > pCraneStat->spec.as_pos_level[ID_BOOM_H][ID_LV_TRIGGER]))

				AgentInf_workbuf.antisway_on_going &= ~ANTISWAY_BH_COMPLETE;
		}
		else {
			AgentInf_workbuf.antisway_on_going &= ~ANTISWAY_BH_COMPLETE;
		}

		//����
		tmp_amp2 = pEnv->cal_sway_th_amp2_m();
		tmp_dist = pEnv->cal_dist4target(ID_SLEW, true);
		//�U���A�ʒu�Ƃ����䊮�����x���ȓ�
		if ((tmp_amp2 < pCraneStat->spec.as_m2_level[ID_SLEW][ID_LV_COMPLE])
			&& (tmp_dist < pCraneStat->spec.as_pos_level[ID_SLEW][ID_LV_COMPLE])) {

			AgentInf_workbuf.antisway_on_going |= ANTISWAY_SLEW_COMPLETE;

		}
		//�U��~�ߊ������
		else if (pAgentInf->antisway_on_going & ANTISWAY_SLEW_COMPLETE) {
			if ((pEnv->cal_sway_th_amp2_m() > pCraneStat->spec.as_m2_level[ID_SLEW][ID_LV_TRIGGER])
				|| (pEnv->cal_dist4target(ID_SLEW, true) > pCraneStat->spec.as_pos_level[ID_SLEW][ID_LV_TRIGGER]))

				AgentInf_workbuf.antisway_on_going &= ~ANTISWAY_SLEW_COMPLETE;
		}
		else {
			AgentInf_workbuf.antisway_on_going &= ~ANTISWAY_SLEW_COMPLETE;
		}
	}

	//###################################################
	//# �����̃��[�h�Z�b�g(PLC�w�ߏo�͎��̔���j
	//###################################################
	{
		//�蓮
		if ((AgentInf_workbuf.auto_on_going == AUTO_TYPE_MANUAL)) {
			AgentInf_workbuf.auto_active[ID_HOIST] = AgentInf_workbuf.auto_active[ID_BOOM_H] = AgentInf_workbuf.auto_active[ID_SLEW] = AgentInf_workbuf.auto_active[ID_AHOIST] = AUTO_TYPE_MANUAL;
		}
		//������
		else if (AgentInf_workbuf.auto_on_going & AUTO_TYPE_SEMIAUTO) {
			for (int i = 0; i <= ID_AHOIST; i++) {
				if (pCSInf->auto_status[i] == L_ON)		AgentInf_workbuf.auto_active[i] = AgentInf_workbuf.auto_on_going;
				else									AgentInf_workbuf.auto_active[i] = AUTO_TYPE_MANUAL;

			}
		}
		//JOB
		else if (AgentInf_workbuf.auto_on_going & AUTO_TYPE_JOB) {
			AgentInf_workbuf.auto_active[ID_HOIST] = AgentInf_workbuf.auto_active[ID_BOOM_H] = AgentInf_workbuf.auto_active[ID_SLEW] = AgentInf_workbuf.auto_active[ID_AHOIST] = AgentInf_workbuf.auto_on_going;
		}
		//FB�U��~��
		else {
			if (AgentInf_workbuf.auto_on_going & AUTO_TYPE_FB_ANTI_SWAY) {
				if (pCraneStat->notch0 & BIT_SEL_BH)	AgentInf_workbuf.auto_active[ID_BOOM_H] = AUTO_TYPE_FB_ANTI_SWAY;
				else								AgentInf_workbuf.auto_active[ID_BOOM_H] = AUTO_TYPE_MANUAL;

				if (pCraneStat->notch0 & BIT_SEL_SLW)	AgentInf_workbuf.auto_active[ID_SLEW] = AUTO_TYPE_FB_ANTI_SWAY;
				else									AgentInf_workbuf.auto_active[ID_SLEW] = AUTO_TYPE_MANUAL;
			}
			else {
				AgentInf_workbuf.auto_active[ID_BOOM_H] = AUTO_TYPE_MANUAL;
				AgentInf_workbuf.auto_active[ID_SLEW] = AUTO_TYPE_MANUAL;
			}
			AgentInf_workbuf.auto_active[ID_HOIST] = AUTO_TYPE_MANUAL;
			AgentInf_workbuf.auto_active[ID_GANTRY] = AUTO_TYPE_MANUAL;
		}
	}

	//###################################################
	//#�U��~�߃R�}���h�Z�b�g�N���A
	//###################################################
	{
		if (!(AgentInf_workbuf.auto_on_going & AUTO_TYPE_FB_ANTI_SWAY)) {
			AgentInf_workbuf.st_as_comset.seq[ID_BOOM_H].seq_status = STAT_END;
			AgentInf_workbuf.st_as_comset.seq[ID_SLEW].seq_status = STAT_END;
		}
	}

	//###################################################
	//#�����ڕW�ʒu�ݒ�
	//###################################################
	{
		//JOB�g���K���o
		if ((pCom_hot != NULL) && (pCom_hot != pCom_hot_last)) {
			AgentInf_workbuf.auto_pos_target = pCom_hot->target;
		}
		//JOB����
		else if (pCom_hot == NULL) {
			//���͌��݈ʒu
			AgentInf_workbuf.auto_pos_target.pos[ID_HOIST] = pPLC_IO->pos[ID_HOIST];
			AgentInf_workbuf.auto_pos_target.pos[ID_AHOIST] = pPLC_IO->pos[ID_AHOIST];

			//�����̓m�b�`����܂��͐U��~��OFF�Ō��݈ʒu�@0�m�b�`�g���K�Ō��݈ʒu�{���������ʒu
			//0�m�b�`�g���K�Ō��݈ʒu�{���������ʒu�ɐݒ�
			if (!(notch0_last & BIT_SEL_BH) && (pCraneStat->notch0 & BIT_SEL_BH)) {
				AgentInf_workbuf.auto_pos_target.pos[ID_BOOM_H] = pPLC_IO->pos[ID_BOOM_H] + pEnv->cal_dist4stop(ID_BOOM_H, false);
			}
			//�m�b�`����܂��͐U��~�߃��[�hOFF�Ō��݈ʒu
			else if (!(pCraneStat->notch0 & BIT_SEL_BH) || (pCSInf->antisway_mode != L_ON)) {
				AgentInf_workbuf.auto_pos_target.pos[ID_BOOM_H] = pPLC_IO->pos[ID_BOOM_H];
			}
			else;

			//����̓m�b�`����܂��͐U��~��OFF�Ō��݈ʒu�@0�m�b�`�g���K�Ō��݈ʒu�{���������ʒu
			if (!(notch0_last & BIT_SEL_SLW) && (pCraneStat->notch0 & BIT_SEL_SLW)) { //0�m�b�`�g���K
				AgentInf_workbuf.auto_pos_target.pos[ID_SLEW] = pPLC_IO->pos[ID_SLEW] + pEnv->cal_dist4stop(ID_SLEW, false);
			}
			//�m�b�`����܂��͐U��~�߃��[�hOFF�Ō��݈ʒu
			else if (!(pCraneStat->notch0 & BIT_SEL_SLW) || (pCSInf->antisway_mode != L_ON)) {
				AgentInf_workbuf.auto_pos_target.pos[ID_SLEW] = pPLC_IO->pos[ID_SLEW];
			}
			else;
		}
		else;
	}
			
	//###################################################	
	//# PLC�ւ̏o�͌v�Z�@
	//###################################################
	set_ref_mh();							//�������x�w��
	set_ref_gt();							//���s���x�w��
	set_ref_slew();							//���񑬓x�w��
	set_ref_bh();							//�������x�w��
	set_ref_ah();							//�⊪���x�w��

	//###################################################
	//# �O��l�ێ�
	//###################################################
	notch0_last = pCraneStat->notch0;
	pCom_hot_last = pCom_hot;
	
	return;
}

/****************************************************************************/
/*   ����������菇3�@�M���o�͏���												*/
/****************************************************************************/
void CAgent::output() {

	//�����֘A�񍐏���
	

	//���L�������o�͏���
	int a = sizeof(ST_AGENT_INFO);
	memcpy_s(pAgentInf, sizeof(ST_AGENT_INFO), &AgentInf_workbuf, sizeof(ST_AGENT_INFO));

	//�^�X�N�p�l���ւ̕\���o��
#if 0
	wostrs << L" #SL TG:" << fixed<<setprecision(3) << AgentInf_workbuf.auto_pos_target.pos[ID_SLEW];
	wostrs << L",GAP: " << pEnv->cal_dist4target(ID_SLEW,false);

	wostrs << L"#BH TG: " << AgentInf_workbuf.auto_pos_target.pos[ID_BOOM_H];
	wostrs << L",GAP: " << pEnv->cal_dist4target(ID_BOOM_H, false);

	wostrs << L",ActiveSet: " << dbg_mont[0];
#endif	

	wostrs.str(L"");
	wostrs << L" pCom_hot:" << pCom_hot << L" pjob_active" << pjob_active;

	wostrs <<  L" --Scan " << inf.period;;

	tweet2owner(wostrs.str()); wostrs.str(L""); wostrs.clear();
	return;

};

/****************************************************************************/
/*   JOB�֘A����															*/
/****************************************************************************/

/****************************************************************************/
/*   ���w�ߏo�͏���		                                                    */
/****************************************************************************/
int CAgent::set_ref_mh(){
	if (AgentInf_workbuf.pc_ctrl_mode & BIT_SEL_HST) {
		if (AgentInf_workbuf.auto_active[ID_HOIST] == AUTO_TYPE_MANUAL)
			AgentInf_workbuf.v_ref[ID_HOIST] = 0.0;
		//JOB���s��
		else if ((AgentInf_workbuf.auto_active[ID_HOIST] & AUTO_TYPE_JOB) ||
			(AgentInf_workbuf.auto_active[ID_HOIST] & AUTO_TYPE_SEMIAUTO)) {
			if (pCom_hot == NULL)	AgentInf_workbuf.v_ref[ID_HOIST] = 0.0;
			else				AgentInf_workbuf.v_ref[ID_HOIST] = cal_step(pCom_hot, ID_HOIST);
		}
		else
			AgentInf_workbuf.v_ref[ID_HOIST] = 0.0;
	}
	else {
		AgentInf_workbuf.v_ref[ID_HOIST] = 0.0;
	}
	return 0; 
}
/****************************************************************************/
/*   �⊪�w�ߏo�͏���		                                                    */
/****************************************************************************/
int CAgent::set_ref_ah() {
	if (AgentInf_workbuf.pc_ctrl_mode & BIT_SEL_AH) {
		if (AgentInf_workbuf.auto_active[ID_AHOIST] == AUTO_TYPE_MANUAL)
			AgentInf_workbuf.v_ref[ID_AHOIST] = 0.0;
		//JOB���s��
		else if ((AgentInf_workbuf.auto_active[ID_AHOIST] & AUTO_TYPE_JOB) ||
			(AgentInf_workbuf.auto_active[ID_AHOIST] & AUTO_TYPE_SEMIAUTO)) {
			if (pCom_hot == NULL)	AgentInf_workbuf.v_ref[ID_AHOIST] = 0.0;
			else					AgentInf_workbuf.v_ref[ID_AHOIST] = cal_step(pCom_hot, ID_AHOIST);
		}
		else
			AgentInf_workbuf.v_ref[ID_HOIST] = 0.0;
	}
	else {
		AgentInf_workbuf.v_ref[ID_HOIST] = 0.0;
	}
	return 0;
}
/****************************************************************************/
/*   ���s�w�ߏo�͏���		                                                */
/****************************************************************************/
int CAgent::set_ref_gt(){
	if (AgentInf_workbuf.pc_ctrl_mode & BIT_SEL_GNT) {
		if (AgentInf_workbuf.auto_active[ID_GANTRY] == AUTO_TYPE_MANUAL)
			AgentInf_workbuf.v_ref[ID_GANTRY] = 0.0;
		else if ((AgentInf_workbuf.auto_active[ID_GANTRY] == AUTO_TYPE_JOB) ||
			(AgentInf_workbuf.auto_active[ID_GANTRY] == AUTO_TYPE_SEMIAUTO)) {
			if (pCom_hot == NULL)	AgentInf_workbuf.v_ref[ID_GANTRY] = 0.0;
			else				AgentInf_workbuf.v_ref[ID_GANTRY] = cal_step(pCom_hot, ID_GANTRY);
		}

		else
			AgentInf_workbuf.v_ref[ID_GANTRY] = 0.0;
	}
	else {
		AgentInf_workbuf.v_ref[ID_GANTRY] = 0.0;
	}
	return 0;
}
/****************************************************************************/
/*   ����w�ߏo�͏���		                                                */
/****************************************************************************/
int CAgent::set_ref_slew(){

	if (AgentInf_workbuf.pc_ctrl_mode & BIT_SEL_SLW) {										//����PC�w�ߑI��ON
		//�}�j���A�����[�h
		if (AgentInf_workbuf.auto_active[ID_SLEW] == AUTO_TYPE_MANUAL) {
			AgentInf_workbuf.v_ref[ID_SLEW] = 0.0;
		}
		//JOB���s��
		else if((AgentInf_workbuf.auto_active[ID_SLEW] & AUTO_TYPE_JOB)||
				(AgentInf_workbuf.auto_active[ID_SLEW] & AUTO_TYPE_SEMIAUTO)){					//�����^�]��
			if (pCom_hot == NULL) {
				AgentInf_workbuf.v_ref[ID_SLEW] = 0.0;
			}
			else {
				AgentInf_workbuf.v_ref[ID_SLEW] = cal_step(pCom_hot, ID_SLEW);
			}
		}
		else if (AgentInf_workbuf.auto_active[ID_SLEW] & AUTO_TYPE_FB_ANTI_SWAY) {				//�U��~�ߒ�
			if (AgentInf_workbuf.antisway_on_going & ANTISWAY_SLEW_COMPLETE){					//�U��~�ߊ���
				AgentInf_workbuf.v_ref[ID_SLEW] = 0.0;
			}
			else {																				//�U��~�ߖ���
				AgentInf_workbuf.v_ref[ID_SLEW] = cal_step(&AgentInf_workbuf.st_as_comset, ID_SLEW);
			}
		}
		else {
				AgentInf_workbuf.v_ref[ID_SLEW] = 0.0;
		} 
	}
	else {
		AgentInf_workbuf.v_ref[ID_SLEW] = 0.0;
	}

	return 0;
}
/****************************************************************************/
/*   �����w�ߏo�͏���		                                                */
/****************************************************************************/
int CAgent::set_ref_bh(){

	if (AgentInf_workbuf.pc_ctrl_mode & BIT_SEL_BH) {									//����PC�w�ߑI��ON
		//�}�j���A�����[�h
		if (AgentInf_workbuf.auto_active[ID_BOOM_H] == AUTO_TYPE_MANUAL) {
			AgentInf_workbuf.v_ref[ID_BOOM_H] = 0.0;
		}
		//JOB���s��
		else if ((AgentInf_workbuf.auto_active[ID_BOOM_H] & AUTO_TYPE_JOB) ||
			(AgentInf_workbuf.auto_active[ID_BOOM_H] & AUTO_TYPE_SEMIAUTO)) {					
			if (pCom_hot == NULL) {
				AgentInf_workbuf.v_ref[ID_BOOM_H] = 0.0;
			}
			else {
				AgentInf_workbuf.v_ref[ID_BOOM_H] = cal_step(pCom_hot, ID_BOOM_H);
			}
		}
		//FB�U��~��
		else if (AgentInf_workbuf.auto_active[ID_BOOM_H] & AUTO_TYPE_FB_ANTI_SWAY) {	//�U��~�ߒ�
			if (!(AgentInf_workbuf.antisway_on_going & ANTISWAY_BH_ACTIVE) || (AgentInf_workbuf.antisway_on_going & ANTISWAY_BH_PAUSED)) {
				AgentInf_workbuf.v_ref[ID_BOOM_H] = 0.0;
			}
			else if (AgentInf_workbuf.antisway_on_going & ANTISWAY_BH_COMPLETE) {			//�U��~�ߊ���
				AgentInf_workbuf.v_ref[ID_BOOM_H] = 0.0;
			}
			else { 																		//�U��~�ߖ���
				AgentInf_workbuf.v_ref[ID_BOOM_H] = cal_step(&AgentInf_workbuf.st_as_comset,ID_BOOM_H);
			}
		}
		//���̑�
		else {
			AgentInf_workbuf.v_ref[ID_BOOM_H] = 0.0;
		}
	}
	else {
		AgentInf_workbuf.v_ref[ID_BOOM_H] = 0.0;
	}
	return 0;
}
/****************************************************************************/
/*  �R�}���h�Z�b�g���s�O����������                                        */
/*  ���s�Ǘ��X�e�[�^�X�̃N���A�ƃR�}���h���s���X�e�[�^�X�Z�b�g				*/
/****************************************************************************/
int CAgent::init_comset(LPST_COMMAND_SET pcom) {
	if (pcom->com_code.i_list == ID_JOBTYPE_ANTISWAY) {
		for (int i = 0; i < MOTION_ID_MAX; i++) {
			pcom->seq[i].seq_status = STAT_END;
			pcom->seq[i].i_hot_step = 0;
			pcom->seq[i].sequence_act_count = 0;
		}

		pcom->seq[ID_BOOM_H].seq_status	= STAT_STANDBY;
		pcom->seq[ID_SLEW].seq_status	= STAT_STANDBY;

	}
	else {
		for (int i = 0; i < MOTION_ID_MAX;i++) {						//�e���̎��s�X�e�[�^�X�̏�����
			if (pcom->seq_mode[i] == L_ON) pcom->seq[i].seq_status	= STAT_STANDBY;
			else						   pcom->seq[i].seq_status	= STAT_END;
			pcom->seq->i_hot_step = 0;
			pcom->seq->sequence_act_count = 0;
		}
	}
	pcom->com_status = STAT_STANDBY;
	return 0;
}

/****************************************************************************/
/*  �R�}���h�Z�b�g���s�O����������                                        */
/*  ���s�Ǘ��X�e�[�^�X�̃N���A�ƃR�}���h���s���X�e�[�^�X�Z�b�g				*/
/****************************************************************************/
int CAgent::comset_abot_end(LPST_COMMAND_SET pcom) {
	for (int i = 0; i < MOTION_ID_MAX; i++) {//�S����̃R�}���h�V�[�P���X��ABOT�Z�b�g
		pcom->seq[i].seq_status = STAT_ABOTED;
		pcom->seq[i].i_hot_step = pcom->seq[i].n_step;
		pcom->seq[i].sequence_act_count = 0;
	}
	return 0;
}
/****************************************************************************/
/*	�U��~�ߊ֘A�ݒ�					*/
/****************************************************************************/

//�U��~�߃p�^�[���v�Z�p��ƃo�b�t�@�Z�b�g
void CAgent::set_as_workbuf(int motion) {

	st_as_work.agent_scan_ms = inf.cycle_ms;						//AGENT�^�X�N�̃X�L�����^�C��

	double temp_d;

	//���݈ʒu
	st_as_work.pos[motion] = pPLC_IO->pos[motion];
	//���ݑ��x
	st_as_work.v[motion] = pPLC_IO->v_fb[motion];
	//�ړ�����
	temp_d = AgentInf_workbuf.auto_pos_target.pos[motion] - st_as_work.pos[motion];
	if (motion == ID_SLEW) {																//����́A180���z����Ƃ��͋t�������߂�
		if (temp_d > PI180)	temp_d -= PI360;
		else if (temp_d < -PI180) temp_d += PI360;
		else;
	}
	st_as_work.dist_for_target[motion] = temp_d;											//�ړ�����
	if (temp_d < 0.0)	st_as_work.dist_for_target_abs[motion] = -1.0 * temp_d;				//�ړ�������Βl
	else				st_as_work.dist_for_target_abs[motion] = temp_d;

	//���쎲�����x
	st_as_work.a_abs[motion] = pCraneStat->spec.accdec[motion][FWD][ACC];
	//�ő呬�x
	st_as_work.vmax[motion] = pCraneStat->spec.notch_spd_f[motion][NOTCH_MAX - 1];
	//�ő��������
	st_as_work.acc_time2Vmax[motion] = st_as_work.vmax[motion] / st_as_work.a_abs[motion];

	if ((motion == ID_BOOM_H) || (motion == ID_SLEW)) {
		//�ݓ_�̉����x
		st_as_work.a_hp_abs[motion] = pEnv->cal_acc(motion, pPLC_IO->pos[motion],UNIT_CODE_M);
		if (st_as_work.a_hp_abs[motion] < 0.0)st_as_work.a_hp_abs[motion] *= -1.0;
		//�������U�ꒆ�S
//		st_as_work.pp_th0[motion][ACC] = pEnv->cal_arad_acc(motion, FWD);
		//�������U�ꒆ�S
//		st_as_work.pp_th0[motion][DEC] = pEnv->cal_arad_dec(motion, FWD);
	}

	st_as_work.T = pCraneStat->T;															//�U�����
	st_as_work.w = pCraneStat->w;															//�U��p���g��
	st_as_work.w2 = pCraneStat->w2;


	 double rs_m;
	if (motion == ID_BOOM_H) {
		rs_m = sqrt(pEnv->cal_sway_r_amp2_m());
	}
	else if (motion == ID_SLEW) {
		rs_m = sqrt(pEnv->cal_sway_th_amp2_m());
	}
	else {
		rs_m = 0.0;
	}

	//�U��~�߃p�^�[���ƃQ�C���i�������ԁj
	st_as_work.as_ptn_type[motion] = AGENT_AS_PTN_PAUSE;

	double r0 = st_as_work.a_hp_abs[motion] / GA;

	double rs = sqrt(pSway_IO->rad_amp2[SID_LOAD_MH][motion]);


	if (st_as_work.dist_for_target_abs[motion] > pCraneStat->spec.as_pos_level[motion][ID_LV_TRIGGER]) {			//�ڕW�܂ł̋����偨�ʒu���킹
		st_as_work.as_ptn_type[motion] = AGENT_AS_PTN_POS_IN;
		//st_as_work.as_gain_ta[motion] = sqrt(st_as_work.dist_for_target_abs[motion] / st_as_work.a_hp_abs[motion]);
		double cos_ph = 1 - 0.5* rs / r0;
		if (cos_ph < -1.0) {
			st_as_work.as_gain_ta[motion] = st_as_work.T / 2.0;
		}
		else {
			//			st_as_work.as_gain_ta[motion] = acos(cos_ph) / st_as_work.a_hp_abs[motion];
			st_as_work.as_gain_ta[motion] = acos(cos_ph) / st_as_work.w;
			//�U�ꂪ�������Ĉړ��ʂ��������Ȃ鎞�́A���ԋ����܂Őڋ�
			double half_ta = sqrt(0.5 * st_as_work.dist_for_target_abs[motion] / st_as_work.a_abs[motion]);
			if (st_as_work.as_gain_ta[motion] < half_ta) {
				st_as_work.as_gain_ta[motion] = half_ta;
			}

		}
	}
	else if(rs_m < pCraneStat->spec.as_m_level[motion][ID_LV_TRIGGER]){												//�ڕW�܂ł̋������ŐU�ꂪ���������ʒu���킹
		st_as_work.as_ptn_type[motion] = AGENT_AS_PTN_POS_IN;
		//st_as_work.as_gain_ta[motion] = sqrt(st_as_work.dist_for_target_abs[motion] / st_as_work.a_hp_abs[motion]);
		double cos_ph = 1 - 0.5 * rs / r0;
		if (cos_ph < -1.0) {
			st_as_work.as_gain_ta[motion] = st_as_work.T / 2.0;
		}
		else {
//			st_as_work.as_gain_ta[motion] = acos(cos_ph) / st_as_work.a_hp_abs[motion];
			st_as_work.as_gain_ta[motion] = acos(cos_ph) / st_as_work.w;

		}
	}
	else if (rs_m > pCraneStat->spec.as_m_level[motion][ID_LV_COMPLE]) {
		st_as_work.as_ptn_type[motion] = AGENT_AS_PTN_POS_OUT;

		double cos_ph = 1 - rs / (4.0 * r0);
		if (cos_ph < -1.0) {
			st_as_work.as_gain_ta[motion] = st_as_work.T / 2.0;
		}
		else {
			//			st_as_work.as_gain_ta[motion] = acos(cos_ph) / st_as_work.a_hp_abs[motion];
			st_as_work.as_gain_ta[motion] = acos(cos_ph) / st_as_work.w;
		}
	}
	else {
		st_as_work.as_ptn_type[motion] = AGENT_AS_PTN_FINE_POS;//�����ʒu���킹��1�m�b�`���x
		st_as_work.as_gain_ta[motion] = pCraneStat->spec.notch_spd_f[motion][1] / st_as_work.a_abs[motion];
	}

	//�C���`���O�̉������Ԃ͐U�����������Ƃ���B
	if (st_as_work.as_gain_ta[motion] > st_as_work.T) st_as_work.as_gain_ta[motion] = st_as_work.T;

	//�C���`���O�̉������Ԃ͍ő�������Ԃ�����B
	if (st_as_work.as_gain_ta[motion] > st_as_work.acc_time2Vmax[motion]) st_as_work.as_gain_ta[motion] = st_as_work.acc_time2Vmax[motion];


	if ((st_as_work.as_ptn_type[motion] == AGENT_AS_PTN_POS_OUT)|| (st_as_work.as_ptn_type[motion] == AGENT_AS_PTN_FINE_POS)) {
		st_as_work.motion_dir[motion] = ID_SELECT;				//�U��o���̈ړ������͐�ɗ����ʑ��Ō��߂�
	}
	else if (st_as_work.dist_for_target[motion] < 0.0) {
		st_as_work.motion_dir[motion] = ID_REV;					//�ړ�����
	}
	else if (temp_d > 0.0) {
		st_as_work.motion_dir[motion] = ID_FWD;					//�ړ�����
	}
	else {
		st_as_work.motion_dir[motion] = ID_STOP;				//�ړ�����
	}


	//�U��~�ߋN���ʑ�
	st_as_work.as_ph[motion] = st_as_work.as_gain_ta[motion]* st_as_work.w;// �N���ʑ��́@�ړ������ɂ���ā@th0 - ph = n��(n=0 or -�΁j

	return;
}

//�U��~�߃p�^�[�����V�s�v�Z
int CAgent::cal_as_seq(int motion) {

	set_as_workbuf(motion);
	AgentInf_workbuf.st_as_comset.com_code.i_list = ID_JOBTYPE_ANTISWAY;//�R�}���h�R�[�h�@�U��~�߃^�C�v�Z�b�g

	/*### ���V�s�ݒ� ###*/
	LPST_MOTION_SEQ pseq = &AgentInf_workbuf.st_as_comset.seq[motion];
	pseq->n_step = 0;								// �X�e�b�v�N���A
	pseq->direction = st_as_work.motion_dir[motion];
	pseq->i_hot_step = 0;
	pseq->sequence_act_count = 0;
	pseq->axis_id = motion;
	pseq->seq_status = STAT_NA;

	LPST_MOTION_STEP pelement = &pseq->steps[0];

	/*### �X�e�b�v�ݒ� ###*/
	//#### STEP 0 �N���ҋ@
	pelement = &(pseq->steps[pseq->n_step++]);											//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
	pelement->_v = 0.0;																			// ���x0
	pelement->_p = st_as_work.pos[motion];														// �ڕW�ʒu
	pelement->status = STAT_STANDBY;
																								// �X�e�[�^�X�Z�b�g
	if (motion == ID_BOOM_H)	pelement->opt_d[STEP_OPT_PHASE_CHK_RANGE] = PARAM_PHASE_CHK_RANGE_BH;
	else 						pelement->opt_d[STEP_OPT_PHASE_CHK_RANGE] = PARAM_PHASE_CHK_RANGE_SLW;

	switch (st_as_work.as_ptn_type[motion]) {
	case AGENT_AS_PTN_POS_IN: {
		pelement->type = CTR_TYPE_WAIT_PH_SINGLE;
		pelement->_t = st_as_work.T * 2.0;														// �^�C���I�[�o�[���~�b�g�l
		if (st_as_work.motion_dir[motion] == ID_REV) {
			if (motion == ID_BOOM_H) {
				pelement->opt_d[STEP_OPT_PHASE_SINGLE] = -PI180 + st_as_work.as_ph[motion];
			}
			else {
				pelement->opt_d[STEP_OPT_PHASE_SINGLE] = st_as_work.as_ph[motion];
			}
		}
		else {
			if (motion == ID_BOOM_H) {
				pelement->opt_d[STEP_OPT_PHASE_SINGLE] = st_as_work.as_ph[motion];

			}
			else {
				pelement->opt_d[STEP_OPT_PHASE_SINGLE] = -PI180 + st_as_work.as_ph[motion];
			}
		}
		CHelper::fit_ph_range_upto_pi(&(pelement->opt_d[STEP_OPT_PHASE_SINGLE]));			  //�����W��-�΁`�΂�

	}break;
	case AGENT_AS_PTN_POS_OUT: {
		pelement->type = CTR_TYPE_WAIT_PH_DOUBLE;
		pelement->_t = st_as_work.T * 2.0;														// �^�C���I�[�o�[���~�b�g�l
		pelement->opt_d[STEP_OPT_PHASE_FWD] = -PI180 + st_as_work.as_ph[motion];
		CHelper::fit_ph_range_upto_pi(&(pelement->opt_d[STEP_OPT_PHASE_FWD]));					//�����W��-�΁`�΂�
		pelement->opt_d[STEP_OPT_PHASE_REV] = st_as_work.as_ph[motion];
		CHelper::fit_ph_range_upto_pi(&(pelement->opt_d[STEP_OPT_PHASE_REV]));					//�����W��-�΁`�΂�
	}break;
	case AGENT_AS_PTN_FINE_POS: {
		pelement->type = CTR_TYPE_WAIT_TIME;
		pelement->_t = 0.1;
	}break;																						// ��������
	default: {
		pelement->type = CTR_TYPE_WAIT_TIME;
		pelement->_t = 0.1;																		// ��������														// �^�C���I�[�o�[���~�b�g�l
	}break;
	}

	//#### STEP 1 ����
	
	double d;

	if ((st_as_work.as_ptn_type[motion] == AGENT_AS_PTN_POS_IN)
		|| (st_as_work.as_ptn_type[motion] == AGENT_AS_PTN_POS_OUT)
		|| (st_as_work.as_ptn_type[motion] == AGENT_AS_PTN_FINE_POS)) {

		pelement = &(pseq->steps[pseq->n_step++]);										//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->status = STAT_STANDBY;														// �X�e�[�^�X�Z�b�g
		pelement->type = CTR_TYPE_VOUT_TIME;
		pelement->_t = st_as_work.as_gain_ta[motion];
		pelement->_v = pelement->_t * st_as_work.a_abs[motion];										//�������� * �m�b�`���x�����x
		for (int i = (NOTCH_MAX - 1);i > 1;i--) {												//�^�߂̃m�b�`���x�����߂�
			if (pelement->_v > pCraneStat->spec.notch_spd_f[motion][i - 1]) {
				pelement->_v = pCraneStat->spec.notch_spd_f[motion][i];
				break;
			}
			else continue;			//���̃m�b�`��
		}
		d = 0.5 * pelement->_v * pelement->_t;
		pelement->_p = (pelement - 1)->_p - (double)st_as_work.motion_dir[motion] * d;
	}

	//#### STEP 2 �葬

	if (st_as_work.as_ptn_type[motion] == AGENT_AS_PTN_POS_IN){
		pelement = &(pseq->steps[pseq->n_step++]);										//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->status = STAT_STANDBY;														// �X�e�[�^�X�Z�b�g
		pelement->type = CTR_TYPE_VOUT_V;
		pelement->_t = st_as_work.as_gain_ta[motion];
		pelement->_v = 0.0;																		//�������� * �m�b�`���x�����x
		pelement->_p = AgentInf_workbuf.auto_pos_target.pos[motion];							//�U��~�ߖڕW�ʒu
	}
	else if (st_as_work.as_ptn_type[motion] == AGENT_AS_PTN_POS_OUT){
		pelement = &(pseq->steps[pseq->n_step++]);										//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->status = STAT_STANDBY;														// �X�e�[�^�X�Z�b�g
		pelement->type = CTR_TYPE_VOUT_V;
		pelement->_t = st_as_work.as_gain_ta[motion];
		pelement->_v = 0.0;																		//�������� * �m�b�`���x�����x
		pelement->_p = (pelement-1)->_p;														//�ړ��������s�聨�O�X�e�b�v����ύX�Ȃ�
	}
	else if (st_as_work.as_ptn_type[motion] == AGENT_AS_PTN_FINE_POS) {
		pelement = &(pseq->steps[pseq->n_step++]);										//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->status = STAT_STANDBY;														// �X�e�[�^�X�Z�b�g
		pelement->type = CTR_TYPE_FINE_POS;
		pelement->_t = st_as_work.as_gain_ta[motion];
		pelement->_v = pCraneStat->spec.notch_spd_f[motion][1];									//�������� * �m�b�`���x�����x
		pelement->_p = AgentInf_workbuf.auto_pos_target.pos[motion];							//�ړ��������s��̂��̂�����̂ŖڕW�ʒu�ُ͈팟�m�p�Ƀ^�[�Q�b�g�ʒu�ɂ���
	}
	else;

	//���ԏ����̃X�L�����J�E���g�l�Z�b�g
	for (int i = 0;i < pseq->n_step;i++) {
		pseq->steps[i].time_count = (int)(pseq->steps[i]._t * 1000.0 / st_as_work.agent_scan_ms);
		pseq->steps[i].act_count = 0;
		//����̓����W��-�΁`�΂�
		if (motion == ID_SLEW)	CHelper::fit_ph_range_upto_pi(&(pseq->steps[i]._p));
	}

	return L_ON;
};

/****************************************************************************/
/*   STEP����		                                                        */
/****************************************************************************/

double CAgent::cal_step(LPST_COMMAND_SET pCom,int motion) {

	double v_out = 0.0;

	//�U��~�߃R�}���h�̎��A���V�s���쐬�̎��́A�����Ń��V�s�Z�b�g
	if ((pCom->com_code.i_list == ID_JOBTYPE_ANTISWAY) && (pCom->seq[motion].seq_status & STAT_END)) {
		if (cal_as_seq(motion) == L_OFF) {
			return 0.0;  //�U��~�߃p�^�[���v�Z�Z�b�g ���V�s�����s����0���^�[��
		}
		else {
			//���s�J�E���^�l�̃N���A
			for (int i = 0;i < AgentInf_workbuf.st_as_comset.seq[motion].n_step;i++) {
				AgentInf_workbuf.st_as_comset.seq[motion].steps[i].act_count = 0;
			}
			//���V�s�쐬�J�E���g�A�b�v�@�X�e�[�^�X�N���A
			AgentInf_workbuf.as_count[motion]++;
			AgentInf_workbuf.st_as_comset.seq[motion].seq_status = STAT_STANDBY;
			AgentInf_workbuf.st_as_comset.seq[motion].i_hot_step = 0;

		}
	}

	LPST_MOTION_SEQ	pseq = &pCom->seq[motion];
	LPST_MOTION_STEP	pStep = &pseq->steps[pseq->i_hot_step];

	if (pStep->status == STAT_STANDBY) {   //�X�e�b�v�N����
		pStep->act_count = 1;
		pStep->status = STAT_ACTIVE;			//�X�e�b�v���s���ɍX�V
	}
	else	pStep->act_count++;					//�X�e�b�v���s�J�E���g�C���N�������g



	switch (pStep->type) {
	//#	�N���ʑ��҂��i�ړ������w��j
	case CTR_TYPE_WAIT_PH_SINGLE: {
		bool is_step_end = false;
		double chk_ph = PI360;
		double trig_point_ph = pStep->opt_d[STEP_OPT_PHASE_SINGLE];
		double ph_io = pSway_IO->ph[SID_LOAD_MH][motion];

		//�ُ튮������
		if ((trig_point_ph > PI180) || (trig_point_ph < -PI180)) {//�w��͈͊O�@-�΁`��
			pStep->status = STAT_LOGICAL_ERROR;is_step_end = true;
		}
		if (pStep->act_count > pStep->time_count) {
//			pStep->status = STAT_TIME_OVER;is_step_end = true;
		}
		if (pSway_IO->rad_amp2[SID_LOAD_MH][motion] < pCraneStat->spec.as_rad2_level[motion][ID_LV_COMPLE]) {//�U��U���������x��
			pStep->status = STAT_END;is_step_end = true;
		}
		if (is_step_end) {
			v_out = 0.0;
			break;
		}

		//���C������
		if ((trig_point_ph <= 0.0) && (ph_io >= 0)) {			//�ڕW�����@���݈ʑ�����
			chk_ph = pSway_IO->ph[SID_LOAD_MH][motion] - trig_point_ph;
			if (chk_ph >= PI180) chk_ph = PI360 - chk_ph;	//���������̊p�x��
		}
		else if ((trig_point_ph >= 0) && (ph_io <= 0)) {			//�ڕW�����@���ݒl����
			chk_ph = trig_point_ph - ph_io;
			if (chk_ph >= PI180) chk_ph = PI360 - chk_ph;	//���������̊p�x��
		}
		else if (trig_point_ph > ph_io) {						//�ڕW�ƌ��ݒl�̕�������v
			chk_ph = trig_point_ph - ph_io;
		}
		else {																	//�ڕW�ƌ��ݒl�̕�������v
			chk_ph = ph_io - trig_point_ph;
		}

		if (chk_ph < pStep->opt_d[STEP_OPT_PHASE_CHK_RANGE]) {	//�ڕW�ʑ��ɓ��B
			pStep->status = STAT_END;
			break;
		}

		v_out = 0.0;

	}break;
	//#	�N���ʑ��҂��i�ړ������w��j
	case CTR_TYPE_WAIT_PH_DOUBLE: {
		pseq->direction = ID_STOP;	//�����܂ňړ���������
		bool is_step_end = false;
		double ph_io = pSway_IO->ph[SID_LOAD_MH][motion];

		//�ُ튮������
		if ((pStep->opt_d[STEP_OPT_PHASE_FWD] > PI180) || (pStep->opt_d[STEP_OPT_PHASE_FWD] < -PI180)) {//�w��͈͊O�@-�΁`��
			pStep->status = STAT_LOGICAL_ERROR;is_step_end = true;
		}
		if ((pStep->opt_d[STEP_OPT_PHASE_REV] > PI180) || (pStep->opt_d[STEP_OPT_PHASE_REV] < -PI180)) {//�w��͈͊O�@-�΁`��
			pStep->status = STAT_LOGICAL_ERROR;is_step_end = true;
		}
		if (pStep->act_count > pStep->time_count) {
	//		pStep->status = STAT_TIME_OVER;is_step_end = true;
		}
		if (pSway_IO->rad_amp2[SID_LOAD_MH][motion] < pCraneStat->spec.as_rad2_level[motion][ID_LV_COMPLE]) {//�U��U���������x��
			pStep->status = STAT_END;is_step_end = true;
		}

		if (is_step_end) {
			pseq->direction = ID_STOP;
			v_out = 0.0;
			break;
		}

		//���C������
		//���]�J�n�p�ʑ�����
		double chk_ph = PI360;
		double trig_point_ph = pStep->opt_d[STEP_OPT_PHASE_FWD];
		if ((trig_point_ph <= 0.0) && (ph_io >= 0)) {			//�ڕW�����@���݈ʑ�����
			chk_ph = ph_io - trig_point_ph;
			if (chk_ph >= PI180) chk_ph = PI360 - chk_ph;	//���������̊p�x��
		}
		else if ((trig_point_ph >= 0) && (ph_io <= 0)) {			//�ڕW�����@���ݒl����
			chk_ph = trig_point_ph - ph_io;
			if (chk_ph >= PI180) chk_ph = PI360 - chk_ph;	//���������̊p�x��
		}
		else if (trig_point_ph > ph_io) {						//�ڕW�ƌ��ݒl�̕�������v
			chk_ph = trig_point_ph - ph_io;
		}
		else {																	//�ڕW�ƌ��ݒl�̕�������v
			chk_ph = ph_io - trig_point_ph;
		}

		if (chk_ph < pStep->opt_d[STEP_OPT_PHASE_CHK_RANGE]) {	//�ڕW�ʑ��ɓ��B
			pStep->status = STAT_END;
			pseq->direction = ID_REV;
			break;
		}

		//�t�]�J�n�p�ʑ�����
		chk_ph = PI360;
		trig_point_ph = pStep->opt_d[STEP_OPT_PHASE_REV];
		if ((trig_point_ph <= 0) && (ph_io >= 0)) {	//�ڕW�����@���ݒl����
			chk_ph = ph_io - trig_point_ph;
			if (chk_ph >= PI180) chk_ph = PI360 - chk_ph;	//���������̊p�x��
		}
		else if ((trig_point_ph >= 0) && (ph_io <= 0)) {	//�ڕW�����@���ݒl����
			chk_ph = trig_point_ph - ph_io;
			if (chk_ph >= PI180) chk_ph = PI360 - chk_ph;	//���������̊p�x��
		}
		else if (trig_point_ph > ph_io) {
			chk_ph = trig_point_ph - ph_io;
		}
		else {
			chk_ph = ph_io - trig_point_ph;
		}
		if (chk_ph < pStep->opt_d[STEP_OPT_PHASE_CHK_RANGE]) {	//�ڕW�ʑ��ɓ��B
			pStep->status = STAT_END;
			pseq->direction = ID_FWD;
			break;
		}

		v_out = 0.0;

	}break;
	//#	���ԑҋ@
	case CTR_TYPE_WAIT_TIME: {
		pStep->status = STAT_ACTIVE;
		if (pStep->act_count >= pStep->time_count) {
			pStep->status = STAT_END;
		}
		v_out = pStep->_v;
	}break;
	//#	�w�莞�ԑ��x�o��
	case CTR_TYPE_VOUT_TIME: {
		pStep->status = STAT_ACTIVE;
		if (pStep->act_count >= pStep->time_count) {
			pStep->status = STAT_END;
		}

		if (pseq->direction == ID_FWD) {
			if (pStep->_v < 0.0) v_out = pStep->_v * -1.0;
			else v_out = pStep->_v;
		}
		else if (pseq->direction == ID_REV) {
			if (pStep->_v > 0.0) v_out = pStep->_v * -1.0;
			else v_out = pStep->_v;
		}
		else v_out = 0.0;

	}break;
	//#	�U��~�߈ʒu���킹
	case CTR_TYPE_FB_SWAY_POS: {
		pStep->status = STAT_ACTIVE;
		if (pStep->act_count > pStep->time_count) {
			pStep->status = STAT_END;
		}
		v_out = cal_step(&AgentInf_workbuf.st_as_comset,motion);
		if(pCom->seq[motion].seq_status == STAT_END) pStep->status = STAT_END;
	}break;
	//#	���ʒu���B�҂�
	case CTR_TYPE_WAIT_POS_HST: {				//�����ʒu���ڕW-������̈ʒu�ɗ���܂őҋ@
		if (pEnv->cal_dist4target(ID_HOIST, false) < AGENT_CHECK_HST_POS_CLEAR_RANGE) {
			pStep->status = STAT_END;
		}
		else {
			pStep->status = STAT_ACTIVE;
		}
		v_out = 0.0;
	
	}break;
	//#	�����ʒu���B�҂�	
	case CTR_TYPE_WAIT_POS_BH: {
		double d = pEnv->cal_dist4target(ID_BOOM_H, true);
		if (motion == ID_SLEW) {
			if (pCom->seq[ID_BOOM_H].direction != ID_REV)	pStep->status = STAT_END;//�����łȂ����͑ҋ@����
			else if (d < AGENT_CHECK_BH_POS_CLEAR_SLW_RANGE)	pStep->status = STAT_END;
			else pStep->status = STAT_ACTIVE;
		}
		else if (motion == ID_HOIST){
			if(d< AGENT_CHECK_BH_POS_CLEAR_HST_DOWN_RANGE) pStep->status = STAT_END;
			else pStep->status = STAT_ACTIVE;
		}
		else {
			pStep->status = STAT_END;
		}
		v_out = 0.0;
	}break;
	//#	����ʒu���B�҂�
	case CTR_TYPE_WAIT_POS_SLW: {
		double d = pEnv->cal_dist4target(ID_SLEW, true);
		if (motion == ID_BOOM_H) {
			if(pCom->seq[ID_BOOM_H].direction == ID_REV)			pStep->status = STAT_END;//�������͑ҋ@����
			else if (d < AGENT_CHECK_SLW_POS_CLEAR_BH_RANGE_rad)	pStep->status = STAT_END;//���񂪖ڕW�ʒu�t�ߓ��B�őҋ@����
			else pStep->status = STAT_ACTIVE;
		}
		else if (motion == ID_HOIST) {
			if (d < AGENT_CHECK_SLW_POS_CLEAR_HST_DOWN_RANGE_rad) pStep->status = STAT_END;
			else pStep->status = STAT_ACTIVE;
		}
		else {
			pStep->status = STAT_END;
		}
		v_out = 0.0;
	}break;
	//#	�����҂�
	case CTR_TYPE_WAIT_LAND: {
		pStep->status = STAT_ACTIVE;
		v_out = 0.0;
	}break;
	//#	�w�葬�x���B�܂ő��x�o��
	case CTR_TYPE_VOUT_V: {//���xFB���B
		pStep->status = STAT_ACTIVE;
		v_out = pStep->_v;

		double v1percent = pCraneStat->spec.notch_spd_f[motion][5] * 0.01; //1%���x
		double dv = v_out - pPLC_IO->v_fb[motion],dv_abs;
		if (dv < 0.0)dv_abs = -dv;
		else dv_abs = dv;

		if ((v_out > 0.0) && (dv < 0.0)) {
			pStep->status = STAT_END;
		}
		else if ((v_out < 0.0) && (dv > 0.0)) {
			pStep->status = STAT_END;

			//�����������͈ʒu���`�F�b�N
			if (pPLC_IO->pos[ID_HOIST] < pStep->_p)
				pStep->status = STAT_END;
		}
		else if (dv_abs < v1percent) {
			pStep->status = STAT_END;
		}
		else;

		if (pStep->act_count >= pStep->time_count) {
			pStep->status = STAT_END;
		}
	}break;
	//#	�w��ʒu���B�܂Ŏw�葬�x�o��
	case CTR_TYPE_VOUT_POS: {
		pStep->status = STAT_ACTIVE;
		v_out = pStep->_v;

		if (pseq->direction >= ID_STOP) {					//�ړ������@�{
			if(pPLC_IO->pos[motion] >= pStep->_p)		//�ڕW�ʒu���z����
				pStep->status = STAT_END;
		}
		else {													//�ړ������@-
			if (pPLC_IO->pos[motion] < pStep->_p)		//�ڕW�ʒu���z����
				pStep->status = STAT_END;
		}

		if (pStep->act_count >= pStep->time_count) {			//�^�C���I�[�o�[
			pStep->status = STAT_END;
		}
	}break;
	//#	�w�葬�x�o�́i�ڕW�ʒu�܂��͐U��}���ʑ����B�܂Ŏw�葬�x�o�́j
	case CTR_TYPE_VOUT_PHASE: {
		pStep->status = STAT_ACTIVE;
		v_out = 0.0;
	}break;
	//#	�����܂Ŏw�葬�x�o��
	case CTR_TYPE_VOUT_LAND: {
		pStep->status = STAT_ACTIVE;
		v_out = 0.0;
	}break;
	//#	�w�莞�ԉ����x�o��	
	case CTR_TYPE_AOUT_TIME: {
		pStep->status = STAT_ACTIVE;
		v_out = 0.0;
	}break;
	//#	�w�葬�x���B�܂ŉ����x�o��
	case CTR_TYPE_AOUT_V: {
		pStep->status = STAT_ACTIVE;
		v_out = 0.0;
	}break;
	//#	�w��ʒu���B�܂ŉ����x�o��
	case CTR_TYPE_AOUT_POS: {
		pStep->status = STAT_ACTIVE;
		v_out = 0.0;
	}break;
	//#	�w��ʑ����B�܂ŉ����x�o��
	case CTR_TYPE_AOUT_PHASE: {
		pStep->status = STAT_ACTIVE;
		v_out = 0.0;
	}break;
	//#	�����܂Ŏw������x�o��
	case CTR_TYPE_AOUT_LAND: {
		pStep->status = STAT_ACTIVE;
		v_out = 0.0;
	}break;
	//#	�����ʒu���킹
	case CTR_TYPE_FINE_POS: {
		pStep->status = STAT_ACTIVE;
		double dx = pStep->_p - pPLC_IO->pos[motion];
		if (motion == ID_SLEW) {
			if (dx > PI180)dx -= PI360;
			else if(dx < -PI180) dx += PI360;
			else;
		}

		if ((dx < pCraneStat->spec.as_pos_level[motion][ID_LV_COMPLE]) && (dx > -pCraneStat->spec.as_pos_level[motion][ID_LV_COMPLE])) {
			pStep->status = STAT_END;
			v_out = 0.0;
		}
		else if (pStep->act_count >= pStep->time_count) {
			pStep->status = STAT_END;
			v_out = 0.0;
		}
		else if (dx > 0.0) {
			v_out = pStep->_v;
		}
		else if (dx < 0.0) {
			v_out = -pStep->_v;
		}
		else {
			v_out = 0.0;
		}
	}break;
	//#	FB�U��~��
	case CTR_TYPE_FB_SWAY: {
		pStep->status = STAT_ACTIVE;
		v_out = 0.0;
	}break;

	default:
		pStep->status = STAT_LOGICAL_ERROR;
		v_out = 0.0;
		break;
	}

	if (pCom->seq[motion].seq_status & STAT_END) {		 //���V�s�o�͊�����
		return 0.0;
	}
		
	if (pStep->status & STAT_END) {
		pseq->i_hot_step++;
		if (pseq->i_hot_step >= pseq->n_step) pCom->seq[motion].seq_status = STAT_END;
	}

	//���j�^�p�o�b�t�@�Z�b�g
	if ((AgentInf_workbuf.auto_on_going & AUTO_TYPE_JOB_MASK)&&(pCom->seq_mode[motion])) {
		AgentInf_workbuf.st_active_com_inf.seq[motion].seq_status = pCom->seq[motion].seq_status;
		AgentInf_workbuf.st_active_com_inf.com_code = pCom->com_code;
		AgentInf_workbuf.st_active_com_inf.seq[motion].i_hot_step = pCom->seq[motion].i_hot_step;
		AgentInf_workbuf.st_active_com_inf.seq[motion].n_step = pCom->seq[motion].n_step;
		AgentInf_workbuf.st_active_com_inf.seq[motion].steps[AgentInf_workbuf.st_active_com_inf.seq[motion].i_hot_step].type = pCom->seq[motion].steps[pCom->seq[motion].i_hot_step].type;
		AgentInf_workbuf.st_active_com_inf.seq[motion].steps[AgentInf_workbuf.st_active_com_inf.seq[motion].i_hot_step].act_count = pCom->seq[motion].steps[pCom->seq[motion].i_hot_step].act_count;
		AgentInf_workbuf.st_active_com_inf.seq[motion].steps[AgentInf_workbuf.st_active_com_inf.seq[motion].i_hot_step].status = pCom->seq[motion].steps[pCom->seq[motion].i_hot_step].status;
	}
	else {
		AgentInf_workbuf.st_active_com_inf.seq[motion].seq_status = STAT_NA;
		AgentInf_workbuf.st_active_com_inf.com_code = pCom->com_code;
		AgentInf_workbuf.st_active_com_inf.seq[motion].i_hot_step = 0;
		AgentInf_workbuf.st_active_com_inf.seq[motion].n_step = 0;
		AgentInf_workbuf.st_active_com_inf.seq[motion].steps[AgentInf_workbuf.st_active_com_inf.seq[motion].i_hot_step].type = CTR_TYPE_WAIT_TIME;
		AgentInf_workbuf.st_active_com_inf.seq[motion].steps[AgentInf_workbuf.st_active_com_inf.seq[motion].i_hot_step].act_count = 0;
		AgentInf_workbuf.st_active_com_inf.seq[motion].steps[AgentInf_workbuf.st_active_com_inf.seq[motion].i_hot_step].status = STAT_NA;
	}
	return v_out;
}

/****************************************************************************/
/*   �^�X�N�ݒ�^�u�p�l���E�B���h�E�̃R�[���o�b�N�֐�                       */
/****************************************************************************/
LRESULT CALLBACK CAgent::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

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
			reset_panel_item_pb(hDlg);
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
void CAgent::set_panel_tip_txt()
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
void CAgent::set_panel_pb_txt() {

	//WCHAR str_func06[] = L"DEBUG";

	//SetDlgItemText(inf.hWnd_opepane, IDC_TASK_FUNC_RADIO6, (LPCWSTR)str_func06);

	return;
};

