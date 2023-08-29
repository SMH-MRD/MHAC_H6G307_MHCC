#include "CEnvironment.h"

//-���L�������I�u�W�F�N�g�|�C���^:
extern CSharedMem* pCraneStatusObj;
extern CSharedMem* pSwayStatusObj;
extern CSharedMem* pSimulationStatusObj;
extern CSharedMem* pPLCioObj;
extern CSharedMem* pSwayIO_Obj;
extern CSharedMem* pOTEioObj;
extern CSharedMem* pCSInfObj;
extern CSharedMem* pPolicyInfObj;
extern CSharedMem* pAgentInfObj;

extern vector<void*>	VectpCTaskObj;

/****************************************************************************/
/*   �R���X�g���N�^�@�f�X�g���N�^                                           */
/****************************************************************************/
CEnvironment::CEnvironment() {
	pCraneStat = NULL;
	pPLC_IO = NULL;
	pSway_IO = NULL;
	pOTE_IO = NULL;
	pSimStat = NULL;
	pCSInf = NULL;
	pPolicyInf = NULL;
	pAgentInf = NULL;
}

CEnvironment::~CEnvironment() {

}


/****************************************************************************/
/*   �^�X�N����������                                                       */
/* �@���C���X���b�h�ŃC���X�^���X��������ɌĂт܂��B                       */
/****************************************************************************/
void CEnvironment::init_task(void* pobj) {

	//���L�N���[���X�e�[�^�X�\���̂̃|�C���^�Z�b�g
	pCraneStat = (LPST_CRANE_STATUS)(pCraneStatusObj->get_pMap());
	pPLC_IO = (LPST_PLC_IO)(pPLCioObj->get_pMap());
	pOTE_IO = (LPST_OTE_IO)(pOTEioObj->get_pMap());
	pSway_IO = (LPST_SWAY_IO)(pSwayIO_Obj->get_pMap());
	pSimStat = (LPST_SIMULATION_STATUS)(pSimulationStatusObj->get_pMap());
	pCSInf=(LPST_CS_INFO)(pCSInfObj->get_pMap());
	pPolicyInf = (LPST_POLICY_INFO)(pPolicyInfObj->get_pMap());
	pAgentInf = (LPST_AGENT_INFO)(pAgentInfObj->get_pMap());
	
	//�N���[���d�l�Z�b�g
	stWorkCraneStat.spec = this->spec;
	stWorkCraneStat.is_tasks_standby_ok = false;


	set_panel_tip_txt();

	inf.is_init_complete = true;

	stWorkCraneStat.notch0 = stWorkCraneStat.notch0_crane = BIT_SEL_MOTION;//�S�m�b�`0�ŏ�����
	motion_bit[ID_HOIST] = BIT_SEL_HST;
	motion_bit[ID_GANTRY] = BIT_SEL_GNT;
	motion_bit[ID_TROLLY] = BIT_SEL_TRY;
	motion_bit[ID_BOOM_H] = BIT_SEL_BH;
	motion_bit[ID_SLEW] = BIT_SEL_SLW;
	motion_bit[ID_OP_ROOM] = BIT_SEL_OPR;
	motion_bit[ID_H_ASSY] = BIT_SEL_ASS;
	motion_bit[ID_COMMON] = BIT_SEL_COM;
	
	return;
};

/****************************************************************************/
/*   �^�X�N���������                                                       */
/* �@�^�X�N�X���b�h�Ŗ��������s�����֐�			�@                      */
/****************************************************************************/
bool CEnvironment::check_tasks_init() {

	CTaskObj* ptask;
	int n_tasks = (int)VectpCTaskObj.size();

	for (int i = 0;i < n_tasks ;i++) {
		ptask = (CTaskObj*)VectpCTaskObj[i];
		if(ptask->inf.is_init_complete == false) return false;
	}
	return true;

}

/****************************************************************************/
/*   �^�X�N���������                                                       */
/* �@�^�X�N�X���b�h�Ŗ��������s�����֐�			�@                      */
/****************************************************************************/
void CEnvironment::routine_work(void* param) {
	input();
	main_proc();
	output();
};

//����������菇1�@�O���M������
void CEnvironment::input(){

	return;

};

//����������菇2�@���C������

void CEnvironment::main_proc() {

	//�e�^�X�N�̏����������`�F�b�N
	if (pCraneStat->is_tasks_standby_ok == false) {
		stWorkCraneStat.is_tasks_standby_ok = check_tasks_init();
	}

	//�w���V�[�J�E���^�Z�b�g
	stWorkCraneStat.env_act_count = inf.total_act;

	//�T�u�v���Z�X�`�F�b�N
	chk_subproc();

	//�V�X�e���̃��[�h�Z�b�g
	sys_mode_set();

	//�m�b�`�w�ߏ�ԃZ�b�g
	parse_notch_com();

	//�ʒu���Z�b�g
	pos_set();
	
	//�����p���Z�b�g
	parse_for_auto_ctrl();

	//���u���[�h�Z�b�g
	parse_ote_status();

	return;
}

//����������菇3�@�M���o�͏���

void CEnvironment::output() {

	//���L�������o��
	memcpy_s(pCraneStat, sizeof(ST_CRANE_STATUS), &stWorkCraneStat, sizeof(ST_CRANE_STATUS));
	
	//���C���E�B���h�E��Tweet���b�Z�[�W�X�V
	tweet_update();
	
	return;

}; 

/****************************************************************************/
/*�@�@���C���E�B���h�E��Tweet���b�Z�[�W�X�V          			            */
/****************************************************************************/
void CEnvironment::tweet_update() {

	TCHAR tbuf[32];
	_stprintf_s(tbuf, L"%08x", stWorkCraneStat.operation_mode);

	wostrs << L" OPmode " << tbuf;
	wostrs << L" --Scan " << inf.period;

	tweet2owner(wostrs.str()); wostrs.str(L""); wostrs.clear();

	return;

};

/****************************************************************************/
/*�@�@�m�b�`���͐M���𑬓x�w�߂ɕϊ����Ď�荞��				            */
/****************************************************************************/
int CEnvironment::parse_notch_com() {

	//�N���[����m�b�`
	INT16* p_notch;
	p_notch = pPLC_IO->ui.notch_pos;
	for (int i = 0;i < MOTION_ID_MAX;i++) {
		notch_pos[i] = *p_notch;
		if (*p_notch != NOTCH_0) {
			stWorkCraneStat.notch0_crane &= ~motion_bit[i];
		}
		else {
			stWorkCraneStat.notch0_crane |= motion_bit[i];
		}
		p_notch++;
	}

	//0�m�b�`�C���^�[���b�N�p����
	if (~stWorkCraneStat.notch0_crane & BIT_SEL_MOTION) stWorkCraneStat.notch0_crane &= ~BIT_SEL_ALL_0NOTCH;
	else												stWorkCraneStat.notch0_crane |= BIT_SEL_ALL_0NOTCH;


	//�[���m�b�`
	if (stWorkCraneStat.operation_mode & OPERATION_MODE_REMOTE) {

		if ((stWorkCraneStat.notch0_crane & BIT_SEL_ALL_0NOTCH) && !(pOTE_IO->rcv_msg_u.body.pb[ID_LAMP_OTE_NOTCH_MODE])) {//�@��S0�m�b�`
			p_notch = pOTE_IO->rcv_msg_u.body.notch_pos;				//�[����M���e
			for (int i = 0;i < MOTION_ID_MAX;i++) {
				if (!(stWorkCraneStat.notch0_crane & motion_bit[i])) {//�@��0�m�b�`�łȂ�
					stWorkCraneStat.notch0 &= ~motion_bit[i];
				}
				else if (*p_notch != NOTCH_0) {
					stWorkCraneStat.notch0 &= ~motion_bit[i];
					notch_pos[i] = *p_notch;
				}
				else {
					stWorkCraneStat.notch0 |= motion_bit[i];
				}
				p_notch++;
			}
			//0�m�b�`�C���^�[���b�N�p����
			if (~stWorkCraneStat.notch0_crane & BIT_SEL_MOTION) stWorkCraneStat.notch0_crane &= ~BIT_SEL_ALL_0NOTCH;
			else
				stWorkCraneStat.notch0_crane |= BIT_SEL_ALL_0NOTCH;
		}
	}
	else {
		stWorkCraneStat.notch0 = stWorkCraneStat.notch0_crane;
	}



	for (int i = 0;i < MOTION_ID_MAX;i++) {
		if (notch_pos[i] == NOTCH_0) {
			stWorkCraneStat.notch_spd_ref[i] = 0.0;
		}
		else {
			if (notch_pos[i] < 0)
				stWorkCraneStat.notch_spd_ref[i] = stWorkCraneStat.spec.notch_spd_r[i][-notch_pos[i]] * 1.001;
			else 
				stWorkCraneStat.notch_spd_ref[i] = stWorkCraneStat.spec.notch_spd_f[i][notch_pos[i]] * 1.001;
		}
	}

	return 0;

};
/****************************************************************************/
/*�@ �U������֘A�@�@											            */
/****************************************************************************/
double CEnvironment::cal_T(double pos_hst){   //�U������v�Z�@�����ʒu�w��
	double ans = cal_w(pos_hst);
	ans = PI360 / ans;
	return ans;
};

double CEnvironment::cal_w(double pos_hst){  //�U��p���g���v�Z�@�����ʒu�w��
	double ans = spec.boom_high - pos_hst;
	if (ans > 1.0) ans = GA / ans; 	//���[�v����������
	else ans = GA;
	ans = sqrt(ans);
	return ans;
}; 

double CEnvironment::cal_w2(double pos_hst) {  //�U��p���g����2��v�Z�@�����ʒu�w��
	double ans = spec.boom_high - pos_hst;
	if (ans > 1.0) ans = GA / ans; 	//���[�v����������
	else ans = GA;

	return ans;
};

double CEnvironment::cal_l(double pos_hst){  //���[�v���v�Z�@�����ʒu�w��
	double ans = spec.boom_high - pos_hst;
	return ans;
};

/****************************************************************************/
/*�@ �e���ԗʌv�Z											            */
/****************************************************************************/
double CEnvironment::get_vmax(int motion) {//�ő呬�x
	return pCraneStat->spec.notch_spd_f[motion][NOTCH_5];
}
/****************************************************************************/
/*�@�V�X�e�����[�h�Z�b�g										            */
/****************************************************************************/
int CEnvironment::sys_mode_set() {
	//�����[�g���[�h�Z�b�g
	if (pPLC_IO->ui.PB[ID_PB_REMOTE_MODE]) {
		stWorkCraneStat.operation_mode |= OPERATION_MODE_OTE_ACTIVE | OPERATION_MODE_REMOTE | OPERATION_MODE_OTE_ONBOARD ;
	}
	else {
		stWorkCraneStat.operation_mode &= ~(OPERATION_MODE_OTE_ACTIVE | OPERATION_MODE_REMOTE | OPERATION_MODE_OTE_ONBOARD);
	}

	//�V�~�����[�^���[�h�Z�b�g
	if (pSimStat->mode & SIM_ACTIVE_MODE)stWorkCraneStat.operation_mode |= OPERATION_MODE_SIMULATOR;
	else stWorkCraneStat.operation_mode &= ~OPERATION_MODE_SIMULATOR;

	//PLC�f�o�b�O���[�h�Z�b�g
	if (pPLC_IO->mode & PLC_IF_PC_DBG_MODE)stWorkCraneStat.operation_mode |= OPERATION_MODE_PLC_DBGIO;
	else stWorkCraneStat.operation_mode &= ~OPERATION_MODE_PLC_DBGIO; 

	return 0;

}

/****************************************************************************/
/*�@�ݓ_�̉������x�v�Z�i�������m/s)�@�I�[�o�[���[�h�i���񔼌a���ݒl or �w��j
*/
/****************************************************************************/

//���񔼌a���݈ʒu�ł̌v�Z
double CEnvironment::cal_hp_acc(int motion, int dir ) {

	double ans = spec.accdec[motion][dir][ACC];
	double r = stWorkCraneStat.R;

	switch (motion) {
	case ID_BOOM_H: {
		ans *= (0.0008 * r * r - 0.0626 * r + 1.9599);
	}break;
	case ID_SLEW: {
		ans *= r;
	}break;
	default:break;
	}
	return ans;      //�ݓ_�̉����x�v�Z
}
double CEnvironment::cal_hp_dec(int motion, int dir) {

	double ans = spec.accdec[motion][dir][DEC];
	double r = stWorkCraneStat.R;

	switch (motion) {
	case ID_BOOM_H: {
		ans *= (0.0008 * r * r - 0.0626 * r + 1.9599);
	}break;
	case ID_SLEW: {
		ans *= r;
	}break;
	default:break;
	}

	return ans;      //�ݓ_�̌����x�v�Z
}

//���񔼌a���w�肵�Ă̌v�Z
double CEnvironment::cal_hp_acc(int motion, int dir,double R) {

	double ans = spec.accdec[motion][dir][ACC];
	double r = R;

	switch (motion) {
	case ID_BOOM_H: {
		ans *= (0.0008 * r * r - 0.0626 * r + 1.9599);
	}break;
	case ID_SLEW: {
		ans *= r;
	}break;
	default:break;
	}

	return ans;      //�ݓ_�̉����x�v�Z
}
double CEnvironment::cal_hp_dec(int motion, int dir, double R) {

	double ans = spec.accdec[motion][dir][DEC];
	double r = R;

	switch (motion) {
	case ID_BOOM_H: {
		ans *= (0.0008 * r * r - 0.0626 * r + 1.9599);
	}break;
	case ID_SLEW: {
		ans *= r;
	}break;
	default:break;
	}

	return ans;      //�ݓ_�̌����x�v�Z
}

/****************************************************************************/
/*�@ �������U��p�v�Z		�I�[�o�[���[�h�i���񔼌a���ݒl or �w��j �@�@   */
/****************************************************************************/

///���񔼌a���݈ʒu�ł̉����U��v�Z�@a/g
double CEnvironment::cal_arad_acc(int motion, int dir) {     //�������U��U���v�Zrad
	double ans = cal_hp_acc(motion, dir);
	ans /= GA;
	return ans;      //�ݓ_�̉����U��v�Z

}
double CEnvironment::cal_arad_dec(int motion, int dir) {     //�������U��U���v�Zrad
	double ans = cal_hp_dec(motion, dir);
	ans /= GA;
	return ans;      //�ݓ_�̉����U��v�Z
}
double CEnvironment::cal_arad2(int motion, int dir) {     //�������U��U���v�Zrad
	double ans = cal_hp_acc(motion, dir);
	ans /= GA;
	return (ans * ans); //�ݓ_�̉����U��2��v�Z

}


//���񔼌a�w��ł̉����U��v�Z�@a/g
double CEnvironment::cal_arad_acc(int motion, int dir, double R) {     //�������U��U���v�Zrad
	double ans = cal_hp_acc(motion, dir, R);
	ans /= GA;
	return ans;      //�ݓ_�̉����U��v�Z

}
double CEnvironment::cal_arad_dec(int motion, int dir, double R) {     //�������U��U���v�Zrad
	double ans = cal_hp_dec(motion, dir, R);
	ans /= GA;
	return ans;      //�ݓ_�̉����U��v�Z
}
double CEnvironment::cal_arad2(int motion, int dir, double R) {     //�������U��U���v�Zrad
	double ans = cal_hp_acc(motion, dir, R);
	ans /= GA;
	return (ans * ans); //�ݓ_�̉����U��2��v�Z
}

bool CEnvironment::is_sway_larger_than_accsway(int motion){
	//�U�p�U���������U�p�����傫��������
	double rad_acc2 = cal_arad2(motion, FWD);	//�����U��p2��
	if (pSway_IO->rad_amp2[motion] > rad_acc2) return true;
	else return false;
}

double CEnvironment::cal_sway_r_amp2_m() { //�U��U��2�攼�a���� m
	double ans = pCraneStat->mh_l* pCraneStat->mh_l* pSway_IO->rad_amp2[ID_BOOM_H];
	return ans;
}			
double CEnvironment::cal_sway_th_amp2_m() { 
	return pCraneStat->mh_l * pCraneStat->mh_l * pSway_IO->rad_amp2[ID_SLEW]; 
}																																		//�U��U��2��~������ m
double CEnvironment::cal_sway_x_amp2_m() { return 0.0; }																				//�U��U��2��x���� m
double CEnvironment::cal_sway_y_amp2_m() { return 0.0; }																				//�U��U��2��y���� m

bool CEnvironment::is_speed_0(int motion) { 

	if (!(pCraneStat->notch0 & motion_bit[motion])) return false;//�m�b�`0�Ŗ���

	if ((pPLC_IO->status.v_fb[motion] >= pCraneStat->spec.notch_spd_f[motion][NOTCH_1] * SPD0_CHECK_RETIO) ||		// 0���`�F�b�N
		(pPLC_IO->status.v_fb[motion] <= pCraneStat->spec.notch_spd_r[motion][NOTCH_1] * SPD0_CHECK_RETIO)) {		//1�m�b�`��10�����x�ȏ�
		return false;	//0���łȂ�
	}
	return true;
}

/****************************************************************************/
/*�@ �����֘A�v�Z											         �@�@   */
/****************************************************************************/
//��~�����v�Z
double CEnvironment::cal_dist4stop(int motion, bool is_abs_answer) {
	
	int dir;
	double v = pPLC_IO->status.v_fb[motion];
	if (v < 0.0) dir = REV; else dir = FWD;
	double r = stWorkCraneStat.R;
	double dec = spec.accdec[motion][dir][DEC];


	if (motion == ID_BOOM_H) {
		dec *= (0.0008 * r * r - 0.0626 * r + 1.9599);
	}

	//���������{���Ԓx�ꕪ
	//�����x�͐i�s�����Ƌt�����ƂȂ�悤�ɃZ�b�g���Ă���̂ŕ������K�v
	double dist = -0.5 * v * v / dec + v * spec.delay_time[motion][0];

	if (is_abs_answer) {
		if (dir == REV) return (-1.0 * dist);
		else return dist;
	}
	else {
		return dist;
	}
}

//�ڕW�ʒu�܂ł̋���
/* #Agent�̎����ڕW�ʒu�܂ł̋������v�Z*/
double CEnvironment::cal_dist4target(int motion, bool is_abs_answer) {
	double dist=pAgentInf->auto_pos_target.pos[motion] - pPLC_IO->status.pos[motion] ;

	if (motion == ID_SLEW) {
		if (dist > PI180) dist -= PI360;
		else if (dist < -PI180) dist += PI360;
		else;
	}

	if ((is_abs_answer == true) && (dist < 0.0)) dist *= -1.0;
	return dist;
}

/****************************************************************************/
/*�@ �ʒu���Z�b�g											            */
/****************************************************************************/
int CEnvironment::pos_set() {

	double sin_slew = sin(pPLC_IO->status.pos[ID_SLEW]);
	double cos_slew = cos(pPLC_IO->status.pos[ID_SLEW]);


	//�N���[���ݓ_�̃N���[����_�Ƃ�x,y,z���΍��W
	stWorkCraneStat.rc.x = pPLC_IO->status.pos[ID_BOOM_H] * cos_slew;
	stWorkCraneStat.rc.y = pPLC_IO->status.pos[ID_BOOM_H] * sin_slew;
	stWorkCraneStat.rc.z = spec.boom_high;



	//���񔼌a
	stWorkCraneStat.R = pPLC_IO->status.pos[ID_BOOM_H];
	
	//�ׂ݉̃J�������W�ł̒݉�xyz���΍��W�@m
	stWorkCraneStat.rcam_m.x = stWorkCraneStat.mh_l * sin(pSway_IO->th[ID_SLEW]) ;
	stWorkCraneStat.rcam_m.y = stWorkCraneStat.mh_l * sin(pSway_IO->th[ID_BOOM_H]);
	stWorkCraneStat.rcam_m.z = -stWorkCraneStat.mh_l;

	//�ׂ݉�x, y, z���W
	stWorkCraneStat.rl.x = pCraneStat->rc.x + stWorkCraneStat.rcam_m.x * sin_slew + stWorkCraneStat.rcam_m.y * cos_slew;
	stWorkCraneStat.rl.y = pCraneStat->rc.y + stWorkCraneStat.rcam_m.x * -cos_slew + stWorkCraneStat.rcam_m.y * sin_slew;
	stWorkCraneStat.rl.z = pPLC_IO->status.pos[ID_HOIST];

	//�Ɍ�����
	if (pPLC_IO->status.pos[ID_GANTRY] < spec.gantry_pos_min) stWorkCraneStat.is_rev_endstop[ID_GANTRY] = true;
	else stWorkCraneStat.is_rev_endstop[ID_GANTRY] = false;
	if (pPLC_IO->status.pos[ID_GANTRY] > spec.gantry_pos_max) stWorkCraneStat.is_fwd_endstop[ID_GANTRY] = true;
	else stWorkCraneStat.is_fwd_endstop[ID_GANTRY] = false;

	if (stWorkCraneStat.rl.z < spec.hoist_pos_min) stWorkCraneStat.is_rev_endstop[ID_HOIST] = true;
	else stWorkCraneStat.is_rev_endstop[ID_HOIST] = false;
	if (stWorkCraneStat.rl.z > spec.hoist_pos_max) stWorkCraneStat.is_fwd_endstop[ID_HOIST] = true;
	else stWorkCraneStat.is_fwd_endstop[ID_HOIST] = false;

	if (pPLC_IO->status.pos[ID_BOOM_H] < spec.boom_pos_min) stWorkCraneStat.is_rev_endstop[ID_BOOM_H] = true;
	else stWorkCraneStat.is_rev_endstop[ID_BOOM_H] = false;
	if (pPLC_IO->status.pos[ID_BOOM_H] > spec.boom_pos_max) stWorkCraneStat.is_fwd_endstop[ID_BOOM_H] = true;
	else stWorkCraneStat.is_fwd_endstop[ID_BOOM_H] = false;

	return 0;

}
/****************************************************************************/
/*�@ �����p���Z�b�g											            */
/****************************************************************************/
int CEnvironment::parse_for_auto_ctrl() {
	//���[�v��
	stWorkCraneStat.mh_l = cal_l(pPLC_IO->status.pos[ID_HOIST]);
	//�p����
	stWorkCraneStat.w2 = cal_w2(pPLC_IO->status.pos[ID_HOIST]);
	stWorkCraneStat.w = sqrt(stWorkCraneStat.w2);
	//����
	stWorkCraneStat.T = PI360 / stWorkCraneStat.w;
	return 0;
}

/****************************************************************************/
/*�@ �����p���Z�b�g											            */
/****************************************************************************/
int CEnvironment::parse_ote_status() {
	if (pOTE_IO->OTE_healty > 0) {
		stWorkCraneStat.OTE_req_status = ID_TE_CONNECT_STATUS_ACTIVE;
	}
	else if (stWorkCraneStat.operation_mode & OPERATION_MODE_OTE_ACTIVE) {
		stWorkCraneStat.OTE_req_status = ID_TE_CONNECT_STATUS_WAITING;
	}
	else {
		stWorkCraneStat.OTE_req_status = ID_TE_CONNECT_STATUS_OFF_LINE;
	}
	return 0;
}


/****************************************************************************/
/*�@�@�T�u�v���Z�X�̏�Ԋm�F			            */
/****************************************************************************/
static DWORD plc_io_helthy_NGcount = 0;
static DWORD plc_io_helthy_count_last = 0;
static DWORD sim_helthy_NGcount = 0;
static DWORD sim_helthy_count_last = 0;
static DWORD sway_helthy_NGcount = 0;
static DWORD sway_helthy_count_last = 0;

void CEnvironment::chk_subproc() {

	//PLC IF
	if (plc_io_helthy_count_last == pPLC_IO->helthy_cnt) plc_io_helthy_NGcount++;
	else plc_io_helthy_NGcount = 0;
	if (plc_io_helthy_NGcount > PLC_IO_HELTHY_NG_COUNT) stWorkCraneStat.subproc_stat.is_plcio_join = false;
	else stWorkCraneStat.subproc_stat.is_plcio_join = true;
	plc_io_helthy_count_last = pPLC_IO->helthy_cnt;

	//SWAY IF
	if (sway_helthy_count_last == pSway_IO->helthy_cnt) sway_helthy_NGcount++;
	else sway_helthy_NGcount = 0;
	if (sway_helthy_NGcount > SWAY_HELTHY_NG_COUNT) stWorkCraneStat.subproc_stat.is_sway_join = false;
	else stWorkCraneStat.subproc_stat.is_sway_join = true;
	sway_helthy_count_last = pSway_IO->helthy_cnt;

	//SIM
	if (sim_helthy_count_last == pSimStat->helthy_cnt) sim_helthy_NGcount++;
	else sim_helthy_NGcount = 0;
	if (sim_helthy_NGcount >SIM_HELTHY_NG_COUNT) stWorkCraneStat.subproc_stat.is_sim_join = false;
	else stWorkCraneStat.subproc_stat.is_sim_join = true;
	sim_helthy_count_last = pSimStat->helthy_cnt;

	return;

};


/****************************************************************************/
/*   �^�X�N�ݒ�^�u�p�l���E�B���h�E�̃R�[���o�b�N�֐�                       */
/****************************************************************************/
LRESULT CALLBACK CEnvironment::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

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
void CEnvironment::set_panel_tip_txt()
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
void CEnvironment::set_panel_pb_txt() {

//	WCHAR str_func06[] = L"DEBUG";

//	SetDlgItemText(inf.hWnd_opepane, IDC_TASK_FUNC_RADIO6, (LPCWSTR)str_func06);

	return;
};