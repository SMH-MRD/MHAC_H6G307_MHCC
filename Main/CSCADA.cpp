#include "CSCADA.h"

#include "framework.h"
#include <windowsx.h>       //# コモンコントロール
#include <iostream>
#include <iomanip>
#include <sstream>
#include <bitset>

//-共有メモリオブジェクトポインタ:
extern CSharedMem* pCraneStatusObj;
extern CSharedMem* pSwayStatusObj;
extern CSharedMem* pPLCioObj;
extern CSharedMem* pSwayIO_Obj;
extern CSharedMem* pOTEioObj;
extern CSharedMem* pCSInfObj;
extern CSharedMem* pPolicyInfObj;
extern CSharedMem* pAgentInfObj;
extern CSharedMem* pSimulationStatusObj;
extern CSharedMem* pJobIO_Obj;

extern vector<void*>	VectpCTaskObj;	//タスクオブジェクトのポインタ
extern ST_iTask g_itask;

//スタティックメンバ
ST_SCAD_MON_WND CSCADA ::st_mon_wnd;
/****************************************************************************/
/*   コンストラクタ　デストラクタ                                           */
/****************************************************************************/
CSCADA::CSCADA() {
	pPLC_IO = NULL;
	pCraneStat = NULL;
	pOTE_IO = NULL;
	pSway_IO = NULL;
	pSimStat = NULL;
	pCSInf = NULL;
	pPolicyInf = NULL;
	pAgentInf = NULL;
}
CSCADA::~CSCADA() {

}
/****************************************************************************/
/*   タスク初期化処理                                                       */
/* 　メインスレッドでインスタンス化した後に呼びます。                       */
/****************************************************************************/
void CSCADA::init_task(void* pobj) {

	//共有メモリ構造体のポインタセット
	pPLC_IO = (LPST_PLC_IO)(pPLCioObj->get_pMap());
	pCraneStat = (LPST_CRANE_STATUS)(pCraneStatusObj->get_pMap());
	pOTE_IO = (LPST_OTE_IO)(pOTEioObj->get_pMap());
	pSway_IO = (LPST_SWAY_IO)(pSwayIO_Obj->get_pMap());
	pSimStat = (LPST_SIMULATION_STATUS)(pSimulationStatusObj->get_pMap());
	pCSInf = (LPST_CS_INFO)(pCSInfObj->get_pMap());
	pPolicyInf = (LPST_POLICY_INFO)(pPolicyInfObj->get_pMap());
	pAgentInf = (LPST_AGENT_INFO)(pAgentInfObj->get_pMap());
	pJOB_IO = (LPST_JOB_IO)(pJobIO_Obj->get_pMap());
	
	//チャートデータバッファクリア
	memset(&chart_plot_buf[SCAD_CHART_WND1][SCAD_X_AXIS], 0, sizeof(ST_CHART_PLOT));
	memset(&chart_plot_buf[SCAD_CHART_WND1][SCAD_Y_AXIS], 0, sizeof(ST_CHART_PLOT));
	memset(&chart_plot_buf[SCAD_CHART_WND2][SCAD_X_AXIS], 0, sizeof(ST_CHART_PLOT));
	memset(&chart_plot_buf[SCAD_CHART_WND2][SCAD_Y_AXIS], 0, sizeof(ST_CHART_PLOT));
	//チャート100％値デフォルト値セット
	for (int i = 0; i < SCAD_N_CHART_PER_WND;i++) {
		for (int j = 0; j < SCAD_N_GRAPH_PAR_CHART;j++) {
			chart_plot_buf[SCAD_CHART_WND1][SCAD_X_AXIS].d100[i][j] = 1.0;
			chart_plot_buf[SCAD_CHART_WND1][SCAD_Y_AXIS].d100[i][j] = 1.0;
			chart_plot_buf[SCAD_CHART_WND2][SCAD_X_AXIS].i100[i][j] = 1;
			chart_plot_buf[SCAD_CHART_WND2][SCAD_Y_AXIS].i100[i][j] = 1;
		}
		chart_plot_buf[SCAD_CHART_WND1][SCAD_X_AXIS].b100 = true;
		chart_plot_buf[SCAD_CHART_WND1][SCAD_Y_AXIS].b100 = true;
		chart_plot_buf[SCAD_CHART_WND2][SCAD_X_AXIS].b100 = true;
		chart_plot_buf[SCAD_CHART_WND2][SCAD_Y_AXIS].b100 = true;
	}
	//チャート100％値ポインタバッファセット
	for (int i = 0; i < SCAD_N_CHART_PER_WND;i++) {
		for (int j = 0; j < SCAD_N_GRAPH_PAR_CHART;j++) {
			chart_plot_buf[SCAD_CHART_WND1][SCAD_X_AXIS].pd100[i][j] = &chart_plot_buf[SCAD_CHART_WND1][SCAD_X_AXIS].d100[i][j];
			chart_plot_buf[SCAD_CHART_WND1][SCAD_Y_AXIS].pd100[i][j] = &chart_plot_buf[SCAD_CHART_WND1][SCAD_Y_AXIS].d100[i][j];
			chart_plot_buf[SCAD_CHART_WND2][SCAD_X_AXIS].pd100[i][j] = &chart_plot_buf[SCAD_CHART_WND2][SCAD_X_AXIS].d100[i][j];
			chart_plot_buf[SCAD_CHART_WND2][SCAD_Y_AXIS].pd100[i][j] = &chart_plot_buf[SCAD_CHART_WND2][SCAD_Y_AXIS].d100[i][j];
			chart_plot_buf[SCAD_CHART_WND1][SCAD_X_AXIS].pi100[i][j] = &chart_plot_buf[SCAD_CHART_WND1][SCAD_X_AXIS].i100[i][j];
			chart_plot_buf[SCAD_CHART_WND1][SCAD_Y_AXIS].pi100[i][j] = &chart_plot_buf[SCAD_CHART_WND1][SCAD_Y_AXIS].i100[i][j];
			chart_plot_buf[SCAD_CHART_WND2][SCAD_X_AXIS].pi100[i][j] = &chart_plot_buf[SCAD_CHART_WND2][SCAD_X_AXIS].i100[i][j];
			chart_plot_buf[SCAD_CHART_WND2][SCAD_Y_AXIS].pi100[i][j] = &chart_plot_buf[SCAD_CHART_WND2][SCAD_Y_AXIS].i100[i][j];
		}
	}

	CMKChart::init_chartfunc();


	set_panel_tip_txt();

	inf.is_init_complete = true;

	open_monitor_wnd(inf.hWnd_parent);

	return;
};

/****************************************************************************/
/*   タスク定周期処理                                                       */
/* 　タスクスレッドで毎周期実行される関数			　                      */
/****************************************************************************/
void CSCADA::routine_work(void* param) {
	input();
	main_proc();
	output();
};

//定周期処理手順1　外部信号入力
void CSCADA::input() {

	return;

};

//定周期処理手順2　メイン処理
void CSCADA::main_proc() {

	//チャートデータセット
	/*
	if (chart_plot_ptn != SCAD_CHART_PTN0) {
		if (CMKChart::is_chart_open(MK_CHART1) || CMKChart::is_chart_open(MK_CHART2)){
			set_chart_data(chart_plot_ptn);
		}
	}
	*/
	return;
}

//定周期処理手順3　信号出力処理
void CSCADA::output() {

	//デバッグモニタ
		
	if (st_mon_wnd.hmon_wnd != NULL) {

		ST_JOB_SET job = pJOB_IO->job_list[ID_JOBTYPE_SEMI].job[pJOB_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot];
		ST_COMMAND_SET com = job.com[job.i_hot_com];

		wostrs.clear(); wostrs.str(L"");
		wostrs << L"JOB MONINTOR\n"
			<< L"JOB STATUS:" << job.status;

		wostrs << L"COMMAND MONINTOR\n"
			<<L"           MH            AH              BH            SL\n"
			<<L"nStep       " << std::dec << com.seq[ID_HOIST].n_step<<L"              "<< com.seq[ID_AHOIST].n_step << L"               "<< com.seq[ID_BOOM_H].n_step << L"               " << com.seq[ID_SLEW].n_step << L"\n"
			<< L"iStep        " << com.seq[ID_HOIST].i_hot_step << L"              " << com.seq[ID_AHOIST].i_hot_step << L"               " << com.seq[ID_BOOM_H].i_hot_step << L"               " << com.seq[ID_SLEW].i_hot_step << L"\n"
			<< L"Step type   " << std::hex << com.seq[ID_HOIST].steps[com.seq[ID_HOIST].i_hot_step].type << L"              " << com.seq[ID_AHOIST].steps[com.seq[ID_AHOIST].i_hot_step].type << L"               " << com.seq[ID_BOOM_H].steps[com.seq[ID_BOOM_H].i_hot_step].type << L"               " << com.seq[ID_SLEW].steps[com.seq[ID_SLEW].i_hot_step].type << L"\n";
	
		if (st_mon_wnd.hmon_wnd != NULL) {
			SetWindowText(st_mon_wnd.hinf_static, wostrs.str().c_str());
		}
	}

	wostrs.clear(); wostrs.str(L"");
	wostrs << L" V: mh " << ((LPST_PLC_IO)pPLCioObj->get_pMap())->v_fb[ID_HOIST];
	wostrs << L" ph: slew " << pSway_IO->ph[SID_LOAD_MH][SID_CAM_X]*180/3.14;
	wostrs << L" --Scan " << inf.period;
	tweet2owner(wostrs.str()); wostrs.str(L""); wostrs.clear();

	return;

};

HWND CSCADA::open_monitor_wnd(HWND h_parent_wnd) {
	InitCommonControls();//コモンコントロール初期化
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MonitorProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = TEXT("SCAD_MON");
	wcex.lpszClassName = TEXT("SCAD_MON");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//メインウィンドウ
	st_mon_wnd.hmon_wnd = CreateWindowW(TEXT("SCAD_MON"), TEXT("SCAD_MON"), WS_OVERLAPPEDWINDOW,
		SCAD_MON_WND_X, SCAD_MON_WND_Y, SCAD_MON_WND_W, SCAD_MON_WND_H,
		h_parent_wnd, nullptr, hInst, nullptr);

	ShowWindow(st_mon_wnd.hmon_wnd, SW_SHOW);
	UpdateWindow(st_mon_wnd.hmon_wnd);

	return st_mon_wnd.hmon_wnd;
};

/// <summary>
/// デバッグモニタ用ウィンドウコールバック
/// </summary>
/// <param name="hWnd"></param>
/// <param name="message"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
/// <returns></returns>
LRESULT CALLBACK CSCADA::MonitorProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		st_mon_wnd.hinf_static = CreateWindowW(TEXT("STATIC"),L"SCAD INF", WS_CHILD | WS_VISIBLE | SS_LEFT,
				0, 30, SCAD_MON_WND_W, SCAD_MON_WND_H,hWnd, (HMENU)(SCAD_ID_STATIC_MON_INF), hInst, NULL);


		st_mon_wnd.h_chk_ag_msg = CreateWindowW(TEXT("BUTTON"), L"AGENT", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			10,5,80,15,	hWnd, (HMENU)(SCAD_ID_CHK_AGENT), hInst, NULL);
		st_mon_wnd.h_chk_pol_msg = CreateWindowW(TEXT("BUTTON"), L"POLICY", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			105, 5, 80, 15, hWnd, (HMENU)(SCAD_ID_CHK_POLICY), hInst, NULL);
		st_mon_wnd.h_chk_cs_msg = CreateWindowW(TEXT("BUTTON"), L"CS", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			200, 5, 50, 15, hWnd, (HMENU)(SCAD_ID_CHK_CS), hInst, NULL);
		st_mon_wnd.h_chk_env_msg= CreateWindowW(TEXT("BUTTON"), L"ENVIRONMENT", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			260, 5, 120, 15, hWnd, (HMENU)(SCAD_ID_CHK_ENVIRONMENT), hInst, NULL);
	
		break;

	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case 1:break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		st_mon_wnd.hmon_wnd = NULL;
		//PostQuitMessage(0);
	}break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
};



/****************************************************************************/
/*   タスク設定タブパネルウィンドウのコールバック関数                       */
/****************************************************************************/
LRESULT CALLBACK CSCADA::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_TASK_FUNC_RADIO1:
		{
			inf.panel_func_id = LOWORD(wp); set_panel_tip_txt(); set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			reset_panel_item_pb(hDlg);
		}break;
		case IDC_TASK_FUNC_RADIO2:
		case IDC_TASK_FUNC_RADIO3:
		case IDC_TASK_FUNC_RADIO4:
		case IDC_TASK_FUNC_RADIO5:
		case IDC_TASK_FUNC_RADIO6:
			inf.panel_func_id = LOWORD(wp); set_panel_tip_txt(); set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			reset_panel_item_pb(hDlg);
			break;

		case IDC_TASK_ITEM_RADIO1:
		{
			if (inf.panel_func_id == IDC_TASK_FUNC_RADIO1) {
				setup_chart(MK_CHART1);	//プロットデータのポインタをライブラリのバッファにセット
				CMKChart::open_chart(MK_CHART1, inf.hWnd_opepane); 
			}
			if (IsDlgButtonChecked(hDlg, IDC_TASK_ITEM_RADIO1) == BST_CHECKED) {
				SendMessage(GetDlgItem(hDlg, IDC_TASK_ITEM_RADIO1), BM_SETCHECK, BST_UNCHECKED, 0L);
			}

			if (inf.panel_func_id == IDC_TASK_FUNC_RADIO6) {
				if (IsDlgButtonChecked(hDlg, IDC_TASK_FUNC_RADIO6) == BST_CHECKED) {
					SendMessage(GetDlgItem(hDlg, IDC_TASK_FUNC_RADIO6), BM_SETCHECK, BST_UNCHECKED, 0L);
				}
				open_monitor_wnd(inf.hWnd_parent);
			}


			inf.panel_type_id = LOWORD(wp);set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hWnd_opepane, IDC_TASK_EDIT1));
			break;
		}
		case IDC_TASK_ITEM_RADIO2:
			if (inf.panel_func_id == IDC_TASK_FUNC_RADIO1) {
				setup_chart(MK_CHART2);	//プロットデータのポインタをライブラリのバッファにセット
				CMKChart::open_chart(MK_CHART2, inf.hWnd_opepane);
			}
			if (IsDlgButtonChecked(hDlg, IDC_TASK_ITEM_RADIO2) == BST_CHECKED) {
				SendMessage(GetDlgItem(hDlg, IDC_TASK_ITEM_RADIO2), BM_SETCHECK, BST_UNCHECKED, 0L);
			}
			inf.panel_type_id = LOWORD(wp);set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hWnd_opepane, IDC_TASK_EDIT1));
			break;
		case IDC_TASK_ITEM_RADIO3:
			if (inf.panel_func_id == IDC_TASK_FUNC_RADIO1) {
				chart_plot_ptn = SCAD_CHART_PTN0;
			}
			inf.panel_type_id = LOWORD(wp);set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hWnd_opepane, IDC_TASK_EDIT1));
			break;
		case IDC_TASK_ITEM_RADIO4:
			if (inf.panel_func_id == IDC_TASK_FUNC_RADIO1) {
				chart_plot_ptn = SCAD_CHART_PTN1;
				set_chart_data(SCAD_CHART_PTN1);
			}
			inf.panel_type_id = LOWORD(wp);set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hWnd_opepane, IDC_TASK_EDIT1));
			break;
		case IDC_TASK_ITEM_RADIO5:
			if (inf.panel_func_id == IDC_TASK_FUNC_RADIO1) {
				chart_plot_ptn = SCAD_CHART_PTN2;
				set_chart_data(SCAD_CHART_PTN2);
			}
			inf.panel_type_id = LOWORD(wp);set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hWnd_opepane, IDC_TASK_EDIT1));
			break;
		case IDC_TASK_ITEM_RADIO6:
			if (inf.panel_func_id == IDC_TASK_FUNC_RADIO1) {
				chart_plot_ptn = SCAD_CHART_PTN3;
				set_chart_data(SCAD_CHART_PTN3);
			}
			else;

			inf.panel_type_id = LOWORD(wp);set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hWnd_opepane, IDC_TASK_EDIT1));
			break;
		case IDSET: {
			wstring wstr, wstr_tmp;

			//サンプルとしていろいろな型で読み込んで表示している
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
/*   タスク設定パネルの操作ボタン説明テキスト設定関数                       */
/****************************************************************************/
void CSCADA::set_panel_tip_txt()
{
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO1: {
		wstr = L"Type for CHART \n\r 1:OPEN Time 2:OPEN Scat 3:NO PLOT \n\r 4:PLOT1 5:PLOT2 6:PLOT3";
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
		wstr = L"Func6 \n\r 1:MON Open 2:?? 3:?? 4:?? 5:?? 6:??";
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
/*　　タスク設定パネルボタンのテキストセット					            */
/****************************************************************************/
void CSCADA::set_panel_pb_txt() {

	WCHAR str_func01[] = L"CHART";
	SetDlgItemText(inf.hWnd_opepane, IDC_TASK_FUNC_RADIO1, (LPCWSTR)str_func01);

	SetDlgItemText(inf.hWnd_opepane, IDC_TASK_FUNC_RADIO6, L"MON");

	return;
};

//****************************************************************************
//　　チャートライブラリモジュールにプロットデータのバッファポインタをセット
//	引数：
//		int iwnd	：開いているチャートウィンドウのID (0,1)
//		int ichart	：ウィンドウ内のチャートのID(0,1)
//		int igraph	：チャート内のグラフ要素ID(0-3)
//		int ibool	：グラフ要素がON/OFFの時の要素ID(0-3)
//		boolis_x	：チャートのタイプが散布図の時、x軸用はtrueをセット
//		int type	：データのタイプ(double,int,bool)
//****************************************************************************
int CSCADA::set_graph_link(int iwnd, int ichart, int igraph, int ibool, bool is_x, int type) {

	if (type == MK_DATA_TYPE_DOUBLE) {
		if (is_x) {
			CMKChart::set_double_data(&chart_plot_buf[iwnd][SCAD_X_AXIS].pd[ichart][igraph], iwnd, ichart, igraph,
				&chart_plot_buf[iwnd][SCAD_X_AXIS].pd100[ichart][igraph], true);
		}
		else {
			CMKChart::set_double_data(&chart_plot_buf[iwnd][SCAD_Y_AXIS].pd[ichart][igraph], iwnd, ichart, igraph,
				&chart_plot_buf[iwnd][SCAD_Y_AXIS].pd100[ichart][igraph], false);
		}
		return MK_DATA_TYPE_DOUBLE;
	}
	else if (type == MK_DATA_TYPE_INT) {
		if (is_x) {
			CMKChart::set_int_data(&chart_plot_buf[iwnd][SCAD_X_AXIS].pi[ichart][igraph], iwnd, ichart, igraph,
				&chart_plot_buf[iwnd][SCAD_X_AXIS].pi100[ichart][igraph], true);
		}
		else {
			CMKChart::set_int_data(&chart_plot_buf[iwnd][SCAD_Y_AXIS].pi[ichart][igraph], iwnd, ichart, igraph,
				&chart_plot_buf[iwnd][SCAD_Y_AXIS].pi100[ichart][igraph], false);
		}
		return MK_DATA_TYPE_INT;
	}
	else if (type == MK_DATA_TYPE_BOOL) {
		if (is_x) {
			CMKChart::set_bool_data(&chart_plot_buf[iwnd][SCAD_X_AXIS].pb[ichart][igraph][ibool], iwnd, ichart, igraph, ibool, 
				&chart_plot_buf[iwnd][SCAD_X_AXIS].pb100[ichart][igraph][ibool]);

		}
		else {
			CMKChart::set_bool_data(&chart_plot_buf[iwnd][SCAD_Y_AXIS].pb[ichart][igraph][ibool], iwnd, ichart, igraph,ibool,
				&chart_plot_buf[iwnd][SCAD_Y_AXIS].pb100[ichart][igraph][ibool]);
		}
		return MK_DATA_TYPE_BOOL;
	}
	else return -1;
}

//****************************************************************************
//　　チャートライブラリリンク用データバッファにデータを割付
//	引数：
//		int iwnd	：開いているチャートウィンドウのID (0,1)
//		int ichart	：ウィンドウ内のチャートのID(0,1)
//		int igraph	：チャート内のグラフ要素ID(0-3)
//		int ibool	：グラフ要素がON/OFFの時の要素ID(0-3)
//		boolis_x	：チャートのタイプが散布図の時、x軸用はtrueをセット
//		int type	：データのタイプ(double,int,bool)
//****************************************************************************
int CSCADA::set_graph_double(int iwnd, int ichart, int igraph, bool is_x, double* pd, double d100) {
	if (iwnd == MK_WND_TYPE_TIME_GRAPH) {
		//set data type
		CMKChart::set_data_type(MK_DATA_TYPE_TIME, MK_WND_TYPE_TIME_GRAPH, ichart, igraph, true);
		CMKChart::set_data_type(MK_DATA_TYPE_DOUBLE, MK_WND_TYPE_TIME_GRAPH, ichart, igraph, false);
		//set data
		chart_plot_buf[MK_WND_TYPE_TIME_GRAPH][SCAD_Y_AXIS].pd[ichart][igraph] = pd;
		chart_plot_buf[MK_WND_TYPE_TIME_GRAPH][SCAD_Y_AXIS].d100[ichart][igraph] = d100;
	}
	else if (iwnd == MK_WND_TYPE_SCATTER) {
		if (is_x) {
			CMKChart::set_data_type(MK_DATA_TYPE_DOUBLE, MK_WND_TYPE_SCATTER, ichart, igraph, true);
			chart_plot_buf[MK_WND_TYPE_SCATTER][SCAD_X_AXIS].pd[ichart][igraph] = pd;
			chart_plot_buf[MK_WND_TYPE_SCATTER][SCAD_X_AXIS].d100[ichart][igraph] = d100;
		}
		else {
			CMKChart::set_data_type(MK_DATA_TYPE_DOUBLE, MK_WND_TYPE_SCATTER, ichart, igraph, false);
			chart_plot_buf[MK_WND_TYPE_SCATTER][SCAD_Y_AXIS].pd[ichart][igraph] = pd;
			chart_plot_buf[MK_WND_TYPE_SCATTER][SCAD_Y_AXIS].d100[ichart][igraph] = d100;
		}
	}
	else return -1;

	return 0;
}
int CSCADA::set_graph_int(int iwnd, int ichart, int igraph,  bool is_x, int* pi, int i100) {
	if (iwnd == MK_WND_TYPE_TIME_GRAPH) {
		//set data type
		CMKChart::set_data_type(MK_DATA_TYPE_TIME, MK_WND_TYPE_TIME_GRAPH, ichart, igraph, true);
		CMKChart::set_data_type(MK_DATA_TYPE_INT, MK_WND_TYPE_TIME_GRAPH, ichart, igraph, false);
		//set data
		chart_plot_buf[MK_WND_TYPE_TIME_GRAPH][SCAD_Y_AXIS].pi[ichart][igraph] = pi;
		chart_plot_buf[MK_WND_TYPE_TIME_GRAPH][SCAD_Y_AXIS].i100[ichart][igraph] = i100;
	}
	else if (iwnd == MK_WND_TYPE_SCATTER) {
		if (is_x) {
			CMKChart::set_data_type(MK_DATA_TYPE_INT, MK_WND_TYPE_SCATTER, ichart, igraph, true);
			chart_plot_buf[MK_WND_TYPE_SCATTER][SCAD_X_AXIS].pi[ichart][igraph] = pi;
			chart_plot_buf[MK_WND_TYPE_SCATTER][SCAD_X_AXIS].i100[ichart][igraph] = i100;
		}
		else {
			CMKChart::set_data_type(MK_DATA_TYPE_INT, MK_WND_TYPE_SCATTER, ichart, igraph, false);
			chart_plot_buf[MK_WND_TYPE_SCATTER][SCAD_Y_AXIS].pi[ichart][igraph] = pi;
			chart_plot_buf[MK_WND_TYPE_SCATTER][SCAD_Y_AXIS].i100[ichart][igraph] = i100;
		}
	}
	return 0;
}
int CSCADA::set_graph_bool(int iwnd, int ichart, int igraph, int ibool, bool* pb) {

	if (iwnd == MK_WND_TYPE_TIME_GRAPH) {
		//set data type
		CMKChart::set_data_type(MK_DATA_TYPE_TIME, MK_WND_TYPE_TIME_GRAPH, ichart, igraph, true);
		CMKChart::set_data_type(MK_DATA_TYPE_BOOL, MK_WND_TYPE_TIME_GRAPH, ichart, igraph, false);
		//set data
		chart_plot_buf[MK_WND_TYPE_TIME_GRAPH][SCAD_Y_AXIS].pb[ichart][igraph][ibool] = pb;
		chart_plot_buf[MK_WND_TYPE_TIME_GRAPH][SCAD_Y_AXIS].b100 = true;
	}
	else return - 1;

	return 0;
}

//****************************************************************************
//　　チャートモジュールにプロットデータのバッファポインタをセット
//****************************************************************************
int CSCADA::setup_chart(int iwnd) {

	for (int ichart = 0;ichart < SCAD_N_CHART_PER_WND;ichart++)
		for (int igraph = 0;igraph < SCAD_N_GRAPH_PAR_CHART;igraph++)
			for (int ibool = 0;ibool < SCAD_N_BOOL_PAR_GRAPH;ibool++)
			{
				set_graph_link(iwnd, ichart, igraph, ibool, true, MK_DATA_TYPE_DOUBLE);
				set_graph_link(iwnd, ichart, igraph, ibool, false, MK_DATA_TYPE_DOUBLE);
				set_graph_link(iwnd, ichart, igraph, ibool, true, MK_DATA_TYPE_INT);
				set_graph_link(iwnd, ichart, igraph, ibool, false, MK_DATA_TYPE_INT);
				set_graph_link(iwnd, ichart, igraph, ibool, true, MK_DATA_TYPE_BOOL);
				set_graph_link(iwnd, ichart, igraph, ibool, false, MK_DATA_TYPE_BOOL);
			}

	return 0;
}
//****************************************************************************
//　　プロットデータをバッファにセット
//****************************************************************************
int CSCADA::set_chart_data(int iptn) {

	//現在の設定をクリア
	CMKChart::clear_set_data(MK_WND_TYPE_TIME_GRAPH);
	CMKChart::clear_set_data(MK_WND_TYPE_SCATTER);

	if (iptn == SCAD_CHART_PTN1) {
	//Window1 Time Chart
	//Chart1
		//Graph1
		set_graph_double(MK_WND_TYPE_TIME_GRAPH, MK_CHART1, MK_GRAPH1,false, &(pAgentInf->v_ref[ID_BOOM_H]), pCraneStat->spec.notch_spd_f[ID_BOOM_H][NOTCH_5]);
		//Graph2
		set_graph_double(MK_WND_TYPE_TIME_GRAPH, MK_CHART1, MK_GRAPH2, false, &(pPLC_IO->v_fb[ID_BOOM_H]), pCraneStat->spec.notch_spd_f[ID_BOOM_H][NOTCH_5]);
		//Graph3
		set_graph_double(MK_WND_TYPE_TIME_GRAPH, MK_CHART1, MK_GRAPH3, false, &(pSway_IO->rad_amp2[SID_LOAD_MH][SID_CAM_Y]), 25*PI1DEG* PI1DEG);
		//Graph4
		set_graph_double(MK_WND_TYPE_TIME_GRAPH, MK_CHART1, MK_GRAPH4, false, &(pSway_IO->th[SID_LOAD_MH][SID_CAM_Y]), PI15);

	//Chart2
		//Graph1
		set_graph_double(MK_WND_TYPE_TIME_GRAPH, MK_CHART2, MK_GRAPH1, false, &(pSway_IO->rad_amp2[SID_LOAD_MH][SID_CAM_X]), 25 * PI1DEG * PI1DEG);

		//Graph3
		set_graph_double(MK_WND_TYPE_TIME_GRAPH, MK_CHART2, MK_GRAPH2, false, &(pSway_IO->th[SID_LOAD_MH][SID_CAM_X]), PI15);

		//Graph3
		set_graph_double(MK_WND_TYPE_TIME_GRAPH, MK_CHART2, MK_GRAPH3, false, &(pAgentInf->v_ref[ID_SLEW]), pCraneStat->spec.notch_spd_f[ID_SLEW][NOTCH_5]);

		//Graph4
		set_graph_double(MK_WND_TYPE_TIME_GRAPH, MK_CHART2, MK_GRAPH4, false, &(pPLC_IO->v_fb[ID_SLEW]), pCraneStat->spec.notch_spd_f[ID_SLEW][NOTCH_5]);



	//Window2 Scatter
	//Chart2
		//Graph1
		set_graph_double(MK_WND_TYPE_SCATTER, MK_CHART1, MK_GRAPH1, true, &(pSway_IO->th[SID_LOAD_MH][SID_CAM_X]), PI15);
		set_graph_double(MK_WND_TYPE_SCATTER, MK_CHART1, MK_GRAPH1, false, &(pSway_IO->dthw[SID_LOAD_MH][SID_CAM_X]), PI15);
		//Graph2
		set_graph_double(MK_WND_TYPE_SCATTER, MK_CHART2, MK_GRAPH2, true, &(pSway_IO->th[SID_LOAD_MH][SID_CAM_Y]), PI15);
		set_graph_double(MK_WND_TYPE_SCATTER, MK_CHART2, MK_GRAPH2, false, &(pSway_IO->dthw[SID_LOAD_MH][SID_CAM_Y]), PI15);

	}
	else if (iptn == SCAD_CHART_PTN2) {
	//Window1 Time Chart	
	//Chart2
	//Graph1
		set_graph_double(MK_WND_TYPE_TIME_GRAPH, MK_CHART2, MK_GRAPH1, false, &(pSway_IO->th[SID_LOAD_MH][SID_CAM_X]), PI15);
	//Graph2
		set_graph_double(MK_WND_TYPE_TIME_GRAPH, MK_CHART2, MK_GRAPH2, false, &(pSway_IO->th[SID_LOAD_MH][SID_CAM_Y]), PI15);
	//Graph3
	//Graph4
	//Window2 Scatter
	//Chart1
		//Graph3
		set_graph_double(MK_WND_TYPE_SCATTER, MK_CHART1, MK_GRAPH3, true, &(pSway_IO->th[SID_LOAD_MH][SID_CAM_X]), PI15);
		set_graph_double(MK_WND_TYPE_SCATTER, MK_CHART1, MK_GRAPH3, false, &(pSway_IO->th[SID_LOAD_MH][SID_CAM_Y]), PI15);
	}
	else if (iptn == SCAD_CHART_PTN3) {
		;
	}
	else;

	return 0;
}