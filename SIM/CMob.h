#pragma once

#include "CVector3.h"
#include "COMMON_DEF.H"
#include "Spec.h"
#include "CSharedMem.h"

//Moving Objectクラス
class CMob
{
public:
    CMob();
    CMob(double _dt);
    CMob(double _dt, Vector3& _r, Vector3& _v);
    ~CMob();

    double dt;      //計算時間間隔
    Vector3 a;      //加速度ベクトル
    Vector3 r;      //位置ベクトル
    Vector3 v;      //速度ベクトル
    Vector3 L;      //ロープベクトル
    Vector3 vL;     //ロープ速度ベクトル
    Vector3 fex;    //外力
    Vector3 dr;     //位置ベクトルの変化分
    Vector3 dv;     //速度ベクトルの変化分
    Vector3 R0;     //基準点
     
    //加速度ベクトルを与えるメソッド　　継承先で再定義する
    virtual Vector3 A(Vector3& r, Vector3& v); 
    virtual void set_fex(double,double,double);//外力
    virtual void set_dt(double);//計算時間間隔セット

    //速度ベクトルを与えるメソッド
    virtual Vector3 V(Vector3& r, Vector3& v);
    //時間発展を計算するメソッド
    virtual void timeEvolution();
    virtual void init_mob(double _dt, Vector3& _r, Vector3& _v) {
        dt = _dt;
        r.copy(_r);
        v.copy(_v);
        return;
    }
 
private:

};

//クレーンクラス
//r,vは、吊点の位置と座標

#define SIM_INIT_R                  21.00       //デフォルト旋回半径主巻
#define SIM_INIT_MHR                21.90       //デフォルト旋回半径主巻
#define SIM_INIT_AHR                26.06       //デフォルト旋回半径補巻
#define SIM_INIT_MH                 60.0        //デフォルト主巻高さ
#define SIM_INIT_AH                 59.2        //デフォルト補巻高さ
#define SIM_INIT_TH                 PI90        //旋回初期値 rad
#define SIM_INIT_L                  9.8         //ロープ長初期値 m
#define SIM_INIT_X                  10.0        //走行初期値 m
#define SIM_INIT_M                  10000.0     //荷重初期値 kg

#define MOB_MODE_SIM                0
#define MOB_MODE_PLC                1


class CJC : public CMob
{
public:
    CJC();
    ~CJC();
    
    double M;                                       //クレーン全体質量　Kg
    double mh_load;                                //主巻荷重
    double ah_load;                                //補巻荷重
    double l_mh;                                    //巻ロープ長 m
    double l_ah;                                    //補巻巻ロープ長 m

    double slw_rad_per_turn;                        //旋回ピニオン1回転の旋回角度
    double gnt_m_per_turn;                          //走行車輪1回転の移動量
    double c_ph, s_ph, c_phb, s_phb;                              //cosφ sinφ
    double cal_Lm2Lp2,cal_Lb2Lp2, cal_2LmLp, cal_2LbLp;                          //ｄ計算用中間変数
 
    Vector3 rc;                                     //クレーン中心点の位置ベクトル
    Vector3 vc;                                     //クレーン中心点の速度ベクトル

    int source_mode;

    double r0[MOTION_ID_MAX];                       //位置・角度(m)
    double v0[MOTION_ID_MAX];                       //速度・角速度(m)
    double a0[MOTION_ID_MAX];                       //加速度・角加速度(m)
    
    double np[MOTION_ID_MAX];                       //ドラム回転位置
    double nv[MOTION_ID_MAX];                       //ドラム回転速度(%rps)
    double na[MOTION_ID_MAX];                       //ドラム回転加速度(%rps2)
    double knv[MOTION_ID_MAX];                      //%rps→rps変換係数
 
    double v_ref[MOTION_ID_MAX];                    //速度・角速度指令
    double a_ref[MOTION_ID_MAX];                    //加速度・角加速度指令

    double nv_ref[MOTION_ID_MAX];                    //ドラム回転速度・角速度指令
    double na_ref[MOTION_ID_MAX];                    //ドラム加速度・角加速度指令
  
    bool is_fwd_endstop[MOTION_ID_MAX];             //正転極限判定
    bool is_rev_endstop[MOTION_ID_MAX];             //逆転極限判定
 
    double trq_fb[MOTION_ID_MAX];                   //モータートルクFB
    bool motion_break[MOTION_ID_MAX];               //ブレーキ開閉状態

  
    void init_crane(double _dt); 
 
    void set_v_ref(double hoist_ref, double gantry_ref, double slew_ref, double boomh_ref, double ah_ref);        //速度指令値入力
    void update_break_status();                     //ブレーキ状態, ブレーキ開放経過時間セット
    
    void timeEvolution();                           //時間発展を計算するメソッド
       
    void set_spec(LPST_SPEC _pspec) { pspec = _pspec; return; }
    void set_mode(int _mode) { source_mode = _mode;return; }

    void set_nbh_d_ph_th_from_r(double r);          //旋回半径からd　起伏角を計算してセットする
    void set_nmh_from_mh( double mh);               //主巻揚程から主巻ドラム回転数をセットする
    void set_nah_from_ah(double mh);                //dと補巻揚程から補巻ドラム回転数をセットする
    void set_nsl_from_slr(double sl_rad);           //旋回位置(rad)から旋回ピニオン回転数をセットする
    void set_ngt_from_gtm(double gt_m);             //走行位置から走行車輪回転数をセットする

    void set_d_th_from_nbh();                        //引込ドラム回転状態からd,θの状態をセットする
    void set_bh_layer();                                   //引込ドラム状態をセットする
    void set_mh_layer();                                   //主巻ドラム状態、ロープ状態をセットする
    void set_ah_layer();                                   //主巻ドラム状態、ロープ状態をセットする
    void set_sl_layer();                                   //旋回ドラム状態をセットする
    void set_gt_layer();                                   //走行ドラム状態をセットする

    void set_load(int id, double m, double x, double y, double z) {   //荷重セット
        pSimStat->load[id].m = m; pSimStat->load[id].wx = x; pSimStat->load[id].dy = y;  pSimStat->load[id].hz = z; return;
    }
        
    LPST_CRANE_STATUS pCraneStat;
    LPST_PLC_IO pPLC;
    LPST_SPEC pspec;
    LPST_SIMULATION_STATUS pSimStat;

  private:
    double brk_elaped_time[MOTION_ID_MAX];          //ブレーキ開放経過時間
    double Tf[MOTION_ID_MAX];                       //加速度一時遅れ

    Vector3 A(Vector3& _r, Vector3& _v);            //吊点加速度計算（旋回、引込方向をxy方向に変換
    void Ac();                                      //クレーン加速度計算 SIM mode, PLC mode


    double accdec_cut_spd_range[MOTION_ID_MAX];     //加減速指令を0にする速度指令とFBの差の範囲
 };

//計算誤差吸収処理　紐長さ補正力＝補正ばね弾性力＋補正粘性抵抗力
#define compensationK 0.5                           //紐長さ補正弾性係数
#define compensationGamma 0.5                       //紐長さ粘性係数

//吊荷クラス
class CLoad : public CMob
{
public:
    CLoad() { m = 10000.0; pCrane = NULL; };
    ~CLoad() {};

    void init_mob(double t, Vector3& r, Vector3& v);
    void update_relative_vec();         //吊点との相対ベクトル更新
    Vector3 A(Vector3& r, Vector3& v);  //Model of acceleration
    double S();	//Rope tension

    CJC * pCrane;
    double m;                   //吊荷質量　Kg
 
    int set_m(double _m) { m = _m; return(0); }
    int set_crane(CJC* _pCrane) { pCrane = _pCrane; return(0); }

private:

};




