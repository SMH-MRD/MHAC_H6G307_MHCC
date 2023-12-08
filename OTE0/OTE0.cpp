﻿// OTE0.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "OTE0.h"
#include "OTE0panel.h"

#include <windowsx.h>       //# コモンコントロール
#include <winsock2.h>
#include <iostream>
#include <iomanip>
#include <sstream>

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

ST_OTE_WORK_WND st_work_wnd;
HWND hWnd_work;					//操作端末メインウィンドウハンドル
HWND hWnd_swy;					//振れウィンドウハンドル
HWND hWnd_sub[OTE0_N_SUB_WND];	//
HWND hwnd_current_subwnd;
COte* pOte0;                    //OTE0オブジェクト

static std::wostringstream msg_wos;

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);

HWND open_connect_Wnd(HWND hwnd);
HWND open_mode_Wnd(HWND hwnd);
HWND open_fault_Wnd(HWND hwnd);
HWND open_moment_Wnd(HWND hwnd);
HWND open_auto_Wnd(HWND hwnd);
HWND open_swy_Wnd(HWND hwnd);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndConnectProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndAutoProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndModeProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndFaultProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndMomentProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndSwyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void set_OTE_panel_objects(HWND hWnd);			//OTEウィンドウ上へコントロール配置
void disp_msg_cnt();
void create_objects(HWND hWnd);					//DC,Bitmap,Brush,Pen削除
void delete_objects(HWND hWnd);					//DC,Bitmap,Brush,Pen生成

void draw_lamp(HDC hdc);						//ノッチランプ描画
void draw_graphic();
void draw_info();
void combine_map();



INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OTE0, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OTE0));

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OTE0));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = NULL;// MAKEINTRESOURCEW(IDC_OTE0);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する
   InitCommonControls();//コモンコントロール初期化
 
   //メインウィンドウ
   hWnd_work = CreateWindowW(TEXT("OTE0"), TEXT("OTE0"), WS_OVERLAPPEDWINDOW,
	   OTE0_WND_X, OTE0_WND_Y, OTE0_WND_W, OTE0_WND_H,
	   nullptr, nullptr, hInst, nullptr);

   if (!hWnd_work)
   {
	   return FALSE;
   }

   //表示ウィンドウエリアサイズ取得
   RECT rc;
   GetClientRect(hWnd_work, &rc);
   st_work_wnd.area_w = rc.right - rc.left;
   st_work_wnd.area_h = rc.bottom - rc.top;

   //ノッチランプ領域セット
   for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
	   RECT rc_add;
	   if (i == ID_SLEW) {
		   rc_add.left = 0; rc_add.top = PRM_OTE_DEF_PB_H; rc_add.right = PRM_OTE_DEF_PB_W2; rc_add.bottom = PRM_OTE_DEF_PB_H/2;
	   }
	   else if (i == ID_GANTRY) {
		   rc_add.left = 0; rc_add.top = -PRM_OTE_DEF_PB_H/2; rc_add.right = PRM_OTE_DEF_PB_W2; rc_add.bottom = PRM_OTE_DEF_PB_H/2;
	   }
	   else {
		   rc_add.left = -PRM_OTE_DEF_PB_W2/2; rc_add.top = 0; rc_add.right = PRM_OTE_DEF_PB_W2 / 2; rc_add.bottom = PRM_OTE_DEF_PB_H;
	   }
	   for (int j = 0; j < N_OTE_NOTCH_ARRAY; j++) {
		   st_work_wnd.notch_rect[i][j].left = st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][i * N_OTE_NOTCH_ARRAY + j].x + rc_add.left;
		   st_work_wnd.notch_rect[i][j].top = st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][i * N_OTE_NOTCH_ARRAY + j].y + rc_add.top;
		   st_work_wnd.notch_rect[i][j].right = st_work_wnd.notch_rect[i][j].left + rc_add.right;
		   st_work_wnd.notch_rect[i][j].bottom = st_work_wnd.notch_rect[i][j].top + rc_add.bottom;
	   }
   }

   //UIオブジェクト生成
   create_objects(hWnd_work);

   //表示フォント設定
   st_work_wnd.hfont_inftext = CreateFont(6, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));


    // OTE0オブジェクトインスタンス化
   pOte0 = new COte(hWnd_work);                              // メイン処理クラスのインスタンス化
   pOte0->init_proc();
   
   ShowWindow(hWnd_work, nCmdShow);
   UpdateWindow(hWnd_work);

   return TRUE;
}

//*********************************************************************************************
/// <summary>
/// ワークウィンドウコールバック関数
/// </summary>
/// <param name="hWnd"></param>
/// <param name="message"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
/// <returns></returns>
static int tmp_counter = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	int id;
	HINSTANCE hInst = GetModuleHandle(0);
	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化

		//ウィンドウにコントロール追加
		set_OTE_panel_objects(hWnd);
		//サブウィンドウ追加
		hwnd_current_subwnd = open_connect_Wnd(hWnd);		//接続表示子ウィンドウ
		open_swy_Wnd(hWnd);										//振れウィンドウ追加

				  //マルチキャストタイマ起動 
		SetTimer(hWnd, ID_OTE_MULTICAST_TIMER, OTE_MULTICAST_SCAN_MS, NULL);
		//ユニキャストタイマ起動
		SetTimer(hWnd, ID_OTE_UNICAST_TIMER, OTE_UNICAST_SCAN_MS, NULL);

	}
	case WM_CTLCOLORSTATIC: {
#if 0
		if ((HWND)lParam == st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_LAMP_HIJYOU]) {
			SetTextColor((HDC)wParam, RGB(255, 0, 0));
			SetWindowTextW((HWND)lParam, L"●");
			return (INT_PTR)st_work_wnd.hbrush[OTE0_GLAY];
		}
#endif
	}break;

	case WM_TIMER: {

		if (wParam == ID_OTE_MULTICAST_TIMER) {
			if (S_OK == pOte0->snd_ote_m_pc(pOte0->set_msg_ote_m())) {//OTEマルチキャスト送信
				pOte0->cnt_snd_ote_m++;
			}
		}
		if (wParam == ID_OTE_UNICAST_TIMER) {
			if (S_OK == pOte0->snd_ote_u_pc(pOte0->set_msg_ote_u(), &pOte0->addrin_ote_u_snd)) {//OTEユニキャスト送信
				pOte0->cnt_snd_ote_m++;
			}
		}
		disp_msg_cnt();	//カウント表示更新

	}break;

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

		//ソケットIF
	case ID_SOCK_EVENT_PC_UNI_OTE: {
		int nEvent = WSAGETSELECTEVENT(lParam);
		switch (nEvent) {
		case FD_READ: {
			if (pOte0->rcv_pc_u_ote(&pOte0->st_msg_ote_u_rcv) == S_OK) {				//PCからのユニキャストメッセージ受信
				pOte0->cnt_rcv_pc_u++;
				disp_msg_cnt();
			}
			else {
				pOte0->msg_ws = L"ERROR : rcv_ote_u_pc()";	pOte0->wstr_out_inf(pOte0->msg_ws);
			}
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case ID_SOCK_EVENT_PC_MULTI_OTE: {
		int nEvent = WSAGETSELECTEVENT(lParam);
		switch (nEvent) {
		case FD_READ: {
			if (pOte0->rcv_pc_m_ote(&pOte0->st_msg_pc_m_ote_rcv) == S_OK) {				//PCからのユニキャストメッセージ受信
				pOte0->cnt_rcv_pc_m++;
				disp_msg_cnt();
			}
			else {
				pOte0->msg_ws = L"ERROR : rcv_ote_u_pc()";	pOte0->wstr_out_inf(pOte0->msg_ws);
			}
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case ID_SOCK_EVENT_OTE_MULTI_OTE: {
		int nEvent = WSAGETSELECTEVENT(lParam);
		switch (nEvent) {
		case FD_READ: {
			if (pOte0->rcv_ote_m_ote(&pOte0->st_msg_ote_m_ote_rcv) == S_OK) {				//PCからのユニキャストメッセージ受信
				pOte0->cnt_rcv_ote_m++;
				disp_msg_cnt();
			}
			else {
				pOte0->msg_ws = L"ERROR : rcv_ote_m_ote()";	pOte0->wstr_out_inf(pOte0->msg_ws);
			}
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		draw_lamp(hdc);

	//	BOOL brtn = TransparentBlt(st_work_wnd.hdc[ID_OTE_HDC_MEM0], 0, 0, OTE0_WND_W, OTE0_WND_H,		//DST
	//		st_work_wnd.hdc[ID_OTE_HDC_MEM0], 0, 0, OTE0_WND_W, OTE0_WND_H,	//SOURCE
	//		RGB(255, 255, 255));
	//	combine_map();//コピー元描画生成
	//	BitBlt(hdc, 0, 0, 800, 800, st_work_wnd.hdc[ID_OTE_HDC_MEM0], 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		KillTimer(hWnd_work, ID_OTE_MULTICAST_TIMER);
		KillTimer(hWnd_work, ID_OTE_UNICAST_TIMER);
		delete_objects(hWnd_work);
		PostQuitMessage(0);
	}break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
}

//*********************************************************************************************
/// <summary>
/// サブウィンドウコールバック関数
/// </summary>
/// <param name="hWnd"></param>
/// <param name="message"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
/// <returns></returns>
LRESULT CALLBACK WndConnectProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		//ウィンドウにコントロール追加
		st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEAD] = CreateWindowW(TEXT("STATIC"), 
			st_work_wnd.ctrl_text[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEAD], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEAD].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEAD].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEAD].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEAD].cy,
			hWnd, (HMENU)(BASE_ID_OTE_STATIC + ID_OTE_SUB_CONNECT_HEAD), hInst, NULL);

		st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_BODY] = CreateWindowW(TEXT("STATIC"),
			st_work_wnd.ctrl_text[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_BODY], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_BODY].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_BODY].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_BODY].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_BODY].cy,
			hWnd, (HMENU)(BASE_ID_OTE_STATIC + ID_OTE_SUB_CONNECT_BODY), hInst, NULL);

		//メッセージ表示切替ラジオ釦
		for (int i = ID_OTE_RADIO_SOU; i <= ID_OTE_RADIO_RPM; i++) {
			if (i == ID_OTE_RADIO_SOU) {// | WS_GROUP あり
				st_work_wnd.hctrl[ID_OTE_CTRL_PB][i] = CreateWindow(L"BUTTON",
					st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].y,
					st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cy,
					hWnd, (HMENU)(BASE_ID_OTE_PB + (LONG64)i), hInst, NULL);
			}
			else {
				st_work_wnd.hctrl[ID_OTE_CTRL_PB][i] = CreateWindow(L"BUTTON",
					st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].y,
					st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cy,
					hWnd, (HMENU)(BASE_ID_OTE_PB + (LONG64)i), hInst, NULL);
			}
		}
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case BASE_ID_OTE_PB + ID_OTE_RADIO_SOU: {
			st_work_wnd.connect_wnd_item = ID_OTE_RADIO_SOU;
		}break;
		case BASE_ID_OTE_PB + ID_OTE_RADIO_RPU: {
			st_work_wnd.connect_wnd_item = ID_OTE_RADIO_RPU;
		}break;
		case BASE_ID_OTE_PB + ID_OTE_RADIO_SOM: {
			st_work_wnd.connect_wnd_item = ID_OTE_RADIO_SOM;
		}break;
		case BASE_ID_OTE_PB + ID_OTE_RADIO_ROM: {
			st_work_wnd.connect_wnd_item = ID_OTE_RADIO_ROM;
		}break;
		case BASE_ID_OTE_PB + ID_OTE_RADIO_RPM: {
			st_work_wnd.connect_wnd_item = ID_OTE_RADIO_RPM;
		}break;
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
		//PostQuitMessage(0);
	}break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
}
LRESULT CALLBACK WndAutoProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case 1: {

		}break;

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
		//PostQuitMessage(0);
	}break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
}
LRESULT CALLBACK WndModeProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
//		CreateWindowW(TEXT("STATIC"), L"MODE WINDOW", WS_CHILD | WS_VISIBLE | SS_LEFT,
//			0, 0, 100, 20, hWnd, (HMENU)(OTE_ID_STATIC + OTE_INDEX_STATIC_COMMON_WND), hInst, NULL);

	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case 1: {

		}break;

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
		//PostQuitMessage(0);
	}break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
}
LRESULT CALLBACK WndFaultProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case 1: {

		}break;

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
		//PostQuitMessage(0);
	}break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
}
LRESULT CALLBACK WndMomentProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加

	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case 1: {

		}break;

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
		//PostQuitMessage(0);
	}break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
}
LRESULT CALLBACK WndSwyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		//ウィンドウにコントロール追加
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case 1: {
		}break;
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
		//PostQuitMessage(0);
	}break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

//****************************************************************************
/// <summary>
/// OTEサブウィンドウオープン
/// </summary>
/// <param name="hwnd"></param>
/// <returns></returns>
HWND open_connect_Wnd(HWND hwnd) {
	InitCommonControls();//コモンコントロール初期化
	HINSTANCE hInst = GetModuleHandle(0);
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndConnectProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(0);;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;// TEXT("OTECON");
	wcex.lpszClassName = TEXT("OTECON");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//メインウィンドウ
	hWnd_sub[ID_OTE0_SUB_WND_CONNECT] = CreateWindowW(TEXT("OTECON"), TEXT("OTECON"), WS_CHILD | WS_BORDER,
		//hWnd_sub[ID_OTE0_SUB_WND_CONNECT] = CreateWindowW(TEXT("OTECON"), TEXT("OTECON"), WS_OVERLAPPEDWINDOW,
		OTE0_SUB_WND_X, OTE0_SUB_WND_Y, OTE0_SUB_WND_W, OTE0_SUB_WND_H,
		hwnd, (HMENU)100, hInst, nullptr);
	//	hwnd, nullptr, hInst, nullptr);
	RECT rc;
	GetClientRect(hWnd_sub[ID_OTE0_SUB_WND_CONNECT], &rc);
	st_work_wnd.area_w = rc.right - rc.left;
	st_work_wnd.area_h = rc.bottom - rc.top;

	//デバイスコンテキスト
	HDC hdc = GetDC(hWnd_sub[ID_OTE0_SUB_WND_CONNECT]);
	TextOutW(hdc, 10, 10, L"<<Information>>", 15);
	ReleaseDC(hWnd_sub[ID_OTE0_SUB_WND_CONNECT], hdc);

	InvalidateRect(hWnd_sub[ID_OTE0_SUB_WND_CONNECT], NULL, TRUE);//表示更新

	ShowWindow(hWnd_sub[ID_OTE0_SUB_WND_CONNECT], SW_SHOW);
	UpdateWindow(hWnd_sub[ID_OTE0_SUB_WND_CONNECT]);

	return hWnd_sub[ID_OTE0_SUB_WND_CONNECT];
}
HWND open_auto_Wnd(HWND hwnd) {

	InitCommonControls();//コモンコントロール初期化
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndAutoProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(0);;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
	wcex.lpszMenuName = TEXT("OTE0 AUTO");
	wcex.lpszClassName = TEXT("OTE0 AUTO");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//メインウィンドウ
	hWnd_sub[ID_OTE0_SUB_WND_AUTO] = CreateWindowW(TEXT("OTE0 AUTO"), TEXT("OTE0 AUTO"), WS_CHILD | WS_BORDER,//WS_OVERLAPPEDWINDOW,
		OTE0_SUB_WND_X, OTE0_SUB_WND_Y, OTE0_SUB_WND_W, OTE0_SUB_WND_H,
		hwnd, nullptr, hInst, nullptr);

	InvalidateRect(hWnd_sub[ID_OTE0_SUB_WND_AUTO], NULL, TRUE);//表示更新

	ShowWindow(hWnd_sub[ID_OTE0_SUB_WND_AUTO], SW_SHOW);
	UpdateWindow(hWnd_sub[ID_OTE0_SUB_WND_AUTO]);

	InitCommonControls();//コモンコントロール初期化

	return hWnd_sub[ID_OTE0_SUB_WND_AUTO];
}
HWND open_mode_Wnd(HWND hwnd) {

	InitCommonControls();//コモンコントロール初期化
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndModeProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(0);;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
	wcex.lpszMenuName = TEXT("OTE0 MODE");
	wcex.lpszClassName = TEXT("OTE0 MODE");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//メインウィンドウ
	hWnd_sub[ID_OTE0_SUB_WND_MODE] = CreateWindowW(TEXT("OTE0 MODE"), TEXT("OTE0 MODE"), WS_CHILD | WS_BORDER,//WS_OVERLAPPEDWINDOW,
		OTE0_SUB_WND_X, OTE0_SUB_WND_Y, OTE0_SUB_WND_W, OTE0_SUB_WND_H,
		hwnd, nullptr, hInst, nullptr);

	InvalidateRect(hWnd_sub[ID_OTE0_SUB_WND_MODE], NULL, TRUE);//表示更新

	ShowWindow(hWnd_sub[ID_OTE0_SUB_WND_MODE], SW_SHOW);
	UpdateWindow(hWnd_sub[ID_OTE0_SUB_WND_MODE]);

	return hWnd_sub[ID_OTE0_SUB_WND_MODE];
}
HWND open_fault_Wnd(HWND hwnd) {

	InitCommonControls();//コモンコントロール初期化
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndFaultProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(0);;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
	wcex.lpszMenuName = TEXT("OTE0 FAULT");
	wcex.lpszClassName = TEXT("OTE0 FAULT");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//メインウィンドウ
	hWnd_sub[ID_OTE0_SUB_WND_FAULT] = CreateWindowW(TEXT("OTE0 FAULT"), TEXT("OTE0 FAULT"), WS_CHILD | WS_BORDER,//WS_OVERLAPPEDWINDOW,
		OTE0_SUB_WND_X, OTE0_SUB_WND_Y, OTE0_SUB_WND_W, OTE0_SUB_WND_H,
		hwnd, nullptr, hInst, nullptr);

	InvalidateRect(hWnd_sub[ID_OTE0_SUB_WND_FAULT], NULL, TRUE);//表示更新

	ShowWindow(hWnd_sub[ID_OTE0_SUB_WND_FAULT], SW_SHOW);
	UpdateWindow(hWnd_sub[ID_OTE0_SUB_WND_FAULT]);

	return hWnd_sub[ID_OTE0_SUB_WND_FAULT];
}
HWND open_moment_Wnd(HWND hwnd) {

	InitCommonControls();//コモンコントロール初期化
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndMomentProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(0);;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
	wcex.lpszMenuName = TEXT("OTE0 MOMENT");
	wcex.lpszClassName = TEXT("OTE0 MOMENT");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//メインウィンドウ
	hWnd_sub[ID_OTE0_SUB_WND_MOMENT] = CreateWindowW(TEXT("OTE0 MOMENT"), TEXT("OTE0 MOMENT"), WS_CHILD | WS_BORDER,//WS_OVERLAPPEDWINDOW,
		OTE0_SUB_WND_X, OTE0_SUB_WND_Y, OTE0_SUB_WND_W, OTE0_SUB_WND_H,
		hwnd, nullptr, hInst, nullptr);


	InvalidateRect(hWnd_sub[ID_OTE0_SUB_WND_MOMENT], NULL, TRUE);//表示更新

	ShowWindow(hWnd_sub[ID_OTE0_SUB_WND_MOMENT], SW_SHOW);
	UpdateWindow(hWnd_sub[ID_OTE0_SUB_WND_MOMENT]);

	return hWnd_sub[ID_OTE0_SUB_WND_MOMENT];
}
HWND open_swy_Wnd(HWND hwnd) {

	InitCommonControls();//コモンコントロール初期化
	
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndSwyProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(0);;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
	wcex.lpszMenuName = TEXT("SWAY STAT");
	wcex.lpszClassName = TEXT("SWAY STAT");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//メインウィンドウ
	hWnd_sub[ID_OTE0_SWY_WND] = CreateWindowW(TEXT("SWAY STAT"), TEXT("SWAY STAT"), WS_CHILD | WS_BORDER,//WS_OVERLAPPEDWINDOW,
		OTE0_SWY_WND_X, OTE0_SWY_WND_Y, OTE0_SWY_WND_W, OTE0_SWY_WND_H,
		hwnd, nullptr, hInst, nullptr);

	InvalidateRect(hWnd_sub[ID_OTE0_SWY_WND], NULL, TRUE);//表示更新

	ShowWindow(hWnd_sub[ID_OTE0_SWY_WND], SW_SHOW);
	UpdateWindow(hWnd_sub[ID_OTE0_SWY_WND]);

	return hWnd_sub[ID_OTE0_SWY_WND];
}

//*********************************************************************************************
/// <summary>
/// OTEウィンドウ上にオブジェクト配置
/// </summary>
void set_OTE_panel_objects(HWND hWnd) {

	HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

	//ランプ
	for (int i = ID_OTE_LAMP_HIJYOU; i <= ID_OTE_LAMP_N3; i++) {
		st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][i] = CreateWindowW(TEXT("STATIC"), st_work_wnd.ctrl_text[ID_OTE_CTRL_STATIC][i], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][i].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][i].cy,
			hWnd, (HMENU)(BASE_ID_OTE_STATIC + i), hInst, NULL);
	}
	//ラベル
	for (int i = ID_OTE_LABEL_MH; i <= ID_OTE_INF_GT0; i++) {
		st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][i] = CreateWindowW(TEXT("STATIC"), st_work_wnd.ctrl_text[ID_OTE_CTRL_STATIC][i], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][i].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][i].cy,
			hWnd, (HMENU)(BASE_ID_OTE_STATIC + i), hInst, NULL);
	}

	//PB
	{
		for (LONGLONG i = ID_OTE_PB_HIJYOU; i <= ID_OTE_PB_N3; i++) {
			st_work_wnd.hctrl[ID_OTE_CTRL_PB][i] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][i], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE ,
				st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].y,
				st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cy,
				hWnd, (HMENU)(BASE_ID_OTE_PB + i), hInst, NULL);
		}
		//CHECK BOX
		for (LONGLONG i = ID_OTE_CHK_WIDE; i <= ID_OTE_CHK_OPE; i++) {
			st_work_wnd.hctrl[ID_OTE_CTRL_PB][i] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][i], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
				st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].y,
				st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cy,
				hWnd, (HMENU)(BASE_ID_OTE_PB + i), hInst, NULL);
		}
		

		
		//RADIO
		st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_COM] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][ID_OTE_RADIO_COM], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_COM].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_COM].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_COM].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_COM].cy,
			hWnd, (HMENU)(BASE_ID_OTE_PB + ID_OTE_RADIO_COM), hInst, NULL);

		for (LONGLONG i = ID_OTE_RADIO_MODE; i <= ID_OTE_RADIO_MOMENT; i++) {
			st_work_wnd.hctrl[ID_OTE_CTRL_PB][i] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
				st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].y,
				st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cy,
				hWnd, (HMENU)(BASE_ID_OTE_PB + i), hInst, NULL);
		}
	}

	//ノッチラジオボタン
	//主巻
	{	st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_HOIST] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][ID_HOIST * N_OTE_NOTCH_ARRAY], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
		st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_HOIST * N_OTE_NOTCH_ARRAY].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_HOIST * N_OTE_NOTCH_ARRAY].y,
		st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_HOIST * N_OTE_NOTCH_ARRAY].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_HOIST * N_OTE_NOTCH_ARRAY].cy,
		hWnd, (HMENU)(ID_OTE_NOTCH_MH0 - 4), hInst, NULL);

	for (LONGLONG i = 1; i <= 8; i++) {
		st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_HOIST] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][ID_HOIST * N_OTE_NOTCH_ARRAY + i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_HOIST * N_OTE_NOTCH_ARRAY + i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_HOIST * N_OTE_NOTCH_ARRAY + i].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_HOIST * N_OTE_NOTCH_ARRAY + i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_HOIST * N_OTE_NOTCH_ARRAY + i].cy,
			hWnd, (HMENU)(ID_OTE_NOTCH_MH0 - 4 + i), hInst, NULL);
	}
	}


	//補巻
	{
		st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_AHOIST] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][ID_AHOIST * N_OTE_NOTCH_ARRAY], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_AHOIST * N_OTE_NOTCH_ARRAY].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_AHOIST * N_OTE_NOTCH_ARRAY].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_AHOIST * N_OTE_NOTCH_ARRAY].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_AHOIST * N_OTE_NOTCH_ARRAY].cy,
			hWnd, (HMENU)(ID_OTE_NOTCH_AH0 - 4), hInst, NULL);

		for (LONGLONG i = 1; i <= 8; i++) {
			st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_AHOIST] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][ID_AHOIST * N_OTE_NOTCH_ARRAY + i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
				st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_AHOIST * N_OTE_NOTCH_ARRAY + i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_AHOIST * N_OTE_NOTCH_ARRAY + i].y,
				st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_AHOIST * N_OTE_NOTCH_ARRAY + i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_AHOIST * N_OTE_NOTCH_ARRAY + i].cy,
				hWnd, (HMENU)(ID_OTE_NOTCH_AH0 - 4 + i), hInst, NULL);
		}
	}
	//引込
	{	st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_BOOM_H] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][ID_BOOM_H * N_OTE_NOTCH_ARRAY], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
		st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_BOOM_H * N_OTE_NOTCH_ARRAY].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_BOOM_H * N_OTE_NOTCH_ARRAY].y,
		st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_BOOM_H * N_OTE_NOTCH_ARRAY].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_BOOM_H * N_OTE_NOTCH_ARRAY].cy,
		hWnd, (HMENU)(ID_OTE_NOTCH_BH0 - 4), hInst, NULL);

	for (LONGLONG i = 1; i <= 8; i++) {
		st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_BOOM_H] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][ID_BOOM_H * N_OTE_NOTCH_ARRAY + i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_BOOM_H * N_OTE_NOTCH_ARRAY + i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_BOOM_H * N_OTE_NOTCH_ARRAY + i].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_BOOM_H * N_OTE_NOTCH_ARRAY + i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_BOOM_H * N_OTE_NOTCH_ARRAY + i].cy,
			hWnd, (HMENU)(ID_OTE_NOTCH_BH0 - 4 + i), hInst, NULL);
	}
	}
	//旋回
	{	st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_SLEW] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][ID_SLEW * N_OTE_NOTCH_ARRAY], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
		st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_SLEW * N_OTE_NOTCH_ARRAY].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_SLEW * N_OTE_NOTCH_ARRAY].y,
		st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_SLEW * N_OTE_NOTCH_ARRAY].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_SLEW * N_OTE_NOTCH_ARRAY].cy,
		hWnd, (HMENU)(ID_OTE_NOTCH_SL0 - 4), hInst, NULL);

	for (LONGLONG i = 1; i <= 8; i++) {
		st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_SLEW] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][ID_SLEW * N_OTE_NOTCH_ARRAY + i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_SLEW * N_OTE_NOTCH_ARRAY + i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_SLEW * N_OTE_NOTCH_ARRAY + i].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_SLEW * N_OTE_NOTCH_ARRAY + i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_SLEW * N_OTE_NOTCH_ARRAY + i].cy,
			hWnd, (HMENU)(ID_OTE_NOTCH_SL0 - 4 + i), hInst, NULL);
	}
	}
	//走行
	{	st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_GANTRY] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][ID_GANTRY * N_OTE_NOTCH_ARRAY], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
		st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_GANTRY * N_OTE_NOTCH_ARRAY].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_GANTRY * N_OTE_NOTCH_ARRAY].y,
		st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_GANTRY * N_OTE_NOTCH_ARRAY].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_GANTRY * N_OTE_NOTCH_ARRAY].cy,
		hWnd, (HMENU)(ID_OTE_NOTCH_GT0 - 4), hInst, NULL);

	for (LONGLONG i = 1; i <= 8; i++) {
		st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_GANTRY] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][ID_GANTRY * N_OTE_NOTCH_ARRAY + i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_GANTRY * N_OTE_NOTCH_ARRAY + i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_GANTRY * N_OTE_NOTCH_ARRAY + i].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_GANTRY * N_OTE_NOTCH_ARRAY + i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_GANTRY * N_OTE_NOTCH_ARRAY + i].cy,
			hWnd, (HMENU)(ID_OTE_NOTCH_GT0 - 4 + i), hInst, NULL);
	}
	}
	return;
}
void disp_msg_cnt() {
	
	tmp_counter++;
	msg_wos.str(L"");
	msg_wos << L"OTE0     " << L"SOU:" << tmp_counter << L"  RPU:" << L"  SOM:" << L"  ROM:" << L"  RPM:";
	SetWindowText(hWnd_work, msg_wos.str().c_str());

	return;
}
void draw_graphic() {
	;
}
void draw_info() {
	;
}
void combine_map() {

	//グラフィックを重ね合わせ
	TransparentBlt(st_work_wnd.hdc[ID_OTE_HBMAP_MEM0], 0, 0, st_work_wnd.area_w, st_work_wnd.area_h,
		st_work_wnd.hdc[ID_OTE_HBMAP_MEM_GR], 0, 0, st_work_wnd.area_w, st_work_wnd.area_h,
		RGB(255, 255, 255));
	//テキストを重ね合わせ
	TransparentBlt(st_work_wnd.hdc[ID_OTE_HBMAP_MEM0], 0, 0, st_work_wnd.area_w, st_work_wnd.area_h,
		st_work_wnd.hdc[ID_OTE_HBMAP_MEM_IF], 0, 0, st_work_wnd.area_w, st_work_wnd.area_h,
		RGB(255, 255, 255));
}

/// <summary>
/// ノッチランプの描画
/// </summary>
void draw_lamp(HDC hdc) {
	HBITMAP hBitmap;
	HDC hmdc;
	HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

	SelectObject(hdc, GetStockObject(NULL_PEN));

	for (int i = 0; i < 9; i++) {
		if (st_work_wnd.lamp_status[ID_OTE_NOTCH_MH + i] & OTE0_LAMP_ON_MASK) {
			SelectObject(hdc, st_work_wnd.hbrush[OTE0_GREEN]);
		}
		else {
			SelectObject(hdc, st_work_wnd.hbrush[OTE0_ORANGE]);
		}
		Rectangle(hdc, st_work_wnd.notch_rect[ID_HOIST][i].left,st_work_wnd.notch_rect[ID_HOIST][i].top,st_work_wnd.notch_rect[ID_HOIST][i].right,st_work_wnd.notch_rect[ID_HOIST][i].bottom);
		Rectangle(hdc, st_work_wnd.notch_rect[ID_GANTRY][i].left, st_work_wnd.notch_rect[ID_GANTRY][i].top, st_work_wnd.notch_rect[ID_GANTRY][i].right, st_work_wnd.notch_rect[ID_GANTRY][i].bottom);
		Rectangle(hdc, st_work_wnd.notch_rect[ID_BOOM_H][i].left, st_work_wnd.notch_rect[ID_BOOM_H][i].top, st_work_wnd.notch_rect[ID_BOOM_H][i].right, st_work_wnd.notch_rect[ID_BOOM_H][i].bottom);
		Rectangle(hdc, st_work_wnd.notch_rect[ID_SLEW][i].left, st_work_wnd.notch_rect[ID_SLEW][i].top, st_work_wnd.notch_rect[ID_SLEW][i].right, st_work_wnd.notch_rect[ID_SLEW][i].bottom);
		Rectangle(hdc, st_work_wnd.notch_rect[ID_AHOIST][i].left, st_work_wnd.notch_rect[ID_AHOIST][i].top, st_work_wnd.notch_rect[ID_AHOIST][i].right, st_work_wnd.notch_rect[ID_AHOIST][i].bottom);
	}
	


//	hBitmap = (HBITMAP)LoadBitmap(hInst, L"IDB_BITMAP_LAMP0_GREEN");

//	hmdc = GetDC(st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_LABEL_MH]);
//	SetBkColor(hmdc, RGB(255, 0, 0));
//	SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_LABEL_MH], L"OK");
//	DeleteDC(hmdc);
	InvalidateRect(st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_LABEL_MH],NULL,true);
//	hmdc = CreateCompatibleDC(st_work_wnd.hdc_pb_ctr_source);
//	SelectObject(hmdc, hBitmap); BitBlt(st_work_wnd.hdc_pb_ctr_source, 0, 0, 50, 20, hmdc, 0, 0, SRCCOPY);
//	DeleteDC(hmdc);
	//DeleteObject(hBitmap);

	return;
} 

void create_objects(HWND hWnd) {
	st_work_wnd.hbrush[OTE0_WHITE]		= CreateSolidBrush(RGB(255, 255, 255));
	st_work_wnd.hbrush[OTE0_GLAY]		= CreateSolidBrush(RGB(192, 192, 192));
	st_work_wnd.hbrush[OTE0_RED]		= CreateSolidBrush(RGB(255, 0, 0));
	st_work_wnd.hbrush[OTE0_BLUE]		= CreateSolidBrush(RGB(0, 0, 255));
	st_work_wnd.hbrush[OTE0_GREEN]		= CreateSolidBrush(RGB(0, 255, 0));
	st_work_wnd.hbrush[OTE0_YELLOW]		= CreateSolidBrush(RGB(255, 253, 85));
	st_work_wnd.hbrush[OTE0_MAZENDA]	= CreateSolidBrush(RGB(234, 63, 247));
	st_work_wnd.hbrush[OTE0_ORANGE]		= CreateSolidBrush(RGB(255, 142, 85));

	st_work_wnd.hpen[OTE0_WHITE] = CreatePen(PS_SOLID,2,RGB(255, 255, 255));
	st_work_wnd.hpen[OTE0_GLAY] = CreatePen(PS_SOLID, 2, RGB(192, 192, 192));
	st_work_wnd.hpen[OTE0_RED] = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	st_work_wnd.hpen[OTE0_BLUE] = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
	st_work_wnd.hpen[OTE0_GREEN] = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
	st_work_wnd.hpen[OTE0_YELLOW] = CreatePen(PS_SOLID, 2, RGB(255, 253, 85));
	st_work_wnd.hpen[OTE0_MAZENDA] = CreatePen(PS_SOLID, 2, RGB(234, 63, 247));
	st_work_wnd.hpen[OTE0_ORANGE] = CreatePen(PS_SOLID, 2, RGB(255, 142, 85));

	HDC hdc = GetDC(hWnd);
	st_work_wnd.hBmap[ID_OTE_HBMAP_MEM0] = CreateCompatibleBitmap(hdc, st_work_wnd.area_w, st_work_wnd.area_h);
	st_work_wnd.hdc[ID_OTE_HBMAP_MEM0] = CreateCompatibleDC(hdc);
	SelectObject(st_work_wnd.hdc[ID_OTE_HBMAP_MEM0], st_work_wnd.hBmap[ID_OTE_HBMAP_MEM0]);

	st_work_wnd.hBmap[ID_OTE_HBMAP_MEM_IF] = CreateCompatibleBitmap(hdc, st_work_wnd.area_w, st_work_wnd.area_h);
	st_work_wnd.hdc[ID_OTE_HBMAP_MEM_IF] = CreateCompatibleDC(hdc);
	SelectObject(st_work_wnd.hdc[ID_OTE_HBMAP_MEM_IF], st_work_wnd.hBmap[ID_OTE_HBMAP_MEM_IF]);

	st_work_wnd.hBmap[ID_OTE_HBMAP_MEM_GR] = CreateCompatibleBitmap(hdc, st_work_wnd.area_w, st_work_wnd.area_h);
	st_work_wnd.hdc[ID_OTE_HBMAP_MEM_GR] = CreateCompatibleDC(hdc);
	SelectObject(st_work_wnd.hdc[ID_OTE_HBMAP_MEM_GR], st_work_wnd.hBmap[ID_OTE_HBMAP_MEM_GR]);

	PatBlt(st_work_wnd.hdc[ID_OTE_HBMAP_MEM0], 0, 0, st_work_wnd.area_w, st_work_wnd.area_h, WHITENESS);
	PatBlt(st_work_wnd.hdc[ID_OTE_HBMAP_MEM_IF], 0, 0, st_work_wnd.area_w, st_work_wnd.area_h, WHITENESS);
	PatBlt(st_work_wnd.hdc[ID_OTE_HBMAP_MEM_GR], 0, 0, st_work_wnd.area_w, st_work_wnd.area_h, WHITENESS);
	
	ReleaseDC(hWnd_work, hdc);

#if 0
	//デバイスコンテキスト
	HDC hdc = GetDC(hWnd_work);
	st_work_wnd.hBmap[ID_OTE_HBMAP_MEM0] = CreateCompatibleBitmap(hdc, st_work_wnd.area_w, st_work_wnd.area_h);
	st_work_wnd.hdc[ID_OTE_HDC_MEM0] = CreateCompatibleDC(hdc);
	// SelectObject(st_work_wnd.hdc[ID_OTE_HDC_MEM0], st_work_wnd.hBmap[ID_OTE_HBMAP_MEM0]);
	PatBlt(st_work_wnd.hdc[ID_OTE_HDC_MEM0], 0, 0, st_work_wnd.area_w, st_work_wnd.area_h, WHITENESS);

	st_work_wnd.hBmap[ID_OTE_HBMAP_MEM_IF] = CreateCompatibleBitmap(hdc, st_work_wnd.area_w, st_work_wnd.area_h);
	st_work_wnd.hdc[ID_OTE_HDC_MEM_IF] = CreateCompatibleDC(hdc);
	SelectObject(st_work_wnd.hdc[ID_OTE_HDC_MEM_IF], st_work_wnd.hBmap[ID_OTE_HBMAP_MEM_IF]);
	PatBlt(st_work_wnd.hdc[ID_OTE_HDC_MEM_IF], 0, 0, st_work_wnd.area_w, st_work_wnd.area_h, WHITENESS);
	TextOutW(st_work_wnd.hdc[ID_OTE_HDC_MEM_IF], 0, 0, L"<<Information>>", 15);

	ReleaseDC(hWnd_work, hdc);
#endif
}

void delete_objects(HWND hWnd) {
	for(int i=0;i< N_OTE_BRUSH;i++)	DeleteObject(st_work_wnd.hbrush[i]);
	for (int i = 0; i < N_OTE_PEN; i++)	DeleteObject(st_work_wnd.hpen[i]);
	for(int i = 0; i < N_OTE_HDC; i++) 	DeleteDC(st_work_wnd.hdc[i]);
	for (int i = 0; i < N_OTE_HBMAP; i++)	DeleteObject(st_work_wnd.hpen[i]);
}