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

#define SIM_INIT_R                  10.0        //引込初期値 m
#define SIM_INIT_TH                 PI90//0.0   //旋回初期値 rad
#define SIM_INIT_L                  9.8        //ロープ長初期値 m
#define SIM_INIT_X                  10.0        //走行初期値 m
#define SIM_INIT_M                  10000.0     //荷重初期値 kg

#define MOB_MODE_SIM                0
#define MOB_MODE_PLC                1

#define N_DRUM_LAYER                10          //ドラム層数
#define ID_BH_HOIST                 ID_TROLLY   //起伏ドラム主巻用

#define SIM_PARA_DP                 25.174
#define SIM_PARA_DB                 63.0
#define SIM_PARA_PHP_RAD            1.6583
#define SIM_PARA_RMIN_RAD           1.275893    //起伏上限角
#define SIM_PARA_RREST_RAD          -0.71126    //ジブレスト時起伏角
#define SIM_PARA_H_MAX_MH           79.0        //起伏上限角
#define SIM_PARA_H_MIN_MH           -5.55       //ジブレスト時起伏角
#define SIM_PARA_Nd                 6           // ジブ起伏ワイヤ掛数
#define SIM_PARA_Lb                 1170        //起伏ロープ全長
#define SIM_PARA_N_BHBHD_GROOVE       12          //起伏ドラム起伏溝数
#define SIM_PARA_N_BHMHD_GROOVE       10          //起伏ドラム主巻溝数
#define SIM_PARA_N_MHD_GROOVE         15          //主巻ドラム溝数
#define SIM_PARA_N_AHD_GROOVE         15          //補巻ドラム溝数


typedef struct SimCraneParams {
    double d_r[MOTION_ID_MAX][N_DRUM_LAYER];            //ドラムロープ半径
    double d_layer_lmax[MOTION_ID_MAX][N_DRUM_LAYER];   //層最大巻時ロープ長
    double n_drum_max[MOTION_ID_MAX][N_DRUM_LAYER];     //ドラムロープ巻数
    double DpDb2;
    double Dp2Db2;
}ST_SIM_CRANE_PRMS, * LPST_SIM_CRANE_PRMS;

class CCrane : public CMob
{
public:
    CCrane();
    ~CCrane();
    
    double M;                                       //クレーン全体質量　Kg
    double l_mh;                                    //巻ロープ長 m
    Vector3 rc;                                     //クレーン中心点の位置ベクトル
    Vector3 vc;                                     //クレーン中心点の速度ベクトル

    int source_mode;

    double r0[MOTION_ID_MAX];                       //位置・角度
    double v0[MOTION_ID_MAX];                       //速度・角速度
    double a0[MOTION_ID_MAX];                       //加速度・角加速度
    double nv0[MOTION_ID_MAX];                      //回転速度・角速度
    double na0[MOTION_ID_MAX];                      //回転加速度・角加速度 

    double v_ref[MOTION_ID_MAX];                    //速度・角速度指令
    double a_ref[MOTION_ID_MAX];                    //加速度・角加速度指令
    double nv_ref[MOTION_ID_MAX];                   //回転速度・角速度指令
    double na_ref[MOTION_ID_MAX];                   //回転加速度・角加速度指令
 
    bool is_fwd_endstop[MOTION_ID_MAX];             //正転極限判定
    bool is_rev_endstop[MOTION_ID_MAX];             //逆転極限判定
 
    double trq_fb[MOTION_ID_MAX];                   //モータートルクFB
    bool motion_break[MOTION_ID_MAX];               //ブレーキ開閉状態

    ST_SIM_CRANE_PRMS st_crane_prms;
    double n_drum[MOTION_ID_MAX];                   //ドラム回転数
    int i_layer[MOTION_ID_MAX];                     //ドラム層数現在値
    double db;                                      //起伏シーブ間距離
    double th_bh, ph_bh;                            //起伏角

    void set_v_ref(double hoist_ref,double gantry_ref,double slew_ref,double boomh_ref);        //速度指令値入力
    void init_crane(double _dt); 
    int set_spec(LPST_SPEC _pspec) { pspec = _pspec; return 0; }
    void update_break_status();                     //ブレーキ状態, ブレーキ開放経過時間セット
    
    void timeEvolution();                           //時間発展を計算するメソッド
       
    
    void set_plc(LPST_PLC_IO _pPLC) { pPLC = _pPLC; return; }   //`PLC IO共有メモリポインタセット
    void set_mode(int _mode) { source_mode = _mode;return; }

  private:
     LPST_PLC_IO pPLC;
    double brk_elaped_time[MOTION_ID_MAX];          //ブレーキ開放経過時間
    double Tf[MOTION_ID_MAX];                       //加速度一時遅れ

    Vector3 A(Vector3& _r, Vector3& _v);            //吊点加速度計算（旋回、引込方向をxy方向に変換
    void Ac();                                      //クレーン加速度計算 SIM mode, PLC mode

    LPST_SPEC pspec;
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

    CCrane * pCrane;
    double m;                   //吊荷質量　Kg
 
    int set_m(double _m) { m = _m; return(0); }
    int set_crane(CCrane* _pCrane) { pCrane = _pCrane; return(0); }

private:

};




