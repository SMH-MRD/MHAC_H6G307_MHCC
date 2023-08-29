#pragma once

#include "CTaskObj.h"

#include "Spec.h"
#include "CSharedMem.h"
#include "MKChart.h"

#define SCAD_CHART_WND1         0   
#define SCAD_CHART_WND2         1 
#define SCAD_CHART_CHART1       0   
#define SCAD_CHART_CHART2       1 
#define SCAD_CHART_GRAPH1       0   
#define SCAD_CHART_GRAPH2       1 
#define SCAD_CHART_GRAPH3       2   
#define SCAD_CHART_GRAPH4       3 

#define SCAD_CHART_PTN0         0   
#define SCAD_CHART_PTN1         1 
#define SCAD_CHART_PTN2         2   
#define SCAD_CHART_PTN3         3 

#define SCAD_N_CHART_WND        2   //Chart�\��Window��
#define SCAD_N_CHART_PER_WND    2   //Window����Chart�\����
#define SCAD_N_GRAPH_PAR_CHART  4   //Chart���̃O���t�\����
#define SCAD_N_BOOL_PAR_GRAPH	4   //�O���t���̃f�W�^���\���_��
#define SCAD_N_LEGEND_CH    	16  //�}�ᕶ����
#define SCAD_N_CHART_AXIS        2   //Chart�\��Window��
#define SCAD_X_AXIS              0   //X��
#define SCAD_Y_AXIS              1   //Y��

typedef struct _stCHART_PLOT
{
	double* pd[SCAD_N_CHART_PER_WND][SCAD_N_GRAPH_PAR_CHART];                           //double�f�[�^���l
    double* pd100[SCAD_N_CHART_PER_WND][SCAD_N_GRAPH_PAR_CHART];                        //double�f�[�^100%�l�o�b�t�@�|�C���^
    double  d100[SCAD_N_CHART_PER_WND][SCAD_N_GRAPH_PAR_CHART];                         //double�f�[�^100%�l
	int*    pi[SCAD_N_CHART_PER_WND][SCAD_N_GRAPH_PAR_CHART];                           //int�f�[�^���l
    int*    pi100[SCAD_N_CHART_PER_WND][SCAD_N_GRAPH_PAR_CHART];                        //int�f�[�^100%�l�o�b�t�@�|�C���^
    int     i100[SCAD_N_CHART_PER_WND][SCAD_N_GRAPH_PAR_CHART];                         //int�f�[�^100%�l
 	bool*   pb[SCAD_N_CHART_PER_WND][SCAD_N_GRAPH_PAR_CHART][SCAD_N_BOOL_PAR_GRAPH];    //bool�f�[�^���l
    bool*   pb100[SCAD_N_CHART_PER_WND][SCAD_N_GRAPH_PAR_CHART][SCAD_N_BOOL_PAR_GRAPH]; //bool�f�[�^100%�l�o�b�t�@�|�C���^
    bool    b100;                                                                       //bool�f�[�^100%�l
    char legend[SCAD_N_CHART_PER_WND][SCAD_N_GRAPH_PAR_CHART][SCAD_N_BOOL_PAR_GRAPH][SCAD_N_LEGEND_CH];//�}�ᕶ����
    
}ST_CHART_PLOT, * LPST_CHART_PLOT;

class CSCADA :public CTaskObj
{
public:
    CSCADA();
    ~CSCADA();

    LRESULT CALLBACK PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

    void init_task(void* pobj);

    void routine_work(void* param);

private:
    LPST_CRANE_STATUS pCraneStat;
    LPST_PLC_IO pPLC_IO;
    LPST_OTE_IO pOTE_IO;
    LPST_SWAY_IO pSway_IO;
    LPST_SIMULATION_STATUS pSimStat;
    LPST_CS_INFO pCSInf;
    LPST_POLICY_INFO pPolicyInf;
    LPST_AGENT_INFO pAgentInf;

    void input();               //�O���f�[�^��荞��
    void main_proc();           //�������e
    void output();              //�o�̓f�[�^�X�V

    //�^�u�p�l����Static�e�L�X�g��ݒ�
    void set_panel_tip_txt();
    //�^�u�p�l����Function�{�^����Static�e�L�X�g��ݒ�
    void set_panel_pb_txt();

    //CHART�֘A
    int chart_plot_ptn = 0;
    ST_CHART_PLOT chart_plot_buf[SCAD_N_CHART_WND][SCAD_N_CHART_AXIS];
 
    int set_graph_link(int iwnd, int ichart, int igraph, int ibool,bool is_x, int type);
    int set_graph_double(int iwnd, int ichart, int igraph, bool is_x, double* pd, double d100);
    int set_graph_int(int iwnd, int ichart, int igraph, bool is_x, int* pi, int i100);
    int set_graph_bool(int iwnd, int ichart, int igraph, int ibool, bool* pb);
    int setup_chart(int iwnd);      //�`���[�g���C�u�����̃o�b�t�@�Ƀv���b�g�f�[�^�̎Q�ƃ|�C���^���Z�b�g
    int set_chart_data(int iptn);   //�w��p�^�[���ɏ]���ăv���b�g�f�[�^�̃o�b�t�@�Z�b�g
};



