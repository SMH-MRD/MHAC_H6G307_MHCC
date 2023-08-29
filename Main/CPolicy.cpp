#include "CPolicy.h"
#include "CAgent.h"
#include "CEnvironment.h"
#include "CClientService.h"
#include "CHelper.h"

//-���L�������I�u�W�F�N�g�|�C���^:
extern CSharedMem* pCraneStatusObj;
extern CSharedMem* pSwayStatusObj;
extern CSharedMem* pPLCioObj;
extern CSharedMem* pSwayIO_Obj;
extern CSharedMem* pOTEioObj;
extern CSharedMem* pCSInfObj;
extern CSharedMem* pPolicyInfObj;
extern CSharedMem* pAgentInfObj;
extern CSharedMem* pJobIO_Obj;


extern vector<void*>	VectpCTaskObj;	//�^�X�N�I�u�W�F�N�g�̃|�C���^
extern ST_iTask g_itask;

/****************************************************************************/
/*   �R���X�g���N�^�@�f�X�g���N�^                                           */
/****************************************************************************/
CPolicy::CPolicy() {
	pPolicyInf = NULL;
	pPLC_IO = NULL;
	pCraneStat = NULL;
	pOTE_IO = NULL;
	pSway_IO = NULL;
	pCSInf = NULL;
	pAgentInf = NULL;
	memset(&st_com_work, 0, sizeof(ST_POLICY_WORK));
	memset(&PolicyInf_workbuf, 0, sizeof(ST_POLICY_INFO));
}

CPolicy::~CPolicy() {

}

/****************************************************************************/
/*   �^�X�N����������                                                       */
/* �@���C���X���b�h�ŃC���X�^���X��������ɌĂт܂��B                       */
/****************************************************************************/

static CAgent* pAgent=NULL;
static CEnvironment* pEnvironment = NULL;
static CClientService* pCS = NULL;

void CPolicy::init_task(void* pobj) {

	//���L�������\���̂̃|�C���^�Z�b�g
	pPolicyInf = (LPST_POLICY_INFO)(pPolicyInfObj->get_pMap());
	pPLC_IO = (LPST_PLC_IO)(pPLCioObj->get_pMap());
	pCraneStat = (LPST_CRANE_STATUS)(pCraneStatusObj->get_pMap());
	pOTE_IO = (LPST_OTE_IO)(pOTEioObj->get_pMap());
	pAgentInf = (LPST_AGENT_INFO)(pAgentInfObj->get_pMap());
	pSway_IO = (LPST_SWAY_IO)(pSwayIO_Obj->get_pMap());
	pCSInf = (LPST_CS_INFO)(pCSInfObj->get_pMap());
	pJob_IO = (LPST_JOB_IO)(pJobIO_Obj->get_pMap());

	pAgent = (CAgent*)VectpCTaskObj[g_itask.agent];
	pEnvironment = (CEnvironment*)VectpCTaskObj[g_itask.environment];
	pCS = (CClientService*)VectpCTaskObj[g_itask.client];

	set_panel_tip_txt();

	inf.is_init_complete = true;
	return;
};

/****************************************************************************/
/*   �^�X�N���������                                                       */
/* �@�^�X�N�X���b�h�Ŗ��������s�����֐�			�@                      */
/****************************************************************************/
void CPolicy::routine_work(void* param) {
	input();
	main_proc();
	output();
};

//����������菇1�@�O���M������
void CPolicy::input() {
	return;
};

//����������菇2�@���C������
void CPolicy::main_proc() {
	return;
}

//����������菇3�@�\��,�M���o�͏���
void CPolicy::output() {
	//���L�������o�͏���
	memcpy_s(pPolicyInf, sizeof(ST_POLICY_INFO), &PolicyInf_workbuf, sizeof(ST_POLICY_INFO));
	//�^�X�N�p�l���ւ̕\���o��
	wostrs << L" --Scan " << dec << inf.period;
	tweet2owner(wostrs.str()); wostrs.str(L""); wostrs.clear();
	return;
};

/****************************************************************************/
/*�@�@COMMAND ����															*/
/****************************************************************************/
// AGENT����̃R�}���h�v������
LPST_COMMAND_SET CPolicy::req_command(LPST_JOB_SET pjob_set) {


	int _i_hot_com = pjob_set->i_hot_com;
	LPST_COMMAND_SET pcom_set;

	if (pjob_set == NULL) return NULL;	//NULL job�ɂ�NULL���^�[��

	if (pjob_set->status & STAT_TRIGED) {							//JOB�̃X�e�[�^�X�����s�҂�
		_i_hot_com = pjob_set->i_hot_com = 0;						//�N�����́A���s���V�s�̂̃C���f�b�N�X�́A0
		pcom_set = setup_job_command(&(pjob_set->recipe[_i_hot_com]), pjob_set->list_id);
		pjob_set->recipe[_i_hot_com].status = STAT_STANDBY;			//�R�}���h�X�e�[�^�X�X�V
	
		pCS->update_job_status(pjob_set, STAT_ACTIVE);				//JOB�̃X�e�[�^�X��ACTIVE�ɍX�V
	}
	else if (pjob_set->status & STAT_ACTIVE) {						//JOB���s������̌Ăяo���� ���̃��V�s���s�҂�
		if (pjob_set->i_hot_com < (pjob_set->n_com - 1)) {			//���̃��V�s�L
			_i_hot_com = pjob_set->i_hot_com++;
			pcom_set = setup_job_command(&(pjob_set->recipe[_i_hot_com]), pjob_set->list_id);
			pjob_set->recipe[_i_hot_com].status = STAT_STANDBY;		//�R�}���h�X�e�[�^�X�X�V
		}
		else {														//�����V�s���@���Ɋ����� 
			_i_hot_com = pjob_set->i_hot_com;
			pcom_set = NULL;										//�����V�s������NULL���^�[��
			pjob_set->recipe[_i_hot_com].status = STAT_END;			//�R�}���h�X�e�[�^�X�X�V
			pCS->update_job_status(pjob_set, STAT_END);				//JOB�̃X�e�[�^�X��NORMAL�@END�X�V
		}
	}
	else {
		pcom_set= NULL;
		pCS->update_job_status(pjob_set, STAT_ABOTED);				//JOB�̃X�e�[�^�X��ABOTE�X�V�@���튮���̎���update_command_status()�����
	}


	if (pcom_set != NULL) {
		//### �R�}���h�R�[�h�Z�b�g
		pcom_set->com_code.i_list = pjob_set->list_id;
		pcom_set->com_code.i_job = pjob_set->id;
		pcom_set->com_code.i_recipe = _i_hot_com;
	}
	return pcom_set;

};

// AGENT����̃R�}���h���s�񍐏���
int CPolicy::update_command_status(LPST_COMMAND_SET pcom, int code) {

	if (pcom == NULL)return STAT_NAK;
	LPST_JOB_SET pjob_set = &pJob_IO->job_list[pcom->com_code.i_list].job[pcom->com_code.i_job];//�R�t��JOB

	//�R�}���h�R�[�h��JOB�QSET�̓��e�ɕs�����ŃG���[
	int _i_recipe = pcom->com_code.i_recipe;
	if (_i_recipe != pjob_set->i_hot_com) return NULL;

	LPST_COM_RECIPE pcom_recipe = &pjob_set->recipe[_i_recipe];
	switch (code) {
	case STAT_END: {
		pcom_recipe->status = code;								//�R�}���h�̃X�e�[�^�X��񍐓��e�ɍX�V
		pCS->update_job_status(pjob_set, STAT_END);				//JOB�̃X�e�[�^�X�X�V
	}break;
	case STAT_ABNORMAL_END: {
		pcom_recipe->status = code;								//�R�}���h�̃X�e�[�^�X��񍐓��e�ɍX�V
		pCS->update_job_status(pjob_set, STAT_ABNORMAL_END);	//JOB�̃X�e�[�^�X�X�V
	}break;
	case STAT_ACTIVE: {
		pCS->update_job_status(pjob_set, STAT_ACTIVE);			//JOB�̃X�e�[�^�X�X�V
		pcom_recipe->status = code;								//�R�}���h�̃X�e�[�^�X��񍐓��e�ɍX�V
	}break;
	case STAT_ABOTED: {
		pcom_recipe->status = code;								//�R�}���h�̃X�e�[�^�X��񍐓��e�ɍX�V
		pCS->update_job_status(pjob_set, STAT_ABOTED);			//JOB�̃X�e�[�^�X�X�V
	}break;
	case STAT_SUSPENDED: {
		pcom_recipe->status = code;								//�R�}���h�̃X�e�[�^�X��񍐓��e�ɍX�V
		pCS->update_job_status(pjob_set, STAT_SUSPENDED);		//JOB�̃X�e�[�^�X�X�V
	}break;
	default: break;
	}
	//return req_command(pjob_set);
	return STAT_ACK;
}

LPST_COMMAND_SET CPolicy::setup_job_command(LPST_COM_RECIPE pcom_recipe, int type) {							//���s���锼�����R�}���h���Z�b�g����

	LPST_COMMAND_SET pcom_set = &pcom_recipe->comset;

	//�������́A���A����A�������Ώ�
	for (int i = 0;i < MOTION_ID_MAX;i++) pcom_set->active_motion[i] = L_OFF;
	if (type == ID_JOBTYPE_SEMI) {
		pcom_set->active_motion[ID_HOIST] = L_ON;
		pcom_set->active_motion[ID_SLEW] = L_ON;
		pcom_set->active_motion[ID_BOOM_H] = L_ON;
	}
	else if (type == ID_JOBTYPE_JOB) {
		pcom_set->active_motion[ID_HOIST] = L_ON;
		pcom_set->active_motion[ID_SLEW] = L_ON;
		pcom_set->active_motion[ID_BOOM_H] = L_ON;
	}
	else;

	set_com_workbuf(pcom_recipe->target);	//�������p�^�[���쐬��Ɨp�\���́ist_com_work�j�Ƀf�[�^��荞��

	bool is_fb_antisway = false;
	if (pCSInf->antisway_mode == L_ON) {
		is_fb_antisway = true;
	}
	//�R�}���h�Z�b�g�ɖڕW�ʒu�Z�b�g
	pcom_set->target = st_com_work.target;
	//����,����,���̃��V�s�Z�b�g�@set_recipe_semiauto_bh(JOB�^�C�v,���V�s�A�h���X,isFB�^�C�v,���V�s�ݒ�����o�b�t�@�A�h���X
	set_recipe_semiauto_bh(ID_JOBTYPE_SEMI, &(pcom_set->recipe[ID_BOOM_H]), is_fb_antisway, &st_com_work);
	set_recipe_semiauto_slw(ID_JOBTYPE_SEMI, &(pcom_set->recipe[ID_SLEW]), is_fb_antisway, &st_com_work);
	set_recipe_semiauto_mh(ID_JOBTYPE_SEMI, &(pcom_set->recipe[ID_HOIST]), is_fb_antisway, &st_com_work);

	return pcom_set;
};

/****************************************************************************/
/*�@�@�ړ��p�^�[�����V�s����												*/
/****************************************************************************/
/* ############################################################################################################################## */
/*   �������V�s�@                                                                                                                 */
/* ############################################################################################################################## */
int CPolicy::set_recipe_semiauto_bh(int jobtype, LPST_MOTION_RECIPE precipe, bool is_fbtype, LPST_POLICY_WORK pwork) {

	//#���V�s�����Z�b�g
	//��ID
	int id = precipe->axis_id = ID_BOOM_H;
	
	//�ړ�����
	precipe->direction = pwork->motion_dir[id];

	double dist_inch_max;
	if (pwork->vmax_abs[id] / pwork->a_abs[id] > pwork->T) {							//�ő呬�x�܂ł̉������Ԃ��U��������傫�����͐U��������̉������Ԃ��C���`���O�ő勗��
		dist_inch_max = pwork->a_abs[id] * pwork->T * pwork->T;
	}
	else {
		dist_inch_max = pwork->vmax_abs[id] * pwork->vmax_abs[id] / pwork->a_abs[id];	//�ő呬�x�܂ł̉������Ԃ��U�������菬������V^2/��
	}

	//�쐬�p�^�[���̃^�C�v   FB����Ȃ��ƂP��̃C���`���O�ňړ��\�ȋ������ŋ��
	double D_abs = pwork->dist_for_target_abs[id];										//�c��ړ�����
	
	int ptn = 0;
	if (is_fbtype) {																	//�C���`���O�ő勗���̌v�Z �ړ��������C���`���O�ő勗����菬�����Ƃ�,FB�L��FB�U��~�߁AFB������2��C���`���O�ړ�
		if (D_abs > dist_inch_max) ptn = PTN_FBSWAY_FULL;
		else ptn = PTN_FBSWAY_AS;
	}
	else {
		if (D_abs > dist_inch_max) ptn = PTN_NON_FBSWAY_FULL;
		else ptn = PTN_NON_FBSWAY_2INCH;
	}
	precipe->motion_type =ptn;

	LPST_MOTION_STEP pelement;
	
	
	//�����x��0.0�̓G���[�@0����h�~
	if (pwork->a_abs[id] == 0.0) return POLICY_PTN_NG;								


	/*### �p�^�[���쐬 ###*/
	precipe->n_step = 0;														// �X�e�b�v�N���A

	/*### STEP0  �ҋ@�@###*/

	switch (ptn) {

	case PTN_NON_FBSWAY_FULL:	//FB���̃t���p�^�[��
	case PTN_NON_FBSWAY_2INCH:	//FB���̃C���`���O�p�^�[��
	case PTN_FBSWAY_AS:			//FB�U��~�߃p�^�[��
	{
		// ���ʒu�҂��@�����ʒu�F���ڕW����-Xm�@�ȏ�ɂȂ�����						
		pelement = &(precipe->steps[precipe->n_step++]);						//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_WAIT_POS_HST;									// �����ʒu�҂�
		pelement->_t = TIME_LIMIT_ERROR_DETECT;									// �^�C���I�[�o�[���~�b�g�l
		pelement->_v = 0.0;														// ���x0
		pelement->_p = pwork->pos[id];											// �ڕW�ʒu
		D_abs = D_abs;															// �c�苗���ύX�Ȃ�


		// ����ʒu�҂��@�����ʒu�F���ڕW����-Xm�@�ȏ�ɂȂ�����  ����F�����o�����͖ڕW�܂ł̋�����X�x�ȉ��A�������ݎ��͏�������						
		pelement = &(precipe->steps[precipe->n_step++]);						//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_WAIT_POS_SLW;									// �����ʒu�҂�
		pelement->_t = TIME_LIMIT_ERROR_DETECT;									// �^�C���I�[�o�[���~�b�g�l
		pelement->_v = 0.0;														// ���x0
		pelement->_p = pwork->pos[id];											// �ڕW�ʒu
		D_abs = D_abs;															// �c�苗���ύX�Ȃ�


	}break;
	 
	case PTN_FBSWAY_FULL:		//FB�L�̃t���p�^�[��
	{																			// ������ʒu�{�ʑ��҂��@�����ʒu�F���ڕW����-Xm�@�ȏ�ɂȂ�����  ����F�����o�����͖ڕW�܂ł̋�����X�x�ȉ��A�������ݎ��͏��������A�����ʑ����B		

		// ���ʒu�҂��@�����ʒu�F���ڕW����-Xm�@�ȏ�ɂȂ�����						
		pelement = &(precipe->steps[precipe->n_step++]);						//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_WAIT_POS_HST;									// �����ʒu�҂�
		pelement->_t = TIME_LIMIT_ERROR_DETECT;									// �^�C���I�[�o�[���~�b�g�l
		pelement->_v = 0.0;														// ���x0
		pelement->_p = pwork->pos[id];											// �ڕW�ʒu
		D_abs = D_abs;															// �c�苗���ύX�Ȃ�


		// ����ʒu�҂��@�����ʒu�F���ڕW����-Xm�@�ȏ�ɂȂ�����  ����F�����o�����͖ڕW�܂ł̋�����X�x�ȉ��A�������ݎ��͏�������						
		pelement = &(precipe->steps[precipe->n_step++]);						//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_WAIT_POS_SLW;									// �����ʒu�҂�
		pelement->_t = TIME_LIMIT_ERROR_DETECT;									// �^�C���I�[�o�[���~�b�g�l
		pelement->_v = 0.0;														// ���x0
		pelement->_p = pwork->pos[id];											// �ڕW�ʒu
		D_abs = D_abs;															// �c�苗���ύX�Ȃ�

		pelement = &(precipe->steps[precipe->n_step++]);						// �X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_WAIT_PH_SINGLE;								// �U��ʑ��҂�
		pelement->_t = TIME_LIMIT_ERROR_DETECT;									// �^�C���I�[�o�[���~�b�g�l
		pelement->_v = 0.0;														// ���x0
		pelement->_p = pwork->pos[id];											// �ڕW�ʒu
		D_abs = D_abs;															// �c�苗���ύX�Ȃ�

		pelement->opt_d[STEP_OPT_PHASE_CHK_RANGE] = PARAM_PHASE_CHK_RANGE_BH;	// �ʑ����B����l�Z�b�g

	}break;
	default:return POLICY_PTN_NG;
	}

	/*### STEP1,2 ���x�X�e�b�v�o�́@2�i��###*/
	switch (ptn) {
	case PTN_NON_FBSWAY_FULL:	//FB���̃t���p�^�[��
	case PTN_FBSWAY_FULL:		//FB�L�̃t���p�^�[��		
	//��`�p�^�[���� �������Q�i�K�ɂ��ċ����𒲐�
	{																			// �o�͂���m�b�`���x���v�Z���Đݒ�
		
		double v_top_abs = 0.0;													//�X�e�b�v���x�p
		double d_move_abs = 0.0, d_accdec, ta, tcmax;
		int n = 0, i;

		// #Step1-1 �Q�i�\���ɂȂ�Ƃ��̂P�i��
		// �܂��́A�ړ�������1�����U��~�߂Əo����Top���x�����߂�

		for (i = (NOTCH_MAX - 1);i > 0;i--) {
			v_top_abs = pCraneStat->spec.notch_spd_f[id][i];
			ta = v_top_abs / st_com_work.a_abs[id];
			d_accdec = v_top_abs * ta;											//��������
			tcmax = (D_abs - d_accdec) / v_top_abs;
			n = (int)((ta + tcmax) / st_com_work.T);

			if ((tcmax < 0.0) || (n < 1)) continue;

			pelement = &(precipe->steps[precipe->n_step++]);					//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
			pelement->type = CTR_TYPE_VOUT_POS;									//�ʒu���B�҂��X�e�b�v�o��
			pelement->_t = (double)n * st_com_work.T;							// n x �U�����

			d_move_abs = (double)n * v_top_abs * st_com_work.T;					// ��`�ړ����� 

			if (precipe->direction == ID_REV) {
				pelement->_p = (pelement - 1)->_p - d_move_abs + 0.5 * d_accdec;// �ڕW�ʒu
				pelement->_v = -v_top_abs;										// �o�͑��x
			}
			else {
				pelement->_p = (pelement - 1)->_p + d_move_abs - 0.5 * d_accdec;// �ڕW�ʒu
				pelement->_v = v_top_abs;										// �o�͑��x
			}
			break;
		}

		//  #Step1-2�Q�i��

		// �m�b�`�I���͑�������
		double d_move_abs2 = 0.0;
		double v_second_abs = 0.0;

		for (;i > 0;i--) {
			v_second_abs = pCraneStat->spec.notch_spd_f[id][i];

			n = (int)((D_abs - d_move_abs) / (v_second_abs * st_com_work.T));	//�ǉ��\�c�苗���͌��������̍l���K�v�@1�����U��~�߂̈ړ������@=nTV
			if (n) {
				pelement = &(precipe->steps[precipe->n_step++]);		//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
				pelement->type = CTR_TYPE_VOUT_POS;						//�ʒu���B�҂��X�e�b�v�o��


				double temp_t = (v_top_abs - v_second_abs) / st_com_work.a_abs[id];
				pelement->_t = (double)n * st_com_work.T + temp_t;				// n x �U�����
				double temp_d_abs = 0.5 * (v_top_abs + v_second_abs) * (v_top_abs - v_second_abs) / st_com_work.a_abs[id];	// �葬�ړ������i�U������̐����{�ړ��j
				d_move_abs2 = (double)n * st_com_work.T * v_second_abs + temp_d_abs;

				if (precipe->direction == ID_REV) {
					pelement->_p = (pelement - 1)->_p - temp_d_abs - d_move_abs2;		// �ڕW�ʒu
					pelement->_v = -v_second_abs;							// �o�͑��x
				}
				else {
					pelement->_p = (pelement - 1)->_p + temp_d_abs + d_move_abs2;		// �ڕW�ʒu
					pelement->_v = v_second_abs;							// �o�͑��x
		}


		break;
	}
	}


		//  #Step2 ��~
		pelement = &(precipe->steps[precipe->n_step++]);			//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_VOUT_V;							//���x���B�҂�
		pelement->_t = (pelement - 1)->_v / st_com_work.a_abs[id];	//��������
		if (pelement->_t < 0.0)pelement->_t *= -1.0;

		pelement->_v = 0.0;											// ���x0

		double d_dec = 0.5 * (pelement - 1)->_v * (pelement - 1)->_v / st_com_work.a_abs[id];

		if (precipe->direction == ID_REV) {
			pelement->_p = (pelement - 1)->_p - d_dec;			// �ڕW�ʒu
		}
		else {
			pelement->_p = (pelement - 1)->_p + d_dec;			// �ڕW�ʒu
		}
		pelement->_v = 0.0;								// �o�͑��x
		CHelper::fit_ph_range_upto_pi(&(pelement->_p));					//�ڕW�ʒu�̍Z��
		D_abs = D_abs - d_move_abs - d_move_abs2;						// �c�苗���X�V
																			
	}break;

	//��`�������P�[�X�̓X�L�b�v
	case PTN_NON_FBSWAY_2INCH:	//FB���̃C���`���O�p�^�[��												
	case PTN_FBSWAY_AS:			//FB�U��~�߃p�^�[��	
	{
		D_abs = D_abs;
	}break;
	default:return POLICY_PTN_NG;
	}

	/*### STEP3,4,5,6  2��C���`���O�ړ����@###*/
	switch (ptn) {
	case PTN_NON_FBSWAY_FULL:	//FB���̃t���p�^�[��	
	case PTN_NON_FBSWAY_2INCH:	//FB���̃C���`���O�p�^�[��	
	{
		double v_inch = sqrt(0.5 * D_abs * st_com_work.a_abs[id]);
		double ta = v_inch/st_com_work.a_abs[id];
		double v_top_abs;
		for (int i = (NOTCH_MAX-1);i > 1;i--) {	
			v_top_abs = pCraneStat->spec.notch_spd_f[id][i];
			if (v_inch > pCraneStat->spec.notch_spd_f[id][i - 1])break;
			else continue;			//���̃m�b�`��
		}

		double d_move_abs = 0.5 * ta * v_inch;
		//STEP3
		pelement = &(precipe->steps[precipe->n_step++]);						// �X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_VOUT_TIME;									// �����p���x�o��
		pelement->_t = ta;														// ��������
		if (precipe->direction == ID_REV) {
			pelement->_p = (pelement - 1)->_p - d_move_abs;					// �ڕW�ʒu
			pelement->_v = -v_top_abs;										// �o�͑��x
		}
		else {
			pelement->_p = (pelement - 1)->_p + d_move_abs;					// �ڕW�ʒu
			pelement->_v = v_top_abs;										// �o�͑��x
		}
		D_abs -= d_move_abs;															

		//STEP4
		double tc = 0.5 * st_com_work.T - 2.0 * ta;
		if (tc < 0.0) {

			int n =(int)(-tc / st_com_work.T)+1;
			tc = tc + (double)(n* st_com_work.T);
		} 
		
		pelement = &(precipe->steps[precipe->n_step++]);						// �X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_VOUT_TIME;									// �����p���x�o��
		pelement->_t = ta + tc;													// �ʑ��҂���~����
		pelement->_v = 0.0;														// �m�b�`���x
		if (precipe->direction == ID_REV) {
			pelement->_p = (pelement - 1)->_p - d_move_abs;					// �ڕW�ʒu
		}
		else {
			pelement->_p = (pelement - 1)->_p + d_move_abs;					// �ڕW�ʒu
		}
		D_abs -= d_move_abs;

		//STEP5
		pelement = &(precipe->steps[precipe->n_step++]);						// �X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_VOUT_TIME;									// �����p���x�o��
		pelement->_t = ta;														// ��������
		if (precipe->direction == ID_REV) {
			pelement->_p = (pelement - 1)->_p - d_move_abs;					// �ڕW�ʒu
			pelement->_v = -v_top_abs;										// �o�͑��x
		}
		else {
			pelement->_p = (pelement - 1)->_p + d_move_abs;					// �ڕW�ʒu
			pelement->_v = v_top_abs;										// �o�͑��x
		}
		D_abs -= d_move_abs;

		//STEP6
		pelement = &(precipe->steps[precipe->n_step++]);						// �X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_VOUT_TIME;									// �����p���x�o��
		pelement->_t = ta;														// �ʑ��҂���~����
		pelement->_v = 0.0;														// �m�b�`���x
		pelement->_p = st_com_work.target.pos[id];								// �ڕW�ʒu
		D_abs = 0.0;
	}break;

	case PTN_FBSWAY_AS:															//�U��FB����p�^�[���̓X�L�b�v
	case PTN_FBSWAY_FULL:														
	{
		D_abs = D_abs;																	//�c�苗���ύX�Ȃ�
	}break;
	default:return POLICY_PTN_NG;
	}

	/*### STEP7  ###*/
	switch (ptn) {
	case PTN_NON_FBSWAY_FULL:	//FB���̃t���p�^�[��													
	case PTN_NON_FBSWAY_2INCH:	//FB���̃C���`���O�p�^�[��	
	{//�����ʒu����
		pelement = &(precipe->steps[precipe->n_step++]);						// �X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_FINE_POS;										// �����ʒu����
		pelement->_t = FINE_POS_TIMELIMIT;										// �ʒu���킹�ő�p������
		pelement->_v = pCraneStat->spec.notch_spd_f[id][NOTCH_1];				// �P�m�b�`���x
		pelement->_p = st_com_work.target.pos[id];								// �ڕW�ʒu
		D_abs = 0.0;															// �c�苗���ύX�Ȃ�
	}break;
	case PTN_FBSWAY_AS:			//FB�U�~�p�^�[��	
	case PTN_FBSWAY_FULL:		//FB�L�̃t���p�^�[��	
	{
		pelement = &(precipe->steps[precipe->n_step++]);						// �X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_FB_SWAY_POS;									// FB�U��~��/�ʒu����
		pelement->_t = st_com_work.T * 4.0;										// �U��4������
		pelement->_v = 0.0;														// �U��~�߃��W�b�N�Ō���
		pelement->_p = st_com_work.target.pos[id];								// �ڕW�ʒu
		D_abs = 0;																// �c�苗���ύX�Ȃ�
	}break;
	default:return POLICY_PTN_NG;
	}
	//���ԏ����̃X�L�����J�E���g�l�Z�b�g
	for (int i = 0;i < precipe->n_step;i++) {
		precipe->steps[i].time_count = (int)(precipe->steps[i]._t / pwork->agent_scan);
	}


	return POLICY_PTN_OK;
}

/* ############################################################################################################################## */
/*   ���񃌃V�s�@                                                                                                                 */
/* ############################################################################################################################## */
int CPolicy::set_recipe_semiauto_slw(int jobtype, LPST_MOTION_RECIPE precipe, bool is_fbtype, LPST_POLICY_WORK pwork) {

	//#���V�s�����Z�b�g
	//��ID
	int id = precipe->axis_id = ID_SLEW;

	//�ړ�����
	precipe->direction = pwork->motion_dir[id];

	double dist_inch_max;
	if (pwork->vmax_abs[id] / pwork->a_abs[id] > pwork->T) {								//�ő呬�x�܂ł̉������Ԃ��U��������傫�����͐U��������̉������Ԃ��C���`���O�ő勗��
		dist_inch_max = pwork->a_abs[id] * pwork->T * pwork->T;
	}
	else {
		dist_inch_max = pwork->vmax_abs[id] * pwork->vmax_abs[id] / pwork->a_abs[id];		//�ő呬�x�܂ł̉������Ԃ��U�������菬������V^2/��
	}

	//�쐬�p�^�[���̃^�C�v   FB����Ȃ��ƂP��̃C���`���O�ňړ��\�ȋ������ŋ��
	double D_abs = pwork->dist_for_target_abs[id];											//�c��ړ�����
	int ptn = 0;
	if (is_fbtype) {																		//�C���`���O�ő勗���̌v�Z �ړ��������C���`���O�ő勗����菬�����Ƃ�,FB�L��FB�U��~�߁AFB������2��C���`���O�ړ�
		if (D_abs > dist_inch_max) ptn = PTN_FBSWAY_FULL;
		else ptn = PTN_FBSWAY_AS;
	}
	else {
		if (D_abs > dist_inch_max) ptn = PTN_NON_FBSWAY_FULL;
		else ptn = PTN_NON_FBSWAY_2INCH;
	}
	precipe->motion_type = ptn;

	LPST_MOTION_STEP pelement;


	//�����x��0.0�̓G���[�@0����h�~
	if (pwork->a_abs[id] == 0.0) return POLICY_PTN_NG;


	/*### �p�^�[���쐬 ###*/
	precipe->n_step = 0;														// �X�e�b�v�N���A

	/*### STEP0  �ҋ@�@###*/

	switch (ptn) {
	case PTN_NON_FBSWAY_FULL:		//FB���̃t���p�^�[��
	case PTN_NON_FBSWAY_2INCH:		//FB���̃C���`���O
	case PTN_FBSWAY_AS:				//FB�U�~
	{																			// ���A�����ʒu�҂��@  
		pelement = &(precipe->steps[precipe->n_step++]);						//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_WAIT_POS_HST;									// ���ʒu�҂��@�����ʒu�F���ڕW����-Xm�@�ȏ�ɂȂ�����
		pelement->_t = TIME_LIMIT_ERROR_DETECT;									// �^�C���I�[�o�[���~�b�g�l
		pelement->_v = 0.0;														// ���x0
		pelement->_p = pwork->pos[id];											// �ڕW�ʒu���݈ʒu
		CHelper::fit_ph_range_upto_pi(&(pelement->_p));							//�ڕW�ʒu�̍Z���i-180���`180���̕\���ɂ���
		D_abs = D_abs;															// �c�苗���ύX�Ȃ�

		pelement = &(precipe->steps[precipe->n_step++]);						//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_WAIT_POS_BH;									// �������ʒu�҂��@�����F�����o�����͏��������A�������ݎ��͈����ʒu���ڕW�{X���ȉ�
		pelement->_t = TIME_LIMIT_ERROR_DETECT;									// �^�C���I�[�o�[���~�b�g�l
		pelement->_v = 0.0;														// ���x0
		pelement->_p = pwork->pos[id];											// �ڕW�ʒu���݈ʒu
		CHelper::fit_ph_range_upto_pi(&(pelement->_p));								//�ڕW�ʒu�̍Z���i-180���`180���̕\���ɂ���
		D_abs = D_abs;																	// �c�苗���ύX�Ȃ�

	}break;

	case PTN_FBSWAY_FULL: 			//FB�L�̃t���p�^�[��
	{	

		// ������,�ʒu�{�ʑ��҂��@�����ʒu�F���ڕW����-Xm�@�ȏ�ɂȂ�����  �����F�����o�����͏��������A�������ݎ��͈����ʒu���ڕW�{X���ȉ�
		pelement = &(precipe->steps[precipe->n_step++]);						//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_WAIT_POS_HST;									// ���ʒu�҂��@�����ʒu�F���ڕW����-Xm�@�ȏ�ɂȂ�����
		pelement->_t = TIME_LIMIT_ERROR_DETECT;									// �^�C���I�[�o�[���~�b�g�l
		pelement->_v = 0.0;														// ���x0
		pelement->_p = pwork->pos[id];											// �ڕW�ʒu���݈ʒu
		CHelper::fit_ph_range_upto_pi(&(pelement->_p));								//�ڕW�ʒu�̍Z���i-180���`180���̕\���ɂ���
		D_abs = D_abs;															// �c�苗���ύX�Ȃ�

		pelement = &(precipe->steps[precipe->n_step++]);						//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_WAIT_POS_BH;									// �������ʒu�҂��@�����F�����o�����͏��������A�������ݎ��͈����ʒu���ڕW�{X���ȉ�
		pelement->_t = TIME_LIMIT_ERROR_DETECT;									// �^�C���I�[�o�[���~�b�g�l
		pelement->_v = 0.0;														// ���x0
		pelement->_p = pwork->pos[id];											// �ڕW�ʒu���݈ʒu
		CHelper::fit_ph_range_upto_pi(&(pelement->_p));								//�ڕW�ʒu�̍Z���i-180���`180���̕\���ɂ���
		D_abs = D_abs;															// �c�苗���ύX�Ȃ�
		
		pelement = &(precipe->steps[precipe->n_step++]);						// �X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_WAIT_PH_SINGLE;								// �ʑ��҂�
		pelement->_t = TIME_LIMIT_ERROR_DETECT;									// �^�C���I�[�o�[���~�b�g�l
		pelement->_v = 0.0;														// ���x0
		pelement->_p = pwork->pos[id];											// �ڕW�ʒu
		CHelper::fit_ph_range_upto_pi(&(pelement->_p));								// �ڕW�ʒu�̍Z���i-180���`180���̕\���ɂ���
		D_abs = D_abs;																	// �c�苗���ύX�Ȃ�

		pelement->opt_d[STEP_OPT_PHASE_CHK_RANGE] = PARAM_PHASE_CHK_RANGE_SLW;
	}break;
	default:return POLICY_PTN_NG;
	}

	/*### STEP1,2 ���x�X�e�b�v�o�́@1,2�i ###*/
	/*### ����ł�FB�p�^�[���ł́A�U������X�e�b�v�p�^�[���͖����A�����^�C�~���O�𒲐����Ō��FB�U��~�߃p�^�[�������� ###*/

	switch (ptn) {
	case PTN_NON_FBSWAY_FULL:									// �����FB�Ȃ��̎��̂ݐU������p�^�[��
	{
		double v_top_abs = 0.0;									//�X�e�b�v���x�p
		double d_move_abs=0.0, d_accdec,ta,tcmax;
		int n = 0, i;


		// #Step1-1 �Q�i�\���ɂȂ�Ƃ��̂P�i��
		// �܂��́A�ړ�������1�����U��~�߂Əo����Top���x�����߂�
		for (i = (NOTCH_MAX - 1);i > 0;i--) {
			v_top_abs = pCraneStat->spec.notch_spd_f[id][i];
			ta = v_top_abs / st_com_work.a_abs[id];
			d_accdec =  v_top_abs * ta;									//��������
			tcmax = (D_abs - d_accdec)/v_top_abs ;
			n = (int)((ta + tcmax) / st_com_work.T);
			
			if((tcmax < 0.0) || (n <1)) continue;

			pelement = &(precipe->steps[precipe->n_step++]);		//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
			pelement->type = CTR_TYPE_VOUT_POS;						//�ʒu���B�҂��X�e�b�v�o��
			pelement->_t = (double)n * st_com_work.T;				// n x �U�����

			d_move_abs = (double)n * v_top_abs * st_com_work.T;	// ��`�ړ����� 

			if (precipe->direction == ID_REV) {
				pelement->_p = (pelement - 1)->_p - d_move_abs + 0.5* d_accdec;	// �ڕW�ʒu
				pelement->_v = -v_top_abs;										// �o�͑��x
			}
			else {
				pelement->_p = (pelement - 1)->_p + d_move_abs - 0.5 * d_accdec;// �ڕW�ʒu
				pelement->_v = v_top_abs;										// �o�͑��x
			}
			CHelper::fit_ph_range_upto_pi(&(pelement->_p));							//�ڕW�ʒu�̍Z��
			break;
		}

		//  #Step1-2�Q�i��

		// �m�b�`�I���͑�������
		double d_move_abs2 = 0.0;
		double v_second_abs = 0.0;

		for (;i > 0;i--) {
			v_second_abs = pCraneStat->spec.notch_spd_f[id][i];

			n = (int)((D_abs- d_move_abs) / (v_second_abs * st_com_work.T));	//�ǉ��\�c�苗���͌��������̍l���K�v�@1�����U��~�߂̈ړ������@=nTV
			if (n) {
				pelement = &(precipe->steps[precipe->n_step++]);		//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
				pelement->type = CTR_TYPE_VOUT_POS;						//�ʒu���B�҂��X�e�b�v�o��


				double temp_t = (v_top_abs - v_second_abs) / st_com_work.a_abs[id];
				pelement->_t = (double)n * st_com_work.T + temp_t;				// n x �U�����
				double temp_d_abs = 0.5* (v_top_abs + v_second_abs) * (v_top_abs - v_second_abs)/ st_com_work.a_abs[id];	// �葬�ړ������i�U������̐����{�ړ��j
				d_move_abs2 = (double)n * st_com_work.T * v_second_abs;


				if (precipe->direction == ID_REV) {
					pelement->_p = (pelement - 1)->_p - d_move_abs2 - temp_d_abs;		// �ڕW�ʒu
					pelement->_v = -v_second_abs;										// �o�͑��x
				}
				else {
					pelement->_p = (pelement - 1)->_p + d_move_abs2 + temp_d_abs;		// �ڕW�ʒu
					pelement->_v = v_second_abs;										// �o�͑��x
				}
				CHelper::fit_ph_range_upto_pi(&(pelement->_p));					//�ڕW�ʒu�̍Z��
				break;
			}
		}


		//  #Step2 ��~
		pelement = &(precipe->steps[precipe->n_step++]);			//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_VOUT_V;							//���x���B�҂�
		pelement->_t = (pelement - 1)->_v / st_com_work.a_abs[id];	//��������
		if (pelement->_t < 0.0)pelement->_t *= -1.0;

		pelement->_v = 0.0;											// ���x0

		double d_dec = 0.5 * (pelement - 1)->_v * (pelement - 1)->_v / st_com_work.a_abs[id];

		if (precipe->direction == ID_REV) {
			pelement->_p = (pelement - 1)->_p - d_dec;			// �ڕW�ʒu
		}
		else {
			pelement->_p = (pelement - 1)->_p + d_dec;			// �ڕW�ʒu
		}
		pelement->_v = 0.0;								// �o�͑��x
		CHelper::fit_ph_range_upto_pi(&(pelement->_p));					//�ڕW�ʒu�̍Z��
		D_abs = D_abs - d_move_abs - d_move_abs2;						// �c�苗���X�V
	}break;

	case PTN_FBSWAY_FULL:															// �����FB����̎��͐U��~�ߖ���1�i�̂݁F�����^�C�~���O�Œ���
	{
		double v_top_abs = 0.0;									//�X�e�b�v���x�p
		double d_move_abs = 0.0, d_accdec, ta, tcmax;
		int n = 0, i;

		for (i = (NOTCH_MAX - 1);i > 0;i--) {
			v_top_abs = pCraneStat->spec.notch_spd_f[id][i];
			ta = v_top_abs / st_com_work.a_abs[id];
			d_accdec = v_top_abs * ta;									//��������
			tcmax = (D_abs - d_accdec) / v_top_abs;
	
			if (tcmax < 0.0) continue;

			pelement = &(precipe->steps[precipe->n_step++]);		//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
			pelement->type = CTR_TYPE_VOUT_POS;						//�ʒu���B�҂��X�e�b�v�o��
			pelement->_t = ta + tcmax;				

			d_move_abs = v_top_abs * (ta + tcmax);					// ��`�ړ����� 

			if (precipe->direction == ID_REV) {
				pelement->_p = (pelement - 1)->_p - d_move_abs + 0.5 * d_accdec;	// �ڕW�ʒu
				pelement->_v = -v_top_abs;										// �o�͑��x
			}
			else {
				pelement->_p = (pelement - 1)->_p + d_move_abs - 0.5 * d_accdec;// �ڕW�ʒu
				pelement->_v = v_top_abs;										// �o�͑��x
			}
			CHelper::fit_ph_range_upto_pi(&(pelement->_p));							//�ڕW�ʒu�̍Z��
			break;
		}

		//  #Step2 ��~
		pelement = &(precipe->steps[precipe->n_step++]);			//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_VOUT_V;							//���x���B�҂�
		pelement->_t = (pelement - 1)->_v / st_com_work.a_abs[id];	//��������
		if (pelement->_t < 0.0)pelement->_t *= -1.0;

		pelement->_v = 0.0;											// ���x0

		double d_dec = 0.5 * (pelement - 1)->_v * (pelement - 1)->_v / st_com_work.a_abs[id];

		if (precipe->direction == ID_REV) {
			pelement->_p = (pelement - 1)->_p - d_dec;			// �ڕW�ʒu
		}
		else {
			pelement->_p = (pelement - 1)->_p + d_dec;			// �ڕW�ʒu
		}
		pelement->_v = 0.0;										// �o�͑��x
		CHelper::fit_ph_range_upto_pi(&(pelement->_p));				//�ڕW�ʒu�̍Z��
		D_abs = D_abs - d_move_abs ;							// �c�苗���X�V

	}break;

	case PTN_NON_FBSWAY_2INCH:													//��`�������P�[�X�̓X�L�b�v
	case PTN_FBSWAY_AS:
	{
		D_abs = D_abs;
	}break;
	default:return POLICY_PTN_NG;
	}
	/*### STEP3,4,5,6  FB�����̃C���`���O�ړ��p�^�[���@###*/
	switch (ptn) {
	case PTN_NON_FBSWAY_FULL:													
	case PTN_NON_FBSWAY_2INCH:
	{
		double v_inch = sqrt(0.5 * D_abs * st_com_work.a_abs[id]);
		double ta = v_inch / st_com_work.a_abs[id];
		double v_top_abs;
		for (int i = (NOTCH_MAX - 1);i > 1;i--) {
			v_top_abs = pCraneStat->spec.notch_spd_f[id][i];
			if (v_inch > pCraneStat->spec.notch_spd_f[id][i - 1])break;
			else continue;			//���̃m�b�`��
		}

		double d_move_abs = 0.5 * ta * v_inch;

		//STEP3
		pelement = &(precipe->steps[precipe->n_step++]);						// �X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_VOUT_TIME;									// �����p���x�o��
		pelement->_t = ta;														// ��������
		if (precipe->direction == ID_REV) {
			pelement->_p = (pelement - 1)->_p - d_move_abs;					// �ڕW�ʒu
			pelement->_v = -v_top_abs;										// �o�͑��x
		}
		else {
			pelement->_p = (pelement - 1)->_p + d_move_abs;					// �ڕW�ʒu
			pelement->_v = v_top_abs;										// �o�͑��x
		}
			
		CHelper::fit_ph_range_upto_pi(&(pelement->_p));										//�ڕW�ʒu�̍Z��
		D_abs -= d_move_abs;

		//STEP4
		double tc = 0.5 * st_com_work.T - 2.0 * ta;
		if (tc < 0.0) {
			int n = (int)(-tc / st_com_work.T) + 1;
			tc += (double)n * st_com_work.T;
		}

		pelement = &(precipe->steps[precipe->n_step++]);						// �X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_VOUT_TIME;									// �����p���x�o��
		pelement->_t = ta + tc;													// �ʑ��҂���~����
		pelement->_v = 0.0;														// �m�b�`���x
		if (precipe->direction == ID_REV) {
			pelement->_p = (pelement - 1)->_p - d_move_abs;					// �ڕW�ʒu
		}
		else {
			pelement->_p = (pelement - 1)->_p + d_move_abs;					// �ڕW�ʒu
		}
		CHelper::fit_ph_range_upto_pi(&(pelement->_p));										//�ڕW�ʒu�̍Z��
		D_abs -= d_move_abs;

		//STEP5
		pelement = &(precipe->steps[precipe->n_step++]);						// �X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_VOUT_TIME;									// �����p���x�o��
		pelement->_t = ta;														// ��������
		if (precipe->direction == ID_REV) {
			pelement->_p = (pelement - 1)->_p - d_move_abs;					// �ڕW�ʒu
			pelement->_v = -v_top_abs;										// �o�͑��x
		}
		else {
			pelement->_p = (pelement - 1)->_p + d_move_abs;					// �ڕW�ʒu
			pelement->_v = v_top_abs;										// �o�͑��x
		}

		CHelper::fit_ph_range_upto_pi(&(pelement->_p));										//�ڕW�ʒu�̍Z��
		D_abs -= d_move_abs;

		//STEP6
		pelement = &(precipe->steps[precipe->n_step++]);						// �X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_VOUT_TIME;									// �����p���x�o��
		pelement->_t = ta;														// �ʑ��҂���~����
		pelement->_v = 0.0;														// �m�b�`���x
		pelement->_p = st_com_work.target.pos[id];								// �ڕW�ʒu
		CHelper::fit_ph_range_upto_pi(&(pelement->_p));								//�ڕW�ʒu�̍Z��
		D_abs = 0.0;

	}break;

	case PTN_FBSWAY_AS:															//�U��FB����p�^�[���̓X�L�b�v
	case PTN_FBSWAY_FULL:
	{
		D_abs = D_abs;																	//�c�苗���ύX�Ȃ�
	}break;
	default:return POLICY_PTN_NG;
	}

	/*### STEP7  ###*/
	switch (ptn) {
	case PTN_NON_FBSWAY_FULL:													//���A����ʒu�҂��@�����ʒu�F���ڕW����-Xm�@�ȏ�ɂȂ�����  ����F�����o�����͖ڕW�܂ł̋�����X�x�ȉ��A�������ݎ��͏�������
	case PTN_NON_FBSWAY_2INCH:
	{
		pelement = &(precipe->steps[precipe->n_step++]);						// �X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_FINE_POS;										// �����ʒu����
		pelement->_t = FINE_POS_TIMELIMIT;										// �ʒu���킹�ő�p������
		pelement->_v = pCraneStat->spec.notch_spd_f[id][NOTCH_1];				// �P�m�b�`���x
		pelement->_p = st_com_work.target.pos[id];								// �ڕW�ʒu
		CHelper::fit_ph_range_upto_pi(&(pelement->_p));										//�ڕW�ʒu�̍Z��
		D_abs = 0;																	// �c�苗���ύX�Ȃ�

	}break;

	case PTN_FBSWAY_AS:
	case PTN_FBSWAY_FULL:														//���A����,�ʒu�ʑ��҂��@�����ʒu�F���ڕW����-Xm�@�ȏ�ɂȂ�����  ����F�����o�����͖ڕW�܂ł̋�����X�x�ȉ��A�������ݎ��͏��������A�����ʑ����B
	{

		pelement = &(precipe->steps[precipe->n_step++]);						//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_FB_SWAY_POS;									// FB�U��~��/�ʒu����
		pelement->_t = st_com_work.T * 4.0;										// �U��4������
		pelement->_v = 0.0;														// �U��~�߃��W�b�N�Ō���
		pelement->_p = st_com_work.target.pos[id];								// �ڕW�ʒu
		CHelper::fit_ph_range_upto_pi(&(pelement->_p));							//�ڕW�ʒu�̍Z��
		D_abs = 0;																// �c�苗���ύX�Ȃ�

		}break;
	default:return POLICY_PTN_NG;
	}

	//���ԏ����̃X�L�����J�E���g�l�Z�b�g
	for (int i = 0;i < precipe->n_step;i++) {
		precipe->steps[i].time_count = (int)(precipe->steps[i]._t / pwork->agent_scan);
	}


	return POLICY_PTN_OK;
}

/* ############################################################################################################################## */
/*   �����V�s�@                                                                                                                 */
/* ############################################################################################################################## */
int CPolicy::set_recipe_semiauto_mh(int jobtype, LPST_MOTION_RECIPE precipe, bool is_fbtype, LPST_POLICY_WORK pwork) {

	//#���V�s�����Z�b�g
	int id = precipe->axis_id = ID_HOIST;										//��ID
	precipe->n_step = 0;														//�X�e�b�v��������
	precipe->direction = pwork->motion_dir[id];									//�ړ�����
	precipe->time_limit = POL_TM_OVER_CHECK_COUNTms /inf.cycle_ms;				//�^�C���I�[�o�[�J�E���g
	precipe->motion_type = PTN_ORDINARY;										//�쐬�p�^�[���̃^�C�v


	//#�p�^�[���v�Z�p�f�[�^�Z�b�g
	double D_abs = pwork->dist_for_target_abs[id];									//�c��ړ��� ��Βl								
	if (pwork->a_abs[id] == 0.0) return POLICY_PTN_NG;							//�����x��0.0�̓G���[�@0����h�~

	/*### �p�^�[���쐬 ###*/
	LPST_MOTION_STEP pelement;

	/*### STEP0  �ҋ@�@###	�����A����ʒu�҂��@���㎞�F���������@�������F �����E���񋤂��ڕW�ʒu�̎w��͈͓� */
	
	//�����������́A����A�������ڕW�t�ߒ��܂őҋ@
	if (precipe->direction == ID_REV) {											
		pelement = &(precipe->steps[precipe->n_step++]);						//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_WAIT_POS_SLW;									//����ʒu�҂�
		pelement->_t = TIME_LIMIT_ERROR_DETECT;									// �^�C���I�[�o�[���~�b�g�l
		pelement->_v = 0.0;														// ���x0
		pelement->_p = pwork->pos[id];											// �ڕW�ʒu�����݈ʒu
		D_abs = D_abs;																	// �c�苗���ύX�Ȃ�

		pelement = &(precipe->steps[precipe->n_step++]);						//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_WAIT_POS_BH;									// �����ʒu�҂�
		pelement->_t = TIME_LIMIT_ERROR_DETECT;									// �^�C���I�[�o�[���~�b�g�l
		pelement->_v = 0.0;														// ���x0
		pelement->_p = pwork->pos[id];											// �ڕW�ʒu�����݈ʒu
		D_abs = D_abs;																	// �c�苗���ύX�Ȃ�
	}
	//���������ȊO�͊m�F�ҋ@
	else {																		//��~�܂��͊����グ���́A�m�F�҂�
		pelement = &(precipe->steps[precipe->n_step++]);						//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
		pelement->type = CTR_TYPE_WAIT_TIME;									// �ҋ@���ԑ҂�
		pelement->_t = TIME_LIMIT_CONFIRMATION;									// �҂�����
		pelement->_v = 0.0;														// ���x0
		pelement->_p = pwork->pos[id];											// �ڕW�ʒu�����݈ʒu
		D_abs = D_abs;																	// �c�苗���ύX�Ȃ�
	}

	/*### STEP1 ���x�X�e�b�v�o�́@###*/

	double v_top = 0.0;																	//�X�e�b�v���x�p
	double check_d_abs, d_time_delay=0.0;
	int n = 0, i;

	pelement = &(precipe->steps[precipe->n_step++]);									//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
	pelement->type = CTR_TYPE_VOUT_POS;													//�ʒu���B�҂��葬�o��

	double ta = 0.0, v_top_abs = 0.0;																	//��������,�葬����

	for (i = (NOTCH_MAX - 1);i > 0;i--) {
		if (precipe->direction == ID_REV)	v_top = pCraneStat->spec.notch_spd_r[id][i];
		else								v_top = pCraneStat->spec.notch_spd_f[id][i];

		v_top_abs = v_top; if (v_top_abs < 0.0) v_top_abs *= -1.0;
		ta = v_top_abs / st_com_work.a_abs[id];
		check_d_abs = v_top_abs * ta;//�C���`���O���� �� V^2/���@+�@V*�x�ꎞ��
		if (check_d_abs < D_abs) break;													//�m�b�`���x�m��
		else continue;																	//���̃m�b�`��
	}

	pelement->_t = D_abs / v_top_abs;													// �������ԁ@�{�葬����
	pelement->_v = v_top;																// ���x

	d_time_delay = SPD_FB_DELAY_TIME * v_top_abs;
	D_abs = 0.5 * v_top_abs * ta;														//��~�ړ�����
	if (precipe->direction == ID_REV) {
		pelement->_p = pwork->target.pos[ID_HOIST] + D_abs + d_time_delay;					// �ڕW�ʒu�@�^�[�Q�b�g�ʒu-��������-�x�ꎞ�ԋ���
	}
	else {
		pelement->_p = pwork->target.pos[ID_HOIST] - D_abs - d_time_delay;					// �ڕW�ʒu�@�^�[�Q�b�g�ʒu-��������-�x�ꎞ�ԋ���
	}

	/*### STEP2 ��~�@###*/
	pelement = &(precipe->steps[precipe->n_step++]);									//�X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
	pelement->type = CTR_TYPE_VOUT_V;													//���x���B�҂�
	pelement->_t = ta;																	//��������
	pelement->_v = 0.0;																	// ���x0
	pelement->_p = st_com_work.target.pos[id];											// �ڕW�ʒu
	D_abs = 0.0;																		// �c�苗���X�V

	/*### STEP3 �ʒu���킹�@###*/
	pelement = &(precipe->steps[precipe->n_step++]);									// �X�e�b�v�̃|�C���^�Z�b�g���Ď��X�e�b�v�p�ɃJ�E���g�A�b�v
	pelement->type = CTR_TYPE_FINE_POS;													// �����ʒu����
	pelement->_t = FINE_POS_TIMELIMIT;													// �ʒu���킹�ő�p������
	pelement->_v = pCraneStat->spec.notch_spd_f[id][NOTCH_1];							// �P�m�b�`���x
	pelement->_p = st_com_work.target.pos[id];											// �ڕW�ʒu
	D_abs = 0.0;																				// �c�苗���ύX�Ȃ�


	//���ԏ����̃X�L�����J�E���g�l�Z�b�g
	for (int i = 0;i < precipe->n_step;i++) {
		precipe->steps[i].time_count = (int)(precipe->steps[i]._t / pwork->agent_scan);
	}

	return POLICY_PTN_OK;
}


/****************************************************************************/
/*�@�@�R�}���h�p�^�[���v�Z�p�̑f�ރf�[�^�v�Z,�Z�b�g									*/
/*�@�@�ڕW�ʒu,�ڕW�܂ł̋���,�ő呬�x,�����x,��������,�����U�ꒆ�S,�U��U��*/
/****************************************************************************/
LPST_POLICY_WORK CPolicy::set_com_workbuf(ST_POS_TARGETS target) {

	st_com_work.agent_scan_ms = pAgent->inf.cycle_ms;				//AGENT�^�X�N�̃X�L�����^�C��
	st_com_work.agent_scan = 0.001 * (double)st_com_work.agent_scan_ms;
	st_com_work.target = target;									//�ڕW�ʒu

	for (int i = 0; i < MOTION_ID_MAX; i++) {
		//���݈ʒu
		st_com_work.pos[i] = pPLC_IO->status.pos[i];
		//���ݑ��x
		st_com_work.v[i] = pPLC_IO->status.v_fb[i];
		//�ړ������@����
		st_com_work.dist_for_target[i] = st_com_work.target.pos[i] - st_com_work.pos[i];

		if (i == ID_SLEW) {		//����́A��Βl��180���z����Ƃ��͋t�������߂�

			if (st_com_work.dist_for_target[ID_SLEW] > PI180)		st_com_work.dist_for_target[ID_SLEW] -= PI360;
			else if (st_com_work.dist_for_target[ID_SLEW] < -PI180) st_com_work.dist_for_target[ID_SLEW] += PI360;
			else;
		}
	
		if (st_com_work.dist_for_target[i] < 0.0) {
			st_com_work.motion_dir[i] = ID_REV;
			st_com_work.dist_for_target_abs[i] = -1.0 * st_com_work.dist_for_target[i];
		}
		else if (st_com_work.dist_for_target[i] > 0.0) {
			st_com_work.motion_dir[i] = ID_FWD;
			st_com_work.dist_for_target_abs[i] = st_com_work.dist_for_target[i];
		}
		else {
			st_com_work.motion_dir[i] = ID_STOP;
			st_com_work.dist_for_target_abs[i] = 0.0;
		}


		//���쎲�����x
		st_com_work.a_abs[i] = pCraneStat->spec.accdec[i][FWD][ACC];

		//�ő呬�x
		st_com_work.vmax_abs[i] = pCraneStat->spec.notch_spd_f[i][NOTCH_MAX - 1];

		//�ő��������
		st_com_work.acc_time2Vmax[i] = st_com_work.vmax_abs[i] / st_com_work.a_abs[i];

		if ((i == ID_BOOM_H) || (i == ID_SLEW)) {
			//�ݓ_�̉����x
			st_com_work.a_hp_abs[i] = pEnvironment->cal_hp_acc(i, st_com_work.motion_dir[i]);
			if (st_com_work.a_hp_abs[i] < 0.0) st_com_work.a_hp_abs[i] *= -1.0;

			//�������U�ꒆ�S
			st_com_work.pp_th0[i][ACC] = pEnvironment->cal_arad_acc(i, FWD);
			//�������U�ꒆ�S
			st_com_work.pp_th0[i][DEC] = pEnvironment->cal_arad_dec(i, REV);
		}
	}

	//�����̖ڕW�ʒu����̎��́A�����ɐ���������ݓ��������̂ŖڕW�ʒu�̎����Ńp�^�[�������
	if (target.pos[ID_HOIST] > st_com_work.pos[ID_HOIST]) {
		st_com_work.T = pEnvironment->cal_T(target.pos[ID_HOIST]);								//�U�����
		st_com_work.w = pEnvironment->cal_w(target.pos[ID_HOIST]);								//�U��p���g��
		st_com_work.w2 = pEnvironment->cal_w2(target.pos[ID_HOIST]);							//�U��p���g��2��
	}
	else {
		st_com_work.T = pCraneStat->T;															//�U�����
		st_com_work.w = pCraneStat->w;															//�U��p���g��
		st_com_work.w2 = pCraneStat->w2;
	}
	return &st_com_work;
}

#if 0
/****************************************************************************/
/*�@�@1STEP,2STEP�U��~�߃p�^�[���̃Q�C���i��������(�p�x�j�v�Z				*/
/****************************************************************************/
void CPolicy::set_as_gain(int motion, int as_type) {

	double a,r,w,l,r0, vmax, max_th_of_as, acc_time2Vmax;

	//�ő呬�x�ɂ��������Ԑ���
	r = sqrt(pSway_IO->rad_amp2[motion]);			//�U���p�]���l�@rad
	r0 = pEnvironment->cal_arad_acc(motion,FWD);	 //�������U���S
	w = pCraneStat->w;								//�U��p�����x
	a = st_work.a[motion];							//�����̉����xSLEW��rad/s2�ŗǂ��i���a���l���j�@r0�U�ꒆ�S�͔��a�l����
	vmax = st_work.vmax[motion];					//�����̑��xSLEW��rad/s�ŗǂ��i���a���l���j
	acc_time2Vmax = st_work.acc_time2Vmax[motion];	//�������ԍő�l
	l = pCraneStat->mh_l;

	if (as_type == AS_PTN_1STEP){	// 1STEP
		max_th_of_as = r0 * 2.0; //1 STEP�̃��W�b�N�Ő���\�ȐU��U�����E�l�@2r0
		//�Q�C���v�Z�pR0��ݒ�i������~�b�g�j
		if (r >= max_th_of_as) {
			st_work.as_gain_phase[motion] = PI180;
		}
		else {
			st_work.as_gain_phase[motion] = acos(1 - 0.5 * r / r0);
		}
		st_work.as_gain_time[motion] = st_work.as_gain_phase[motion] / w;
		//�ő呬�x�ɂ��������Ԑ���
		if (st_work.as_gain_time[motion] > acc_time2Vmax) {
			st_work.as_gain_time[motion] = acc_time2Vmax;
			st_work.as_gain_phase[motion] = st_work.as_gain_time[motion] * w;
		}
	}
	else if (as_type == AS_PTN_2PN) { //2STEP round type
		
		if (r < r0) {//�U��U���������U�����
			//�U��U���ɉ����ăQ�C����ݒ�i�Q��ڂŌ���������B1��ڂ͐U��ێ��B�J�n�ʑ��Œ���
			st_work.as_gain_phase[motion] = acos(r0/(r+r0));
			st_work.as_gain_time[motion] = st_work.as_gain_phase[motion] / w;
		}
		else {//�U��U���������U��O��
			//2��ڂ̃C���`���O�ŐU��~�߁i�҂����Ԓ����^�C�v�j�p
			st_work.as_gain_phase[motion] = acos(r / (r + r0));
			st_work.as_gain_time[motion] = st_work.as_gain_phase[motion] / w;
		}
		//�ő呬�x�ɂ��������Ԑ���
		if (st_work.as_gain_time[motion] > acc_time2Vmax) {
			st_work.as_gain_time[motion] = acc_time2Vmax;
			st_work.as_gain_phase[motion] = st_work.as_gain_time[motion] * w;
		}
	}
	else if (as_type == AS_PTN_2PP) { //2STEP one way

		double dist_for_target = st_work.pos_target[motion] - pPLC_IO->status.pos[motion];
		if (motion == ID_SLEW) {
			if (st_work.dist_for_target[motion] > PI180) st_work.dist_for_target[motion] -= PI360;
			else if (st_work.dist_for_target[motion] < -PI180) st_work.dist_for_target[motion] += PI360;
			else;
		}

		if (dist_for_target < 0.0) dist_for_target *= -1.0;

	
		//1��̃C���`���O�ړ������@�ڕW�܂ł̋���S=2d�@�@d = a*t^2�@t=��(S/2a)
		st_work.as_gain_time[motion] = sqrt(0.5 * dist_for_target /a);
		if (st_work.as_gain_time[motion] > acc_time2Vmax) //�ő呬�x�ɂ��������Ԑ���
			st_work.as_gain_time[motion] = acc_time2Vmax;

		st_work.as_gain_phase[motion] = st_work.as_gain_time[motion] * w;
	}
	else {
		st_work.as_gain_phase[motion] = 0.0;
		st_work.as_gain_time[motion] = 0.0;
	}
	return;
}

#endif

/****************************************************************************/
/*   �^�X�N�ݒ�^�u�p�l���E�B���h�E�̃R�[���o�b�N�֐�                       */
/****************************************************************************/
LRESULT CALLBACK CPolicy::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

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
void CPolicy::set_panel_tip_txt()
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
void CPolicy::set_panel_pb_txt() {

	//WCHAR str_func06[] = L"DEBUG";

	//SetDlgItemText(inf.hWnd_opepane, IDC_TASK_FUNC_RADIO6, (LPCWSTR)str_func06);

	return;
};


