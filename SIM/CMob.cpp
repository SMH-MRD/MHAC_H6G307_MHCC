#include "CMob.h"
#include "SIMparaDEF.h"

extern ST_SPEC def_spec;

//計算短縮用変数
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

//加速度ベクトル　継承先で再定義する
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

//速度ベクトル
Vector3 CMob::V(Vector3& r, Vector3& v) {
	return v.clone();
}
//オイラー方による時間発展
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
	pspec = &def_spec;							//クレーン仕様

	//0速とみなす速度上限（ドラム回転速度）
	//速度表現は1.0＝100%で正規化する
	accdec_cut_spd_range[ID_HOIST]	= 0.005 * pspec->prm_drv[DRIVE_ITEM_RATE_NV][ID_HOIST];//0.5%
	accdec_cut_spd_range[ID_BOOM_H] = 0.005 * pspec->prm_drv[DRIVE_ITEM_RATE_NV][ID_BOOM_H];//0.5%
	accdec_cut_spd_range[ID_SLEW]	= 0.005 * pspec->prm_drv[DRIVE_ITEM_RATE_NV][ID_SLEW];//0.5%
	accdec_cut_spd_range[ID_GANTRY] = 0.005 * pspec->prm_drv[DRIVE_ITEM_RATE_NV][ID_GANTRY];//0.5%	
	accdec_cut_spd_range[ID_AHOIST] = 0.005 * pspec->prm_drv[DRIVE_ITEM_RATE_NV][ID_AHOIST];//0.5%

	mh_load = pspec->Load0_mh;//初期主巻荷重
	ah_load = pspec->Load0_ah;//初期補巻荷重

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
		trq_fb[i]=0.0;    //モータートルクFB
		motion_break[i] = false;
	}

	r0[ID_GANTRY] = pspec->gantry_pos_min;
	r0[ID_HOIST] = pspec->boom_high - 9.8;	//初期値　周期　2πとなる巻き位置
	r0[ID_BOOM_H] = pspec->boom_pos_min;
	r0[ID_SLEW] = 0.0;

	source_mode = MOB_MODE_SIM;
	
}
CJC::~CJC() {}

/// <summary>
/// ドラム速度指令取り込み
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
// ﾄﾙｸT(N・m）= F x R　= J x dω/dt  仕事率P=Tω=Mav　a=Tω/Mv=MT/r
//吊点加速度
Vector3 CJC::A(Vector3& _r, Vector3& _v) {					//吊点の加速度

	Vector3 a;
	double r_bm = r0[ID_BOOM_H];//旋回半径
	//等角加速度運動　a = r x dω/dt(-sinθ,cosθ) -rxω^2(cosθ,sinθ)
	a.x = v0[ID_BOOM_H] * sin(r0[ID_SLEW]) - r_bm * a0[ID_SLEW] * sin(r0[ID_SLEW]) - r_bm * v0[ID_SLEW] * v0[ID_SLEW] * cos(r0[ID_SLEW]) + a0[ID_GANTRY];
	a.y = v0[ID_BOOM_H] * cos(r0[ID_SLEW]) + r_bm * a0[ID_SLEW] * cos(r0[ID_SLEW]) - r_bm * v0[ID_SLEW] * v0[ID_SLEW] * sin(r0[ID_SLEW]);
	a.z = 0.0;


	//吊点加速度ベクトル（円柱座標）
	double a_er = a0[ID_BOOM_H] - r0[ID_BOOM_H] * v0[ID_SLEW] * v0[ID_SLEW];		//引込方向加速度　引込加速度＋旋回分(Rω2）
	double a_eth = r0[ID_BOOM_H] * a0[ID_SLEW] + 2.0 * v0[ID_BOOM_H] * v0[ID_SLEW];	//旋回方向加速度
	double a_z = 0.0;																//吊点の高さは一定

	//xyz座標吊点加速度ベクトル
	a.x = a0[ID_GANTRY] + a_er * cos(r0[ID_SLEW]) - a_eth * sin(r0[ID_SLEW]);
	a.y = a_er * sin(r0[ID_SLEW]) + a_eth * cos(r0[ID_SLEW]);
	a.z = a_z;

	return a;
}
//軸加速度
void CJC::Ac() {	//加速度計算

	//ドラム加速指令計算
	// #主巻
	{
		//ブレーキ閉
		if (!motion_break[ID_HOIST]) na_ref[ID_HOIST] = 0.0;
		//速度指令に未達(ドラム回転速度）
		else if ((nv_ref[ID_HOIST] - pSimStat->nd[ID_HOIST].v) > accdec_cut_spd_range[ID_HOIST]) {
			// 当面正転逆転で同じ加速度とする
			if (nv_ref[ID_HOIST] > 0.0) na_ref[ID_HOIST] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_HOIST];		//正転加速指令
			else						na_ref[ID_HOIST] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_HOIST];		//逆転減速指令
		}
		//速度指令に未達
		else if ((nv_ref[ID_HOIST] - pSimStat->nd[ID_HOIST].v) < -accdec_cut_spd_range[ID_HOIST]) {
			// 当面正転逆転で同じ減速度とする
			if (nv_ref[ID_HOIST] > 0.0) na_ref[ID_HOIST] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_HOIST]; 		//正転減速指令
			else						na_ref[ID_HOIST] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_HOIST];		//逆転加速指令
		}
		//速度指令に到達
		else {
			na_ref[ID_HOIST] = 0.0;
		}

		//極限停止
		if ((na_ref[ID_HOIST] > 0.0) && (is_fwd_endstop[ID_HOIST])) na_ref[ID_HOIST] = 0.0;
		if ((na_ref[ID_HOIST] < 0.0) && (is_rev_endstop[ID_HOIST])) na_ref[ID_HOIST] = 0.0;
	}

	// #補巻
	{
		//ブレーキ閉
		if (!motion_break[ID_AHOIST]) na_ref[ID_AHOIST] = 0.0;
		//速度指令に未達
		else if ((nv_ref[ID_AHOIST] - pSimStat->nd[ID_AHOIST].v) > accdec_cut_spd_range[ID_AHOIST]) {
			// 当面正転逆転で同じ加速度とする
			if (nv_ref[ID_AHOIST] > 0.0)na_ref[ID_AHOIST] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_AHOIST];		//正転加速指令
			else						na_ref[ID_AHOIST] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_AHOIST];		//逆転減速指令
		}
		//速度指令に未達
		else if ((nv_ref[ID_AHOIST] - pSimStat->nd[ID_AHOIST].v) < -accdec_cut_spd_range[ID_AHOIST]) {
			if (nv_ref[ID_AHOIST] > 0.0)na_ref[ID_AHOIST] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_AHOIST];		//正転減速指令
			else						na_ref[ID_AHOIST] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_AHOIST];		//逆転加速指令
		}
		//速度指令に到達
		else {
			na_ref[ID_AHOIST] = 0.0;
		}

		//極限停止
		if ((na_ref[ID_AHOIST] > 0.0) && (is_fwd_endstop[ID_AHOIST])) na_ref[ID_AHOIST] = 0.0;
		if ((na_ref[ID_AHOIST] < 0.0) && (is_rev_endstop[ID_AHOIST])) na_ref[ID_AHOIST] = 0.0;
	}

	// #走行
	{
		if (!motion_break[ID_GANTRY]) na_ref[ID_GANTRY] = 0.0;
		else if ((nv_ref[ID_GANTRY] - pSimStat->nd[ID_GANTRY].v) > accdec_cut_spd_range[ID_GANTRY]) {
			if (nv_ref[ID_GANTRY] > 0.0)na_ref[ID_GANTRY] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_GANTRY];	//正転加速
			else						na_ref[ID_GANTRY] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_GANTRY];	//逆転減速
		}
		else if ((nv_ref[ID_GANTRY] - pSimStat->nd[ID_GANTRY].v) < -accdec_cut_spd_range[ID_GANTRY]) {
			if (nv_ref[ID_GANTRY] > 0.0)na_ref[ID_GANTRY] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_GANTRY];	//正転減速
			else						na_ref[ID_GANTRY] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_GANTRY];	//逆転加速
		}
		else {
			na_ref[ID_GANTRY] = 0.0;
		}

		//極限停止
		if ((na_ref[ID_GANTRY] > 0.0) && (is_fwd_endstop[ID_GANTRY])) na_ref[ID_GANTRY] = 0.0;
		if ((na_ref[ID_GANTRY] < 0.0) && (is_rev_endstop[ID_GANTRY])) na_ref[ID_GANTRY] = 0.0;
	}

	// #引込
	{
		if (!motion_break[ID_BOOM_H]) na_ref[ID_BOOM_H] = 0.0;
		else if ((nv_ref[ID_BOOM_H] - pSimStat->nd[ID_BOOM_H].v) > accdec_cut_spd_range[ID_BOOM_H]) {
			if (nv_ref[ID_BOOM_H] > 0.0)na_ref[ID_BOOM_H] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_BOOM_H];	//正転加速
			else						na_ref[ID_BOOM_H] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_BOOM_H];	//逆転減速
		}
		else if ((nv_ref[ID_BOOM_H] - pSimStat->nd[ID_BOOM_H].v) < -accdec_cut_spd_range[ID_BOOM_H]) {
			if (nv_ref[ID_BOOM_H] > 0.0)na_ref[ID_BOOM_H] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_BOOM_H];	//正転減速
			else						na_ref[ID_BOOM_H] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_BOOM_H];	//逆転加速
		}
		else {
			na_ref[ID_BOOM_H] = 0.0;
		}

		//極限停止
		if ((na_ref[ID_BOOM_H] > 0.0) && (is_fwd_endstop[ID_BOOM_H])) na_ref[ID_BOOM_H] = 0.0;
		if ((na_ref[ID_BOOM_H] < 0.0) && (is_rev_endstop[ID_BOOM_H])) na_ref[ID_BOOM_H] = 0.0;
	}

	// #旋回
	{
		if (!motion_break[ID_SLEW]) na_ref[ID_SLEW] = 0.0;
		else if ((nv_ref[ID_SLEW] - pSimStat->nd[ID_SLEW].v) > accdec_cut_spd_range[ID_SLEW]) {
			if (nv_ref[ID_SLEW] > 0.0)	na_ref[ID_SLEW] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_SLEW];//正転加速
			else						na_ref[ID_SLEW] = pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_SLEW];//逆転減速
		}
		else if ((nv_ref[ID_SLEW] - pSimStat->nd[ID_SLEW].v) < -accdec_cut_spd_range[ID_SLEW]) {
			if (nv_ref[ID_SLEW] > 0.0)	na_ref[ID_SLEW] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_SLEW];//正転減速
			else						na_ref[ID_SLEW] = -pspec->prm_drv[DRIVE_ITEM_RATE_NA][ID_SLEW];//逆転加速
		}
		else {
			na_ref[ID_SLEW] = 0.0;
		}
	}

	//ドラム加速度計算　当面指令に対して一次遅れフィルタを入れる形で計算
	{
		//主巻
		if ((motion_break[ID_HOIST]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_HOIST].a = (dt * na_ref[ID_HOIST] + Tf[ID_HOIST] * pSimStat->nd[ID_HOIST].a )/ (dt + Tf[ID_HOIST]);
		}
		else {
			pSimStat->nd[ID_HOIST].a = 0.0;
		}
		//補巻
		if ((motion_break[ID_AHOIST]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_AHOIST].a = (dt * na_ref[ID_AHOIST] + Tf[ID_AHOIST] * pSimStat->nd[ID_AHOIST].a) / (dt + Tf[ID_AHOIST]);
		}
		else {
			pSimStat->nd[ID_AHOIST].a = 0.0;
		}
		//引込
		if ((motion_break[ID_BOOM_H]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_BOOM_H].a = (dt * na_ref[ID_BOOM_H] + Tf[ID_BOOM_H] * pSimStat->nd[ID_BOOM_H].a) / (dt + Tf[ID_BOOM_H]);
		}
		else {
			pSimStat->nd[ID_BOOM_H].a = 0.0;
		}
		//旋回
		if ((motion_break[ID_SLEW]) || (source_mode != MOB_MODE_SIM)) {
			pSimStat->nd[ID_SLEW].a = (dt * na_ref[ID_SLEW] + Tf[ID_SLEW] * pSimStat->nd[ID_SLEW].a) / (dt + Tf[ID_SLEW]);
		}
		else {
			pSimStat->nd[ID_SLEW].a = 0.0;
		}
		//走行
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
	//クレーン部
	//ドラム加速度計算
	Ac();

	//ドラム速度計算(オイラー法）
	pSimStat->nd[ID_HOIST].v	+= pSimStat->nd[ID_HOIST].a * dt;	if (!motion_break[ID_HOIST])	pSimStat->nd[ID_HOIST].v = 0.0;
	pSimStat->nd[ID_AHOIST].v	+= pSimStat->nd[ID_AHOIST].a * dt;	if (!motion_break[ID_AHOIST])	pSimStat->nd[ID_AHOIST].v = 0.0;
	pSimStat->nd[ID_BOOM_H].v	+= pSimStat->nd[ID_BOOM_H].a * dt;	if (!motion_break[ID_BOOM_H])	pSimStat->nd[ID_BOOM_H].v = 0.0;
	pSimStat->nd[ID_SLEW].v		+= pSimStat->nd[ID_SLEW].a * dt;	if (!motion_break[ID_SLEW])		pSimStat->nd[ID_SLEW].v = 0.0;
	pSimStat->nd[ID_GANTRY].v	+= pSimStat->nd[ID_GANTRY].a * dt;	if (!motion_break[ID_GANTRY])	pSimStat->nd[ID_GANTRY].v = 0.0;

	//ドラム位置計算(オイラー法）
	pSimStat->nd[ID_HOIST].p += pSimStat->nd[ID_HOIST].v * dt;
	pSimStat->nd[ID_AHOIST].p += pSimStat->nd[ID_AHOIST].v * dt;
	pSimStat->nd[ID_GANTRY].p += pSimStat->nd[ID_GANTRY].v * dt;
	pSimStat->nd[ID_BOOM_H].p += pSimStat->nd[ID_BOOM_H].v * dt;
	pSimStat->nd[ID_SLEW].p += pSimStat->nd[ID_SLEW].v * dt;

	//クレーン状態セット
	set_d_th_from_nbh();	//引込ドラム回転状態からd,d'd'' th th' th''の状態をセットする
	set_bh_layer();         //引込ドラム状態をセットする
	set_mh_layer();         //主巻ドラム状態、ロープ状態をセットする
	set_ah_layer();         //補巻ドラム状態、ロープ状態をセットする
	set_sl_layer();         //旋回ドラム状態をセットする
	set_gt_layer();         //走行ドラム状態をセットする

	//軸加速度計算
	double thm = pSimStat->th.p;
	double tha = pSimStat->th.p - pspec->Alpa_a;

	//ロープ長加速度計算
	pSimStat->lrm.a = (
		-pCraneStat->Cdr[ID_HOIST][pSimStat->i_layer[ID_HOIST]] * pSimStat->nd[ID_HOIST].a		//主巻ドラム回転分
		- pSimStat->db.a * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_HOIST]							//d変化分
		+ pCraneStat->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]] * pSimStat->nd[ID_BOOM_H].a		//起伏ドラム回転分
		) / pspec->prm_nw[NW_ITEM_WIND][ID_HOIST];

	pSimStat->lra.a = (
		-pCraneStat->Cdr[ID_AHOIST][pSimStat->i_layer[ID_AHOIST]] * pSimStat->nd[ID_AHOIST].a	//補巻ドラム回転分
		- pSimStat->d.a * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_AHOIST]							//d変化分
		) / pspec->prm_nw[NW_ITEM_WIND][ID_AHOIST];

	//h=Lm・sinθ　h'=Lm・θ'cosθ h''=Lm・θ''cosθ-θ'sinθ
	a0[ID_HOIST] = pspec->Lm * (pSimStat->th.a * cos(thm) - pSimStat->th.v * pSimStat->th.v * sin(thm)) + pSimStat->lrm.a;//吊点ｚ加速度＋ロープ長加速度
	a0[ID_AHOIST] = pspec->La * (pSimStat->th.a * cos(tha) - pSimStat->th.v * pSimStat->th.v * sin(tha)) + pSimStat->lra.a;//吊点ｚ加速度＋ロープ長加速度
	//r=Lm・cosθ　r'=-Lm・θ'sinθ r''=-Lm・(θ''sinθ-θ'cosθ)
	a0[ID_BOOM_H] = -pspec->Lm * (pSimStat->th.a * sin(thm) + pSimStat->th.v * cos(thm));

	a0[ID_SLEW] = pSimStat->nd[ID_SLEW].a * pspec->Kttb;						//ピニオン回転加速度×ピニオン径/TTB径
	a0[ID_GANTRY] = pSimStat->nd[ID_GANTRY].a * pCraneStat->Cdr[ID_GANTRY][1];	//ドラム回転加速度×車輪径


	pSimStat->lrm.v = (
		-pCraneStat->Cdr[ID_HOIST][pSimStat->i_layer[ID_HOIST]] * pSimStat->nd[ID_HOIST].v	//主巻ドラム回転分
		- pSimStat->db.v * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_HOIST]							//d変化分
		+ pCraneStat->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]] * pSimStat->nd[ID_BOOM_H].v	//起伏ドラム回転分
		) / pspec->prm_nw[NW_ITEM_WIND][ID_HOIST];
	pSimStat->lra.v = (
		-pCraneStat->Cdr[ID_AHOIST][pSimStat->i_layer[ID_AHOIST]] * pSimStat->nd[ID_AHOIST].v	//補巻ドラム回転分
		- pSimStat->d.v * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_AHOIST]							//d変化分
		) / pspec->prm_nw[NW_ITEM_WIND][ID_AHOIST];


	v0[ID_HOIST]	= pspec->Lm * pSimStat->th.v * cos(pSimStat->th.p) - pSimStat->lrm.v;
	v0[ID_AHOIST]	= pspec->La * pSimStat->th.v * cos(pSimStat->th.p - pspec->Alpa_a) - pSimStat->lra.v;
	v0[ID_SLEW]		= pSimStat->nd[ID_SLEW].v * PI360;
	v0[ID_GANTRY]	= pSimStat->nd[ID_GANTRY].v * pCraneStat->Cdr[ID_GANTRY][1];
	v0[ID_BOOM_H]	= -pspec->Lb * pSimStat->th.v * sin(pSimStat->th.p+pspec->Alpa_b);


	pSimStat->lrm.p = (
		pCraneStat->Cdr[0][ID_HOIST] 														//全ロープ
		- pSimStat->db.p * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_HOIST]						//d部ロープ
		- pSimStat->l_drum[ID_BHMH] - pSimStat->l_drum[ID_HOIST]							//ドラム部ロープ(主巻ドラム＋引込ドラム）
		) / pspec->prm_nw[NW_ITEM_WIND][ID_HOIST];											//ワイヤ掛け数
	pSimStat->lra.p = (
		pCraneStat->Cdr[0][ID_AHOIST] 															//全ロープ
		- pSimStat->d.p * pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_AHOIST]							//d部ロープ
		- pSimStat->l_drum[ID_AHOIST]															//ドラム部ロープ
		) / pspec->prm_nw[NW_ITEM_WIND][ID_AHOIST];												//ワイヤ掛け数


	r0[ID_HOIST]	= pspec->Hp + pspec->Lm * sin(pSimStat->th.p) - pSimStat->lrm.p;
	r0[ID_AHOIST]	= pspec->Hp + pspec->La * sin(pSimStat->th.p - pspec->Alpa_a) - pSimStat->lra.p;
	r0[ID_SLEW]		= pSimStat->n_layer[ID_SLEW] * PI360 - PI180; //旋回は±180°で表現
	r0[ID_GANTRY]	= pSimStat->l_drum[ID_GANTRY];
	r0[ID_BOOM_H]	= pspec->Lm * cos(pSimStat->th.p);

	vc.x = v0[ID_GANTRY]; vc.y = 0.0; vc.z = 0.0;		//クレーン中心位置
	rc.x = r0[ID_GANTRY]; rc.y = R0.y; rc.z = R0.z;		//クレーン中心位置
														
	//吊点部

	r.x = r0[ID_BOOM_H] * cos(r0[ID_SLEW]) + r0[ID_GANTRY];
	r.y = r0[ID_BOOM_H] * sin(r0[ID_SLEW]);
	r.z = pspec->Hp + r0[ID_BOOM_H] * sin(pSimStat->th.p);

	v.x = v0[ID_BOOM_H] * cos(r0[ID_SLEW]) - r0[ID_BOOM_H] * v0[ID_SLEW] * sin(r0[ID_SLEW]) + v0[ID_GANTRY];
	v.y = v0[ID_BOOM_H] * sin(r0[ID_SLEW]) + r0[ID_BOOM_H] * v0[ID_SLEW] * cos(r0[ID_SLEW]);
	v.z = pspec->Hp + r0[ID_BOOM_H] * sin(pSimStat->th.p);

	a.x = v0[ID_BOOM_H] * cos(r0[ID_SLEW]) - r0[ID_BOOM_H] * v0[ID_SLEW] * sin(r0[ID_SLEW]) + v0[ID_GANTRY];
	a.y = v0[ID_BOOM_H] * sin(r0[ID_SLEW]) + r0[ID_BOOM_H] * v0[ID_SLEW] * cos(r0[ID_SLEW]);
	a.z = pspec->Hp + r0[ID_BOOM_H] * sin(pSimStat->th.p);

	//ロープ長セット　LOADオブジェクトから参照
	l_mh = pSimStat->lrm.p;
	l_ah = pSimStat->lra.p;

	return;
}
void CJC::init_crane(double _dt) {
	
	//計算パラメータ
	Lmb2 = pspec->Lmb * pspec->Lmb;
	LmbCosAdb = pspec->Lmb * cos(pspec->Alpa_db);
	LmLb = pspec->Lm * pspec->Lb;
	
	//r0は、各軸アブソコーダの値
	r0[ID_HOIST]	= SIM_INIT_MH;
	r0[ID_AHOIST]	= SIM_INIT_AH;
	r0[ID_GANTRY]	= SIM_INIT_X;
	r0[ID_SLEW]		= SIM_INIT_TH;
	r0[ID_BOOM_H]	= SIM_INIT_R;		//旋回半径は主巻吊点位置の半径とする

	//クレーン基準点の初期位置,速度
	rc.x = 10.0; rc.y = 0.0; rc.z = 0.0;
	vc.x = 0.0; vc.y = 0.0; vc.z = 0.0;

	//ドラム回転位置セット
	set_nbh_d_ph_th_from_r(r0[ID_BOOM_H]);												//旋回半径より引込ドラム回転量他をセット

	set_nmh_from_mh(r0[ID_HOIST]); 														//主巻揚程より主巻ドラム回転量他をセット

	set_nah_from_ah(r0[ID_AHOIST]); 													//補巻揚程より補巻ドラム回転量他をセット

	slw_rad_per_turn = PI360 * pspec->prm_nw[DRUM_ITEM_DIR][ID_SLEW] / pspec->Dttb;		//ピニオン（モータ）１回転での旋回角度変化量セット
	set_nsl_from_slr(r0[ID_SLEW]); 														//旋回位置からピニオン回転数をセット

	gnt_m_per_turn = PI180 * pspec->prm_nw[DRUM_ITEM_DIR][ID_GANTRY];					//車輪1回転での移動量セット
	set_ngt_from_gtm(r0[ID_GANTRY]);													//走行位置から車輪回転数セット

																						//吊点状態セット
	Vector3 _r(r0[ID_BOOM_H] * cos(r0[ID_SLEW]) + r0[ID_GANTRY], r0[ID_BOOM_H] * sin(r0[ID_SLEW]), pspec->Lp + pspec->Lm * sin(pSimStat->th.p));
	Vector3 _v(0.0, 0.0, 0.0);
	init_mob(_dt, _r, _v);

	set_v_ref(0.0, 0.0, 0.0, 0.0, 0.0);	//初期速度指令値セット
	set_fex(0.0, 0.0, 0.0);			//初期外力セット
		
	//加速度一次遅れフィルタ時定数
	Tf[ID_HOIST]	= SIM_TF_HOIST;
	Tf[ID_BOOM_H]	= SIM_TF_BOOM_H;
	Tf[ID_SLEW]		= SIM_TF_SLEW;
	Tf[ID_GANTRY]	= SIM_TF_GANTRY;
	Tf[ID_AHOIST]	= SIM_TF_AHOIST;

	//計算用定数セット
	cal_d1 = pspec->Lm * pspec->Lm + pspec->Lp * pspec->Lp;
	cal_d2 = 2.0 * pspec->Lm * pspec->Lp;

}
// 各モーションのブレーキ状態をセット
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
	pSimStat->th.p	= acos(r/pspec->Lm);					//半径は主巻吊点位置
	pSimStat->ph.p	= pspec->Php - pSimStat->th.p;
	pSimStat->d.p	= sqrt(pspec->Lm * pspec->Lm + pspec->Lp * pspec->Lp - 2.0 * pspec->Lm * pspec->Lp * cos(pSimStat->ph.p));
	pSimStat->db.p	= sqrt(pspec->Lb * pspec->Lb + pspec->Lp * pspec->Lp - 2.0 * pspec->Lb * pspec->Lp * cos(pSimStat->ph.p - pspec->Alpa_b ));
	pSimStat->th.v = pSimStat->ph.v = 0.0;
	pSimStat->th.a = pSimStat->ph.a = 0.0;


	//起伏ドラム巻取り量＝基準起伏ロープ長（ジブ部ロープ＋ドラム巻取り量の和）‐ジブ部分ロープ長さ
	pSimStat->l_drum[ID_BOOM_H] = pCraneStat->Cdr[ID_BOOM_H][0] - pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_BOOM_H] * pSimStat->d.p;
	int i;
	//引込ドラムの層数
	for (i = 1; i < PLC_DRUM_LAYER_MAX-1; i++) {
		if (pSimStat->l_drum[ID_BOOM_H] < pCraneStat->Ldr[ID_BOOM_H][i]) break;
	}
	//起伏起伏ドラム層数
	pSimStat->i_layer[ID_BOOM_H] = i;																										//現在の層
	pSimStat->n_layer[ID_BOOM_H] = (pSimStat->l_drum[ID_BOOM_H] - pCraneStat->Ldr[ID_BOOM_H][i-1])/ pCraneStat->Cdr[ID_BOOM_H][i];			//現在の層の巻数
																																			//ドラム巻取り量
	//ドラム回転数
	pSimStat->nd[ID_BOOM_H].p = pspec->prm_nw[NW_ITEM_GROOVE][ID_BOOM_H] * ((double)i - 1.0) + pSimStat->n_layer[ID_BOOM_H];				//引込ドラム部（溝数×（現在層数-1）＋現在層の巻き数
	pSimStat->nd[ID_BHMH].p = pspec->Nbh_drum- pSimStat->nd[ID_BOOM_H].p;																	//主巻ドラム部（主巻引込巻き数の和‐引込部巻き数）

	//起伏主巻ドラム層数
	pSimStat->i_layer[ID_BHMH] = (UINT32)(pSimStat->nd[ID_BHMH].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_BHMH]);								//現在層数-1（計算途中）
	pSimStat->n_layer[ID_BHMH] = pSimStat->nd[ID_BHMH].p - (double)pSimStat->i_layer[ID_BHMH] * pspec->prm_nw[NW_ITEM_GROOVE][ID_BHMH];		//層巻き数＝回転数-（現在層数-1）*溝数
	pSimStat->i_layer[ID_BHMH]++;																											//現在層数
	
	//起伏主巻ドラム巻取り量
	pSimStat->l_drum[ID_BHMH] = pCraneStat->Ldr[ID_BHMH][pSimStat->i_layer[ID_BHMH] - 1] + pCraneStat->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]] * pSimStat->n_layer[ID_BHMH];
	return;
}
//旋回半径と主巻揚程から主巻ドラム回転数をセットする
void CJC::set_nmh_from_mh(double mh) {
	//ロープ長
	double lrope = pspec->Hp + pspec->Lm * sin(pSimStat->th.p) - mh;//主巻吊ロープ長＝ジブ軸高さ＋ジブ起伏高さ-揚程
	
	//主巻ドラム巻取り量 基準ロープ長（主巻、起伏ドラム巻取り量＋ジブ部＋ロープ長部の総和）-　ジブ部　- 起伏ドラム巻取 - ロープ長部
	pSimStat->l_drum[ID_HOIST] = pCraneStat->Cdr[ID_HOIST][0] - pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_HOIST] * pSimStat->d.p - pSimStat->l_drum[ID_BHMH] - lrope* pspec->prm_nw[NW_ITEM_WIND][ID_HOIST];

	int i;
	for (i = 1; i < PLC_DRUM_LAYER_MAX - 1; i++) {
		if (pSimStat->l_drum[ID_HOIST] < pCraneStat->Ldr[ID_HOIST][i]) break;
	}
	pSimStat->i_layer[ID_HOIST] = i;
	pSimStat->n_layer[ID_HOIST] = (pSimStat->l_drum[ID_HOIST] - pCraneStat->Ldr[ID_HOIST][i - 1])/ pCraneStat->Cdr[ID_HOIST][i];

	//主巻ドラム回転数
	pSimStat->nd[ID_HOIST].p = pspec->prm_nw[NW_ITEM_GROOVE][ID_HOIST] * ((double)i - 1.0) + pSimStat->n_layer[ID_HOIST];
	return;

}
//dと補巻揚程から補巻ドラム回転数をセットする
void CJC::set_nah_from_ah(double ah) { 
	
	//ロープ長
	double lrope = pspec->Hp + pspec->La * sin(pSimStat->th.p + pspec->Alpa_a) - ah;//補巻吊ロープ長＝ジブ軸高さ＋ジブ起伏高さ-揚程

	//補巻ドラム巻取り量 基準ロープ長（補巻ドラム巻取り量＋ジブ部＋ロープ長部の総和）-　ジブ部　- 起伏ドラム巻取 - ロープ長部
	pSimStat->l_drum[ID_AHOIST] = pCraneStat->Cdr[ID_AHOIST][0] - pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_AHOIST] * pSimStat->d.p  - lrope * pspec->prm_nw[NW_ITEM_WIND][ID_AHOIST];

	int i;
	for (i = 1; i < PLC_DRUM_LAYER_MAX - 1; i++) {
		if (pSimStat->l_drum[ID_AHOIST] < pCraneStat->Ldr[ID_AHOIST][i]) break;
	}
	pSimStat->i_layer[ID_AHOIST] = i;
	pSimStat->n_layer[ID_AHOIST] = (pSimStat->l_drum[ID_AHOIST] - pCraneStat->Ldr[ID_AHOIST][i - 1]) / pCraneStat->Cdr[ID_AHOIST][i];

	//補巻ドラム回転数
	pSimStat->nd[ID_AHOIST].p = pspec->prm_nw[NW_ITEM_GROOVE][ID_AHOIST] * ((double)i - 1.0) + pSimStat->n_layer[ID_AHOIST];

	return; 
} 
//旋回位置(rad)から旋回ピニオン回転数をセットする
void CJC::set_nsl_from_slr(double sl_rad) { 
	
	pSimStat->nd[ID_SLEW].p = sl_rad / pCraneStat->Cdr[ID_SLEW][0];						//Cdr[0]ピニオン円周
	pSimStat->i_layer[ID_SLEW] = 1;
	pSimStat->n_layer[ID_SLEW] = pSimStat->nd[ID_SLEW].p;

	pSimStat->l_drum[ID_SLEW] = pCraneStat->Cdr[ID_SLEW][0] * pSimStat->n_layer[ID_SLEW];//Cdr[0]ピニオン円周
	return; 
}
//走行位置から走行車輪回転数をセットする
void CJC::set_ngt_from_gtm(double gt_m) {
	pSimStat->nd[ID_GANTRY].p = gt_m / pCraneStat->Cdr[ID_GANTRY][0];											//Cdr[0]車輪円周
	pSimStat->i_layer[ID_GANTRY] = 1;
	pSimStat->n_layer[ID_GANTRY] = pSimStat->nd[ID_GANTRY].p;

	pSimStat->l_drum[ID_GANTRY] = pCraneStat->Cdr[ID_GANTRY][0] * pSimStat->n_layer[ID_GANTRY];//Cdr[0]車輪円周
	return;
}
//引込ドラム回転状態からd,th,d",th",d"",th""をセットする
void CJC::set_d_th_from_nbh() {

	//引込ドラム層状態
	pSimStat->i_layer[ID_BOOM_H] = (UINT32)(pSimStat->nd[ID_BOOM_H].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_BOOM_H]) + 1;
	pSimStat->n_layer[ID_BOOM_H] = pSimStat->nd[ID_BOOM_H].p - pspec->prm_nw[NW_ITEM_GROOVE][ID_BOOM_H]*(pSimStat->i_layer[ID_BOOM_H]-1);
	//起伏ドラム巻取り量	
	pSimStat->l_drum[ID_BOOM_H] = pCraneStat->Ldr[pSimStat->i_layer[ID_BOOM_H] - 1][ID_BOOM_H] + pSimStat->n_layer[ID_BOOM_H] * pCraneStat->Cdr[pSimStat->i_layer[ID_BOOM_H]][ID_BOOM_H];
	//ジブ部距離　（基準ロープ長-ドラム巻取り量）ロープ掛数
	pSimStat->db.p = (pCraneStat->Cdr[0][ID_BOOM_H]- pSimStat->l_drum[ID_BOOM_H])/ pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_BOOM_H];
	pSimStat->d.p = sqrt(pSimStat->db.p* pSimStat->db.p + Lmb2-2.0 * pSimStat->db.p * LmbCosAdb);

	c_ph = (cal_d1 - pSimStat->d.p * pSimStat->d.p) / cal_d2;
	pSimStat->ph.p = acos(c_ph);					//φ
	s_ph = sin(pSimStat->ph.p);
	pSimStat->th.p = pspec->Php - pSimStat->ph.p;	//θ
			
	double LLsinPh = pspec->Lb * pspec->Lp * s_ph;
	double c1 = pCraneStat->Cdr[pSimStat->i_layer[ID_BOOM_H]][ID_BOOM_H] / pspec->prm_nw[NW_ITEM_WIND_BOOM][ID_BOOM_H];//ドラム周長/ワイヤ掛数
	
	pSimStat->db.v = -pSimStat->nd[ID_BOOM_H].v * c1;//ドラム周長/ワイヤ掛数;
	pSimStat->d.v = (pSimStat->db.p - LmbCosAdb) / LmLb * pSimStat->db.v;

	pSimStat->ph.v = pSimStat->d.p * pSimStat->d.v / LLsinPh;		//dφ/dt = dd'/LpLbsinφ
	pSimStat->th.v = -pSimStat->ph.v;								//-dφ/dt

	//加速度
	pSimStat->db.a = -pSimStat->nd[ID_BOOM_H].a * c1;														//回転は引込方向が＋（＋回転→d縮）
	pSimStat->d.a = pSimStat->db.v* pSimStat->db.v/LmLb + pSimStat->d.v* pSimStat->db.a / pSimStat->db.v;	//回転は引込方向が＋（＋回転→d縮）

	pSimStat->ph.a = pSimStat->ph.v * (pSimStat->d.v / pSimStat->d.p + pSimStat->d.a/ pSimStat->d.v  - pSimStat->ph.v * c_ph / s_ph);
	pSimStat->th.a = -pSimStat->ph.a;

	return; 
} 

//引込ドラム状態をセットする
void  CJC::set_bh_layer() { 
	UINT32 i = (UINT32)(pSimStat->nd[ID_BOOM_H].p  / pspec->prm_nw[NW_ITEM_GROOVE][ID_BOOM_H]); //現在層数-1
	pSimStat->i_layer[ID_BOOM_H] = i + 1;
	pSimStat->n_layer[ID_BOOM_H] = (pCraneStat->Cdr[ID_BOOM_H][0] - pCraneStat->Ldr[ID_BOOM_H][i]) / pCraneStat->Cdr[ID_BOOM_H][pSimStat->i_layer[ID_BOOM_H]];
	pSimStat->l_drum[ID_BOOM_H] = pCraneStat->Ldr[ID_BOOM_H][i] + pSimStat->n_layer[ID_BOOM_H] * pCraneStat->Cdr[ID_BOOM_H][pSimStat->i_layer[ID_BOOM_H]];
	return; 
}
//主巻ドラム回転状態をセットする
void  CJC::set_mh_layer(){
	//引込主巻ドラム部
	pSimStat->nd[ID_BHMH].p = pspec->Nbh_drum - pSimStat->nd[ID_BOOM_H].p;
	int i = (UINT32)(pSimStat->nd[ID_BHMH].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_BHMH]); //現在層数-1
	pSimStat->i_layer[ID_BHMH] = i + 1;
	pSimStat->n_layer[ID_BHMH] = (pCraneStat->Cdr[ID_BHMH][0] - pCraneStat->Ldr[ID_BHMH][i]) / pCraneStat->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]];
	pSimStat->l_drum[ID_BHMH] = pCraneStat->Ldr[ID_BHMH][i] + pSimStat->n_layer[ID_BHMH] * pCraneStat->Cdr[ID_BHMH][pSimStat->i_layer[ID_BHMH]];

	//主巻ドラム部
	i = (UINT32)(pSimStat->nd[ID_HOIST].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_HOIST]); //現在層数-1
	pSimStat->i_layer[ID_HOIST] = i + 1;
	pSimStat->n_layer[ID_HOIST] = (pSimStat->nd[ID_HOIST].p - pCraneStat->Ldr[ID_HOIST][i]) / pCraneStat->Cdr[ID_HOIST][pSimStat->i_layer[ID_HOIST]];
	pSimStat->l_drum[ID_HOIST] = pCraneStat->Ldr[ID_HOIST][i] + pSimStat->n_layer[ID_HOIST] * pCraneStat->Cdr[ID_HOIST][pSimStat->i_layer[ID_HOIST]];

	return; 
} 
//補巻ドラム回転状態とロープ長をセットする
void  CJC::set_ah_layer(){
	//補巻ドラム部
	int i= (UINT32)(pSimStat->nd[ID_AHOIST].p / pspec->prm_nw[NW_ITEM_GROOVE][ID_AHOIST]); //現在層数-1
	pSimStat->i_layer[ID_AHOIST] = i + 1;
	pSimStat->n_layer[ID_AHOIST] = (pSimStat->nd[ID_AHOIST].p - pCraneStat->Ldr[ID_AHOIST][i]) / pCraneStat->Cdr[ID_AHOIST][pSimStat->i_layer[ID_AHOIST]];
	pSimStat->l_drum[ID_AHOIST] = pCraneStat->Ldr[ID_AHOIST][i] + pSimStat->n_layer[ID_AHOIST] * pCraneStat->Cdr[ID_AHOIST][pSimStat->i_layer[ID_AHOIST]];//ドラム巻取り量

	return;
} 
//旋回ドラム回転状態をセットする
void  CJC::set_sl_layer(){
	//旋回ピニオン部
	pSimStat->l_drum[ID_SLEW]	= pSimStat->nd[ID_SLEW].p * pCraneStat->Cdr[ID_SLEW][1];										//旋回移動量(m)
	pSimStat->i_layer[ID_SLEW]	= 1;// (UINT32)(pSimStat->l_drum[ID_SLEW] / pCraneStat->Cdr[ID_SLEW][0]);						//旋回回転数整数部 = 回転移動量/TTB円周
	pSimStat->n_layer[ID_SLEW]	= 0;// (pSimStat->l_drum[ID_SLEW] / pCraneStat->Cdr[ID_SLEW][0] - pSimStat->i_layer[ID_SLEW]);	//旋回回転数小数点以下
	return; 
} 
//走行ドラム回転状態をセットする
void  CJC::set_gt_layer(){
	pSimStat->l_drum[ID_GANTRY] = pSimStat->nd[ID_GANTRY].p * pCraneStat->Cdr[ID_GANTRY][1];										//移動量(m)
	pSimStat->i_layer[ID_GANTRY] = 1;// (UINT32)(pSimStat->nd[ID_GANTRY].p);																//走行車輪回転数部
	pSimStat->n_layer[ID_GANTRY] = 0;// (pSimStat->l_drum[ID_GANTRY] / pCraneStat->Cdr[ID_GANTRY][0] - pSimStat->i_layer[ID_GANTRY]);	//旋回回転数小数点以下
	return; 
} 

/********************************************************************************/
/*      Load Object(吊荷）                                                      */
/********************************************************************************/

//吊荷位置の初期化
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

	//計算誤差によるロープ長ずれ補正
	Vector3 hatL = L_.clone().normalize();
	// 補正ばね弾性力
	Vector3 ak = hatL.clone().multiplyScalor(-compensationK * (pCrane->l_mh - L_.length()));
	Vector3 v_ = v_.subVectors(v, pCrane->v);
	// 補正粘性抵抗力
	Vector3 agamma = hatL.clone().multiplyScalor(-compensationGamma * v_.dot(hatL));
	// 張力にひもの長さの補正力を加える
	a.add(ak).add(agamma);

	return a;
} //Model of acceleration

double  CLoad::S() { //Aの計算部の関係でS/Lとなっている。巻きの加速度分が追加されている。
	Vector3 v_ = v.clone().sub(pCrane->v);
	double v_abs2 = v_.lengthSq();
	Vector3 vectmp;
	Vector3 vecL = vectmp.subVectors(r, pCrane->r);

	return -m * (v_abs2 - pCrane->a.dot(vecL) - GA * vecL.z - (pCrane->a0[ID_HOIST] * pCrane->l_mh + pCrane->v0[ID_HOIST] * pCrane->v0[ID_HOIST])) / (pCrane->l_mh * pCrane->l_mh);

}

void CLoad::update_relative_vec() {//クレーン吊点との相対速度
	Vector3 vectmp;
	L = vectmp.subVectors(r,pCrane->r);
	vL = vectmp.subVectors(v,pCrane->v);
	return;
}


