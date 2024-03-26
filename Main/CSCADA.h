#pragma once

#include "CTaskObj.h"

#include "Spec.h"
#include "CSharedMem.h"
#include "MKChart.h"

#define SCAD_MON_WND_X          950
#define SCAD_MON_WND_Y          0
#define SCAD_MON_WND_W          640
#define SCAD_MON_WND_H          600



//-Main Windowの配置設定

#define MAIN_WND_INIT_SIZE_W		220		//-Main Windowの初期サイズ　W
#define MAIN_WND_INIT_SIZE_H		140		//-Main Windowの初期サイズ　H
#define MAIN_WND_INIT_POS_X			648		//-Main Windowの初期位置設定　X
#define MAIN_WND_INIT_POS_Y			424		//-Main Windowの初期位置設定　Y


//-ID定義 Mainスレッド用　2000 + 100 +α
#define ID_STATUS					2100
#define IDC_STATIC_0				2101
#define IDC_PB_EXIT					2102
#define IDC_PB_ACTIVE				2103
#define IDC_PB_PACKET_MODE			2104
#define IDC_CHK_MON_WND				2105

//メインウィンドウ管理構造体
typedef struct stMainWndTag {
    HWND hWnd_status_bar;								//ステータスバーのハンドル
    HWND h_static0;										//スタティックオブジェクトのハンドル
    HWND h_pb_exit;										//ボタンオブジェクトのハンドル
    HWND h_pb_debug;									//ボタンオブジェクトのハンドル
    HWND h_chk_packetout;								//振れセンサのパケットを送信する
    HWND h_chk_monwnd;									//SIM状態モニタウィンドウ表示チェックボタン
}ST_MAIN_WND, * LPST_MAIN_WND;




//操作端末ウィンドウ構造体
#define SCAD_ID_STATIC_MON_INF      100201
#define SCAD_ID_CHK_AGENT           100202
#define SCAD_ID_CHK_CS              100203
#define SCAD_ID_CHK_POLICY          100204
#define SCAD_ID_CHK_ENVIRONMENT     100205

typedef struct _stScadMonWnd {
    HWND hinf_static;
    HWND hmon_wnd;

    HWND h_chk_ag_msg;      //AGENT       Message checkbox handle
    HWND h_chk_cs_msg;      //CS          Message checkbox handle
    HWND h_chk_pol_msg;     //POLICY      Message checkbox handle
    HWND h_chk_env_msg;     //ENVIRONMENT Message checkbox handle

}ST_SCAD_MON_WND, *LPST_SCAD_MON_WND;


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

#define SCAD_N_CHART_WND        2   //Chart表示Window数
#define SCAD_N_CHART_PER_WND    2   //Window内のChart表示数
#define SCAD_N_GRAPH_PAR_CHART  4   //Chart内のグラフ表示数
#define SCAD_N_BOOL_PAR_GRAPH	4   //グラフ毎のデジタル表示点数
#define SCAD_N_LEGEND_CH    	16  //凡例文字数
#define SCAD_N_CHART_AXIS        2   //Chart表示Window数
#define SCAD_X_AXIS              0   //X軸
#define SCAD_Y_AXIS              1   //Y軸

typedef struct _stCHART_PLOT
{
	double* pd[SCAD_N_CHART_PER_WND][SCAD_N_GRAPH_PAR_CHART];                           //doubleデータ生値
    double* pd100[SCAD_N_CHART_PER_WND][SCAD_N_GRAPH_PAR_CHART];                        //doubleデータ100%値バッファポインタ
    double  d100[SCAD_N_CHART_PER_WND][SCAD_N_GRAPH_PAR_CHART];                         //doubleデータ100%値
	int*    pi[SCAD_N_CHART_PER_WND][SCAD_N_GRAPH_PAR_CHART];                           //intデータ生値
    int*    pi100[SCAD_N_CHART_PER_WND][SCAD_N_GRAPH_PAR_CHART];                        //intデータ100%値バッファポインタ
    int     i100[SCAD_N_CHART_PER_WND][SCAD_N_GRAPH_PAR_CHART];                         //intデータ100%値
 	bool*   pb[SCAD_N_CHART_PER_WND][SCAD_N_GRAPH_PAR_CHART][SCAD_N_BOOL_PAR_GRAPH];    //boolデータ生値
    bool*   pb100[SCAD_N_CHART_PER_WND][SCAD_N_GRAPH_PAR_CHART][SCAD_N_BOOL_PAR_GRAPH]; //boolデータ100%値バッファポインタ
    bool    b100;                                                                       //boolデータ100%値
    char legend[SCAD_N_CHART_PER_WND][SCAD_N_GRAPH_PAR_CHART][SCAD_N_BOOL_PAR_GRAPH][SCAD_N_LEGEND_CH];//凡例文字列
    
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
    LPST_JOB_IO pJOB_IO;

    void input();               //外部データ取り込み
    void main_proc();           //処理内容
    void output();              //出力データ更新

    static ST_SCAD_MON_WND st_mon_wnd;

    //タブパネルのStaticテキストを設定
    void set_panel_tip_txt();
    //タブパネルのFunctionボタンのStaticテキストを設定
    void set_panel_pb_txt();

    //CHART関連
    int chart_plot_ptn = 0;
    ST_CHART_PLOT chart_plot_buf[SCAD_N_CHART_WND][SCAD_N_CHART_AXIS];
 
    int set_graph_link(int iwnd, int ichart, int igraph, int ibool,bool is_x, int type);
    int set_graph_double(int iwnd, int ichart, int igraph, bool is_x, double* pd, double d100);
    int set_graph_int(int iwnd, int ichart, int igraph, bool is_x, int* pi, int i100);
    int set_graph_bool(int iwnd, int ichart, int igraph, int ibool, bool* pb);
    int setup_chart(int iwnd);      //チャートライブラリのバッファにプロットデータの参照ポインタをセット
    int set_chart_data(int iptn);   //指定パターンに従ってプロットデータのバッファセット


    //モニタリングウィンドウ
    HWND open_monitor_wnd(HWND h_parent_wnd);
    static LRESULT CALLBACK MonitorProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

};



