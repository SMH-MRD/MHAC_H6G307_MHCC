#pragma once

#include "CTaskObj.h"
#include "Spec.h"
#include "CSharedMem.h"

#define PLC_IO_HELTHY_NG_COUNT      8
#define SIM_HELTHY_NG_COUNT         8
#define SWAY_HELTHY_NG_COUNT        8

class CEnvironment :public CTaskObj
{
public:
    CEnvironment();
    ~CEnvironment();

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    void init_task(void* pobj);
    void routine_work(void* param);
    bool check_tasks_init();            //����^�X�N�̏��������������Ă��邩�`�F�b�N
  
    double cal_hp_acc(int motion, int dir);                 //�ݓ_�̉����x�v�Z(�����m/s2�j���񔼌a���݈ʒu
    double cal_hp_acc(int motion, int dir, double R);       //�ݓ_�̉����x�v�Z(�����m/s2�j���񔼌a�w��R
    double cal_hp_dec(int motion, int dir);                 //�ݓ_�̌����x�v�Z(�����m/s2�j���񔼌a���݈ʒu
    double cal_hp_dec(int motion, int dir, double R);       //�ݓ_�̌����x�v�Z(�����m/s2�j���񔼌a�w��R

    double cal_arad_acc(int motion, int dir);               //�������U��U�p�v�Zrad ���񔼌a���݈ʒu
    double cal_arad_acc(int motion, int dir, double R);     //�������U��U�p�v�Zrad���񔼌a�w��R
    double cal_arad_dec(int motion, int dir);               //�������U��U�p�v�Zrad ���񔼌a���݈ʒu
    double cal_arad_dec(int motion, int dir, double R);     //�������U��U�p�v�Zrad ���񔼌a�w��R

    double cal_arad2(int motion, int dir);                  //�������U��U�p��2��v�Zrad
    double cal_arad2(int motion, int dir, double R);        //�������U��U�p��2��v�Zrad ���񔼌a�w��R

    double cal_sway_r_amp2_m();                              //�U��U�����a���� m
    double cal_sway_th_amp2_m();                             //�U��U���~������ m
    double cal_sway_x_amp2_m();                              //�U��U��x���� m
    double cal_sway_y_amp2_m();                              //�U��U��y���� m

    double cal_dist4stop(int motion, bool is_abs_answer);   //��~�����v�Z
    double cal_dist4target(int motion, bool is_abs_answer); //�ڕW�ʒu�܂ł̋���

    bool is_speed_0(int motion);                            // 0���`�F�b�N

    double cal_T(double pos_hst);                            //�U������v�Z�@�����ʒu�w��
    double cal_w(double pos_hst);                            //�U��p���g���v�Z�@�����ʒu�w��
    double cal_w2(double pos_hst);                           //�U��p���g����2��v�Z�@�����ʒu�w��
    double cal_l(double pos_hst);                            //���[�v���v�Z�@�����ʒu�w��
     
    bool is_sway_larger_than_accsway(int motion);           //�U��p�������U������傫��������

    double get_vmax(int motion);                            //�ő呬�x
 
private:
    ST_SPEC spec;       //�d�l��� Environment�����L�������ɃZ�b�g����B
    ST_CRANE_STATUS stWorkCraneStat;

    LPST_CRANE_STATUS pCraneStat;
    LPST_PLC_IO pPLC_IO;
    LPST_SWAY_IO pSway_IO;
    LPST_OTE_IO pOTE_IO;
    LPST_SIMULATION_STATUS pSimStat;
    LPST_CS_INFO pCSInf;
    LPST_POLICY_INFO pPolicyInf;
    LPST_AGENT_INFO pAgentInf;
    INT32 motion_bit[MOTION_ID_MAX];
    INT16 notch_pos[MOTION_ID_MAX];

    void input();                   //�O���f�[�^��荞��
    void main_proc();               //�������e
    void output();                  //�o�̓f�[�^�X�V

    int parse_notch_com();          //�m�b�`�M���𑬓x�w�߂ɕϊ��Z�b�g
    int sys_mode_set();             //�V�X�e�����[�h��ԃZ�b�g
    int parse_for_auto_ctrl();      //�U�����,�U��~�ߖڕW,�m�b�`��Ԍv�Z
    int pos_set();                  //�ʒu���Z�b�g
    int parse_ote_status();         //���u�[���ڑ���Ԕ���Z�b�g

    void chk_subproc();             //�T�u�v���Z�X��ԃ`�F�b�N

    //���C���p�l����Tweet�e�L�X�g��ݒ�
    void tweet_update();
                                
    //�^�u�p�l����Static�e�L�X�g��ݒ�
    void set_panel_tip_txt();
    //�^�u�p�l����Function�{�^����Static�e�L�X�g��ݒ�
    void set_panel_pb_txt();
    
};

