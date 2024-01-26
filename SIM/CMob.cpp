#include "CMob.h"
#include "SIMparaDEF.h"

extern ST_SPEC def_spec;

//�v�Z�Z�k�p�ϐ�
static double LmbCosAdb, LmLb,Lmb2;

/********************************************************************************/
/*       Moving Object                                                          */
/********************************************************************************/
CMob::CMob() {
	dt = 0.01;
 	r = { 0.0,0.0,0.0 };
	v = { 0.0,0.0,0.0 };
	dr = { 0.0,0.0,0.0 };
	dv = { 0.0,0.0,0.0 };
	R0 = { 0.0,0.0,0.0};
}
CMob::CMob(double _dt) {
	dt = _dt;
	r = { 0.0,0.0,0.0 };
	v = { 0.0,0.0,0.0 };
	dr = { 0.0,0.0,0.0 };
	dv = { 0.0,0.0,0.0 };
	R0 = { 0.0,0.0,0.0 };
}
CMob::CMob(double _dt, Vector3& _r, Vector3& _v) {
	dt = _dt;
	r.copy(_r);
	v.copy(_v);
	R0 = { 0.0,0.0,0.0 };
}
CMob::~CMob() {}

//�����x�x�N�g���@�p����ōĒ�`����
Vector3 CMob::A(Vector3& r, Vector3& v) {
	return r.clone();
};
void CMob::set_fex(double fx,double fy,double fz) {
	fex.x = fx;
	fex.y = fy;
	fex.z = fz;
	return;
};
void CMob::set_dt(double _dt) {
	dt = _dt;
	return;
};

//���x�x�N�g��
Vector3 CMob::V(Vector3& r, Vector3& v) {
	return v.clone();
}
//�I�C���[���ɂ�鎞�Ԕ��W
void CMob::timeEvolution() {

	Vector3 v1 = V(r, v);
	Vector3 a1 = A(r, v);

	Vector3 _v1 = Vector3(r.x + v1.x * dt / 2.0, r.y + v1.y * dt / 2.0, r.z + v1.z * dt / 2.0);
	Vector3 _a1 = Vector3(v.x + a1.x * dt / 2.0, v.y + a1.y * dt / 2.0, v.z + a1.z * dt / 2.0);
	Vector3 v2 = V(_v1, _a1);
	Vector3 a2 = A(_v1, _a1);

	Vector3 _v2 = Vector3(r.x + v2.x * dt / 2.0, r.y + v2.y * dt / 2.0, r.z + v2.z * dt / 2.0);
	Vector3 _a2 = Vector3(v.x + a2.x * dt / 2.0, v.y + a2.y * dt / 2.0, v.z + a2.z * dt / 2.0);
	Vector3 v3 = V(_v2, _a2);
	Vector3 a3 = A(_v2, _a2);

	Vector3 _v3 = Vector3(r.x + v3.x * dt, r.y + v3.y * dt, r.z + v3.z * dt);
	Vector3 _a3 = Vector3(v.x + a3.x * dt, v.y + a3.y * dt, v.z + a3.z * dt);
	Vector3 v4 = V(_v3, _a3);
	Vector3 a4 = A(_v3, _a3);

	dr.x = dt * v.x;
	dr.y = dt * v.y;
	dr.z = dt * v.z;
	dv.x = dt / 6.0 * (a1.x + 2.0 * a2.x + 2.0 * a3.x + a4.x);
	dv.y = dt / 6.0 * (a1.y + 2.0 * a2.y + 2.0 * a3.y + a4.y);
	dv.z = dt / 6.0 * (a1.z + 2.0 * a2.z + 2.0 * a3.z + a4.z);
}

/********************************************************************************/
/*       Crane Object                                                          */
/********************************************************************************/
CJC::CJC() { 
	pspec = &def_spec;							//�N���[���d�l

	//0���Ƃ݂Ȃ����x����i�h������]���x�j
	//���x�\����1.0��100%�Ő��K������
	accdec_cut_spd_range[ID_HOIST]	= 0.005 * pspec->prm_drv[DRIVE_ITEM_RATE_NV][ID_HOIST];//0.5%
	accdec_cut_spd_range[ID_BOOM_H] = 0.005 * pspec->prm_drv[DRIVE_ITEM_RATE_NV][ID_BOOM_H];//0.5%
	accdec_cut_spd_range[ID_SLEW]	= 0.005 * pspec->prm_drv[DRIVE_ITEM_RATE_NV][ID_SLEW];//0.5%
	accdec_cut_spd_range[ID_GANTRY] = 0.005 * pspec->prm_drv[DRIVE_ITEM_RATE_NV][ID_GANTRY];//0.5%	
	accdec_cut_spd_range[ID_AHOIST] = 0.005 * pspec->prm_drv[DRIVE_ITEM_RATE_NV][ID_AHOIST];//0.5%

	mh_load = pspec->Load0_mh;//�����努�׏d
	ah_load = pspec->Load0_ah;//�����⊪�׏d

	for (int i = 0; i < MOTION_ID_MAX;i++) {
		is_fwd_endstop[i] = false;
		is_rev_endstop[i] = false;
		Tf[i] = 1.0;
		brk_elaped_time[i] = 0.0;
		a0[i] = 0.0;
		v0[i] = 0.0;
		a_ref[i] = 0.0;
		v_ref[i] = 0.0;
		is_fwd_endstop[i] = false;
		is_rev_endstop[i] = false;
		trq_fb[i]=0.0;    //���[�^�[�g���NFB
		motion_break[i] = false;
	}

	r0[ID_GANTRY] = pspec->gantry_pos_min;
	r0[ID_HOIST] = pspec->boom_high - 9.8;	//�����l�@�����@2�΂ƂȂ銪���ʒu
	r0[ID_BOOM_H] = pspec->boom_pos_min;
	r0[ID_SLEW] = 0.0;

	source_mode = MOB_MODE_SIM;
	
}
CJC::~CJC() {}

/// <summary>
/// �h�������x�w�ߎ�荞��
/// </summary>
/// <param name="hoist_ref"></param>
/// <param name="gantry_ref"></param>
/// <param name="slew_ref"></param>
/// <param name="boomh_ref"></param>
/// <param name="ah_ref"></param>
void CJC::set_v_ref(double hoist_ref, double gantry_ref, double slew_ref, double boomh_ref, double ah_ref) {
	nv_ref[ID_HOIST] = hoist_ref;
	nv_ref[ID_BOOM_H] = boomh_ref;
	nv_ref[ID_SLEW] = slew_ref;
	nv_ref[ID_GANTRY] = gantry_ref;
	nv_ref[ID_AHOIST] = ah_ref;
	return;
}
// �ٸT(N�Em�j= F x R�@= J x d��/dt  �d����P=T��=Mav�@a=T��/Mv=MT/r
//�ݓ_�����x
Vector3 CJC::A(Vector3& _r, Vector3& _v) {					//�ݓ_�̉����x

	Vector3 a;
	double r_bm = r0[ID_BOOM_H];//���񔼌a
	//���p�����x�^���@a = r x d��/dt(-sin��,cos��) -rx��^2(cos��,sin��)
	a.x = v0[ID_BOOM_H] * sin(r0[ID_SLEW]) - r_bm * a0[ID_SLEW] * sin(r0[ID_SLEW]) - r_bm * v0[ID_SLEW] * v0[ID_SLEW] * cos(r0[ID_SLEW]) + a0[ID_GANTRY];
	a.y = v0[ID_BOOM_H] * cos(r0[ID_SLEW]) + r_bm * a0[ID_SLEW] * cos(r0[ID_SLEW]) - r_bm * v0[ID_SLEW] * v0[ID_SLEW] * sin(r0[ID_SLEW]);
	a.z = 0.0;


	//�ݓ_�����x�x�N�g���i�~�����W�j
	double a_er = a0[ID_BOOM_H] - r0[ID_BOOM_H] * v0[ID_SLEW] * v0[ID_SLEW];		//�������������x�@���������x�{����(R��2�j
	double a_eth = r0[ID_BOOM_H] * a0[ID_SLEW] + 2.0 * v0[ID_BOOM_H] * v0[ID_SLEW];	//������������x
	double a_z = 0.0;																//�ݓ_�̍����͈��

	//xyz���W�ݓ_�����x�x�N�g��
	a.x = a0[ID_GANTRY] + a_er * cos(r0[ID_SLEW]) - a_eth * sin(r0[ID_SLEW]);
	a.y = a_er * sin(r0[ID_SLEW]) + a_eth * cos(r0[ID_SLEW]);
	a.z = a_z;

	return a;
}
//�������x
void CJC::Ac() {	//�����x�v�Z

	//�h���������w�ߌv�Z
	// #�努
	{
		//�u���[�L��
		if (!motion_break[ID_HOIST]) na_ref[ID_HOIST] = 0.0;
		//���x�w�߂ɖ��B(�h������]���x�j
		else if ((nv_ref[ID_HOIST] - pSimStat->nd[ID_HOIST].v) > accdec_cut_spd_range[ID_HOIST]) {
			// ���ʐ��]�t�]�œ��������x�Ƃ���
			if (nv_ref[ID_HOIST] > 0.0) na_ref[ID_HOIST] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_HOIST];		//���]�����w��
			else						na_ref[ID_HOIST] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_HOIST];		//�t�]�����w��
		}
		//���x�w�߂ɖ��B
		else if ((nv_ref[ID_HOIST] - pSimStat->nd[ID_HOIST].v) < -accdec_cut_spd_range[ID_HOIST]) {
			// ���ʐ��]�t�]�œ��������x�Ƃ���
			if (nv_ref[ID_HOIST] > 0.0) na_ref[ID_HOIST] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_HOIST]; 		//���]�����w��
			else						na_ref[ID_HOIST] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_HOIST];		//�t�]�����w��
		}
		//���x�w�߂ɓ��B
		else {
			na_ref[ID_HOIST] = 0.0;
		}

		//�Ɍ���~
		if ((na_ref[ID_HOIST] > 0.0) && (is_fwd_endstop[ID_HOIST])) na_ref[ID_HOIST] = 0.0;
		if ((na_ref[ID_HOIST] < 0.0) && (is_rev_endstop[ID_HOIST])) na_ref[ID_HOIST] = 0.0;
	}

	// #�⊪
	{
		//�u���[�L��
		if (!motion_break[ID_AHOIST]) na_ref[ID_AHOIST] = 0.0;
		//���x�w�߂ɖ��B
		else if ((nv_ref[ID_AHOIST] - pSimStat->nd[ID_AHOIST].v) > accdec_cut_spd_range[ID_AHOIST]) {
			// ���ʐ��]�t�]�œ��������x�Ƃ���
			if (nv_ref[ID_AHOIST] > 0.0)na_ref[ID_AHOIST] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_AHOIST];		//���]�����w��
			else						na_ref[ID_AHOIST] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_AHOIST];		//�t�]�����w��
		}
		//���x�w�߂ɖ��B
		else if ((nv_ref[ID_AHOIST] - pSimStat->nd[ID_AHOIST].v) < -accdec_cut_spd_range[ID_AHOIST]) {
			if (nv_ref[ID_AHOIST] > 0.0)na_ref[ID_AHOIST] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_AHOIST];		//���]�����w��
			else						na_ref[ID_AHOIST] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_AHOIST];		//�t�]�����w��
		}
		//���x�w�߂ɓ��B
		else {
			na_ref[ID_AHOIST] = 0.0;
		}

		//�Ɍ���~
		if ((na_ref[ID_AHOIST] > 0.0) && (is_fwd_endstop[ID_AHOIST])) na_ref[ID_AHOIST] = 0.0;
		if ((na_ref[ID_AHOIST] < 0.0) && (is_rev_endstop[ID_AHOIST])) na_ref[ID_AHOIST] = 0.0;
	}

	// #���s
	{
		if (!motion_break[ID_GANTRY]) na_ref[ID_GANTRY] = 0.0;
		else if ((nv_ref[ID_GANTRY] - pSimStat->nd[ID_GANTRY].v) > accdec_cut_spd_range[ID_GANTRY]) {
			if (nv_ref[ID_GANTRY] > 0.0)na_ref[ID_GANTRY] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_GANTRY];	//���]����
			else						na_ref[ID_GANTRY] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_GANTRY];	//�t�]����
		}
		else if ((nv_ref[ID_GANTRY] - pSimStat->nd[ID_GANTRY].v) < -accdec_cut_spd_range[ID_GANTRY]) {
			if (nv_ref[ID_GANTRY] > 0.0)na_ref[ID_GANTRY] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_GANTRY];	//���]����
			else						na_ref[ID_GANTRY] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_GANTRY];	//�t�]����
		}
		else {
			na_ref[ID_GANTRY] = 0.0;
		}

		//�Ɍ���~
		if ((na_ref[ID_GANTRY] > 0.0) && (is_fwd_endstop[ID_GANTRY])) na_ref[ID_GANTRY] = 0.0;
		if ((na_ref[ID_GANTRY] < 0.0) && (is_rev_endstop[ID_GANTRY])) na_ref[ID_GANTRY] = 0.0;
	}

	// #����
	{
		if (!motion_break[ID_BOOM_H]) na_ref[ID_BOOM_H] = 0.0;
		else if ((nv_ref[ID_BOOM_H] - pSimStat->nd[ID_BOOM_H].v) > accdec_cut_spd_range[ID_BOOM_H]) {
			if (nv_ref[ID_BOOM_H] > 0.0)na_ref[ID_BOOM_H] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_BOOM_H];	//���]����
			else						na_ref[ID_BOOM_H] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_BOOM_H];	//�t�]����
		}
		else if ((nv_ref[ID_BOOM_H] - pSimStat->nd[ID_BOOM_H].v) < -accdec_cut_spd_range[ID_BOOM_H]) {
			if (nv_ref[ID_BOOM_H] > 0.0)na_ref[ID_BOOM_H] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_BOOM_H];	//���]����
			else						na_ref[ID_BOOM_H] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_BOOM_H];	//�t�]����
		}
		else {
			na_ref[ID_BOOM_H] = 0.0;
		}

		//�Ɍ���~
		if ((na_ref[ID_BOOM_H] > 0.0) && (is_fwd_endstop[ID_BOOM_H])) na_ref[ID_BOOM_H] = 0.0;
		if ((na_ref[ID_BOOM_H] < 0.0) && (is_rev_endstop[ID_BOOM_H])) na_ref[ID_BOOM_H] = 0.0;
	}

	// #����
	{
		if (!motion_break[ID_SLEW]) na_ref[ID_SLEW] = 0.0;
		else if ((nv_ref[ID_SLEW] - pSimStat->nd[ID_SLEW].v) > accdec_cut_spd_range[ID_SLEW]) {
			if (nv_ref[ID_SLEW] > 0.0)	na_ref[ID_SLEW] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_SLEW];//���]����
			else						na_ref[ID_SLEW] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_SLEW];//�t�]����
		}
		else if ((nv_ref[ID_SLEW] - pSimStat->nd[ID_SLEW].v) < -accdec_cut_spd_range[ID_SLEW]) {
			if (nv_ref[ID_SLEW] > 0.0)	na_ref[ID_SLEW] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_SLEW];//���]����
			else						na_ref[ID_SLEW] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_SLEW];//�t�]����
		}
		else {
			na_ref[ID_SLEW] = 0.0;
		}
	}

	//�h���������x�v�Z�@���ʎw�߂ɑ΂��Ĉꎟ�x��t�B���^������`�Ōv�Z
	{
		//�努
		if ((motion_break[ID_HOIST]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_HOIST].a = (dt * na_ref[ID_HOIST] + Tf[ID_HOIST] * pSimStat->nd[ID_HOIST].a )/ (dt + Tf[ID_HOIST]);
		}
		else {
			pSimStat->nd[ID_HOIST].a = 0.0;
		}
		//�⊪
		if ((motion_break[ID_AHOIST]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_AHOIST].a = (dt * na_ref[ID_AHOIST] + Tf[ID_AHOIST] * pSimStat->nd[ID_AHOIST].a) / (dt + Tf[ID_AHOIST]);
		}
		else {
			pSimStat->nd[ID_AHOIST].a = 0.0;
		}
		//����
		if ((motion_break[ID_BOOM_H]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_BOOM_H].a = (dt * na_ref[ID_BOOM_H] + Tf[ID_BOOM_H] * pSimStat->nd[ID_BOOM_H].a) / (dt + Tf[ID_BOOM_H]);
		}
		else {
			pSimStat->nd[ID_BOOM_H].a = 0.0;
		}
		//����
		if ((motion_break[ID_SLEW]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_SLEW].a = (dt * na_ref[ID_SLEW] + Tf[ID_SLEW] * pSimStat->nd[ID_SLEW].a) / (dt + Tf[ID_SLEW]);
		}
		else {
			pSimStat->nd[ID_SLEW].a = 0.0;
		}
		//���s
		if ((motion_break[ID_GANTRY]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_GANTRY].a = (dt * na_ref[ID_GANTRY] + Tf[ID_GANTRY] * pSimStat->nd[ID_GANTRY].a) / (dt + Tf[ID_GANTRY]);
		}
		else {
			pSimStat->nd[ID_GANTRY].a = 0.0;
		}

	}


	return;
}
void CJC::timeEvolution() {
	//�N���[����
	//�h���������x�v�Z
	Ac();

	//�h�������x�v�Z(�I�C���[�@�j
	pSimStat->nd[ID_HOIST].v	+= pSimStat->nd[ID_HOIST].a * dt;	if (!motion_break[ID_HOIST])	pSimStat->nd[ID_HOIST].v = 0.0;
	pSimStat->nd[ID_AHOIST].v	+= pSimStat->nd[ID_AHOIST].a * dt;	if (!motion_break[ID_AHOIST])	pSimStat->nd[ID_AHOIST].v = 0.0;
	pSimStat->nd[ID_BOOM_H].v	+= pSimStat->nd[ID_BOOM_H].a * dt;	if (!motion_break[ID_BOOM_H])	pSimStat->nd[ID_BOOM_H].v = 0.0;
	pSimStat->nd[ID_SLEW].v		+= pSimStat->nd[ID_SLEW].a * dt;	if (!motion_break[ID_SLEW])		pSimStat->nd[ID_SLEW].v = 0.0;
	pSimStat->nd[ID_GANTRY].v	+= pSimStat->nd[ID_GANTRY].a * dt;	if (!motion_break[ID_GANTRY])	pSimStat->nd[ID_GANTRY].v = 0.0;

	//�h�����ʒu�v�Z(�I�C���[�@�j
	pSimStat->nd[ID_HOIST].p += pSimStat->nd[ID_HOIST].v * dt;
	pSimStat->nd[ID_AHOIST].p += pSimStat->nd[ID_AHOIST].v * dt;
	pSimStat->nd[ID_GANTRY].p += pSimStat->nd[ID_GANTRY].v * dt;
	pSimStat->nd[ID_BOOM_H].p += pSimStat->nd[ID_BOOM_H].v * dt;
	pSimStat->nd[ID_SLEW].p += pSimStat->nd[ID_SLEW].v * dt;

	//�N���[����ԃZ�b�g
	set_d_th_from_nbh();	//�����h������]��Ԃ���d,d'd'' th th' th''�̏�Ԃ��Z�b�g����
	set_bh_layer();         //�����h������Ԃ��Z�b�g����
	set_mh_layer();         //�努�h������ԁA���[�v��Ԃ��Z�b�g����
	set_ah_layer();         //�⊪�h������ԁA���[�v��Ԃ��Z�b�g����
	set_sl_layer();         //����h������Ԃ��Z�b�g����
	set_gt_layer();         //���s�h������Ԃ��Z�b�g����

	//�������x�v�Z
	double thm = pSimStat->th.p;
	double tha = pSimStat->th.p - pspec->Alpa_a;

	//���[�v�������x�v�Z
	pSimStat->lrm.a = (
		-pCraneStat->Cdr[ID_HOIST][pSimStat->i_layer[ID_HOIST]] * pSimStat->nd[ID_HOIST].a		//�努�h������]��
		- pSimStat->db.a * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_HOIST]							//d�ω���
		+ pCraneStat->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]] * pSimStat->nd[ID_BOOM_H].a		//�N���h������]��
		) / pspec->prm_nw[NW_ITEM_WIND][ID_HOIST];

	pSimStat->lra.a = (
		-pCraneStat->Cdr[ID_AHOIST][pSimStat->i_layer[ID_AHOIST]] * pSimStat->nd[ID_AHOIST].a	//�⊪�h������]��
		- pSimStat->d.a * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_AHOIST]							//d�ω���
		) / pspec->prm_nw[NW_ITEM_WIND][ID_AHOIST];

	//h=Lm�Esin�Ɓ@h'=Lm�E��'cos�� h''=Lm�E��''cos��-��'sin��
	a0[ID_HOIST] = pspec->Lm * (pSimStat->th.a * cos(thm) - pSimStat->th.v * pSimStat->th.v * sin(thm)) + pSimStat->lrm.a;//�ݓ_�������x�{���[�v�������x
	a0[ID_AHOIST] = pspec->La * (pSimStat->th.a * cos(tha) - pSimStat->th.v * pSimStat->th.v * sin(tha)) + pSimStat->lra.a;//�ݓ_�������x�{���[�v�������x
	//r=Lm�Ecos�Ɓ@r'=-Lm�E��'sin�� r''=-Lm�E(��''sin��-��'cos��)
	a0[ID_BOOM_H] = -pspec->Lm * (pSimStat->th.a * sin(thm) + pSimStat->th.v * cos(thm));

	a0[ID_SLEW] = pSimStat->nd[ID_SLEW].a * pspec->Kttb;						//�s�j�I����]�����x�~�s�j�I���a/TTB�a
	a0[ID_GANTRY] = pSimStat->nd[ID_GANTRY].a * pCraneStat->Cdr[ID_GANTRY][1];	//�h������]�����x�~�ԗ֌a


	pSimStat->lrm.v = (
		-pCraneStat->Cdr[ID_HOIST][pSimStat->i_layer[ID_HOIST]] * pSimStat->nd[ID_HOIST].v	//�努�h������]��
		- pSimStat->db.v * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_HOIST]							//d�ω���
		+ pCraneStat->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]] * pSimStat->nd[ID_BOOM_H].v	//�N���h������]��
		) / pspec->prm_nw[NW_ITEM_WIND][ID_HOIST];
	pSimStat->lra.v = (
		-pCraneStat->Cdr[ID_AHOIST][pSimStat->i_layer[ID_AHOIST]] * pSimStat->nd[ID_AHOIST].v	//�⊪�h������]��
		- pSimStat->d.v * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_AHOIST]							//d�ω���
		) / pspec->prm_nw[NW_ITEM_WIND][ID_AHOIST];


	v0[ID_HOIST]	= pspec->Lm * pSimStat->th.v * cos(pSimStat->th.p) - pSimStat->lrm.v;
	v0[ID_AHOIST]	= pspec->La * pSimStat->th.v * cos(pSimStat->th.p - pspec->Alpa_a) - pSimStat->lra.v;
	v0[ID_SLEW]		= pSimStat->nd[ID_SLEW].v * PI360;
	v0[ID_GANTRY]	= pSimStat->nd[ID_GANTRY].v * pCraneStat->Cdr[ID_GANTRY][1];
	v0[ID_BOOM_H]	= -pspec->Lb * pSimStat->th.v * sin(pSimStat->th.p+pspec->Alpa_b);


	pSimStat->lrm.p = (
		pCraneStat->Cdr[0][ID_HOIST] 														//�S���[�v
		- pSimStat->db.p * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_HOIST]						//d�����[�v
		- pSimStat->l_drum[ID_BHMH] - pSimStat->l_drum[ID_HOIST]							//�h���������[�v(�努�h�����{�����h�����j
		) / pspec->prm_nw[NW_ITEM_WIND][ID_HOIST];											//���C���|����
	pSimStat->lra.p = (
		pCraneStat->Cdr[0][ID_AHOIST] 															//�S���[�v
		- pSimStat->d.p * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_AHOIST]							//d�����[�v
		- pSimStat->l_drum[ID_AHOIST]															//�h���������[�v
		) / pspec->prm_nw[NW_ITEM_WIND][ID_AHOIST];												//���C���|����


	r0[ID_HOIST]	= pspec->Hp + pspec->Lm * sin(pSimStat->th.p) - pSimStat->lrm.p;
	r0[ID_AHOIST]	= pspec->Hp + pspec->La * sin(pSimStat->th.p - pspec->Alpa_a) - pSimStat->lra.p;
	r0[ID_SLEW]		= pSimStat->n_layer[ID_SLEW] * PI360 - PI180; //����́}180���ŕ\��
	r0[ID_GANTRY]	= pSimStat->l_drum[ID_GANTRY];
	r0[ID_BOOM_H]	= pspec->Lm * cos(pSimStat->th.p);

	vc.x = v0[ID_GANTRY]; vc.y = 0.0; vc.z = 0.0;		//�N���[�����S�ʒu
	rc.x = r0[ID_GANTRY]; rc.y = R0.y; rc.z = R0.z;		//�N���[�����S�ʒu
														
	//�ݓ_��

	r.x = r0[ID_BOOM_H] * cos(r0[ID_SLEW]) + r0[ID_GANTRY];
	r.y = r0[ID_BOOM_H] * sin(r0[ID_SLEW]);
	r.z = pspec->Hp + r0[ID_BOOM_H] * sin(pSimStat->th.p);

	v.x = v0[ID_BOOM_H] * cos(r0[ID_SLEW]) - r0[ID_BOOM_H] * v0[ID_SLEW] * sin(r0[ID_SLEW]) + v0[ID_GANTRY];
	v.y = v0[ID_BOOM_H] * sin(r0[ID_SLEW]) + r0[ID_BOOM_H] * v0[ID_SLEW] * cos(r0[ID_SLEW]);
	v.z = pspec->Hp + r0[ID_BOOM_H] * sin(pSimStat->th.p);

	a.x = v0[ID_BOOM_H] * cos(r0[ID_SLEW]) - r0[ID_BOOM_H] * v0[ID_SLEW] * sin(r0[ID_SLEW]) + v0[ID_GANTRY];
	a.y = v0[ID_BOOM_H] * sin(r0[ID_SLEW]) + r0[ID_BOOM_H] * v0[ID_SLEW] * cos(r0[ID_SLEW]);
	a.z = pspec->Hp + r0[ID_BOOM_H] * sin(pSimStat->th.p);

	//���[�v���Z�b�g�@LOAD�I�u�W�F�N�g����Q��
	l_mh = pSimStat->lrm.p;
	l_ah = pSimStat->lra.p;

	return;
}
void CJC::init_crane(double _dt) {
	
	//�v�Z�p�����[�^
	Lmb2 = pspec->Lmb * pspec->Lmb;
	LmbCosAdb = pspec->Lmb * cos(pspec->Alpa_db);
	LmLb = pspec->Lm * pspec->Lb;
	
	//r0�́A�e���A�u�\�R�[�_�̒l
	r0[ID_HOIST]	= SIM_INIT_MH;
	r0[ID_AHOIST]	= SIM_INIT_AH;
	r0[ID_GANTRY]	= SIM_INIT_X;
	r0[ID_SLEW]		= SIM_INIT_TH;
	r0[ID_BOOM_H]	= SIM_INIT_R;		//���񔼌a�͎努�ݓ_�ʒu�̔��a�Ƃ���

	//�N���[����_�̏����ʒu,���x
	rc.x = 10.0; rc.y = 0.0; rc.z = 0.0;
	vc.x = 0.0; vc.y = 0.0; vc.z = 0.0;

	//�h������]�ʒu�Z�b�g
	set_nbh_d_ph_th_from_r(r0[ID_BOOM_H]);												//���񔼌a�������h������]�ʑ����Z�b�g

	set_nmh_from_mh(r0[ID_HOIST]); 														//�努�g�����努�h������]�ʑ����Z�b�g

	set_nah_from_ah(r0[ID_AHOIST]); 													//�⊪�g�����⊪�h������]�ʑ����Z�b�g

	slw_rad_per_turn = PI360 * pspec->prm_nw[DRUM_ITEM_DIR][ID_SLEW] / pspec->Dttb;		//�s�j�I���i���[�^�j�P��]�ł̐���p�x�ω��ʃZ�b�g
	set_nsl_from_slr(r0[ID_SLEW]); 														//����ʒu����s�j�I����]�����Z�b�g

	gnt_m_per_turn = PI180 * pspec->prm_nw[DRUM_ITEM_DIR][ID_GANTRY];					//�ԗ�1��]�ł̈ړ��ʃZ�b�g
	set_ngt_from_gtm(r0[ID_GANTRY]);													//���s�ʒu����ԗ։�]���Z�b�g

																						//�ݓ_��ԃZ�b�g
	Vector3 _r(r0[ID_BOOM_H] * cos(r0[ID_SLEW]) + r0[ID_GANTRY], r0[ID_BOOM_H] * sin(r0[ID_SLEW]), pspec->Lp + pspec->Lm * sin(pSimStat->th.p));
	Vector3 _v(0.0, 0.0, 0.0);
	init_mob(_dt, _r, _v);

	set_v_ref(0.0, 0.0, 0.0, 0.0, 0.0);	//�������x�w�ߒl�Z�b�g
	set_fex(0.0, 0.0, 0.0);			//�����O�̓Z�b�g
		
	//�����x�ꎟ�x��t�B���^���萔
	Tf[ID_HOIST]	= SIM_TF_HOIST;
	Tf[ID_BOOM_H]	= SIM_TF_BOOM_H;
	Tf[ID_SLEW]		= SIM_TF_SLEW;
	Tf[ID_GANTRY]	= SIM_TF_GANTRY;
	Tf[ID_AHOIST]	= SIM_TF_AHOIST;

	//�v�Z�p�萔�Z�b�g
	cal_d1 = pspec->Lm * pspec->Lm + pspec->Lp * pspec->Lp;
	cal_d2 = 2.0 * pspec->Lm * pspec->Lp;

}
// �e���[�V�����̃u���[�L��Ԃ��Z�b�g
void CJC::update_break_status() {

	if(pPLC->brk[ID_HOIST]	!= 0) motion_break[ID_HOIST]	= true;
	if(pPLC->brk[ID_AHOIST]	!= 0) motion_break[ID_AHOIST] = true;
	if(pPLC->brk[ID_GANTRY]	!= 0) motion_break[ID_GANTRY] = true;
	if(pPLC->brk[ID_BOOM_H]	!= 0) motion_break[ID_BOOM_H] = true;
	if(pPLC->brk[ID_SLEW]	!= 0) motion_break[ID_SLEW] = true;

#if 0
	if (v_ref[ID_HOIST] != 0.0) {
		motion_break[ID_HOIST] = true;
	}
	else if ((v0[ID_HOIST] > pspec->notch_spd_r[ID_HOIST][1] * BREAK_CLOSE_RETIO)
		&& (v0[ID_HOIST] < pspec->notch_spd_f[ID_HOIST][1] * BREAK_CLOSE_RETIO)) {
		motion_break[ID_HOIST] = false;
	}
	else;

	if (v_ref[ID_GANTRY] != 0.0) {
		motion_break[ID_GANTRY] = true;
	}
	else if ((v0[ID_GANTRY] > pspec->notch_spd_r[ID_GANTRY][1] * BREAK_CLOSE_RETIO)
		&& (v0[ID_GANTRY] < pspec->notch_spd_f[ID_GANTRY][1] * BREAK_CLOSE_RETIO)) {
		motion_break[ID_GANTRY] = false;
	}
	else;

	if (v_ref[ID_SLEW] != 0.0) {
		motion_break[ID_SLEW] = true;
	}
	else if ((v0[ID_SLEW] > pspec->notch_spd_r[ID_SLEW][1] * BREAK_CLOSE_RETIO)
		&& (v0[ID_SLEW] < pspec->notch_spd_f[ID_SLEW][1] * BREAK_CLOSE_RETIO)) {
		motion_break[ID_SLEW] = false;
	}
	else;

	if (v_ref[ID_BOOM_H] != 0.0) {
		motion_break[ID_BOOM_H] = true;
	}
	else if ((v0[ID_BOOM_H] > pspec->notch_spd_r[ID_BOOM_H][1] * BREAK_CLOSE_RETIO)
		&& (v0[ID_BOOM_H] < pspec->notch_spd_f[ID_BOOM_H][1] * BREAK_CLOSE_RETIO)) {
		motion_break[ID_BOOM_H] = false;
	}
	else;


	if (motion_break[ID_HOIST]) {
		brk_elaped_time[ID_HOIST] += dt;
	}
	else {
		brk_elaped_time[ID_HOIST] = 0.0;
	}
	if (motion_break[ID_BOOM_H]) {
		brk_elaped_time[ID_BOOM_H] += dt;
	}
	else {
		brk_elaped_time[ID_BOOM_H] = 0.0;
	}
	if (motion_break[ID_SLEW]) {
		brk_elaped_time[ID_SLEW] += dt;
	}
	else {
		brk_elaped_time[ID_SLEW] = 0.0;
	}
	if (motion_break[ID_GANTRY]) {
		brk_elaped_time[ID_GANTRY] += dt;
	}
	else {
		brk_elaped_time[ID_GANTRY] = 0.0;
	}

#endif
	return;
}
void CJC::set_nbh_d_ph_th_from_r(double r) {
	pSimStat->th.p	= acos(r/pspec->Lm);					//���a�͎努�ݓ_�ʒu
	pSimStat->ph.p	= pspec->Php - pSimStat->th.p;
	pSimStat->d.p	= sqrt(pspec->Lm * pspec->Lm + pspec->Lp * pspec->Lp - 2.0 * pspec->Lm * pspec->Lp * cos(pSimStat->ph.p));
	pSimStat->db.p	= sqrt(pspec->Lb * pspec->Lb + pspec->Lp * pspec->Lp - 2.0 * pspec->Lb * pspec->Lp * cos(pSimStat->ph.p - pspec->Alpa_b ));
	pSimStat->th.v = pSimStat->ph.v = 0.0;
	pSimStat->th.a = pSimStat->ph.a = 0.0;


	//�N���h���������ʁ���N�����[�v���i�W�u�����[�v�{�h���������ʂ̘a�j�]�W�u�������[�v����
	pSimStat->l_drum[ID_BOOM_H] = pCraneStat->Cdr[ID_BOOM_H][0] - pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_BOOM_H] * pSimStat->d.p;
	int i;
	//�����h�����̑w��
	for (i = 1; i < PLC_DRUM_LAYER_MAX-1; i++) {
		if (pSimStat->l_drum[ID_BOOM_H] < pCraneStat->Ldr[ID_BOOM_H][i]) break;
	}
	//�N���N���h�����w��
	pSimStat->i_layer[ID_BOOM_H] = i;																										//���݂̑w
	pSimStat->n_layer[ID_BOOM_H] = (pSimStat->l_drum[ID_BOOM_H] - pCraneStat->Ldr[ID_BOOM_H][i-1])/ pCraneStat->Cdr[ID_BOOM_H][i];			//���݂̑w�̊���
																																			//�h����������
	//�h������]��
	pSimStat->nd[ID_BOOM_H].p = pspec->prm_nw[NW_ITEM_GROOVE][ID_BOOM_H] * ((double)i - 1.0) + pSimStat->n_layer[ID_BOOM_H];				//�����h�������i�a���~�i���ݑw��-1�j�{���ݑw�̊�����
	pSimStat->nd[ID_BHMH].p = pspec->Nbh_drum- pSimStat->nd[ID_BOOM_H].p;																	//�努�h�������i�努�����������̘a�]�������������j

	//�N���努�h�����w��
	pSimStat->i_layer[ID_BHMH] = (UINT32)(pSimStat->nd[ID_BHMH].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_BHMH]);								//���ݑw��-1�i�v�Z�r���j
	pSimStat->n_layer[ID_BHMH] = pSimStat->nd[ID_BHMH].p - (double)pSimStat->i_layer[ID_BHMH] * pspec->prm_nw[NW_ITEM_GROOVE][ID_BHMH];		//�w����������]��-�i���ݑw��-1�j*�a��
	pSimStat->i_layer[ID_BHMH]++;																											//���ݑw��
	
	//�N���努�h����������
	pSimStat->l_drum[ID_BHMH] = pCraneStat->Ldr[ID_BHMH][pSimStat->i_layer[ID_BHMH] - 1] + pCraneStat->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]] * pSimStat->n_layer[ID_BHMH];
	return;
}
//���񔼌a�Ǝ努�g������努�h������]�����Z�b�g����
void CJC::set_nmh_from_mh(double mh) {
	//���[�v��
	double lrope = pspec->Hp + pspec->Lm * sin(pSimStat->th.p) - mh;//�努�݃��[�v�����W�u�������{�W�u�N������-�g��
	
	//�努�h���������� ����[�v���i�努�A�N���h���������ʁ{�W�u���{���[�v�����̑��a�j-�@�W�u���@- �N���h�������� - ���[�v����
	pSimStat->l_drum[ID_HOIST] = pCraneStat->Cdr[ID_HOIST][0] - pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_HOIST] * pSimStat->d.p - pSimStat->l_drum[ID_BHMH] - lrope* pspec->prm_nw[NW_ITEM_WIND][ID_HOIST];

	int i;
	for (i = 1; i < PLC_DRUM_LAYER_MAX - 1; i++) {
		if (pSimStat->l_drum[ID_HOIST] < pCraneStat->Ldr[ID_HOIST][i]) break;
	}
	pSimStat->i_layer[ID_HOIST] = i;
	pSimStat->n_layer[ID_HOIST] = (pSimStat->l_drum[ID_HOIST] - pCraneStat->Ldr[ID_HOIST][i - 1])/ pCraneStat->Cdr[ID_HOIST][i];

	//�努�h������]��
	pSimStat->nd[ID_HOIST].p = pspec->prm_nw[NW_ITEM_GROOVE][ID_HOIST] * ((double)i - 1.0) + pSimStat->n_layer[ID_HOIST];
	return;

}
//d�ƕ⊪�g������⊪�h������]�����Z�b�g����
void CJC::set_nah_from_ah(double ah) { 
	
	//���[�v��
	double lrope = pspec->Hp + pspec->La * sin(pSimStat->th.p + pspec->Alpa_a) - ah;//�⊪�݃��[�v�����W�u�������{�W�u�N������-�g��

	//�⊪�h���������� ����[�v���i�⊪�h���������ʁ{�W�u���{���[�v�����̑��a�j-�@�W�u���@- �N���h�������� - ���[�v����
	pSimStat->l_drum[ID_AHOIST] = pCraneStat->Cdr[ID_AHOIST][0] - pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_AHOIST] * pSimStat->d.p  - lrope * pspec->prm_nw[NW_ITEM_WIND][ID_AHOIST];

	int i;
	for (i = 1; i < PLC_DRUM_LAYER_MAX - 1; i++) {
		if (pSimStat->l_drum[ID_AHOIST] < pCraneStat->Ldr[ID_AHOIST][i]) break;
	}
	pSimStat->i_layer[ID_AHOIST] = i;
	pSimStat->n_layer[ID_AHOIST] = (pSimStat->l_drum[ID_AHOIST] - pCraneStat->Ldr[ID_AHOIST][i - 1]) / pCraneStat->Cdr[ID_AHOIST][i];

	//�⊪�h������]��
	pSimStat->nd[ID_AHOIST].p = pspec->prm_nw[NW_ITEM_GROOVE][ID_AHOIST] * ((double)i - 1.0) + pSimStat->n_layer[ID_AHOIST];

	return; 
} 
//����ʒu(rad)�������s�j�I����]�����Z�b�g����
void CJC::set_nsl_from_slr(double sl_rad) { 
	
	pSimStat->nd[ID_SLEW].p = sl_rad / pCraneStat->Cdr[ID_SLEW][0];						//Cdr[0]�s�j�I���~��
	pSimStat->i_layer[ID_SLEW] = 1;
	pSimStat->n_layer[ID_SLEW] = pSimStat->nd[ID_SLEW].p;

	pSimStat->l_drum[ID_SLEW] = pCraneStat->Cdr[ID_SLEW][0] * pSimStat->n_layer[ID_SLEW];//Cdr[0]�s�j�I���~��
	return; 
}
//���s�ʒu���瑖�s�ԗ։�]�����Z�b�g����
void CJC::set_ngt_from_gtm(double gt_m) {
	pSimStat->nd[ID_GANTRY].p = gt_m / pCraneStat->Cdr[ID_GANTRY][0];											//Cdr[0]�ԗ։~��
	pSimStat->i_layer[ID_GANTRY] = 1;
	pSimStat->n_layer[ID_GANTRY] = pSimStat->nd[ID_GANTRY].p;

	pSimStat->l_drum[ID_GANTRY] = pCraneStat->Cdr[ID_GANTRY][0] * pSimStat->n_layer[ID_GANTRY];//Cdr[0]�ԗ։~��
	return;
}
//�����h������]��Ԃ���d,th,d",th",d"",th""���Z�b�g����
void CJC::set_d_th_from_nbh() {

	//�����h�����w���
	pSimStat->i_layer[ID_BOOM_H] = (UINT32)(pSimStat->nd[ID_BOOM_H].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_BOOM_H]) + 1;
	pSimStat->n_layer[ID_BOOM_H] = pSimStat->nd[ID_BOOM_H].p - pspec->prm_nw[NW_ITEM_GROOVE][ID_BOOM_H]*(pSimStat->i_layer[ID_BOOM_H]-1);
	//�N���h����������	
	pSimStat->l_drum[ID_BOOM_H] = pCraneStat->Ldr[pSimStat->i_layer[ID_BOOM_H] - 1][ID_BOOM_H] + pSimStat->n_layer[ID_BOOM_H] * pCraneStat->Cdr[pSimStat->i_layer[ID_BOOM_H]][ID_BOOM_H];
	//�W�u�������@�i����[�v��-�h���������ʁj���[�v�|��
	pSimStat->db.p = (pCraneStat->Cdr[0][ID_BOOM_H]- pSimStat->l_drum[ID_BOOM_H])/ pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_BOOM_H];
	pSimStat->d.p = sqrt(pSimStat->db.p* pSimStat->db.p + Lmb2-2.0 * pSimStat->db.p * LmbCosAdb);

	c_ph = (cal_d1 - pSimStat->d.p * pSimStat->d.p) / cal_d2;
	pSimStat->ph.p = acos(c_ph);					//��
	s_ph = sin(pSimStat->ph.p);
	pSimStat->th.p = pspec->Php - pSimStat->ph.p;	//��
			
	double LLsinPh = pspec->Lb * pspec->Lp * s_ph;
	double c1 = pCraneStat->Cdr[pSimStat->i_layer[ID_BOOM_H]][ID_BOOM_H] / pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_BOOM_H];//�h��������/���C���|��
	
	pSimStat->db.v = -pSimStat->nd[ID_BOOM_H].v * c1;//�h��������/���C���|��;
	pSimStat->d.v = (pSimStat->db.p - LmbCosAdb) / LmLb * pSimStat->db.v;

	pSimStat->ph.v = pSimStat->d.p * pSimStat->d.v / LLsinPh;		//d��/dt = dd'/LpLbsin��
	pSimStat->th.v = -pSimStat->ph.v;								//-d��/dt

	//�����x
	pSimStat->db.a = -pSimStat->nd[ID_BOOM_H].a * c1;														//��]�͈����������{�i�{��]��d�k�j
	pSimStat->d.a = pSimStat->db.v* pSimStat->db.v/LmLb + pSimStat->d.v* pSimStat->db.a / pSimStat->db.v;	//��]�͈����������{�i�{��]��d�k�j

	pSimStat->ph.a = pSimStat->ph.v * (pSimStat->d.v / pSimStat->d.p + pSimStat->d.a/ pSimStat->d.v  - pSimStat->ph.v * c_ph / s_ph);
	pSimStat->th.a = -pSimStat->ph.a;

	return; 
} 

//�����h������Ԃ��Z�b�g����
void  CJC::set_bh_layer() { 
	UINT32 i = (UINT32)(pSimStat->nd[ID_BOOM_H].p  / pspec->prm_nw[NW_ITEM_GROOVE][ID_BOOM_H]); //���ݑw��-1
	pSimStat->i_layer[ID_BOOM_H] = i + 1;
	pSimStat->n_layer[ID_BOOM_H] = (pCraneStat->Cdr[ID_BOOM_H][0] - pCraneStat->Ldr[ID_BOOM_H][i]) / pCraneStat->Cdr[ID_BOOM_H][pSimStat->i_layer[ID_BOOM_H]];
	pSimStat->l_drum[ID_BOOM_H] = pCraneStat->Ldr[ID_BOOM_H][i] + pSimStat->n_layer[ID_BOOM_H] * pCraneStat->Cdr[ID_BOOM_H][pSimStat->i_layer[ID_BOOM_H]];
	return; 
}
//�努�h������]��Ԃ��Z�b�g����
void  CJC::set_mh_layer(){
	//�����努�h������
	pSimStat->nd[ID_BHMH].p = pspec->Nbh_drum - pSimStat->nd[ID_BOOM_H].p;
	int i = (UINT32)(pSimStat->nd[ID_BHMH].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_BHMH]); //���ݑw��-1
	pSimStat->i_layer[ID_BHMH] = i + 1;
	pSimStat->n_layer[ID_BHMH] = (pCraneStat->Cdr[ID_BHMH][0] - pCraneStat->Ldr[ID_BHMH][i]) / pCraneStat->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]];
	pSimStat->l_drum[ID_BHMH] = pCraneStat->Ldr[ID_BHMH][i] + pSimStat->n_layer[ID_BHMH] * pCraneStat->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]];

	//�努�h������
	i = (UINT32)(pSimStat->nd[ID_HOIST].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_HOIST]); //���ݑw��-1
	pSimStat->i_layer[ID_HOIST] = i + 1;
	pSimStat->n_layer[ID_HOIST] = (pSimStat->nd[ID_HOIST].p - pCraneStat->Ldr[ID_HOIST][i]) / pCraneStat->Cdr[ID_HOIST][pSimStat->i_layer[ID_HOIST]];
	pSimStat->l_drum[ID_HOIST] = pCraneStat->Ldr[ID_HOIST][i] + pSimStat->n_layer[ID_HOIST] * pCraneStat->Cdr[ID_HOIST][pSimStat->i_layer[ID_HOIST]];

	return; 
} 
//�⊪�h������]��Ԃƃ��[�v�����Z�b�g����
void  CJC::set_ah_layer(){
	//�⊪�h������
	int i= (UINT32)(pSimStat->nd[ID_AHOIST].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_AHOIST]); //���ݑw��-1
	pSimStat->i_layer[ID_AHOIST] = i + 1;
	pSimStat->n_layer[ID_AHOIST] = (pSimStat->nd[ID_AHOIST].p - pCraneStat->Ldr[ID_AHOIST][i]) / pCraneStat->Cdr[ID_AHOIST][pSimStat->i_layer[ID_AHOIST]];
	pSimStat->l_drum[ID_AHOIST] = pCraneStat->Ldr[ID_AHOIST][i] + pSimStat->n_layer[ID_AHOIST] * pCraneStat->Cdr[ID_AHOIST][pSimStat->i_layer[ID_AHOIST]];//�h����������

	return;
} 
//����h������]��Ԃ��Z�b�g����
void  CJC::set_sl_layer(){
	//����s�j�I����
	pSimStat->l_drum[ID_SLEW]	= pSimStat->nd[ID_SLEW].p * pCraneStat->Cdr[ID_SLEW][1];										//����ړ���(m)
	pSimStat->i_layer[ID_SLEW]	= 1;// (UINT32)(pSimStat->l_drum[ID_SLEW] / pCraneStat->Cdr[ID_SLEW][0]);						//�����]�������� = ��]�ړ���/TTB�~��
	pSimStat->n_layer[ID_SLEW]	= 0;// (pSimStat->l_drum[ID_SLEW] / pCraneStat->Cdr[ID_SLEW][0] - pSimStat->i_layer[ID_SLEW]);	//�����]�������_�ȉ�
	return; 
} 
//���s�h������]��Ԃ��Z�b�g����
void  CJC::set_gt_layer(){
	pSimStat->l_drum[ID_GANTRY] = pSimStat->nd[ID_GANTRY].p * pCraneStat->Cdr[ID_GANTRY][1];										//�ړ���(m)
	pSimStat->i_layer[ID_GANTRY] = 1;// (UINT32)(pSimStat->nd[ID_GANTRY].p);																//���s�ԗ։�]����
	pSimStat->n_layer[ID_GANTRY] = 0;// (pSimStat->l_drum[ID_GANTRY] / pCraneStat->Cdr[ID_GANTRY][0] - pSimStat->i_layer[ID_GANTRY]);	//�����]�������_�ȉ�
	return; 
} 

/********************************************************************************/
/*      Load Object(�ׁ݉j                                                      */
/********************************************************************************/

//�݉׈ʒu�̏�����
void CLoad ::init_mob(double _dt, Vector3& _r, Vector3& _v) {
	dt = _dt;
	r.copy(_r);
	v.copy(_v);
	return;
}

Vector3 CLoad::A(Vector3& r, Vector3& v) {
	Vector3 a;
	Vector3 L_;

	L_ = L_.subVectors(r, pCrane->r);

	double Sdivm = S() / m;

	a = L_.clone().multiplyScalor(Sdivm);
	a.z -= GA;

	double temp_d = L_.length();

	//�v�Z�덷�ɂ�郍�[�v������␳
	Vector3 hatL = L_.clone().normalize();
	// �␳�΂˒e����
	Vector3 ak = hatL.clone().multiplyScalor(-compensationK * (pCrane->l_mh - L_.length()));
	Vector3 v_ = v_.subVectors(v, pCrane->v);
	// �␳�S����R��
	Vector3 agamma = hatL.clone().multiplyScalor(-compensationGamma * v_.dot(hatL));
	// ���͂ɂЂ��̒����̕␳�͂�������
	a.add(ak).add(agamma);

	return a;
} //Model of acceleration

double  CLoad::S() { //A�̌v�Z���̊֌W��S/L�ƂȂ��Ă���B�����̉����x�����ǉ�����Ă���B
	Vector3 v_ = v.clone().sub(pCrane->v);
	double v_abs2 = v_.lengthSq();
	Vector3 vectmp;
	Vector3 vecL = vectmp.subVectors(r, pCrane->r);

	return -m * (v_abs2 - pCrane->a.dot(vecL) - GA * vecL.z - (pCrane->a0[ID_HOIST] * pCrane->l_mh + pCrane->v0[ID_HOIST] * pCrane->v0[ID_HOIST])) / (pCrane->l_mh * pCrane->l_mh);

}

void CLoad::update_relative_vec() {//�N���[���ݓ_�Ƃ̑��Α��x
	Vector3 vectmp;
	L = vectmp.subVectors(r,pCrane->r);
	vL = vectmp.subVectors(v,pCrane->v);
	return;
}


