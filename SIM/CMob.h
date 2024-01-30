#pragma once

#include "CVector3.h"
#include "COMMON_DEF.H"
#include "Spec.h"
#include "CSharedMem.h"

//Moving Object�N���X
class CMob
{
public:
    CMob();
    CMob(double _dt);
    CMob(double _dt, Vector3& _r, Vector3& _v);
    ~CMob();

    double dt;      //�v�Z���ԊԊu
    Vector3 a;      //�����x�x�N�g��
    Vector3 r;      //�ʒu�x�N�g��
    Vector3 v;      //���x�x�N�g��
    Vector3 L;      //���[�v�x�N�g��
    Vector3 vL;     //���[�v���x�x�N�g��
    Vector3 fex;    //�O��
    Vector3 dr;     //�ʒu�x�N�g���̕ω���
    Vector3 dv;     //���x�x�N�g���̕ω���
    Vector3 R0;     //��_
     
    //�����x�x�N�g����^���郁�\�b�h�@�@�p����ōĒ�`����
    virtual Vector3 A(Vector3& r, Vector3& v); 
    virtual void set_fex(double,double,double);//�O��
    virtual void set_dt(double);//�v�Z���ԊԊu�Z�b�g

    //���x�x�N�g����^���郁�\�b�h
    virtual Vector3 V(Vector3& r, Vector3& v);
    //���Ԕ��W���v�Z���郁�\�b�h
    virtual void timeEvolution();
    virtual void init_mob(double _dt, Vector3& _r, Vector3& _v) {
        dt = _dt;
        r.copy(_r);
        v.copy(_v);
        return;
    }
 
private:

};

//�N���[���N���X
//r,v�́A�ݓ_�̈ʒu�ƍ��W

#define SIM_INIT_R                  21.00       //�f�t�H���g���񔼌a�努
#define SIM_INIT_MHR                21.90       //�f�t�H���g���񔼌a�努
#define SIM_INIT_AHR                26.06       //�f�t�H���g���񔼌a�⊪
#define SIM_INIT_MH                 60.0        //�f�t�H���g�努����
#define SIM_INIT_AH                 59.2        //�f�t�H���g�⊪����
#define SIM_INIT_TH                 PI90        //���񏉊��l rad
#define SIM_INIT_L                  9.8         //���[�v�������l m
#define SIM_INIT_X                  10.0        //���s�����l m
#define SIM_INIT_M                  10000.0     //�׏d�����l kg

#define MOB_MODE_SIM                0
#define MOB_MODE_PLC                1


class CJC : public CMob
{
public:
    CJC();
    ~CJC();
    
    double M;                                       //�N���[���S�̎��ʁ@Kg
    double mh_load;                                //�努�׏d
    double ah_load;                                //�⊪�׏d
    double l_mh;                                    //�����[�v�� m
    double l_ah;                                    //�⊪�����[�v�� m

    double slw_rad_per_turn;                        //����s�j�I��1��]�̐���p�x
    double gnt_m_per_turn;                          //���s�ԗ�1��]�̈ړ���
    double c_ph, s_ph, c_phb, s_phb;                              //cos�� sin��
    double cal_Lm2Lp2,cal_Lb2Lp2, cal_2LmLp, cal_2LbLp;                          //���v�Z�p���ԕϐ�
 
    Vector3 rc;                                     //�N���[�����S�_�̈ʒu�x�N�g��
    Vector3 vc;                                     //�N���[�����S�_�̑��x�x�N�g��

    int source_mode;

    double r0[MOTION_ID_MAX];                       //�ʒu�E�p�x(m)
    double v0[MOTION_ID_MAX];                       //���x�E�p���x(m)
    double a0[MOTION_ID_MAX];                       //�����x�E�p�����x(m)
    
    double np[MOTION_ID_MAX];                       //�h������]�ʒu
    double nv[MOTION_ID_MAX];                       //�h������]���x(%rps)
    double na[MOTION_ID_MAX];                       //�h������]�����x(%rps2)
    double knv[MOTION_ID_MAX];                      //%rps��rps�ϊ��W��
 
    double v_ref[MOTION_ID_MAX];                    //���x�E�p���x�w��
    double a_ref[MOTION_ID_MAX];                    //�����x�E�p�����x�w��

    double nv_ref[MOTION_ID_MAX];                    //�h������]���x�E�p���x�w��
    double na_ref[MOTION_ID_MAX];                    //�h���������x�E�p�����x�w��
  
    bool is_fwd_endstop[MOTION_ID_MAX];             //���]�Ɍ�����
    bool is_rev_endstop[MOTION_ID_MAX];             //�t�]�Ɍ�����
 
    double trq_fb[MOTION_ID_MAX];                   //���[�^�[�g���NFB
    bool motion_break[MOTION_ID_MAX];               //�u���[�L�J���

  
    void init_crane(double _dt); 
 
    void set_v_ref(double hoist_ref, double gantry_ref, double slew_ref, double boomh_ref, double ah_ref);        //���x�w�ߒl����
    void update_break_status();                     //�u���[�L���, �u���[�L�J���o�ߎ��ԃZ�b�g
    
    void timeEvolution();                           //���Ԕ��W���v�Z���郁�\�b�h
       
    void set_spec(LPST_SPEC _pspec) { pspec = _pspec; return; }
    void set_mode(int _mode) { source_mode = _mode;return; }

    void set_nbh_d_ph_th_from_r(double r);          //���񔼌a����d�@�N���p���v�Z���ăZ�b�g����
    void set_nmh_from_mh( double mh);               //�努�g������努�h������]�����Z�b�g����
    void set_nah_from_ah(double mh);                //d�ƕ⊪�g������⊪�h������]�����Z�b�g����
    void set_nsl_from_slr(double sl_rad);           //����ʒu(rad)�������s�j�I����]�����Z�b�g����
    void set_ngt_from_gtm(double gt_m);             //���s�ʒu���瑖�s�ԗ։�]�����Z�b�g����

    void set_d_th_from_nbh();                        //�����h������]��Ԃ���d,�Ƃ̏�Ԃ��Z�b�g����
    void set_bh_layer();                                   //�����h������Ԃ��Z�b�g����
    void set_mh_layer();                                   //�努�h������ԁA���[�v��Ԃ��Z�b�g����
    void set_ah_layer();                                   //�努�h������ԁA���[�v��Ԃ��Z�b�g����
    void set_sl_layer();                                   //����h������Ԃ��Z�b�g����
    void set_gt_layer();                                   //���s�h������Ԃ��Z�b�g����

    void set_load(int id, double m, double x, double y, double z) {   //�׏d�Z�b�g
        pSimStat->load[id].m = m; pSimStat->load[id].wx = x; pSimStat->load[id].dy = y;  pSimStat->load[id].hz = z; return;
    }
        
    LPST_CRANE_STATUS pCraneStat;
    LPST_PLC_IO pPLC;
    LPST_SPEC pspec;
    LPST_SIMULATION_STATUS pSimStat;

  private:
    double brk_elaped_time[MOTION_ID_MAX];          //�u���[�L�J���o�ߎ���
    double Tf[MOTION_ID_MAX];                       //�����x�ꎞ�x��

    Vector3 A(Vector3& _r, Vector3& _v);            //�ݓ_�����x�v�Z�i����A����������xy�����ɕϊ�
    void Ac();                                      //�N���[�������x�v�Z SIM mode, PLC mode


    double accdec_cut_spd_range[MOTION_ID_MAX];     //�������w�߂�0�ɂ��鑬�x�w�߂�FB�̍��͈̔�
 };

//�v�Z�덷�z�������@�R�����␳�́��␳�΂˒e���́{�␳�S����R��
#define compensationK 0.5                           //�R�����␳�e���W��
#define compensationGamma 0.5                       //�R�����S���W��

//�݉׃N���X
class CLoad : public CMob
{
public:
    CLoad() { m = 10000.0; pCrane = NULL; };
    ~CLoad() {};

    void init_mob(double t, Vector3& r, Vector3& v);
    void update_relative_vec();         //�ݓ_�Ƃ̑��΃x�N�g���X�V
    Vector3 A(Vector3& r, Vector3& v);  //Model of acceleration
    double S();	//Rope tension

    CJC * pCrane;
    double m;                   //�݉׎��ʁ@Kg
 
    int set_m(double _m) { m = _m; return(0); }
    int set_crane(CJC* _pCrane) { pCrane = _pCrane; return(0); }

private:

};




