#include "CMob.h"
#include "SIMparaDEF.h"

extern ST_SPEC def_spec;

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
	accdec_cut_spd_range[ID_HOIST]	= 0.1;		//%
	accdec_cut_spd_range[ID_BOOM_H] = 0.1;		//%
	accdec_cut_spd_range[ID_SLEW]	= 0.1;		//%
	accdec_cut_spd_range[ID_GANTRY] = 0.1;		//%		
	accdec_cut_spd_range[ID_AHOIST] = 0.1;		//%

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
void CJC::set_v_ref(double hoist_ref, double gantry_ref, double slew_ref, double boomh_ref, double ah_ref) {
	v_ref[ID_HOIST] = hoist_ref;
	v_ref[ID_BOOM_H] = boomh_ref;
	v_ref[ID_SLEW] = slew_ref;
	v_ref[ID_GANTRY] = gantry_ref;
	v_ref[ID_AHOIST] = ah_ref;
	return;
}
// �ٸT(N�Em�j= F x R�@= J x d��/dt  �d����P=T��=Mav�@a=T��/Mv=MT/r
Vector3 CJC::A(Vector3& _r, Vector3& _v) {					//�ݓ_�̉����x

	Vector3 a;
	double r_bm = r0[ID_BOOM_H];//���񔼌a
	//���p�����x�^���@a = r x d��/dt(-sin��,cos��) -rx��^2(cos��,sin��)
	a.x = v0[ID_BOOM_H] * sin(r0[ID_SLEW]) - r_bm * a0[ID_SLEW] * sin(r0[ID_SLEW]) - r_bm * v0[ID_SLEW] * v0[ID_SLEW] * cos(r0[ID_SLEW]) + a0[ID_GANTRY];
	a.y = v0[ID_BOOM_H] * cos(r0[ID_SLEW]) + r_bm * a0[ID_SLEW] * cos(r0[ID_SLEW]) - r_bm * v0[ID_SLEW] * v0[ID_SLEW] * sin(r0[ID_SLEW]);
	a.z = 0.0;




	return a;
}
void CJC::Ac() {	//�����x�v�Z

	//�����w�ߌv�Z
	// #�努
	{
		//�u���[�L��
		if (!motion_break[ID_HOIST]) a_ref[ID_HOIST] = 0.0;
		//���x�w�߂ɖ��B
		else if ((v_ref[ID_HOIST] - v0[ID_HOIST]) > accdec_cut_spd_range[ID_HOIST]) {
			if (v_ref[ID_HOIST] > 0.0) a_ref[ID_HOIST] = pspec->accdec100[ID_HOIST][FWD][ACC];		//���]�����w��
			else a_ref[ID_HOIST] = pspec->accdec100[ID_HOIST][REV][DEC];		//�t�]�����w��
		}
		//���x�w�߂ɖ��B
		else if ((v_ref[ID_HOIST] - v0[ID_HOIST]) < -accdec_cut_spd_range[ID_HOIST]) {
			if (v_ref[ID_HOIST] > 0.0) a_ref[ID_HOIST] = pspec->accdec100[ID_HOIST][FWD][DEC];		//���]�����w��
			else a_ref[ID_HOIST] = pspec->accdec100[ID_HOIST][REV][ACC];		//�t�]�����w��
		}
		//���x�w�߂ɓ��B
		else {
			a_ref[ID_HOIST] = 0.0;
		}

		//�Ɍ���~
		if ((a_ref[ID_HOIST] > 0.0) && (is_fwd_endstop[ID_HOIST])) a_ref[ID_HOIST] = 0.0;
		if ((a_ref[ID_HOIST] < 0.0) && (is_rev_endstop[ID_HOIST])) a_ref[ID_HOIST] = 0.0;
	}

	// #�⊪
	{
		//�u���[�L��
		if (!motion_break[ID_AHOIST]) a_ref[ID_AHOIST] = 0.0;
		//���x�w�߂ɖ��B
		else if ((v_ref[ID_AHOIST] - v0[ID_AHOIST]) > accdec_cut_spd_range[ID_AHOIST]) {
			if (v_ref[ID_AHOIST] > 0.0) a_ref[ID_AHOIST] = pspec->accdec100[ID_AHOIST][FWD][ACC];		//���]�����w��
			else a_ref[ID_AHOIST] = pspec->accdec100[ID_AHOIST][REV][DEC];		//�t�]�����w��
		}
		//���x�w�߂ɖ��B
		else if ((v_ref[ID_AHOIST] - v0[ID_AHOIST]) < -accdec_cut_spd_range[ID_AHOIST]) {
			if (v_ref[ID_AHOIST] > 0.0) a_ref[ID_AHOIST] = pspec->accdec100[ID_AHOIST][FWD][DEC];		//���]�����w��
			else a_ref[ID_AHOIST] = pspec->accdec100[ID_AHOIST][REV][ACC];		//�t�]�����w��
		}
		//���x�w�߂ɓ��B
		else {
			a_ref[ID_AHOIST] = 0.0;
		}

		//�Ɍ���~
		if ((a_ref[ID_AHOIST] > 0.0) && (is_fwd_endstop[ID_AHOIST])) a_ref[ID_AHOIST] = 0.0;
		if ((a_ref[ID_AHOIST] < 0.0) && (is_rev_endstop[ID_AHOIST])) a_ref[ID_AHOIST] = 0.0;
	}

	// #���s
	{
		if (!motion_break[ID_GANTRY]) a_ref[ID_GANTRY] = 0.0;
		else if ((v_ref[ID_GANTRY] - v0[ID_GANTRY]) > accdec_cut_spd_range[ID_GANTRY]) {
			if (v_ref[ID_GANTRY] > 0.0) a_ref[ID_GANTRY] = pspec->accdec100[ID_GANTRY][FWD][ACC];	//���]����
			else a_ref[ID_GANTRY] = pspec->accdec100[ID_GANTRY][REV][DEC];	//�t�]����
		}
		else if ((v_ref[ID_GANTRY] - v0[ID_GANTRY]) < -accdec_cut_spd_range[ID_GANTRY]) {
			if (v_ref[ID_GANTRY] > 0.0) a_ref[ID_GANTRY] = pspec->accdec100[ID_GANTRY][FWD][DEC];	//���]����
			else a_ref[ID_GANTRY] = pspec->accdec100[ID_GANTRY][REV][ACC];	//�t�]����
		}
		else {
			a_ref[ID_GANTRY] = 0.0;
		}

		//�Ɍ���~
		if ((a_ref[ID_GANTRY] > 0.0) && (is_fwd_endstop[ID_GANTRY])) a_ref[ID_GANTRY] = 0.0;
		if ((a_ref[ID_GANTRY] < 0.0) && (is_rev_endstop[ID_GANTRY])) a_ref[ID_GANTRY] = 0.0;
	}

	// #����
	{
		if (!motion_break[ID_BOOM_H]) a_ref[ID_BOOM_H] = 0.0;
		else if ((v_ref[ID_BOOM_H] - v0[ID_BOOM_H]) > accdec_cut_spd_range[ID_BOOM_H]) {
			if (v_ref[ID_BOOM_H] > 0.0) a_ref[ID_BOOM_H] = pspec->accdec100[ID_BOOM_H][FWD][ACC];	//���]����
			else a_ref[ID_BOOM_H] = pspec->accdec100[ID_BOOM_H][REV][DEC];	//�t�]����
		}
		else if ((v_ref[ID_BOOM_H] - v0[ID_BOOM_H]) < -accdec_cut_spd_range[ID_BOOM_H]) {
			if (v_ref[ID_BOOM_H] > 0.0) a_ref[ID_BOOM_H] = pspec->accdec100[ID_BOOM_H][FWD][DEC];	//���]����
			else a_ref[ID_BOOM_H] = pspec->accdec100[ID_BOOM_H][REV][ACC];	//�t�]����
		}
		else {
			a_ref[ID_BOOM_H] = 0.0;
		}

		//�Ɍ���~
		if ((a_ref[ID_BOOM_H] > 0.0) && (is_fwd_endstop[ID_BOOM_H])) a_ref[ID_BOOM_H] = 0.0;
		if ((a_ref[ID_BOOM_H] < 0.0) && (is_rev_endstop[ID_BOOM_H])) a_ref[ID_BOOM_H] = 0.0;
	}

	// #����
	{
		if (!motion_break[ID_SLEW]) a_ref[ID_SLEW] = 0.0;
		else if ((v_ref[ID_SLEW] - v0[ID_SLEW]) > accdec_cut_spd_range[ID_SLEW]) {
			if (v_ref[ID_SLEW] > 0.0) a_ref[ID_SLEW] = pspec->accdec100[ID_SLEW][FWD][ACC];//���]����
			else a_ref[ID_SLEW] = pspec->accdec100[ID_SLEW][REV][DEC];//�t�]����
		}
		else if ((v_ref[ID_SLEW] - v0[ID_SLEW]) < -accdec_cut_spd_range[ID_SLEW]) {
			if (v_ref[ID_SLEW] > 0.0) a_ref[ID_SLEW] = pspec->accdec100[ID_SLEW][FWD][DEC];//���]����
			else a_ref[ID_SLEW] = pspec->accdec[ID_SLEW][REV][ACC];//�t�]����
		}
		else {
			a_ref[ID_SLEW] = 0.0;
		}
	}

	//�h���������x�v�Z�@���ʎw�߂ɑ΂��Ĉꎟ�x��t�B���^������`�Ōv�Z�i�����I�Ƀg���N�w�߂���̓��o�����j
	{
		//�努
		if ((motion_break[ID_HOIST]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_HOIST].a = (dt * a_ref[ID_HOIST] + Tf[ID_HOIST] * pSimStat->nd[ID_HOIST].a )/ (dt + Tf[ID_HOIST]);
		}
		else {
			pSimStat->nd[ID_HOIST].a = 0.0;
		}
		//�⊪
		if ((motion_break[ID_AHOIST]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_AHOIST].a = (dt * a_ref[ID_AHOIST] + Tf[ID_AHOIST] * pSimStat->nd[ID_AHOIST].a) / (dt + Tf[ID_AHOIST]);
		}
		else {
			pSimStat->nd[ID_AHOIST].a = 0.0;
		}
		//����
		if ((motion_break[ID_BOOM_H]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_BOOM_H].a = (dt * a_ref[ID_BOOM_H] + Tf[ID_BOOM_H] * pSimStat->nd[ID_BOOM_H].a) / (dt + Tf[ID_BOOM_H]);
		}
		else {
			pSimStat->nd[ID_BOOM_H].a = 0.0;
		}
		//����
		if ((motion_break[ID_SLEW]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_SLEW].a = (dt * a_ref[ID_SLEW] + Tf[ID_SLEW] * pSimStat->nd[ID_SLEW].a) / (dt + Tf[ID_SLEW]);
		}
		else {
			pSimStat->nd[ID_SLEW].a = 0.0;
		}
		//���s
		if ((motion_break[ID_GANTRY]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_GANTRY].a = (dt * a_ref[ID_GANTRY] + Tf[ID_GANTRY] * pSimStat->nd[ID_GANTRY].a) / (dt + Tf[ID_GANTRY]);
		}
		else {
			pSimStat->nd[ID_GANTRY].a = 0.0;
		}
	}

	//�ݓ_�A�݋�����x�v�Z
	double thm = pSimStat->th.p - pspec->Alpa_m;
	double tha = pSimStat->th.p - pspec->Alpa_a;
	a0[ID_HOIST] = pspec->Lm * (pSimStat->th.a * cos(thm) - pSimStat->th.v * pSimStat->th.v * sin(thm))  + pSimStat->lrm.a;
	a0[ID_BOOM_H] = -pspec->Lm * (pSimStat->th.a * sin(thm) + pSimStat->th.a * pSimStat->th.a * cos(thm));
	a0[ID_AHOIST] = pspec->La * (pSimStat->th.a * cos(tha) - pSimStat->th.v * pSimStat->th.v * sin(tha)) + pSimStat->lra.a;
	a0[ID_SLEW] = pSimStat->nd[ID_SLEW].a * PI360;
	a0[ID_GANTRY] = pSimStat->nd[ID_GANTRY].a * pPLC->Cdr[ID_GANTRY][1];

	//�ݓ_�����x�x�N�g���i�~�����W�j
	double a_er = a0[ID_BOOM_H] - r0[ID_BOOM_H] * v0[ID_SLEW] * v0[ID_SLEW];		//�������������x�@���������x�{����(R��2�j
	double a_eth = r0[ID_BOOM_H] * a0[ID_SLEW] + 2.0 * v0[ID_BOOM_H] * v0[ID_SLEW];	//������������x
	double a_z = 0.0;																//�ݓ_�̍����͈��

	//xyz���W�ݓ_�����x�x�N�g��
	a.x = a0[ID_GANTRY] + a_er * cos(r0[ID_SLEW]) - a_eth * sin(r0[ID_SLEW]);
	a.y = a_er * sin(r0[ID_SLEW]) + a_eth * cos(r0[ID_SLEW]);
	a.z = a_z;
	return;
}
void CJC::timeEvolution() {
	//�N���[����
	//�����x�v�Z
	Ac();

	//�h�������x�v�Z(�I�C���[�@�j
	pSimStat->nd[ID_HOIST].v	+= pSimStat->nd[ID_HOIST].a * dt;	if (!motion_break[ID_HOIST])	pSimStat->nd[ID_HOIST].v = 0.0;
	pSimStat->nd[ID_AHOIST].v	+= pSimStat->nd[ID_AHOIST].a * dt;	if (!motion_break[ID_AHOIST])	pSimStat->nd[ID_AHOIST].v = 0.0;
	pSimStat->nd[ID_BOOM_H].v	+= pSimStat->nd[ID_BOOM_H].a * dt;	if (!motion_break[ID_BOOM_H])	pSimStat->nd[ID_BOOM_H].v = 0.0;
	pSimStat->nd[ID_SLEW].v		+= pSimStat->nd[ID_SLEW].a * dt;	if (!motion_break[ID_SLEW])		pSimStat->nd[ID_SLEW].v = 0.0;
	pSimStat->nd[ID_GANTRY].v	+= pSimStat->nd[ID_GANTRY].a * dt;	if (!motion_break[ID_GANTRY])	pSimStat->nd[ID_GANTRY].v = 0.0;

	//�Ɍ���~
	if (((v0[ID_HOIST]  > 0.0) && (is_fwd_endstop[ID_HOIST]))	|| (v0[ID_HOIST] < 0.0)	 && (is_rev_endstop[ID_HOIST]))		pSimStat->nd[ID_HOIST].v  = 0.0;
	if (((v0[ID_AHOIST] > 0.0) && (is_fwd_endstop[ID_AHOIST]))	|| (v0[ID_AHOIST] < 0.0) && (is_rev_endstop[ID_AHOIST]))	pSimStat->nd[ID_AHOIST].v = 0.0;
	if (((v0[ID_GANTRY] > 0.0) && (is_fwd_endstop[ID_GANTRY]))	|| (v0[ID_GANTRY] < 0.0) && (is_rev_endstop[ID_GANTRY]))	pSimStat->nd[ID_GANTRY].v = 0.0;
	if (((v0[ID_BOOM_H] > 0.0) && (is_fwd_endstop[ID_BOOM_H]))	|| (v0[ID_BOOM_H] < 0.0) && (is_rev_endstop[ID_BOOM_H]))	pSimStat->nd[ID_BOOM_H].v = 0.0;

	//�h�����ʒu�v�Z(�I�C���[�@�j
	pSimStat->nd[ID_HOIST].p	+= pSimStat->nd[ID_HOIST].v  * dt;
	pSimStat->nd[ID_AHOIST].p	+= pSimStat->nd[ID_AHOIST].v * dt;
	pSimStat->nd[ID_GANTRY].p	+= pSimStat->nd[ID_GANTRY].v * dt;
	pSimStat->nd[ID_BOOM_H].p	+= pSimStat->nd[ID_BOOM_H].v * dt;
	pSimStat->nd[ID_SLEW].p		+= pSimStat->nd[ID_SLEW].v   * dt;

	//�N���[����ԃZ�b�g
	set_d_th_from_nbh();                        //�����h������]��Ԃ���d,�Ƃ̏�Ԃ��Z�b�g����
	set_bh();                                   //�����h������Ԃ��Z�b�g����
	set_mh();                                   //�努�h������ԁA���[�v��Ԃ��Z�b�g����
	set_ah();                                   //�努�h������ԁA���[�v��Ԃ��Z�b�g����
	set_sl();                                   //����h������Ԃ��Z�b�g����
	set_gt();                                   //���s�h������Ԃ��Z�b�g����

	double thm = pSimStat->th.p - pspec->Alpa_m;

	v0[ID_HOIST]	= pspec->Lm * pSimStat->th.v * cos(pSimStat->th.p - pspec->Alpa_m) - pSimStat->lrm.v;
	v0[ID_AHOIST]	= pspec->La * pSimStat->th.v * cos(pSimStat->th.p - pspec->Alpa_a) - pSimStat->lra.v;
	v0[ID_SLEW]		= pSimStat->nd[ID_SLEW].v * PI360;
	v0[ID_GANTRY]	= pSimStat->nd[ID_GANTRY].v * pPLC->Cdr[ID_GANTRY][1];
	v0[ID_BOOM_H]	= -pspec->Lm * pSimStat->th.v * sin(thm);

	r0[ID_HOIST]	= pspec->Hp + pspec->Lm * sin(pSimStat->th.p - pspec->Alpa_m) - pSimStat->lrm.p;
	r0[ID_AHOIST]	= pspec->Hp + pspec->La * sin(pSimStat->th.p - pspec->Alpa_a) - pSimStat->lra.p;
	r0[ID_SLEW]		= pSimStat->n_layer[ID_SLEW] * PI360 - PI180; //����́}180���ŕ\��
	r0[ID_GANTRY]	= pSimStat->l_drum[ID_GANTRY];
	r0[ID_BOOM_H] = pspec->Lm * cos(thm);

	vc.x = v0[ID_GANTRY]; vc.y = 0.0; vc.z = 0.0;		//�N���[�����S�ʒu
	rc.x = r0[ID_GANTRY]; rc.y = R0.y; rc.z = R0.z;		//�N���[�����S�ʒu

	//�ݓ_��
	double r_bm = r0[ID_BOOM_H];//���񔼌a
	v.x = v0[ID_BOOM_H] * cos(r0[ID_SLEW]) - r_bm * v0[ID_SLEW] * sin(r0[ID_SLEW]) + v0[ID_GANTRY];
	v.y = v0[ID_BOOM_H] * sin(r0[ID_SLEW]) + r_bm * v0[ID_SLEW] * cos(r0[ID_SLEW]);
	v.z = 0.0;

	r.x = r_bm * cos(r0[ID_SLEW]) + r0[ID_GANTRY];
	r.y = r_bm * sin(r0[ID_SLEW]);
	r.z = pspec->boom_high;

	//���[�v���Z�b�g�@LOAD�I�u�W�F�N�g����Q��
	l_mh = pSimStat->lrm.p;
	l_ah = pSimStat->lra.p;

	return;
}
void CJC::init_crane(double _dt) {
	
	//�h������]�ʒu�Z�b�g
	set_nbh_d_ph_th_from_r(SIM_INIT_R);

	set_nmh_from_d_mh(pSimStat->d.p, SIM_INIT_MH); 

	set_nah_from_d_ah(pSimStat->d.p, SIM_INIT_AH);

	slw_rad_per_turn = PI360 * pspec->prm_nw[DRUM_ITEM_DIR][ID_SLEW] / pspec->Dttb;		//�s�j�I���P��]�ł̐���p�x
	set_nsl_from_slr(SIM_INIT_TH); 

	gnt_m_per_turn = PI180 * pspec->prm_nw[DRUM_ITEM_DIR][ID_GANTRY];
	set_ngt_from_gtm(SIM_INIT_X);
	
	//�N���[���p�����[�^�Z�b�g

	if (source_mode == MOB_MODE_PLC) {
		//�N���[����_�̏����ʒu,���x
		rc.x = pPLC->pos[ID_GANTRY]; rc.y = 0.0; rc.z = 0.0;
		vc.x = pPLC->v_fb[ID_GANTRY]; vc.y = 0.0; vc.z = 0.0;

		Vector3 _r(SIM_INIT_R * cos(SIM_INIT_TH) + SIM_INIT_X, SIM_INIT_R * sin(SIM_INIT_TH), pspec->boom_high);

		init_mob(_dt, _r, vc);

		//r0�́A�e���A�u�\�R�[�_�̒l,�@r�́A�ݓ_��xyz���W�̒l
		r0[ID_HOIST] = pPLC->pos[ID_HOIST];
		r0[ID_GANTRY] = rc.x;
		r0[ID_SLEW] = pPLC->pos[ID_SLEW];
		r0[ID_BOOM_H] = pPLC->pos[ID_BOOM_H];
	}
	else {
		//�N���[����_�̏����ʒu,���x
		rc.x = SIM_INIT_X; rc.y = 0.0; rc.z = 0.0;
		vc.x = 0.0; vc.y = 0.0; vc.z = 0.0;

		Vector3 _r(SIM_INIT_R * cos(SIM_INIT_TH) + SIM_INIT_X, SIM_INIT_R * sin(SIM_INIT_TH), pspec->boom_high);

		init_mob(_dt, _r, vc);

		//r0�́A�e���A�u�\�R�[�_�̒l,�@r�́A�ݓ_��xyz���W�̒l
		r0[ID_HOIST] = r.z - SIM_INIT_L;
		r0[ID_GANTRY] = rc.x;
		r0[ID_SLEW] = SIM_INIT_TH;
		r0[ID_BOOM_H] = SIM_INIT_R;
	}

	set_v_ref(0.0, 0.0, 0.0, 0.0, 0.0);	//�������x�w�ߒl�Z�b�g
	set_fex(0.0, 0.0, 0.0);			//�����O�̓Z�b�g
		
	//�����x�ꎟ�x��t�B���^���萔
	Tf[ID_HOIST]	= SIM_TF_HOIST;
	Tf[ID_BOOM_H]	= SIM_TF_BOOM_H;
	Tf[ID_SLEW]		= SIM_TF_SLEW;
	Tf[ID_GANTRY]	= SIM_TF_GANTRY;
	Tf[ID_AHOIST]	= SIM_TF_AHOIST;

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
	pSimStat->th.p = acos(r/pspec->Lb);
	pSimStat->ph.p = pspec->Php - pSimStat->th.p;
	pSimStat->d.p = sqrt(pspec->Lb* pspec->Lb + pspec->Lp * pspec->Lp - 2.0* pspec->Lb * pspec->Lp * cos(pSimStat->ph.p));

	double ln = pPLC->Cdr[ID_BOOM_H][0] - pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_BOOM_H] * pSimStat->d.p;
	int i;
	for (i = 1; i < PLC_DRUM_LAYER_MAX-1; i++) {
		if (ln < pPLC->Ldr[ID_BOOM_H][i]) break;
	}

	//�N���N���h�����w��
	pSimStat->i_layer[ID_BOOM_H] = i;
	pSimStat->n_layer[ID_BOOM_H] = (ln-pPLC->Ldr[ID_BOOM_H][i-1])/ pPLC->Cdr[ID_BOOM_H][i];
	pSimStat->l_drum[ID_BOOM_H] = ln;

	//��]��
	pSimStat->nd[ID_BOOM_H].p = pspec->prm_nw[NW_ITEM_GROOVE][ID_BOOM_H] * ((double)i - 1.0) + pSimStat->n_layer[ID_BOOM_H];
	pSimStat->nd[ID_BHMH].p = pspec->Nbh_drum- pSimStat->nd[ID_BOOM_H].p;

	//�N���努�h�����w��
	pSimStat->i_layer[ID_BHMH] = (UINT32)(pSimStat->nd[ID_BHMH].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_BHMH]);									//���ݑw��-1
	pSimStat->n_layer[ID_BHMH] = pSimStat->nd[ID_BHMH].p - (double)pSimStat->i_layer[ID_BHMH] * pspec->prm_nw[NW_ITEM_GROOVE][ID_BHMH];		// ��]��-�i���ݑw��-1�j*�a��
	pSimStat->i_layer[ID_BHMH]++;//���ݑw��
	
	pSimStat->l_drum[ID_BOOM_H] = pPLC->Ldr[ID_BHMH][pSimStat->i_layer[ID_BHMH] - 1] + pPLC->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]] * pSimStat->n_layer[ID_BHMH];

	return;
}
//���񔼌a�Ǝ努�g������努�h������]�����Z�b�g����
void CJC::set_nmh_from_d_mh(double d, double mh) {
	//���[�v��
	double lrope = pspec->Hp + pspec->Lm * sin(pSimStat->th.p - pspec->Alpa_m) - mh;
	
	//�N���h����������
	double lbhm;
	if (pSimStat->i_layer[ID_BHMH] > 0) 
		lbhm = pPLC->Ldr[ID_BHMH][pSimStat->i_layer[ID_BHMH] - 1] + pPLC->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH] ] * pSimStat->n_layer[ID_BHMH];
	else 
		lbhm = pPLC->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]] * pSimStat->n_layer[ID_BHMH];
	
	//�努�h����������
	double ln = pPLC->Cdr[ID_HOIST][0] - pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_HOIST] * pSimStat->d.p - lbhm - lrope* pspec->prm_nw[NW_ITEM_WIND][ID_HOIST];

	pSimStat->l_drum[ID_HOIST] = ln;

	int i;
	for (i = 1; i < PLC_DRUM_LAYER_MAX - 1; i++) {
		if (ln < pPLC->Ldr[ID_HOIST][i]) break;
	}
	pSimStat->i_layer[ID_HOIST] = i;
	pSimStat->n_layer[ID_HOIST] = (ln - pPLC->Ldr[ID_HOIST][i - 1])/pPLC->Cdr[ID_HOIST][i];

	pSimStat->nd[ID_HOIST].p = pspec->prm_nw[NW_ITEM_GROOVE][ID_HOIST] * ((double)i - 1.0) + pSimStat->n_layer[ID_HOIST];
	return;

}
//d�ƕ⊪�g������⊪�h������]�����Z�b�g����
void CJC::set_nah_from_d_ah(double d, double ah) { 
	
	//���[�v��
	double lrope = pspec->Hp + pspec->La * sin(pSimStat->th.p - pspec->Alpa_a) - ah;

	//�⊪�h����������
	double ln = pPLC->Cdr[ID_AHOIST][0] - pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_AHOIST] * pSimStat->d.p  - lrope * pspec->prm_nw[NW_ITEM_WIND][ID_AHOIST];

	pSimStat->l_drum[ID_AHOIST] = ln;

	int i;
	for (i = 1; i < PLC_DRUM_LAYER_MAX - 1; i++) {
		if (ln < pPLC->Ldr[ID_AHOIST][i]) break;
	}
	pSimStat->i_layer[ID_AHOIST] = i;
	pSimStat->n_layer[ID_AHOIST] = (ln - pPLC->Ldr[ID_AHOIST][i - 1]) / pPLC->Cdr[ID_AHOIST][i];

	pSimStat->nd[ID_AHOIST].p = pspec->prm_nw[NW_ITEM_GROOVE][ID_AHOIST] * ((double)i - 1.0) + pSimStat->n_layer[ID_AHOIST];

	return; 
} 
//����ʒu(rad)�������s�j�I����]�����Z�b�g����
void CJC::set_nsl_from_slr(double sl_rad) { 
	
	pSimStat->nd[ID_SLEW].p = sl_rad / slw_rad_per_turn;
	pSimStat->i_layer[ID_SLEW] = 1;
	pSimStat->n_layer[ID_SLEW] = pSimStat->nd[ID_SLEW].p;

	pSimStat->l_drum[ID_SLEW] = pPLC->Cdr[ID_SLEW][0] * pSimStat->n_layer[ID_SLEW];
	return; 
}
//���s�ʒu���瑖�s�ԗ։�]�����Z�b�g����
void CJC::set_ngt_from_gtm(double gt_m) {
	pSimStat->nd[ID_GANTRY].p = gt_m / gnt_m_per_turn;
	pSimStat->i_layer[ID_GANTRY] = 1;
	pSimStat->n_layer[ID_GANTRY] = pSimStat->nd[ID_GANTRY].p;

	pSimStat->l_drum[ID_GANTRY] = pPLC->Cdr[ID_GANTRY][0] * pSimStat->n_layer[ID_GANTRY];
	return;
}
//�����h������]��Ԃ���d,th,d",th",d"",th""���Z�b�g����
void CJC::set_d_th_from_nbh() {

	//�ʒu
	pSimStat->i_layer[ID_BOOM_H] = (UINT32)(pSimStat->nd[ID_BOOM_H].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_BOOM_H]) + 1;
	pSimStat->n_layer[ID_BOOM_H] = pSimStat->nd[ID_BOOM_H].p - pPLC->Ldr[pSimStat->i_layer[ID_BOOM_H]-1][ID_BOOM_H];
	
	double c_layer	= pPLC->Cdr[pSimStat->i_layer[ID_BOOM_H]][ID_BOOM_H];//���ݑw�~��
	double nw_bhb	= pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_BOOM_H];//���ݑw�~��
	
	pSimStat->l_drum[ID_BOOM_H] = pPLC->Ldr[pSimStat->i_layer[ID_BOOM_H] - 1][ID_BOOM_H] + pSimStat->n_layer[ID_BOOM_H] * c_layer;

	pSimStat->d.p = (pPLC->Cdr[0][ID_BOOM_H]- pSimStat->l_drum[ID_BOOM_H])/ nw_bhb;	

	c_ph = (pspec->Lb * pspec->Lb + pspec->Lp * pspec->Lp - pSimStat->d.p * pSimStat->d.p) / (2.0 * pspec->Lb * pspec->Lp);
	pSimStat->ph.p = acos(c_ph);
	s_ph = sin(pSimStat->ph.p);
	
	pSimStat->th.p = pspec->Php- pSimStat->ph.p;
		
	double LLsinPh = pspec->Lb * pspec->Lp * s_ph;
	//���x
	pSimStat->d.v = -pSimStat->nd[ID_BOOM_H].v * c_layer / nw_bhb;							//��]�͈����������{�i�{��]��d�k�j
	pSimStat->th.v = -pSimStat->d.p * pSimStat->d.v / LLsinPh;

	//�����x
	pSimStat->d.a = -pSimStat->nd[ID_BOOM_H].a * c_layer / nw_bhb;							//��]�͈����������{�i�{��]��d�k�j
	pSimStat->th.a = -(pSimStat->th.v* pSimStat->th.v + pSimStat->d.p * pSimStat->d.a )/ LLsinPh + pSimStat->th.v * pSimStat->th.v * c_ph /s_ph;

	return; 
} 
//�����h������]��Ԃ��Z�b�g����
void  CJC::set_bh() { 
	UINT32 i;
	i = (UINT32)(pSimStat->nd[ID_BOOM_H].p  / pspec->prm_nw[NW_ITEM_GROOVE][ID_BOOM_H]); //���ݑw��-1
	pSimStat->i_layer[ID_BOOM_H] = i + 1;
	pSimStat->n_layer[ID_BOOM_H] = (pPLC->Cdr[ID_BOOM_H][0] - pPLC->Ldr[ID_BOOM_H][i]) / pPLC->Cdr[ID_BOOM_H][pSimStat->i_layer[ID_BOOM_H]];
	pSimStat->l_drum[ID_BOOM_H] = pPLC->Ldr[ID_BOOM_H][i] + pSimStat->n_layer[ID_BOOM_H] * pPLC->Cdr[ID_BOOM_H][pSimStat->i_layer[ID_BOOM_H]];

	return; 
}
//�努�h������]��Ԃƃ��[�v�����Z�b�g����
void  CJC::set_mh(){
	//�����努�h������
	pSimStat->nd[ID_BHMH].p = pspec->Nbh_drum - pSimStat->nd[ID_BOOM_H].p;
	int i = (UINT32)(pSimStat->nd[ID_BHMH].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_BHMH]); //���ݑw��-1
	pSimStat->i_layer[ID_BHMH] = i + 1;
	pSimStat->n_layer[ID_BHMH] = (pPLC->Cdr[ID_BHMH][0] - pPLC->Ldr[ID_BHMH][i]) / pPLC->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]];
	pSimStat->l_drum[ID_BHMH] = pPLC->Ldr[ID_BHMH][i] + pSimStat->n_layer[ID_BHMH] * pPLC->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]];

	//�努�h������

	i = (UINT32)(pSimStat->nd[ID_HOIST].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_HOIST]); //���ݑw��-1
	pSimStat->i_layer[ID_HOIST] = i + 1;
	pSimStat->n_layer[ID_HOIST] = (pSimStat->nd[ID_HOIST].p - pPLC->Ldr[ID_HOIST][i]) / pPLC->Cdr[ID_HOIST][pSimStat->i_layer[ID_HOIST]];
	pSimStat->l_drum[ID_HOIST] = pPLC->Ldr[ID_HOIST][i] + pSimStat->n_layer[ID_HOIST] * pPLC->Cdr[ID_HOIST][pSimStat->i_layer[ID_HOIST]];

	pSimStat->lrm.p = (
						pPLC->Cdr[0][ID_HOIST] 																//�S���[�v
						- pSimStat->d.p * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_HOIST]						//d�����[�v
						- pSimStat->l_drum[ID_BHMH] - pSimStat->l_drum[ID_HOIST]							//�h���������[�v
						)/ pspec->prm_nw[NW_ITEM_WIND][ID_HOIST];											//���C���|����

	pSimStat->lrm.v = (
						-pPLC->Cdr[ID_HOIST][pSimStat->i_layer[ID_HOIST]] * pSimStat->nd[ID_HOIST].v		//�努�h������]��
						-pSimStat->d.v * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_BHMH]							//d�ω���
						+ pPLC->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]] * pSimStat->nd[ID_BOOM_H].v		//�N���h������]��
						)/ pspec->prm_nw[NW_ITEM_WIND][ID_HOIST];

	pSimStat->lrm.a = (
						-pPLC->Cdr[ID_HOIST][pSimStat->i_layer[ID_HOIST]] * pSimStat->nd[ID_HOIST].a		//�努�h������]��
						- pSimStat->d.a * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_BHMH]							//d�ω���
						+ pPLC->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]] * pSimStat->nd[ID_BOOM_H].a		//�N���h������]��
						) / pspec->prm_nw[NW_ITEM_WIND][ID_HOIST];
	return; 
} 
//�⊪�h������]��Ԃƃ��[�v�����Z�b�g����
void  CJC::set_ah(){ 
	//�⊪�h������
	int i= (UINT32)(pSimStat->nd[ID_AHOIST].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_AHOIST]); //���ݑw��-1
	pSimStat->i_layer[ID_AHOIST] = i + 1;
	pSimStat->n_layer[ID_AHOIST] = (pSimStat->nd[ID_AHOIST].p - pPLC->Ldr[ID_AHOIST][i]) / pPLC->Cdr[ID_AHOIST][pSimStat->i_layer[ID_AHOIST]];
	pSimStat->l_drum[ID_AHOIST] = pPLC->Ldr[ID_AHOIST][i] + pSimStat->n_layer[ID_AHOIST] * pPLC->Cdr[ID_AHOIST][pSimStat->i_layer[ID_AHOIST]];//�h����������

	pSimStat->lra.p = (
		pPLC->Cdr[0][ID_AHOIST] 															//�S���[�v
		- pSimStat->d.p * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_AHOIST]						//d�����[�v
		- pSimStat->l_drum[ID_AHOIST]														//�h���������[�v
		) / pspec->prm_nw[NW_ITEM_WIND][ID_AHOIST];											//���C���|����

	pSimStat->lrm.v = (
		-pPLC->Cdr[ID_AHOIST][pSimStat->i_layer[ID_AHOIST]] * pSimStat->nd[ID_AHOIST].v		//�努�h������]��
		- pSimStat->d.v * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_AHOIST]						//d�ω���
		) / pspec->prm_nw[NW_ITEM_WIND][ID_HOIST];

	pSimStat->lrm.a = (
		-pPLC->Cdr[ID_HOIST][pSimStat->i_layer[ID_HOIST]] * pSimStat->nd[ID_HOIST].a		//�努�h������]��
		- pSimStat->d.a * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_BHMH]							//d�ω���
		) / pspec->prm_nw[NW_ITEM_WIND][ID_HOIST];
	return;
} 
//����h������]��Ԃ��Z�b�g����
void  CJC::set_sl(){
	//����s�j�I����
	pSimStat->l_drum[ID_SLEW]	= pSimStat->nd[ID_SLEW].p * pPLC->Cdr[ID_SLEW][1];										//����ړ���(m)
	pSimStat->i_layer[ID_SLEW]	= (UINT32)(pSimStat->l_drum[ID_SLEW]/ pPLC->Cdr[ID_SLEW][0]);							//�����]�������� = ��]�ړ���/TTB�~��
	pSimStat->n_layer[ID_SLEW]	= (pSimStat->l_drum[ID_SLEW] / pPLC->Cdr[ID_SLEW][0] - pSimStat->i_layer[ID_SLEW]) ;	//�����]�������_�ȉ�
	return; 
} 
//���s�h������]��Ԃ��Z�b�g����
void  CJC::set_gt(){
	pSimStat->l_drum[ID_GANTRY] = pSimStat->nd[ID_GANTRY].p * pPLC->Cdr[ID_GANTRY][1];										//�ړ���(m)
	pSimStat->i_layer[ID_GANTRY] = (UINT32)(pSimStat->nd[ID_GANTRY].p);														//���s�ԗ։�]����
	pSimStat->n_layer[ID_GANTRY] = (pSimStat->l_drum[ID_GANTRY] / pPLC->Cdr[ID_GANTRY][0] - pSimStat->i_layer[ID_GANTRY]);	//�����]�������_�ȉ�
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


