// OTE0.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "OTE0.h"
#include "OTE0panel.h"
#include "CPsaMain.h"
#include "PLC_DEF.h"
#include "COTE0_GR.h"
#include "spec.h"
#include "CDio.h"

#include <windowsx.h>       //# コモンコントロール
#include <winsock2.h>
#include <iostream>
#include <iomanip>
#include <sstream>

//@ GDI+
#include <objbase.h>//gdiplus.hのコンパイルを通すために必要
#include <gdiplus.h>
using namespace Gdiplus;

//DIRECTINPUT
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>


#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

ST_OTE_WORK_WND st_work_wnd;
HWND hWnd_work;					//操作端末メインウィンドウハンドル
HWND hWnd_swy;					//振れウィンドウハンドル
HWND hWnd_sub[OTE0_N_SUB_WND];	//
HWND hwnd_current_subwnd = NULL;

COte* pCOte0;						//OTE0オブジェクト

//@GDI+
GdiplusStartupInput gdiSI;
ULONG_PTR           gdiToken;

//DIRECTINPUT
static LPDIRECTINPUT8 lpDI = NULL;			//!< DIRECTINPUT8のポインタ
static LPDIRECTINPUTDEVICE8 lpGamePad;		//!< DIRECTINPUTDEVICE8のポインタ
static DIJOYSTATE pad_data;					//!< DIRECTINPUTDEVICE8の状態読み込みバッファ

static INT16 disp_cnt=0;
static bool is_init_disp = true;

//IP CAMERA
static ST_IPCAM_SET st_ipcam[OTE0_N_IP_CAMERA];

static std::wostringstream msg_wos;
static ST_SPEC spec;

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);

HWND open_connect_Wnd(HWND hwnd);
HWND open_mode_Wnd(HWND hwnd);
HWND open_fault_Wnd(HWND hwnd);
HWND open_status_Wnd(HWND hwnd);
HWND open_auto_Wnd(HWND hwnd);
HWND open_swy_Wnd(HWND hwnd); 
HWND open_camera_Wnd(HWND hwnd,int id_cam);
HWND open_camera_Wnd2(HWND hwnd, int id_cam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndConnectProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndAutoProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndModeProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndFaultProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndStatusProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndSwyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndCamProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndCam2Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void set_OTE_panel_objects(HWND hWnd);			//OTEウィンドウ上へコントロール配置
void disp_msg_cnt();
void create_objects(HWND hWnd);					//DC,Bitmap,Brush,Pen削除
void delete_objects(HWND hWnd);					//DC,Bitmap,Brush,Pen生成

void set_lamp();								//ランプ指令セット
void draw_lamp(HDC hdc,bool is_init);			//ランプ描画

void draw_graphic();
void draw_info();
void init_graphic();

void draw_graphic_swy();
void draw_bk_swy();



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
		GdiplusStartup(&gdiToken, &gdiSI, NULL);

        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
	for(int i=0;i< OTE0_N_IMAGE;i++)	delete st_work_wnd.pimg[i];
	for (int i = 0; i < OTE0_N_GRAPHIC; i++)	delete st_work_wnd.pgraphic[i];

	GdiplusShutdown(gdiToken);
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

   //PBランプ領域セット
   for (int i = ID_OTE_PB_TEISHI; i <= ID_OTE_CHK_N3; i++) {
		RECT rc_add;
		rc_add.left = -PRM_OTE_DEF_LAMP_W; rc_add.top = 0; rc_add.right = PRM_OTE_DEF_LAMP_W; rc_add.bottom = PRM_OTE_DEF_LAMP_H;
		st_work_wnd.pb_rect[i].left = st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].x + rc_add.left;
		st_work_wnd.pb_rect[i].top = st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].y + rc_add.top;
		st_work_wnd.pb_rect[i].right = st_work_wnd.pb_rect[i].left + rc_add.right;
		st_work_wnd.pb_rect[i].bottom = st_work_wnd.pb_rect[i].top + rc_add.bottom;
   }
   for (int i = ID_OTE_CHK_ASET_MH; i <= ID_OTE_CHK_ASET_SL; i++) {
	   RECT rc_add;
	   rc_add.left = 0; rc_add.top = PRM_OTE_DEF_LAMP_H; rc_add.right = PRM_OTE_DEF_PB_W; rc_add.bottom = PRM_OTE_DEF_LAMP_H/2;
	   st_work_wnd.pb_rect[i].left = st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].x + rc_add.left;
	   st_work_wnd.pb_rect[i].top = st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].y + rc_add.top;
	   st_work_wnd.pb_rect[i].right = st_work_wnd.pb_rect[i].left + rc_add.right;
	   st_work_wnd.pb_rect[i].bottom = st_work_wnd.pb_rect[i].top + rc_add.bottom;
   }

   //ノッチランプ領域セット
   for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
	   RECT rc_add;
		if ((i == ID_SLEW)||(i == ID_GANTRY)) {
		   rc_add.left = 0; rc_add.top = -PRM_OTE_DEF_PB_H/2; rc_add.right = PRM_OTE_DEF_PB_W2; rc_add.bottom = PRM_OTE_DEF_PB_H/2;
	   }
	   else if (i == ID_OTE_GRIP_SWITCH) {
			rc_add.left = 0; rc_add.top = PRM_OTE_DEF_PB_H; rc_add.right = PRM_OTE_DEF_PB_W; rc_add.bottom = PRM_OTE_DEF_PB_H / 2;

	   }
	   else {
		   rc_add.left = -PRM_OTE_DEF_PB_W2/3; rc_add.top = 0; rc_add.right = PRM_OTE_DEF_PB_W2 / 3; rc_add.bottom = PRM_OTE_DEF_PB_H;
	   }
	   for (int j = 0; j < N_OTE_NOTCH_ARRAY; j++) {
		   if ((i == ID_OTE_GRIP_SWITCH) && (j > 3))continue;
		   st_work_wnd.notch_rect[i][j].left = st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][i * N_OTE_NOTCH_ARRAY + j].x + rc_add.left;
		   st_work_wnd.notch_rect[i][j].top = st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][i * N_OTE_NOTCH_ARRAY + j].y + rc_add.top;
		   st_work_wnd.notch_rect[i][j].right = st_work_wnd.notch_rect[i][j].left + rc_add.right;
		   st_work_wnd.notch_rect[i][j].bottom = st_work_wnd.notch_rect[i][j].top + rc_add.bottom;
	   }
   }

   for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
	   if (i == ID_OTE_GRIP_SWITCH)continue;
	   st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][i] = st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_TRIG][i] = ID_OTE_0NOTCH_POS;
   }
   
    //UIオブジェクト生成
   create_objects(hWnd_work);

    // OTE0オブジェクトインスタンス化
   pCOte0 = new COte(hWnd_work);                              // メイン処理クラスのインスタンス化
   pCOte0->init_proc(&st_work_wnd);
      
   ShowWindow(hWnd_work, nCmdShow);
   UpdateWindow(hWnd_work);

   //MODEスイッチ初期値セット
	st_work_wnd.mh_spd_mode = ID_OTE_RADIO_MHSPD_7;
    st_work_wnd.ah_spd_mode = ID_OTE_RADIO_AHSPD_14;
	st_work_wnd.bh_work_mode = ID_OTE_RADIO_BH_57;
	st_work_wnd.jib_chk_mode = ID_OTE_RADIO_JIB_NARROW;

	//Image 登録
	st_work_wnd.pimg[OTE0_GRID_JC_BODY] = new Image(L"C:/Users/SHI/source/repos/MHAC_H6G307_MHCC/Img/JC_BODY.png");
	st_work_wnd.pimg[OTE0_GRID_JC_JIB] = new Image(L"C:/Users/SHI/source/repos/MHAC_H6G307_MHCC/Img/JC_JIB.png");
	st_work_wnd.pimg[OTE0_GRID_JC_HOOK1] = new Image(L"C:/Users/SHI/source/repos/MHAC_H6G307_MHCC/Img/JC_HOOK1.png");
	st_work_wnd.pimg[OTE0_GRID_JC_HOOK2] = new Image(L"C:/Users/SHI/source/repos/MHAC_H6G307_MHCC/Img/JC_HOOK2.png");


	//DIRECTINPUT
	HRESULT ret = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&lpDI, NULL);
	if (FAILED(ret)) {
		// 作成に失敗
		return -1;
	}
	// IDirectInputDevice8の取得
	ret = lpDI->CreateDevice(GUID_Joystick, &lpGamePad, NULL);
	if (FAILED(ret)) {
		lpDI->Release();
		return -1;
	}

	// 入力データ形式のセット
	ret = lpGamePad->SetDataFormat(&c_dfDIJoystick);
	if (FAILED(ret)) {
		lpGamePad->Release();
		lpDI->Release();
		return -1;
	}

#if 0
	// 排他制御のセット
	ret = lpGamePad->SetCooperativeLevel(hWnd_work, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(ret)) {
		lpGamePad->Release();
		lpDI->Release();
		return -1;
	}
#endif
	// 動作開始
	lpGamePad->Acquire();


	//IP CAMERA
	for (int i = 0; i < OTE0_N_IP_CAMERA; i++) {
		st_ipcam[i].hwnd = NULL;
		st_ipcam[i].pPSA = NULL;
		st_ipcam[i].icam = OTE_CAMERA_ID_NA;
	}

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
static BYTE gmpad_PB_last[32];
static DWORD gmpad_POV_last[4];
static INT auto_mode_last;
static INT gpad_mode_last;
static short dio_id;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HDC hdc;

	HINSTANCE hInst = GetModuleHandle(0);
	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化

		char DeviceName[100]= OTE_DIO_NAME;
		DioInit(DeviceName, &dio_id);

		//ウィンドウにコントロール追加
		set_OTE_panel_objects(hWnd);
		//サブウィンドウ追加
		hwnd_current_subwnd = open_fault_Wnd(hWnd);			//故障表示子ウィンドウ

		//マルチキャストタイマ起動 
		SetTimer(hWnd, ID_OTE_MULTICAST_TIMER, OTE_MULTICAST_SCAN_MS, NULL);
		//ユニキャストタイマ起動
		SetTimer(hWnd, ID_OTE_UNICAST_TIMER, OTE_UNICAST_SCAN_MS, NULL);

	}break;
	case WM_TIMER: {

		pCOte0->parse();//受信データ展開

		pCOte0->parse_auto_status();

		//目標位置クリアセット
		if (pCOte0->data.auto_mode == OTE_ID_AUTOSTAT_OFF) {
			pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_HOIST] = pCOte0->st_msg_pc_u_rcv.body.pos[ID_HOIST];
			pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_BOOM_H] = pCOte0->st_msg_pc_u_rcv.body.pos[ID_BOOM_H];
			pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_SLEW] = pCOte0->st_msg_pc_u_rcv.body.pos[ID_SLEW];
			pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_AHOIST] = pCOte0->st_msg_pc_u_rcv.body.pos[ID_AHOIST];
			pCOte0->data.auto_sel[ID_HOIST] = L_OFF;
			pCOte0->data.auto_sel[ID_AHOIST] = L_OFF;
			pCOte0->data.auto_sel[ID_BOOM_H] = L_OFF;
			pCOte0->data.auto_sel[ID_SLEW] = L_OFF;
		}
		else {
			if (pCOte0->data.auto_sel[ID_HOIST] == L_OFF) {
				pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_HOIST] = pCOte0->st_msg_pc_u_rcv.body.pos[ID_HOIST];
			}
			if (pCOte0->data.auto_sel[ID_AHOIST] == L_OFF) {
				pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_AHOIST] = pCOte0->st_msg_pc_u_rcv.body.pos[ID_AHOIST];
			}
			if ((pCOte0->data.auto_sel[ID_BOOM_H] == L_OFF) || (pCOte0->data.auto_sel[ID_SLEW] == L_OFF)) {
				pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_BOOM_H] = pCOte0->st_msg_pc_u_rcv.body.pos[ID_BOOM_H];
				pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_SLEW] = pCOte0->st_msg_pc_u_rcv.body.pos[ID_SLEW];

			}
		}

		//グラフィック目標位置セット
		pCOte0->data.pt_tgpos[OTE_ID_HOT_TARGET][ID_HOIST] = pCOte0->cal_gr_pos_from_d_pos(ID_HOIST, pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_HOIST], 0.0);
		pCOte0->data.pt_tgpos[OTE_ID_HOT_TARGET][ID_AHOIST] = pCOte0->cal_gr_pos_from_d_pos(ID_AHOIST, pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_AHOIST], 0.0);
		pCOte0->data.pt_tgpos[OTE_ID_HOT_TARGET][ID_BOOM_H] = pCOte0->data.pt_tgpos[OTE_ID_HOT_TARGET][ID_SLEW]
			= pCOte0->cal_gr_pos_from_d_pos(ID_BOOM_H, pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_BOOM_H], pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_SLEW]);


		//自動軸選択チェックボタン解除
		if ((!pCOte0->data.auto_sel[ID_HOIST]) && (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_ASET_MH], BM_GETCHECK, 0, 0))) {
			SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_ASET_MH], BM_SETCHECK, BST_UNCHECKED, 0);
		}
		if ((!pCOte0->data.auto_sel[ID_AHOIST]) && (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_ASET_AH], BM_GETCHECK, 0, 0))) {
			SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_ASET_AH], BM_SETCHECK, BST_UNCHECKED, 0);
		}
		if ((!pCOte0->data.auto_sel[ID_BOOM_H]) && (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_ASET_BH], BM_GETCHECK, 0, 0))) {
			SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_ASET_BH], BM_SETCHECK, BST_UNCHECKED, 0);
		}
		if ((!pCOte0->data.auto_sel[ID_SLEW]) && (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_ASET_SL], BM_GETCHECK, 0, 0))) {
			SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_ASET_SL], BM_SETCHECK, BST_UNCHECKED, 0);
		}


		if (pCOte0->data.auto_mode != auto_mode_last) {
			if (pCOte0->data.auto_mode == OTE_ID_AUTOSTAT_OFF) {
				for (int i = 0; i < 9; i++) SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_HOIST * N_OTE_NOTCH_ARRAY + i], st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][ID_HOIST * N_OTE_NOTCH_ARRAY + i]);
				for (int i = 0; i < 9; i++) SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_BOOM_H * N_OTE_NOTCH_ARRAY + i], st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][ID_BOOM_H * N_OTE_NOTCH_ARRAY + i]);
				for (int i = 0; i < 9; i++) SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_SLEW * N_OTE_NOTCH_ARRAY + i], st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][ID_SLEW * N_OTE_NOTCH_ARRAY + i]);
				for (int i = 0; i < 9; i++) SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_AHOIST * N_OTE_NOTCH_ARRAY + i], st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][ID_AHOIST * N_OTE_NOTCH_ARRAY + i]);
				for (int i = 0; i < 9; i++) SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_GANTRY * N_OTE_NOTCH_ARRAY + i], st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][ID_GANTRY * N_OTE_NOTCH_ARRAY + i]);
			}
			else {
				for (int i = 0; i < 9;i++) SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_HOIST* N_OTE_NOTCH_ARRAY + i], st_work_wnd.notch_auto_text[ID_HOIST][i]);
				for (int i = 0; i < 9; i++) SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_BOOM_H * N_OTE_NOTCH_ARRAY + i], st_work_wnd.notch_auto_text[ID_BOOM_H][i]);
				for (int i = 0; i < 9; i++) SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_SLEW * N_OTE_NOTCH_ARRAY + i], st_work_wnd.notch_auto_text[ID_SLEW][i]);
				for (int i = 0; i < 9; i++) SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_AHOIST * N_OTE_NOTCH_ARRAY + i], st_work_wnd.notch_auto_text[ID_AHOIST][i]);
				for (int i = 0; i < 9; i++) SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_GANTRY * N_OTE_NOTCH_ARRAY + i], st_work_wnd.notch_auto_text[ID_GANTRY][i]);

			}
			 auto_mode_last = pCOte0->data.auto_mode;
		}

		if (wParam == ID_OTE_UNICAST_TIMER) {
			//ON PAINT　呼び出し　表示更新

			//グラフィック表示エリア
			RECT rc = { OTE0_WND_X,OTE0_GR_AREA_Y,OTE0_GR_AREA_X + OTE0_GR_AREA_W+20 ,OTE0_GR_AREA_Y + OTE0_GR_AREA_H };
			InvalidateRect(hWnd, &rc, FALSE);
			rc.top -= 35; rc.bottom = OTE0_GR_AREA_Y;
			InvalidateRect(hWnd, &rc, FALSE);

			rc = { OTE0_GR_AREA2_X,OTE0_GR_AREA_Y,OTE0_WND_X + OTE0_WND_W ,OTE0_GR_AREA_Y + OTE0_GR_AREA_H };
			InvalidateRect(hWnd, &rc, FALSE);


			//情報表示エリア+カメラエリア
			rc.left = OTE0_CAM2_WND_X; rc.right= 810; rc.top = 0; rc.bottom = 300;
			InvalidateRect(hWnd, &rc, FALSE);

			//ボタンエリア
			rc.left = 800; rc.right = 960; rc.top = 0; rc.bottom = 300;
			InvalidateRect(hWnd, &rc, FALSE);
				//######
		}

		if (wParam == ID_OTE_MULTICAST_TIMER) {
			if (S_OK == pCOte0->snd_ote_m_pc(pCOte0->set_msg_ote_m())) {//OTEマルチキャスト送信
				pCOte0->cnt_snd_ote_m_pc++;
			}
			if (S_OK == pCOte0->snd_ote_m_ote(pCOte0->set_msg_ote_m())) {//OTEマルチキャスト送信
				pCOte0->cnt_snd_ote_m_ote++;
			}
		}

		if (wParam == ID_OTE_UNICAST_TIMER) {
			if (S_OK == pCOte0->snd_ote_u_pc(pCOte0->set_msg_ote_u(), &pCOte0->addrin_ote_u_snd)) {//OTEユニキャスト送信
				pCOte0->cnt_snd_ote_u++;
			}
		}

		//操作入力状態取り込み更新（NOTCHエリアグリップスイッチetc）

		//グリップスイッチ DIO入力
		long dio_ret = DioInpByte(dio_id, 0, &pCOte0->data.grip_stat.b[0]);
		dio_ret = DioInpByte(dio_id, 1, &pCOte0->data.grip_stat.b[1]);

		if (pCOte0->data.grip_stat.b[0] & OTE_GRIP_ENABLE) {
			//緊急停止
			if (!(pCOte0->data.grip_stat.b[0] & OTE_GRIP_ESTP)) {
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_OTE_GRIP_SWITCH] |= 0x0001;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_ESTOP], BM_SETCHECK, BST_CHECKED, 0);
			}
			else {
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_OTE_GRIP_SWITCH] |= ~0x0001;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_ESTOP], BM_SETCHECK, BST_UNCHECKED, 0);
			}

			if (pCOte0->data.grip_stat.b[0] & OTE_GRIP_ACTIVE) {	//グリップ入力
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_OTE_GRIP_SWITCH] |= 0x0002;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_NOTCH], BM_SETCHECK, BST_CHECKED, 0);
			}
			else {
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_OTE_GRIP_SWITCH] &= ~0x0002;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_NOTCH], BM_SETCHECK, BST_UNCHECKED, 0);
			}

			pCOte0->data.grip_stat.b[0] &= ~OTE_GRIP_DBG_ENABLE;
		}
		else {
			pCOte0->data.grip_stat.b[0] |= OTE_GRIP_DBG_ENABLE;
			//緊急停止

			if (BST_UNCHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_ESTOP], BM_GETCHECK, 0, 0)){
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_OTE_GRIP_SWITCH] &= ~0x0001;
				pCOte0->data.grip_stat.b[0] &= ~OTE_GRIP_ESTP;
			}
			else {
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_OTE_GRIP_SWITCH] |= 0x0001;
				pCOte0->data.grip_stat.b[0] |= OTE_GRIP_ESTP;
			}

			//グリップ入力
			if ((BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_NOTCH], BM_GETCHECK, 0, 0))) {
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_OTE_GRIP_SWITCH] |= 0x0002;
				pCOte0->data.grip_stat.b[0] |= OTE_GRIP_ACTIVE;
			}
			else {
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_OTE_GRIP_SWITCH] &= ~0x0002;
				pCOte0->data.grip_stat.b[0] &= ~OTE_GRIP_ACTIVE;
			}
		}
#if 0
		for (int i = 0; i < N_OTE_NOTCH_ARRAY; i++) {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_ESTOP + i], BM_GETCHECK, 0, 0)) {
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_OTE_GRIP_SWITCH] |= mask;
				st_work_wnd.notch_pb_stat[ID_OTE_GRIP_ESTOP + i] = OTE0_PB_OFF_DELAY_COUNT;

				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_OTE_GRIP_SWITCH] |= mask;

			}
			else {
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_OTE_GRIP_SWITCH] &= ~mask;
				st_work_wnd.notch_pb_stat[ID_OTE_GRIP_ESTOP + i] = L_OFF;

				if (i == (ID_OTE_GRIP_PAD_MODE - ID_OTE_GRIP_ESTOP)) pCOte0->data.gpad_mode = L_OFF;
			}
			mask=mask << 1;

			gpad_mode_last = pCOte0->data.gpad_mode;
		}
#endif
		//リモート無効時
		if (BST_CHECKED != SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_RMT], BM_GETCHECK, 0, 0)) {
			pCOte0->data.ope_mode == OTE_ID_OPE_MODE_MONITOR;
			for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
				if (i == ID_OTE_GRIP_SWITCH)continue;//	グリップスイッチは対象外
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][i] = st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_TRIG][i] = ID_OTE_0NOTCH_POS;	//0ノッチで初期化
			}
		}
		else {
			pCOte0->data.ope_mode |= OTE_ID_OPE_MODE_COMMAND;
		}
		
		//PB Stat 更新　PBはカウントダウン
		{
			//メインパネル
			for (int i = ID_OTE_PB_TEISHI; i <= ID_OTE_PB_FUREDOME; i++) {
				if (st_work_wnd.pb_stat[i] > 0)st_work_wnd.pb_stat[i]--;
			}
			//故障サブウィンドウ
			if (st_work_wnd.pb_stat[ID_OTE_PB_FLT_RESET] > 0)st_work_wnd.pb_stat[ID_OTE_PB_FLT_RESET]--;

			//状態サブウィンドウ

			//モードウィンドウ
			for (int i = ID_OTE_RADIO_MHSPD_7; i <= ID_OTE_RADIO_MHSPD_21; i++) {
				if (i == st_work_wnd.mh_spd_mode) st_work_wnd.pb_stat[i] = OTE0_PB_OFF_DELAY_COUNT;
				else st_work_wnd.pb_stat[i] = L_OFF;
			}
			for (int i = ID_OTE_RADIO_AHSPD_14; i <= ID_OTE_RADIO_AHSPD_24; i++) {
				if (i == st_work_wnd.ah_spd_mode) st_work_wnd.pb_stat[i] = OTE0_PB_OFF_DELAY_COUNT;
				else st_work_wnd.pb_stat[i] = L_OFF;
			}
			for (int i = ID_OTE_RADIO_BH_57; i <= ID_OTE_RADIO_BH_REST; i++) {
				if (i == st_work_wnd.bh_work_mode) st_work_wnd.pb_stat[i] = OTE0_PB_OFF_DELAY_COUNT;
				else st_work_wnd.pb_stat[i] = L_OFF;
			}
			for (int i = ID_OTE_RADIO_JIB_NARROW; i <= ID_OTE_RADIO_JIB_WIDE; i++) {
				if (i == st_work_wnd.jib_chk_mode) st_work_wnd.pb_stat[i] = OTE0_PB_OFF_DELAY_COUNT;
				else st_work_wnd.pb_stat[i] = L_OFF;
			}

		}

		//半自動目標CHK更新（CHK PBはONでカウントアップ　OFFで0
		for (int i = ID_OTE_CHK_S1; i <= ID_OTE_CHK_N3; i++) {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][i], BM_GETCHECK, 0, 0))
				st_work_wnd.pb_stat[i]++;
			else 				st_work_wnd.pb_stat[i]=0;

		}

		//NOTCH RADIO POS COUNT 更新　トリガチェック用　PB ONでセットされた値をカウントダウン
		for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
			if (st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_CNT][i] > 0)st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_CNT][i] --;
			else st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_TRIG][i] = 0;
		}

		//サブウィンドウ情報 更新
		switch (st_work_wnd.subpanel_sel) {
		case ID_OTE_RADIO_COM: {
			switch (st_work_wnd.connect_wnd_item) {
			case ID_OTE_RADIO_SOCK_PU: {
				//受信アドレス
				msg_wos.str(L"");
				msg_wos  << L" PCIP:" << pCOte0->addrin_pc_u_from.sin_addr.S_un.S_un_b.s_b1 << L"." << pCOte0->addrin_pc_u_from.sin_addr.S_un.S_un_b.s_b2 << L"." << pCOte0->addrin_pc_u_from.sin_addr.S_un.S_un_b.s_b3 << L"." << pCOte0->addrin_pc_u_from.sin_addr.S_un.S_un_b.s_b4 << L": "
					<< htons(pCOte0->addrin_pc_u_from.sin_port)
					<< L" \n MyIP:" << pCOte0->addrin_pc_u_ote.sin_addr.S_un.S_un_b.s_b1 << L"." << pCOte0->addrin_pc_u_ote.sin_addr.S_un.S_un_b.s_b2 << L"." << pCOte0->addrin_pc_u_ote.sin_addr.S_un.S_un_b.s_b3 << L"." << pCOte0->addrin_pc_u_ote.sin_addr.S_un.S_un_b.s_b4 << L": "
					<< htons(pCOte0->addrin_pc_u_ote.sin_port);
				SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_ADDR], msg_wos.str().c_str());

				//ヘッダ部情報追加
				msg_wos.str(L"");
				msg_wos << L" HEADR :(PC) " << pCOte0->st_msg_pc_u_rcv.head.myid << L" (EVENT)" << pCOte0->st_msg_pc_u_rcv.head.code
					<< L" (IP)" << pCOte0->st_msg_pc_u_rcv.head.addr.sin_addr.S_un.S_un_b.s_b1 << L"." << pCOte0->st_msg_pc_u_rcv.head.addr.sin_addr.S_un.S_un_b.s_b2 << L"." << pCOte0->st_msg_pc_u_rcv.head.addr.sin_addr.S_un.S_un_b.s_b3 << L"." << pCOte0->st_msg_pc_u_rcv.head.addr.sin_addr.S_un.S_un_b.s_b4
					<< L" (PORT) " << htons(pCOte0->st_msg_pc_u_rcv.head.addr.sin_port)
					<< L"\n(COM)" << pCOte0->st_msg_pc_u_rcv.head.status << L"(OTE)" << pCOte0->st_msg_pc_u_rcv.head.tgid;
				SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEADR], msg_wos.str().c_str());

				msg_wos.str(L"");
				msg_wos << L" HEADS :(OTE) " << pCOte0->st_msg_ote_u_snd.head.myid << L" (EVENT)" << pCOte0->st_msg_ote_u_snd.head.code
					<< L" (IP)" << pCOte0->st_msg_ote_u_snd.head.addr.sin_addr.S_un.S_un_b.s_b1 << L"." << pCOte0->st_msg_ote_u_snd.head.addr.sin_addr.S_un.S_un_b.s_b2 << L"." << pCOte0->st_msg_ote_u_snd.head.addr.sin_addr.S_un.S_un_b.s_b3 << L"." << pCOte0->st_msg_ote_u_snd.head.addr.sin_addr.S_un.S_un_b.s_b4
					<< L" (PORT) " << htons(pCOte0->st_msg_ote_u_snd.head.addr.sin_port)
					<< L"\n(COM)" << pCOte0->st_msg_ote_u_snd.head.status << L"(PC)" << pCOte0->st_msg_ote_u_snd.head.tgid;
				SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEADS], msg_wos.str().c_str());
			}break;
			case ID_OTE_RADIO_SOCK_PM: {
				//受信アドレス
				msg_wos.str(L"");
				msg_wos << L" PCIP:" << pCOte0->addrin_pc_m_from.sin_addr.S_un.S_un_b.s_b1 << L"." << pCOte0->addrin_pc_m_from.sin_addr.S_un.S_un_b.s_b2 << L"." << pCOte0->addrin_pc_m_from.sin_addr.S_un.S_un_b.s_b3 << L"." << pCOte0->addrin_pc_m_from.sin_addr.S_un.S_un_b.s_b4 << L": "
					<< htons(pCOte0->addrin_pc_m_from.sin_port)
					<< L" \n MyIP:" << pCOte0->addrin_pc_m_rcv.sin_addr.S_un.S_un_b.s_b1 << L"." << pCOte0->addrin_pc_m_rcv.sin_addr.S_un.S_un_b.s_b2 << L"." << pCOte0->addrin_pc_m_rcv.sin_addr.S_un.S_un_b.s_b3 << L"." << pCOte0->addrin_pc_m_rcv.sin_addr.S_un.S_un_b.s_b4 << L": "
					<< htons(pCOte0->addrin_pc_m_ote.sin_port);
				SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_ADDR], msg_wos.str().c_str());

				//ヘッダ部情報追加
				msg_wos.str(L"");
				msg_wos << L" HEADR :(PC) " << pCOte0->st_msg_pc_m_ote_rcv.head.myid << L" (EVENT)" << pCOte0->st_msg_pc_m_ote_rcv.head.code
					<< L" (IP)" << pCOte0->st_msg_pc_m_ote_rcv.head.addr.sin_addr.S_un.S_un_b.s_b1 << L"." << pCOte0->st_msg_pc_m_ote_rcv.head.addr.sin_addr.S_un.S_un_b.s_b2 << L"." << pCOte0->st_msg_pc_m_ote_rcv.head.addr.sin_addr.S_un.S_un_b.s_b3 << L"." << pCOte0->st_msg_pc_m_ote_rcv.head.addr.sin_addr.S_un.S_un_b.s_b4
					<< L" (PORT) " << htons(pCOte0->st_msg_pc_m_ote_rcv.head.addr.sin_port)
					<< L"\n(COM)" << pCOte0->st_msg_pc_m_ote_rcv.head.status << L"(OTE)" << pCOte0->st_msg_pc_m_ote_rcv.head.tgid;
				SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEADR], msg_wos.str().c_str());

				msg_wos.str(L"");
				msg_wos << L" HEADS :(OTE) " << pCOte0->st_msg_ote_m_snd.head.myid << L" (EVENT)" << pCOte0->st_msg_ote_m_snd.head.code
					<< L" (IP)" << pCOte0->st_msg_ote_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b1 << L"." << pCOte0->st_msg_ote_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b2 << L"." << pCOte0->st_msg_ote_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b3 << L"." << pCOte0->st_msg_ote_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b4
					<< L" (PORT) " << htons(pCOte0->st_msg_ote_m_snd.head.addr.sin_port)
					<< L"\n(COM)" << pCOte0->st_msg_ote_m_snd.head.status << L"(PC)" << pCOte0->st_msg_ote_m_snd.head.tgid;
				SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEADS], msg_wos.str().c_str());

			}break;
			case ID_OTE_RADIO_SOCK_OM: {
				//受信アドレス
				msg_wos.str(L"");
				msg_wos << L" OTEIP:" << pCOte0->addrin_ote_m_from.sin_addr.S_un.S_un_b.s_b1 << L"." << pCOte0->addrin_ote_m_from.sin_addr.S_un.S_un_b.s_b2 << L"." << pCOte0->addrin_ote_m_from.sin_addr.S_un.S_un_b.s_b3 << L"." << pCOte0->addrin_ote_m_from.sin_addr.S_un.S_un_b.s_b4 << L": "
					<< htons(pCOte0->addrin_ote_m_from.sin_port)
					<< L" \n MyIP:" << pCOte0->addrin_ote_m_rcv.sin_addr.S_un.S_un_b.s_b1 << L"." << pCOte0->addrin_ote_m_rcv.sin_addr.S_un.S_un_b.s_b2 << L"." << pCOte0->addrin_ote_m_rcv.sin_addr.S_un.S_un_b.s_b3 << L"." << pCOte0->addrin_ote_m_rcv.sin_addr.S_un.S_un_b.s_b4 << L": "
					<< htons(pCOte0->addrin_ote_m_ote.sin_port);
				SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_ADDR], msg_wos.str().c_str());

				//ヘッダ部情報追加
				msg_wos.str(L"");
				msg_wos << L" HEADR :(OTE) " << pCOte0->st_msg_ote_m_ote_rcv.head.myid << L" (EVENT)" << pCOte0->st_msg_ote_m_ote_rcv.head.code
					<< L" (IP)" << pCOte0->st_msg_ote_m_ote_rcv.head.addr.sin_addr.S_un.S_un_b.s_b1 << L"." << pCOte0->st_msg_ote_m_ote_rcv.head.addr.sin_addr.S_un.S_un_b.s_b2 << L"." << pCOte0->st_msg_ote_m_ote_rcv.head.addr.sin_addr.S_un.S_un_b.s_b3 << L"." << pCOte0->st_msg_ote_m_ote_rcv.head.addr.sin_addr.S_un.S_un_b.s_b4
					<< L" (PORT) " << htons(pCOte0->st_msg_ote_m_ote_rcv.head.addr.sin_port)
					<< L"\n(COM)" << pCOte0->st_msg_ote_m_ote_rcv.head.status << L"(OTE)" << pCOte0->st_msg_ote_m_ote_rcv.head.tgid;
				SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEADR], msg_wos.str().c_str());

				msg_wos.str(L"");
				msg_wos << L" HEADS :(OTE) " << pCOte0->st_msg_ote_m_snd.head.myid << L" (EVENT)" << pCOte0->st_msg_ote_m_snd.head.code
					<< L" (IP)" << pCOte0->st_msg_ote_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b1 << L"." << pCOte0->st_msg_ote_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b2 << L"." << pCOte0->st_msg_ote_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b3 << L"." << pCOte0->st_msg_ote_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b4
					<< L" (PORT) " << htons(pCOte0->st_msg_ote_m_snd.head.addr.sin_port)
					<< L"\n(COM)" << pCOte0->st_msg_ote_m_snd.head.status << L"(PC)" << pCOte0->st_msg_ote_m_snd.head.tgid;
				SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEADS], msg_wos.str().c_str());
			}break;
			default:break;
			}

		}break;
		case ID_OTE_RADIO_MODE: {
		}break;
		case ID_OTE_RADIO_FAULT: {
			}break;
		case ID_OTE_RADIO_STAT: {
		}break;
		default:break;
		}

		disp_cnt++;//表示更新用カウンタ（STATIC　Lampフリッカ）
		disp_msg_cnt();	//通信カウント表示更新

		//サブウィンドウにSTATIC表示更新メッセージ送信
		if (disp_cnt & OTE_CODE_SUB_STATIC_FREQ) {
			SendMessage(hwnd_current_subwnd, ID_OTE0_STATIC_UPDATE, 0L, 0L);
		}

		//ゲームパッド
		{
			if (lpGamePad != NULL) {
				HRESULT hr = lpGamePad->GetDeviceState(sizeof(DIJOYSTATE), &pad_data);
				if (FAILED(hr)) {
					lpGamePad->Acquire();
					lpGamePad->GetDeviceState(sizeof(DIJOYSTATE), &pad_data);
				}
			}

			//Game pad入力
			//主幹PB
			if (pad_data.rgbButtons[4]) st_work_wnd.pb_stat[ID_OTE_PB_SYUKAN] = OTE0_PB_OFF_DELAY_COUNT;
			//故障リセットPB
			if (pad_data.rgbButtons[5]) st_work_wnd.pb_stat[ID_OTE_PB_FLT_RESET] = OTE0_PB_OFF_DELAY_COUNT;

			if (pad_data.rgbButtons[3] && !gmpad_PB_last[3]) {
				if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_WND], BM_GETCHECK, 0, 0)) {

					if (st_ipcam[OTE_CAMERA_WND_ID_BASE].hwnd != NULL) {
						st_ipcam[OTE_CAMERA_WND_ID_BASE].pPSA->LiveStop();
						st_ipcam[OTE_CAMERA_WND_ID_BASE].pPSA->OnClose();
						delete st_ipcam[OTE_CAMERA_WND_ID_BASE].pPSA;
						DestroyWindow(st_ipcam[OTE_CAMERA_WND_ID_BASE].hwnd);
					}

					if (st_ipcam[OTE_CAMERA_WND_ID_OPT1].hwnd != NULL) {
						st_ipcam[OTE_CAMERA_WND_ID_OPT1].pPSA->LiveStop();
						st_ipcam[OTE_CAMERA_WND_ID_OPT1].pPSA->OnClose();
						delete st_ipcam[OTE_CAMERA_WND_ID_OPT1].pPSA;
						DestroyWindow(st_ipcam[OTE_CAMERA_WND_ID_OPT1].hwnd);
					}

					SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_WND], BM_SETCHECK, BST_UNCHECKED, 0);
				}
				else {
					if (st_work_wnd.camera_sel == ID_OTE_RADIO_WIDE)open_camera_Wnd(hWnd, OTE_CAMERA_ID_PTZ0);
					else if (st_work_wnd.camera_sel == ID_OTE_RADIO_OPE1)open_camera_Wnd(hWnd, OTE_CAMERA_ID_FISH0);
					else if (st_work_wnd.camera_sel == ID_OTE_RADIO_HOOK)open_camera_Wnd(hWnd, OTE_CAMERA_ID_HOOK0);
					else open_camera_Wnd(hWnd, OTE_CAMERA_ID_PTZ0);

					open_camera_Wnd2(hWnd, OTE_CAMERA_ID_PTZ0);

					SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_WND], BM_SETCHECK, BST_CHECKED, 0);
				}
			}

			//カメラ切替PB

			if (pad_data.rgbButtons[0] && !gmpad_PB_last[0]) {
				SendMessage(st_ipcam[OTE_CAMERA_WND_ID_BASE].hwnd, OTE0_MSG_SWICH_CAMERA, 0, ID_OTE_RADIO_WIDE);
				SendMessage(st_ipcam[OTE_CAMERA_WND_ID_OPT1].hwnd, OTE0_MSG_SWICH_CAMERA, 0, ID_OTE_RADIO_WIDE);
			}
			if (pad_data.rgbButtons[1] && !gmpad_PB_last[1]) {
				SendMessage(st_ipcam[OTE_CAMERA_WND_ID_BASE].hwnd, OTE0_MSG_SWICH_CAMERA, 0, ID_OTE_RADIO_OPE1);
				SendMessage(st_ipcam[OTE_CAMERA_WND_ID_OPT1].hwnd, OTE0_MSG_SWICH_CAMERA, 0, ID_OTE_RADIO_OPE1);
			}

			//if (pad_data.rgbButtons[11]) 
			if (pCOte0->data.gpad_mode) {
				if ((pad_data.lRz < OTE0_GMPAD_NOTCH0_MIN) || (pad_data.lRz > OTE0_GMPAD_NOTCH0_MAX)) {
					st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_HOIST] = (INT16)((pad_data.lRz - OTE0_GMPAD_NOTCH0) / OTE0_GMPAD_NOTCH_PITCH) + ID_OTE_0NOTCH_POS;
				}
				else {
					st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_HOIST] = ID_OTE_0NOTCH_POS;
				}
				if ((pad_data.lZ < OTE0_GMPAD_NOTCH0_MIN) || (pad_data.lZ > OTE0_GMPAD_NOTCH0_MAX)) {
					st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_GANTRY] = (INT16)((pad_data.lZ - OTE0_GMPAD_NOTCH0) / OTE0_GMPAD_NOTCH_PITCH) + ID_OTE_0NOTCH_POS;
				}
				else {
					st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_GANTRY] = ID_OTE_0NOTCH_POS;
				}
			}
			//if (pad_data.rgbButtons[10]) {//ノッチPB　ONの時有効
			if (pCOte0->data.gpad_mode) {
				if ((pad_data.lY < OTE0_GMPAD_NOTCH0_MIN) || (pad_data.lY > OTE0_GMPAD_NOTCH0_MAX)) {
					st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_BOOM_H] = (INT16)((pad_data.lY - OTE0_GMPAD_NOTCH0) / OTE0_GMPAD_NOTCH_PITCH) + ID_OTE_0NOTCH_POS;
				}
				else {
					st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_BOOM_H] = ID_OTE_0NOTCH_POS;
				}
				if ((pad_data.lX < OTE0_GMPAD_NOTCH0_MIN) || (pad_data.lX > OTE0_GMPAD_NOTCH0_MAX)) {
					st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_SLEW] = (INT16)(-(pad_data.lX - OTE0_GMPAD_NOTCH0) / OTE0_GMPAD_NOTCH_PITCH) + ID_OTE_0NOTCH_POS;
				}
				else {
					st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_SLEW] = ID_OTE_0NOTCH_POS;
				}
			}
			if (gpad_mode_last != pCOte0->data.gpad_mode) {
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_HOIST] = st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_GANTRY] = ID_OTE_0NOTCH_POS;
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_BOOM_H] = st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_SLEW] = ID_OTE_0NOTCH_POS;
			}
			gpad_mode_last = pCOte0->data.gpad_mode;

			//カメラチルト、パン、ズーム
			if (st_ipcam[OTE_CAMERA_WND_ID_BASE].hwnd == NULL);							//表示カメラ未登録時はスルー
			else {

				CPsaMain* pPSA = st_ipcam[OTE_CAMERA_WND_ID_BASE].pPSA;

				//カメラズーム,チルト、パン
				if (pad_data.rgbButtons[6]) {
					pPSA->lzoom = DEF_SPD_CAM_ZOOM;//ZOOM N
				}
				else if (pad_data.rgbButtons[8]) {
					pPSA->lzoom = -DEF_SPD_CAM_ZOOM;//ZOOM W
				}
				else;
				if ((pad_data.rgbButtons[7] != gmpad_PB_last[7]) || (pad_data.rgbButtons[9] != gmpad_PB_last[9])) {
					pPSA->lzoom = 0;
				}
				else;

				if ((gmpad_POV_last[0] > 36000) && (pad_data.rgdwPOV[0] <= 36000)) {//入力ON時指令クリア
					pPSA->ltilt = pPSA->lpan = 0;
				}
				else if ((gmpad_POV_last[0] <= 36000) && (pad_data.rgdwPOV[0] > 36000)) {//入力OFF時指令クリア
					pPSA->ltilt = pPSA->lpan = 0;
				}
				else if (pad_data.rgdwPOV[0] < 1500) {
					pPSA->ltilt = DEF_SPD_CAM_TILT;	pPSA->lpan = 0;
				}
				else if (pad_data.rgdwPOV[0] < 7500) {
					pPSA->ltilt = DEF_SPD_CAM_TILT;	pPSA->lpan = DEF_SPD_CAM_PAN;
				}
				else if (pad_data.rgdwPOV[0] < 10500) {
					pPSA->ltilt = 0; pPSA->lpan = DEF_SPD_CAM_PAN;
				}
				else if (pad_data.rgdwPOV[0] < 16500) {
					pPSA->ltilt = -DEF_SPD_CAM_TILT; pPSA->lpan = DEF_SPD_CAM_PAN;
				}
				else if (pad_data.rgdwPOV[0] < 19500) {
					pPSA->ltilt = -DEF_SPD_CAM_TILT; pPSA->lpan = 0;
				}
				else if (pad_data.rgdwPOV[0] < 25500) {
					pPSA->ltilt = -DEF_SPD_CAM_TILT; pPSA->lpan = -DEF_SPD_CAM_PAN;
				}
				else if (pad_data.rgdwPOV[0] < 28500) {
					pPSA->ltilt = 0; pPSA->lpan = -DEF_SPD_CAM_PAN;
				}
				else if (pad_data.rgdwPOV[0] < 34500) {
					pPSA->ltilt = DEF_SPD_CAM_TILT;	pPSA->lpan = -DEF_SPD_CAM_PAN;
				}
				else if (pad_data.rgdwPOV[0] <= 36000) {
					pPSA->ltilt = DEF_SPD_CAM_TILT;	pPSA->lpan = 0;
				}
				else;

				//全方位カメラ
				if (pad_data.rgbButtons[12] > gmpad_PB_last[12]) {
					pPSA->m_psapi->SetCameraImageCap(20, 0);//魚眼天井
					pPSA->LiveStop(); Sleep(1000);
					pPSA->LiveStart();
				}
				else if (pad_data.rgbButtons[13] > gmpad_PB_last[13]) {

					pPSA->m_psapi->SetCameraImageCap(21, 1);//１PTZ壁
					pPSA->LiveStop(); Sleep(1000);
					pPSA->LiveStart();
				}
				else if (pad_data.rgbButtons[14] > gmpad_PB_last[14]) {
					pPSA->m_psapi->SetCameraImageCap(22, 0);//4PTZ天井
					pPSA->LiveStop(); Sleep(1000);
					pPSA->LiveStart();
				}
				else;

			}
		}

		//前回値保持
		for (int i = 0; i < 16; i++) gmpad_PB_last[i] = pad_data.rgbButtons[i];
		gmpad_POV_last[0] = pad_data.rgdwPOV[0];
			
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);

		//PB オフディレイカウントセット
		if ((wmId >= BASE_ID_OTE_PB + ID_OTE_PB_TEISHI) && (wmId <= BASE_ID_OTE_PB + ID_OTE_PB_FUREDOME)) {
			st_work_wnd.pb_stat[wmId - BASE_ID_OTE_PB] = OTE0_PB_OFF_DELAY_COUNT;
		}

		//NOTCH　RADIO PB
		if ((wmId >= ID_OTE_NOTCH_MH_MIN) && (wmId <= ID_OTE_NOTCH_AH_MAX)) {
			if (wmId < ID_OTE_NOTCH_MH_MAX) {
				if(pCOte0->data.auto_mode == OTE_ID_AUTOSTAT_OFF)
					st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_HOIST] = wmId - ID_OTE_NOTCH_MH_MIN;
				else 
					st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_HOIST] = ID_OTE_0NOTCH_POS;

				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_TRIG][ID_HOIST] = wmId - ID_OTE_NOTCH_MH_MIN;
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_CNT][ID_HOIST] = OTE0_PB_OFF_DELAY_COUNT;

				if ((pCOte0->data.auto_mode != OTE_ID_AUTOSTAT_OFF) && (pCOte0->data.auto_sel[ID_HOIST] == L_ON)) {
					pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_HOIST] += st_work_wnd.notch_auto_shift[ID_HOIST][st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_TRIG][ID_HOIST]];
				}
			}
			else if (wmId < ID_OTE_NOTCH_GT_MAX) {
				if (pCOte0->data.auto_mode == OTE_ID_AUTOSTAT_OFF)
					st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_GANTRY] = wmId - ID_OTE_NOTCH_GT_MIN;
				else
					st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_GANTRY] = ID_OTE_0NOTCH_POS;

				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_TRIG][ID_GANTRY] = wmId - ID_OTE_NOTCH_GT_MIN;
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_CNT][ID_GANTRY] = OTE0_PB_OFF_DELAY_COUNT;
			}
			else if (wmId < ID_OTE_NOTCH_GR_MAX) {//グリップノッチの非常停止とグリップはチェックボックスを定周期チェック
				//リモート
				if ((wmId - ID_OTE_NOTCH_MH_MIN) == ID_OTE_GRIP_RMT) {
					if (BST_UNCHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_RMT], BM_GETCHECK, 0, 0)) {
						pCOte0->data.ope_mode &= ~OTE_ID_OPE_MODE_COMMAND;
					}
					else {
						pCOte0->data.ope_mode |= OTE_ID_OPE_MODE_COMMAND;
					}
				}
				//ジョイパッドモード
				if ((wmId - ID_OTE_NOTCH_MH_MIN) == ID_OTE_GRIP_PAD_MODE) {
					if (BST_UNCHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_PAD_MODE], BM_GETCHECK, 0, 0)) {
						pCOte0->data.gpad_mode = L_OFF;
						pCOte0->data.ope_mode &= ~OTE_ID_OPE_MODE_GPAD;
					}
					else {
						pCOte0->data.gpad_mode = L_ON;
						pCOte0->data.ope_mode |= OTE_ID_OPE_MODE_GPAD;
					}
				}
			}
			else if (wmId < ID_OTE_NOTCH_BH_MAX) {
				if (pCOte0->data.auto_mode == OTE_ID_AUTOSTAT_OFF)
					st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_BOOM_H] = wmId - ID_OTE_NOTCH_BH_MIN;
				else
					st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_BOOM_H] = ID_OTE_0NOTCH_POS;
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_TRIG][ID_BOOM_H] = wmId - ID_OTE_NOTCH_BH_MIN;
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_CNT][ID_BOOM_H] = OTE0_PB_OFF_DELAY_COUNT;

				if ((pCOte0->data.auto_mode != OTE_ID_AUTOSTAT_OFF) && (pCOte0->data.auto_sel[ID_BOOM_H] == L_ON)) {
					pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_BOOM_H] += st_work_wnd.notch_auto_shift[ID_BOOM_H][st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_TRIG][ID_BOOM_H]];
				}
			}
			else if (wmId < ID_OTE_NOTCH_SL_MAX) {
				if (pCOte0->data.auto_mode == OTE_ID_AUTOSTAT_OFF)
					st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_SLEW] = wmId - ID_OTE_NOTCH_SL_MIN;
				else
					st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_SLEW] = ID_OTE_0NOTCH_POS;

				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_TRIG][ID_SLEW] = wmId - ID_OTE_NOTCH_SL_MIN;
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_CNT][ID_SLEW] = OTE0_PB_OFF_DELAY_COUNT;

				if ((pCOte0->data.auto_mode != OTE_ID_AUTOSTAT_OFF) && (pCOte0->data.auto_sel[ID_SLEW] == L_ON)) {
					pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_SLEW] += st_work_wnd.notch_auto_shift[ID_SLEW][st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_TRIG][ID_SLEW]];
				}
			}
			else {
				if (pCOte0->data.auto_mode == OTE_ID_AUTOSTAT_OFF)
					st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_AHOIST] = wmId - ID_OTE_NOTCH_AH_MIN;
				else
					st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_AHOIST] = ID_OTE_0NOTCH_POS;

				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_TRIG][ID_AHOIST] = wmId - ID_OTE_NOTCH_AH_MIN;
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_CNT][ID_AHOIST] = OTE0_PB_OFF_DELAY_COUNT;

				if ((pCOte0->data.auto_mode != OTE_ID_AUTOSTAT_OFF) && (pCOte0->data.auto_sel[ID_AHOIST] == L_ON)) {
					pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_AHOIST] += st_work_wnd.notch_auto_shift[ID_SLEW][st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_TRIG][ID_AHOIST]];
				}
			}
		}
		//SUB WINDOW選択　RADIO PB
		if ((wmId >= BASE_ID_OTE_PB + ID_OTE_RADIO_COM) && (wmId <= BASE_ID_OTE_PB + ID_OTE_RADIO_SWAY)) {
			DestroyWindow(hwnd_current_subwnd);
			switch (st_work_wnd.subpanel_sel = wmId - BASE_ID_OTE_PB) {
			case ID_OTE_RADIO_COM:hwnd_current_subwnd = open_connect_Wnd(hWnd); break;
			case ID_OTE_RADIO_MODE:hwnd_current_subwnd = open_mode_Wnd(hWnd); break;
			case ID_OTE_RADIO_FAULT:hwnd_current_subwnd = open_fault_Wnd(hWnd); break;
			case ID_OTE_RADIO_STAT:hwnd_current_subwnd = open_status_Wnd(hWnd); break;
			case ID_OTE_RADIO_SWAY:hwnd_current_subwnd = open_swy_Wnd(hWnd); break;
			default:break;
			}
		}
		//表示カメラ選択　RADIO PB
		if ((wmId >= BASE_ID_OTE_PB + ID_OTE_RADIO_WIDE) && (wmId <= BASE_ID_OTE_PB + ID_OTE_RADIO_OPE2)) {
			if (st_ipcam[OTE_CAMERA_WND_ID_BASE].hwnd != NULL) {
				SendMessage(st_ipcam[OTE_CAMERA_WND_ID_BASE].hwnd, OTE0_MSG_SWICH_CAMERA,0, wmId - BASE_ID_OTE_PB);
				SendMessage(st_ipcam[OTE_CAMERA_WND_ID_OPT1].hwnd, OTE0_MSG_SWICH_CAMERA, 0, wmId - BASE_ID_OTE_PB);
			}
			st_work_wnd.camera_sel = wmId - BASE_ID_OTE_PB;
			break;
		}

		switch (wmId)
		{
		//自動選択
		case BASE_ID_OTE_PB + ID_OTE_PB_ARESET_ALL: {
			for (int i = ID_OTE_CHK_ASET_MH; i <= ID_OTE_CHK_ASET_SL; i++) {
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][i], BM_SETCHECK, BST_UNCHECKED, 0L);
			
				st_work_wnd.pb_lamp[i] .com= L_OFF;	//設定OFF　
				for (int i = 0; i < MOTION_ID_MAX; i++){
					pCOte0->data.auto_sel[ID_HOIST] = OTE_ID_AUTOSTAT_OFF;
				}
			}
			break;
		}
		case BASE_ID_OTE_PB + ID_OTE_CHK_ASET_MH: {
			if ((BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][wmId - BASE_ID_OTE_PB], BM_GETCHECK, 0, 0)&&
				(pCOte0->data.auto_mode == OTE_ID_AUTOSTAT_STANDBY))) {
				st_work_wnd.pb_lamp[wmId - BASE_ID_OTE_PB].com = L_ON;
				pCOte0->data.auto_sel[ID_HOIST] = OTE_ID_AUTOSTAT_STANDBY;
			}
			else {
				st_work_wnd.pb_lamp[wmId - BASE_ID_OTE_PB].com = L_OFF;
				pCOte0->data.auto_sel[ID_HOIST] = OTE_ID_AUTOSTAT_OFF;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][wmId - BASE_ID_OTE_PB], BM_SETCHECK, BST_UNCHECKED, 0);
			}
		}break;
		case BASE_ID_OTE_PB + ID_OTE_CHK_ASET_AH: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][wmId - BASE_ID_OTE_PB], BM_GETCHECK, 0, 0) &&
				(pCOte0->data.auto_mode == OTE_ID_AUTOSTAT_STANDBY)) {
				st_work_wnd.pb_lamp[wmId - BASE_ID_OTE_PB].com = L_ON;
				pCOte0->data.auto_sel[ID_AHOIST] = OTE_ID_AUTOSTAT_STANDBY;
			}
			else {
				st_work_wnd.pb_lamp[wmId - BASE_ID_OTE_PB].com = L_OFF;
				pCOte0->data.auto_sel[ID_AHOIST] = OTE_ID_AUTOSTAT_OFF;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][wmId - BASE_ID_OTE_PB], BM_SETCHECK, BST_UNCHECKED, 0);
			}
		}break;
		case BASE_ID_OTE_PB + ID_OTE_CHK_ASET_BH: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][wmId - BASE_ID_OTE_PB], BM_GETCHECK, 0, 0) &&
				(pCOte0->data.auto_mode == OTE_ID_AUTOSTAT_STANDBY)) {
				st_work_wnd.pb_lamp[wmId - BASE_ID_OTE_PB].com = L_ON;
				pCOte0->data.auto_sel[ID_BOOM_H] = OTE_ID_AUTOSTAT_STANDBY;
			}
			else {
				st_work_wnd.pb_lamp[wmId - BASE_ID_OTE_PB].com = L_OFF;
				pCOte0->data.auto_sel[ID_BOOM_H] = OTE_ID_AUTOSTAT_OFF;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][wmId - BASE_ID_OTE_PB], BM_SETCHECK, BST_UNCHECKED, 0);
			}
		}break;
		case BASE_ID_OTE_PB + ID_OTE_CHK_ASET_SL: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][wmId - BASE_ID_OTE_PB], BM_GETCHECK, 0, 0) &&
				(pCOte0->data.auto_mode == OTE_ID_AUTOSTAT_STANDBY)) {
				st_work_wnd.pb_lamp[wmId - BASE_ID_OTE_PB].com = L_ON;
				pCOte0->data.auto_sel[ID_SLEW] = OTE_ID_AUTOSTAT_STANDBY;
			}
			else {
				st_work_wnd.pb_lamp[wmId - BASE_ID_OTE_PB].com = L_OFF;
				pCOte0->data.auto_sel[ID_SLEW] = OTE_ID_AUTOSTAT_OFF;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][wmId - BASE_ID_OTE_PB], BM_SETCHECK, BST_UNCHECKED, 0);
			}
		}break;

		//カメラウィンドウ表示
		case BASE_ID_OTE_PB + ID_OTE_CHK_CAMERA_WND: {
			if (BST_UNCHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][wmId - BASE_ID_OTE_PB], BM_GETCHECK, 0, 0)) {
				
				if (st_ipcam[OTE_CAMERA_WND_ID_BASE].hwnd != NULL) {
					st_ipcam[OTE_CAMERA_WND_ID_BASE].pPSA->LiveStop();
					st_ipcam[OTE_CAMERA_WND_ID_BASE].pPSA->OnClose();
					delete st_ipcam[OTE_CAMERA_WND_ID_BASE].pPSA;
					DestroyWindow(st_ipcam[OTE_CAMERA_WND_ID_BASE].hwnd);
				}

				if (st_ipcam[OTE_CAMERA_WND_ID_OPT1].hwnd != NULL) {
					st_ipcam[OTE_CAMERA_WND_ID_OPT1].pPSA->LiveStop();
					st_ipcam[OTE_CAMERA_WND_ID_OPT1].pPSA->OnClose();
					delete st_ipcam[OTE_CAMERA_WND_ID_OPT1].pPSA;
					DestroyWindow(st_ipcam[OTE_CAMERA_WND_ID_OPT1].hwnd);
				}
			}
			else {
				if (st_work_wnd.camera_sel == ID_OTE_RADIO_WIDE)open_camera_Wnd(hWnd, OTE_CAMERA_ID_PTZ0);
				else if (st_work_wnd.camera_sel == ID_OTE_RADIO_OPE1)open_camera_Wnd(hWnd, OTE_CAMERA_ID_FISH0);
				else if (st_work_wnd.camera_sel == ID_OTE_RADIO_HOOK)open_camera_Wnd(hWnd, OTE_CAMERA_ID_HOOK0);
				else open_camera_Wnd(hWnd, OTE_CAMERA_ID_PTZ0);

				open_camera_Wnd2(hWnd, OTE_CAMERA_ID_PTZ0);
			}

		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}break;
		//ソケットIF
	case ID_SOCK_EVENT_PC_UNI_OTE: {
		int nEvent = WSAGETSELECTEVENT(lParam);
		switch (nEvent) {
		case FD_READ: {
			if (pCOte0->rcv_pc_u_ote(&pCOte0->st_msg_pc_u_rcv) == S_OK) {				//PCからのユニキャストメッセージ受信
				pCOte0->addrin_pc_u_from = pCOte0->pSockPcUniCastOte->addr_in_from;
				pCOte0->cnt_rcv_pc_u++;
				disp_msg_cnt();
			}
			else {
				pCOte0->msg_ws = L"ERROR : rcv_ote_u_pc()";	pCOte0->wstr_out_inf(pCOte0->msg_ws);
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
			if (pCOte0->rcv_pc_m_ote(&pCOte0->st_msg_pc_m_ote_rcv) == S_OK) {				//PCからのユニキャストメッセージ受信
				pCOte0->cnt_rcv_pc_m++;
				pCOte0->addrin_pc_m_from = pCOte0->pSockPcMultiCastOte->addr_in_from;
				disp_msg_cnt();
			}
			else {
				pCOte0->msg_ws = L"ERROR : rcv_ote_u_pc()";	pCOte0->wstr_out_inf(pCOte0->msg_ws);
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
			if (pCOte0->rcv_ote_m_ote(&pCOte0->st_msg_ote_m_ote_rcv) == S_OK) {				//PCからのユニキャストメッセージ受信
				pCOte0->cnt_rcv_ote_m++;
				pCOte0->addrin_ote_m_from = pCOte0->pSockOteMultiCastOte->addr_in_from;
				disp_msg_cnt();
			}
			else {
				pCOte0->msg_ws = L"ERROR : rcv_ote_m_ote()";	pCOte0->wstr_out_inf(pCOte0->msg_ws);
			}
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case WM_LBUTTONDOWN: {
		LONG x = (LONG)LOWORD(lParam), y = (LONG)HIWORD(lParam);
		LONG d2 = (x - OTE0_GR_AREA_CX) * (x - OTE0_GR_AREA_CX)  + (y - OTE0_GR_AREA_CY)* (y - OTE0_GR_AREA_CY);
		//旋回引込目標位置設定エリア
		if (d2 < OTE0_GR_AREA_R * OTE0_GR_AREA_R) {								
			pCOte0->update_auto_target_touch(OTE0_ID_AREA_GR_BHSL, x, y);
		}
		//主巻目標位置設定エリア(有効エリアの描画は線）
		else if ((x> OTE0_GR_AREA2_MH_SET_X- OTE0_GR_AREA2_MH_SET_W/2)&&(x< (OTE0_GR_AREA2_MH_SET_X + OTE0_GR_AREA2_MH_SET_W/2))){
			if((y > OTE0_GR_AREA2_MH_SET_Y) && (y < (OTE0_GR_AREA2_MH_SET_Y + OTE0_GR_AREA2_MH_SET_H))) 
				pCOte0->update_auto_target_touch(OTE0_ID_AREA_GR_MH, x, y);
		}
		//補巻目標位置設定エリア(有効エリアの描画は線）
		else if ((x > OTE0_GR_AREA2_AH_SET_X - OTE0_GR_AREA2_AH_SET_W / 2) && (x < (OTE0_GR_AREA2_AH_SET_X + OTE0_GR_AREA2_AH_SET_W/2))){
			if((y > OTE0_GR_AREA2_AH_SET_Y) && (y < (OTE0_GR_AREA2_AH_SET_Y + OTE0_GR_AREA2_AH_SET_H)))
				pCOte0->update_auto_target_touch(OTE0_ID_AREA_GR_AH, x, y);
		}
		else;
	}break;
	case WM_MOVE: {
		//int x = LOWORD(lParam);
		//int y = HIWORD(lParam);
		RECT rc;
		GetWindowRect(hWnd_work, &rc);
		SetWindowPos(st_ipcam[OTE_CAMERA_WND_ID_OPT1].hwnd, HWND_TOPMOST, rc.left + 350, rc.top + 35, OTE0_CAM2_WND_W, OTE0_CAM2_WND_H, true);
	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);


		PatBlt(st_work_wnd.hdc[ID_OTE_HDC_MEM0], 0, 0, st_work_wnd.area_w, st_work_wnd.area_h, WHITENESS);
		draw_graphic();
		draw_info();
	
		//Windowに合成画像を書き込み
		LONG cpyw = OTE0_GR_AREA2_X + OTE0_GR_AREA2_W - OTE0_GR_AREA_X;//MEM DCからのコピー幅

		//カメラエリア
		if (disp_cnt == 1) {
			init_graphic();
			BitBlt(hdc, OTE0_CAM2_WND_X , OTE0_CAM2_WND_Y, OTE0_CAM2_WND_W + OTE0_CAM2_WND_X, OTE0_CAM2_WND_H, st_work_wnd.hdc[ID_OTE_HDC_MEM_BK], OTE0_CAM2_WND_X, OTE0_CAM2_WND_Y, SRCCOPY);
		}
		//グラフィックエリア	
		BitBlt(hdc, OTE0_GR_AREA_X, OTE0_GR_AREA_Y, cpyw, OTE0_GR_AREA_H, st_work_wnd.hdc[ID_OTE_HDC_MEM_BK], OTE0_GR_AREA_X, OTE0_GR_AREA_Y, SRCCOPY);
		TransparentBlt(hdc, OTE0_GR_AREA_X, OTE0_GR_AREA_Y, cpyw, OTE0_GR_AREA_H, st_work_wnd.hdc[ID_OTE_HDC_MEM0], OTE0_GR_AREA_X, OTE0_GR_AREA_Y, cpyw, OTE0_GR_AREA_H, RGB(255, 255, 255));
		
		//情報エリア
		BitBlt(hdc, OTE0_IF_AREA_X, OTE0_IF_AREA_Y, OTE0_IF_AREA_W, OTE0_IF_AREA_H,	st_work_wnd.hdc[ID_OTE_HDC_MEM0], OTE0_IF_AREA_X, OTE0_IF_AREA_Y,SRCCOPY);

		//ランプ描画
		draw_lamp(hdc, is_init_disp);

		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		KillTimer(hWnd_work, ID_OTE_MULTICAST_TIMER);
		KillTimer(hWnd_work, ID_OTE_UNICAST_TIMER);
		delete_objects(hWnd_work);

		for (int i = 0; i < OTE0_N_IP_CAMERA; i++) {
			if (st_ipcam[i].hwnd!=NULL) {
				st_ipcam[i].pPSA->LiveStop();
				st_ipcam[i].pPSA->OnClose();
				delete st_ipcam[i].pPSA;
				DestroyWindow(st_ipcam[i].hwnd);
			}
		}

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
		st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_ADDR] = CreateWindowW(TEXT("STATIC"), 
			st_work_wnd.ctrl_text[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_ADDR], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_ADDR].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_ADDR].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_ADDR].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_ADDR].cy,
			hWnd, (HMENU)(BASE_ID_OTE_STATIC + ID_OTE_SUB_CONNECT_ADDR), hInst, NULL);

		st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEADS] = CreateWindowW(TEXT("STATIC"),
			st_work_wnd.ctrl_text[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEADS], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEADS].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEADS].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEADS].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEADS].cy,
			hWnd, (HMENU)(BASE_ID_OTE_STATIC + ID_OTE_SUB_CONNECT_HEADS), hInst, NULL);

		st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEADR] = CreateWindowW(TEXT("STATIC"),
			st_work_wnd.ctrl_text[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEADR], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEADR].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEADR].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEADR].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_CONNECT_HEADR].cy,
			hWnd, (HMENU)(BASE_ID_OTE_STATIC + ID_OTE_SUB_CONNECT_HEADR), hInst, NULL);

		//メッセージ表示切替ラジオ釦
		for (int i = ID_OTE_RADIO_SOCK_PU; i <= ID_OTE_RADIO_SOCK_OM; i++) {
			if (i == ID_OTE_RADIO_SOCK_PU) {// | WS_GROUP あり
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
		SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][st_work_wnd.connect_wnd_item], BM_SETCHECK, BST_CHECKED, 0L);
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case BASE_ID_OTE_PB + ID_OTE_RADIO_SOCK_PU:
		case BASE_ID_OTE_PB + ID_OTE_RADIO_SOCK_PM:
		case BASE_ID_OTE_PB + ID_OTE_RADIO_SOCK_OM:
			st_work_wnd.connect_wnd_item = wmId - BASE_ID_OTE_PB;break;
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
		CreateWindowW(TEXT("STATIC"),L"主巻上速度", WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_MHSPD_7].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_MHSPD_7].y -25,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_MHSPD_7].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_MHSPD_7].cy,
			hWnd, NULL, hInst, NULL);

		CreateWindowW(TEXT("STATIC"), L"ﾎｲｯﾌﾟ速度", WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_AHSPD_14].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_AHSPD_14].y -25,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_AHSPD_14].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_AHSPD_14].cy,
			hWnd, NULL, hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"引込ｴﾘｱ", WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_BH_57].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_BH_57].y -25,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_BH_57].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_BH_57].cy,
			hWnd, NULL, hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"走行緩和", WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_GT_SHOCK].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_GT_SHOCK].y -25,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_GT_SHOCK].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_GT_SHOCK].cy,
			hWnd, NULL, hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"ジブ接近", WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_JIB_NARROW].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_JIB_NARROW].y -25,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_JIB_NARROW].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_RADIO_JIB_NARROW].cy,
			hWnd, NULL, hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"吊荷減少", WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_LOAD_SWY].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_LOAD_SWY].y -25,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_LOAD_SWY].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_LOAD_SWY].cy,
			hWnd, NULL, hInst, NULL);

		//モード設定ラジオ釦
		for (int i = ID_OTE_RADIO_MHSPD_7; i <= ID_OTE_RADIO_BH_REST; i++) {
			if ((i == ID_OTE_RADIO_MHSPD_7)|| (i == ID_OTE_RADIO_AHSPD_14) || (i == ID_OTE_RADIO_BH_57)){// | WS_GROUP あり
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
		SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][st_work_wnd.mh_spd_mode], BM_SETCHECK, BST_CHECKED, 0L);
		SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][st_work_wnd.ah_spd_mode], BM_SETCHECK, BST_CHECKED, 0L);
		SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][st_work_wnd.bh_work_mode], BM_SETCHECK, BST_CHECKED, 0L);


		for (int i = ID_OTE_RADIO_JIB_NARROW; i <= ID_OTE_RADIO_JIB_WIDE; i++) {
			if (i == ID_OTE_RADIO_JIB_NARROW) {// | WS_GROUP あり
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
		SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][st_work_wnd.jib_chk_mode], BM_SETCHECK, BST_CHECKED, 0L);

		st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_GT_SHOCK] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][ID_OTE_CHK_GT_SHOCK], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
				st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_GT_SHOCK].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_GT_SHOCK].y,
				st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_GT_SHOCK].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_GT_SHOCK].cy,
				hWnd, (HMENU)(BASE_ID_OTE_PB + ID_OTE_CHK_GT_SHOCK), hInst, NULL);

		st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_LOAD_SWY] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][ID_OTE_CHK_LOAD_SWY], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_LOAD_SWY].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_LOAD_SWY].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_LOAD_SWY].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_LOAD_SWY].cy,
			hWnd, (HMENU)(BASE_ID_OTE_PB + ID_OTE_CHK_LOAD_SWY), hInst, NULL);

	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		if ((wmId >= BASE_ID_OTE_PB + ID_OTE_RADIO_MHSPD_7) && (wmId <= BASE_ID_OTE_PB + ID_OTE_RADIO_MHSPD_21)) {
			st_work_wnd.mh_spd_mode = wmId - BASE_ID_OTE_PB;
			break;
		}
		if ((wmId >= BASE_ID_OTE_PB + ID_OTE_RADIO_AHSPD_14) && (wmId <= BASE_ID_OTE_PB + ID_OTE_RADIO_AHSPD_24)) {
			st_work_wnd.ah_spd_mode = wmId - BASE_ID_OTE_PB;
			break;
		}
		if ((wmId >= BASE_ID_OTE_PB + ID_OTE_RADIO_BH_57) && (wmId <= BASE_ID_OTE_PB + ID_OTE_RADIO_BH_REST)) {
			st_work_wnd.bh_work_mode = wmId - BASE_ID_OTE_PB;
			break;
		}
		if ((wmId >= BASE_ID_OTE_PB + ID_OTE_RADIO_JIB_NARROW) && (wmId <= BASE_ID_OTE_PB + ID_OTE_RADIO_JIB_WIDE)) {
			st_work_wnd.jib_chk_mode = wmId - BASE_ID_OTE_PB;
			break;
		}
		switch (wmId)
		{
		case BASE_ID_OTE_PB + ID_OTE_CHK_LOAD_SWY: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_LOAD_SWY], BM_GETCHECK, 0, 0)) {
				st_work_wnd.pb_stat[ID_OTE_CHK_LOAD_SWY] = OTE0_PB_OFF_DELAY_COUNT;
			}
			else {
				st_work_wnd.pb_stat[ID_OTE_CHK_LOAD_SWY] = L_OFF;
			}
		}break;
		case BASE_ID_OTE_PB + ID_OTE_CHK_GT_SHOCK: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_GT_SHOCK], BM_GETCHECK, 0, 0)) {
				st_work_wnd.pb_stat[ID_OTE_CHK_GT_SHOCK] = OTE0_PB_OFF_DELAY_COUNT;
			}
			else {
				st_work_wnd.pb_stat[ID_OTE_CHK_GT_SHOCK] = L_OFF;
			}
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
		//表示切替ラジオ釦
		for (int i = ID_OTE_RADIO_FLT_ALL; i <= ID_OTE_RADIO_FLT_WARN; i++) {
			if (i == ID_OTE_RADIO_FLT_ALL) {// | WS_GROUP あり
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
		SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][st_work_wnd.flt_disp_mode], BM_SETCHECK, BST_CHECKED, 0L);

		//故障リセットPB
		st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_PB_FLT_RESET] = CreateWindow(L"BUTTON",
			st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][ID_OTE_PB_FLT_RESET], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE | WS_GROUP,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_PB_FLT_RESET].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_PB_FLT_RESET].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_PB_FLT_RESET].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_PB_FLT_RESET].cy,
			hWnd, (HMENU)(BASE_ID_OTE_PB + ID_OTE_PB_FLT_RESET), hInst, NULL);

		//Bypass CHK BOX
		st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_IL_BYPASS] = CreateWindow(L"BUTTON",
			st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][ID_OTE_CHK_IL_BYPASS], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | WS_GROUP,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_IL_BYPASS].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_IL_BYPASS].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_IL_BYPASS].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_IL_BYPASS].cy,
			hWnd, (HMENU)(BASE_ID_OTE_PB + ID_OTE_CHK_IL_BYPASS), hInst, NULL);


		//故障リスト表示STATIC
		st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_FAULT_LIST] = CreateWindowW(TEXT("STATIC"),
			st_work_wnd.ctrl_text[ID_OTE_CTRL_STATIC][ID_OTE_SUB_FAULT_LIST], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_FAULT_LIST].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_FAULT_LIST].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_FAULT_LIST].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_FAULT_LIST].cy,
			hWnd, (HMENU)(BASE_ID_OTE_STATIC + ID_OTE_SUB_FAULT_LIST), hInst, NULL);


	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case BASE_ID_OTE_PB + ID_OTE_PB_FLT_RESET:
		{
			st_work_wnd.pb_stat[wmId - BASE_ID_OTE_PB] = OTE0_PB_OFF_DELAY_COUNT;//カウントダウンはメインウィンドウで一括実施
		}break;
		case BASE_ID_OTE_PB + ID_OTE_CHK_IL_BYPASS:
		{
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_IL_BYPASS], BM_GETCHECK, 0, 0)) {
				st_work_wnd.pb_stat[ID_OTE_CHK_IL_BYPASS] = L_ON;
			}
			else {
				st_work_wnd.pb_stat[ID_OTE_CHK_IL_BYPASS] = L_OFF;
			}
		}break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}break;
	case ID_OTE0_STATIC_UPDATE: {
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
LRESULT CALLBACK WndStatusProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		for (int i = ID_OTE_SUB_STAT_MH; i <= ID_OTE_SUB_STAT_GT; i++) {
			st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][i] = CreateWindowW(TEXT("STATIC"),
				st_work_wnd.ctrl_text[ID_OTE_CTRL_STATIC][i], WS_CHILD | WS_VISIBLE | SS_LEFT,
				st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][i].y,
				st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][i].cy,
				hWnd, (HMENU)(BASE_ID_OTE_STATIC + i), hInst, NULL);
		}

		int x0 = st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_STAT_MH].x -43, y0 = st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_STAT_MH].y+18;

		CreateWindowW(TEXT("STATIC"),L"目標", WS_CHILD | WS_VISIBLE | SS_LEFT,x0,y0,40,20,hWnd, NULL, hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"指令", WS_CHILD | WS_VISIBLE | SS_LEFT,x0,y0 + 18, 40, 20, hWnd, NULL, hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"VFB", WS_CHILD | WS_VISIBLE | SS_LEFT,x0,y0 + 36, 40, 20, hWnd, NULL, hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"TFB", WS_CHILD | WS_VISIBLE | SS_LEFT,x0,y0 + 54, 40, 20, hWnd, NULL, hInst, NULL);

		y0 = st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_STAT_BH].y + 20;
		CreateWindowW(TEXT("STATIC"), L"目標", WS_CHILD | WS_VISIBLE | SS_LEFT, x0, y0, 40, 20, hWnd, NULL, hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"指令", WS_CHILD | WS_VISIBLE | SS_LEFT, x0, y0 + 18, 40, 20, hWnd, NULL, hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"VFB", WS_CHILD | WS_VISIBLE | SS_LEFT, x0, y0 + 36, 40, 20, hWnd, NULL, hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"TFB", WS_CHILD | WS_VISIBLE | SS_LEFT, x0, y0 + 54, 40, 20, hWnd, NULL, hInst, NULL);
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

	case ID_OTE0_STATIC_UPDATE: {
		msg_wos.str(L"");msg_wos << L"MH:\n"<< pCOte0->st_msg_pc_u_rcv.body.plc_in.spd_tg[PLC_IF_CCID_MH1] << L"\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.inv_cc_Ww1[PLC_IF_CCID_MH1] << L"\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.inv_cc_Wr1[PLC_IF_CCID_MH1] << L"\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.inv_cc_Wr2[PLC_IF_CCID_MH1] << L"\n" ;
		SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_STAT_MH], msg_wos.str().c_str());
		msg_wos.str(L""); msg_wos << L"AH:\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.spd_tg[PLC_IF_CCID_AH] << L"\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.inv_cc_Ww1[PLC_IF_CCID_AH] << L"\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.inv_cc_Wr1[PLC_IF_CCID_AH] << L"\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.inv_cc_Wr2[PLC_IF_CCID_AH] << L"\n";
		SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_STAT_AH], msg_wos.str().c_str());
		msg_wos.str(L""); msg_wos << L"GT:\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.spd_tg[PLC_IF_CCID_GT] << L"\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.inv_cc_Ww1[PLC_IF_CCID_GT] << L"\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.inv_cc_Wr1[PLC_IF_CCID_GT] << L"\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.inv_cc_Wr2[PLC_IF_CCID_GT] << L"\n";
		SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_STAT_GT], msg_wos.str().c_str());
		msg_wos.str(L""); msg_wos << L"BH:\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.spd_tg[PLC_IF_CCID_BH] << L"\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.inv_cc_Ww1[PLC_IF_CCID_BH] << L"\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.inv_cc_Wr1[PLC_IF_CCID_BH] << L"\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.inv_cc_Wr2[PLC_IF_CCID_BH] << L"\n";
		SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_STAT_BH], msg_wos.str().c_str());
		msg_wos.str(L""); msg_wos << L"SL:\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.spd_tg[PLC_IF_CCID_SL] << L"\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.inv_cc_Ww1[PLC_IF_CCID_SL] << L"\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.inv_cc_Wr1[PLC_IF_CCID_SL] << L"\n" << pCOte0->st_msg_pc_u_rcv.body.plc_in.inv_cc_Wr2[PLC_IF_CCID_SL] << L"\n";
		SetWindowText(st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][ID_OTE_SUB_STAT_SL], msg_wos.str().c_str());
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

		
		// ウィンドウを透明にする
		SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

		SetTimer(hWnd, 123, 100, NULL);
	}
	case WM_TIMER: {
		InvalidateRect(hWnd_sub[ID_OTE0_SWY_WND], NULL, TRUE);//表示更新
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

		if (1) {
		//if (st_ipcam[OTE_CAMERA_WND_ID_BASE].hwnd == NULL) {
	
			PatBlt(st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM0], 0, 0, OTE0_SWY_WND_W, OTE0_SWY_WND_H, WHITENESS);
	
			draw_bk_swy();
			draw_graphic_swy();

			BitBlt(hdc, 0, 0, OTE0_SWY_WND_W, OTE0_SWY_WND_H, st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM_BK], 0, 0, SRCCOPY);
			//グラフィックを重ね合わせ
			TransparentBlt(hdc, 0, 0, OTE0_SWY_WND_W, OTE0_SWY_WND_H,
				st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM0], 0, 0, OTE0_SWY_WND_W, OTE0_SWY_WND_H,
				RGB(255, 255, 255));
		}
		else {
			HDC hCamDC = GetDC(st_ipcam[OTE_CAMERA_WND_ID_BASE].hwnd);
			StretchBlt(hdc, 0, 0, 280, 240, hCamDC, 0, 0, 640, 480, SRCCOPY);
		}
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
LRESULT CALLBACK WndCamProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	HINSTANCE hInst = GetModuleHandle(0);

	CPsaMain* pPSA=NULL;
	int id_cam;
	if (st_ipcam[OTE_CAMERA_WND_ID_BASE].hwnd != NULL) {
		pPSA = st_ipcam[OTE_CAMERA_WND_ID_BASE].pPSA;
		id_cam = st_ipcam[OTE_CAMERA_WND_ID_BASE].icam;
	}
	else
		;//	return S_OK;

	switch (message)
	{

	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		//マルチキャストタイマ起動 
		SetTimer(hWnd, ID_OTE_CAMERA_TIMER, OTE_CAMERA_SCAN_MS,NULL);
		//SetTimer(hWnd, ID_OTE_CAMERA_TIMER, 1000, NULL);

		//CHECK BOX
		for (LONGLONG i = ID_OTE_CHK_CAMERA_LIVE; i <= ID_OTE_CHK_CAMERA_ZOMN; i++) {
			st_work_wnd.hctrl[ID_OTE_CTRL_PB][i] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][i], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
				st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].y,
				st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cy,
				hWnd, (HMENU)(BASE_ID_OTE_PB + i), hInst, NULL);
		}
		//PB
		st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_PB_CAMERA_STOP] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][ID_OTE_PB_CAMERA_STOP], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_PB_CAMERA_STOP].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_PB_CAMERA_STOP].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_PB_CAMERA_STOP].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_PB_CAMERA_STOP].cy,
			hWnd, (HMENU)(BASE_ID_OTE_PB + ID_OTE_PB_CAMERA_STOP), hInst, NULL);

		//TOUCH
		st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_TOUCH] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][ID_OTE_CHK_TOUCH], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_TOUCH].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_TOUCH].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_TOUCH].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_TOUCH].cy,
			hWnd, (HMENU)(BASE_ID_OTE_PB + ID_OTE_CHK_TOUCH), hInst, NULL);

		//カメラ起動
		SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_LIVE], BM_SETCHECK, BST_CHECKED, 0);
		st_work_wnd.pb_lamp[ID_OTE_CHK_CAMERA_LIVE].com = L_ON;
		

	}break;
	case WM_TIMER: {
	//	InvalidateRect(hWnd, NULL, FALSE);

		pPSA->UpdateControl();

	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);

		switch (wmId)
		{

		case BASE_ID_OTE_PB + ID_OTE_CHK_CAMERA_LIVE:
		{
			//コマンドのON/OFFはLamp指令領域を利用
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][wmId - BASE_ID_OTE_PB], BM_GETCHECK, 0, 0)) {
				st_work_wnd.pb_lamp[wmId - BASE_ID_OTE_PB].com = L_ON;
				pPSA->LiveStart();
			}
			else {
				st_work_wnd.pb_lamp[wmId - BASE_ID_OTE_PB].com = L_OFF;
				pPSA->LiveStop();
			}
		}break;
		case BASE_ID_OTE_PB + ID_OTE_CHK_CAMERA_TILU: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_TILU], BM_GETCHECK, 0, 0)) {
				pPSA->ltilt = -DEF_SPD_CAM_TILT;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_TILD], BM_SETCHECK, BST_UNCHECKED, 0L);
			}
			else {
				pPSA->ltilt = 0;
			}
			pPSA->UpdateControl();
		}break;
		case BASE_ID_OTE_PB + ID_OTE_CHK_CAMERA_TILD: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_TILD], BM_GETCHECK, 0, 0)) {
				pPSA->ltilt = DEF_SPD_CAM_TILT;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_TILU], BM_SETCHECK, BST_UNCHECKED, 0L);
			}
			else
				pPSA->ltilt = 0;

			pPSA->UpdateControl();
		}break;

		case BASE_ID_OTE_PB + ID_OTE_CHK_CAMERA_PANL: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_PANL], BM_GETCHECK, 0, 0)) {
				pPSA->lpan = -DEF_SPD_CAM_PAN;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_PANR], BM_SETCHECK, BST_UNCHECKED, 0L);
			}
			else
				pPSA->lpan = 0;

			pPSA->UpdateControl();
		}break;

		case BASE_ID_OTE_PB + ID_OTE_CHK_CAMERA_PANR: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_PANR], BM_GETCHECK, 0, 0)) {
				pPSA->lpan = DEF_SPD_CAM_PAN;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_PANL], BM_SETCHECK, BST_UNCHECKED, 0L);
			}
			else
				pPSA->lpan = 0;

			pPSA->UpdateControl();
		}break;

		case BASE_ID_OTE_PB + ID_OTE_CHK_CAMERA_ZOMW: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_ZOMW], BM_GETCHECK, 0, 0)) {
				pPSA->lzoom = -DEF_SPD_CAM_ZOOM;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_ZOMN], BM_SETCHECK, BST_UNCHECKED, 0L);
			}
			else
				pPSA->lzoom = 0;

			pPSA->UpdateControl();
		}break;

		case BASE_ID_OTE_PB + ID_OTE_CHK_CAMERA_ZOMN: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_ZOMN], BM_GETCHECK, 0, 0)) {
				pPSA->lzoom = DEF_SPD_CAM_ZOOM;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_ZOMW], BM_SETCHECK, BST_UNCHECKED, 0L);
			}
			else
				pPSA->lzoom = 0;

			pPSA->UpdateControl();
		}break;
		case BASE_ID_OTE_PB + ID_OTE_PB_CAMERA_STOP: {
			pPSA->CtrlStop();
			for (int i = ID_OTE_CHK_CAMERA_TILU; i <= ID_OTE_CHK_CAMERA_ZOMN; i++) {
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][i], BM_SETCHECK, BST_UNCHECKED, 0L);
			}
		}break;

		case BASE_ID_OTE_PB + ID_OTE_CHK_TOUCH: {
			RECT rc;
			if (st_work_wnd.is_test_wnd_follow_main) {
				st_work_wnd.is_test_wnd_follow_main = false;
				GetWindowRect(hWnd, &rc);
				SetWindowPos(st_ipcam[OTE_CAMERA_WND_ID_OPT1].hwnd, HWND_TOPMOST, rc.left + 100, rc.top + 80, OTE0_CAM2_WND_W, OTE0_CAM2_WND_H, true);
			}
			else {
				st_work_wnd.is_test_wnd_follow_main = true;
				GetWindowRect(hWnd_work, &rc);
				SetWindowPos(st_ipcam[OTE_CAMERA_WND_ID_OPT1].hwnd, HWND_TOPMOST, rc.left + 280, rc.top + 40, OTE0_CAM2_WND_W, OTE0_CAM2_WND_H, true);
			}
		}

		}
	}break;
	case OTE0_MSG_SWICH_CAMERA: 
	{
		
		pPSA->LiveStop();
		switch (lParam) {
		case ID_OTE_RADIO_HOOK: {
			pPSA->SwitchCamera(hWnd,OTE_CAMERA_HOOK0_IP, OTE_CAMERA_USER, OTE_CAMERA_PASS, OTE_CAMERA_FORMAT_H264);
		}break;
		case ID_OTE_RADIO_OPE1: 
		case ID_OTE_RADIO_OPE2:
		{
			pPSA->SwitchCamera(hWnd, OTE_CAMERA_FISH0_IP, OTE_CAMERA_USER, OTE_CAMERA_PASS, OTE_CAMERA_FORMAT_H265);
		}break;

		case ID_OTE_RADIO_WIDE:
		case ID_OTE_RADIO_ZOOM:
		default:
		{
			pPSA->SwitchCamera(hWnd, OTE_CAMERA_PTZ0_IP, OTE_CAMERA_USER, OTE_CAMERA_PASS, OTE_CAMERA_FORMAT_H264);
		}break;
		}
	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
//画面切取サンプル
#if 0
		// ウィンドウを透明にする
		SetLayeredWindowAttributes(hwnd_camera[0], RGB(0, 0, 0), 0, LWA_COLORKEY);

		// デスクトップ画面の一部を切り取る
		//st_work_wnd.hdc[ID_OTE_HDC_CAMERA_VIEW] = GetDC(NULL);
		
		hCaptureDC = CreateCompatibleDC(st_work_wnd.hdc[ID_OTE_HDC_CAMERA_VIEW]);
		hCaptureBitmap = CreateCompatibleBitmap(st_work_wnd.hdc[ID_OTE_HDC_CAMERA_VIEW], 640, 480);
		SelectObject(hCaptureDC, hCaptureBitmap);
		BitBlt(hCaptureDC, 0, 0, 640, 480, st_work_wnd.hdc[ID_OTE_HDC_CAMERA_VIEW], OTE0_CAM_WND_TG_X, OTE0_CAM_WND_TG_Y, SRCCOPY);

		// 切り取った画像をウィンドウに表示する
	//	HDC hWindowDC = GetDC(hwnd_camera[0]);
		BitBlt(hdc, 0, 0, 640, 480, hCaptureDC, 0, 0, SRCCOPY);

		// 後始末
//		SelectObject(hCaptureDC, hOldBitmap);

#endif
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		id_cam = OTE_CAMERA_ID_NA;
		st_ipcam[OTE_CAMERA_WND_ID_BASE].hwnd = NULL;

		KillTimer(hWnd_work, ID_OTE_CAMERA_TIMER);
		//PostQuitMessage(0);
	}break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
}
LRESULT CALLBACK WndCam2Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	HINSTANCE hInst = GetModuleHandle(0);

	CPsaMain* pPSA = NULL;
	int id_cam;
	if (st_ipcam[OTE_CAMERA_WND_ID_OPT1].hwnd != NULL) {
		pPSA = st_ipcam[OTE_CAMERA_WND_ID_OPT1].pPSA;
		id_cam = st_ipcam[OTE_CAMERA_WND_ID_OPT1].icam;
	}

	switch (message)
	{
	
	case WM_TIMER: {
		SetTimer(hWnd, 12345, 100, NULL);
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
#if 0
		switch (wmId)
		{

		case BASE_ID_OTE_PB + ID_OTE_CHK_CAMERA_LIVE:
		{
			//コマンドのON/OFFはLamp指令領域を利用
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][wmId - BASE_ID_OTE_PB], BM_GETCHECK, 0, 0)) {
				st_work_wnd.pb_lamp[wmId - BASE_ID_OTE_PB].com = L_ON;
				pPSA->LiveStart();
			}
			else {
				st_work_wnd.pb_lamp[wmId - BASE_ID_OTE_PB].com = L_OFF;
				pPSA->LiveStop();
			}
		}break;
		case BASE_ID_OTE_PB + ID_OTE_CHK_CAMERA_TILU: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_TILU], BM_GETCHECK, 0, 0)) {
				pPSA->ltilt = -DEF_SPD_CAM_TILT;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_TILD], BM_SETCHECK, BST_UNCHECKED, 0L);
			}
			else {
				pPSA->ltilt = 0;
			}
			pPSA->UpdateControl();
		}break;
		case BASE_ID_OTE_PB + ID_OTE_CHK_CAMERA_TILD: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_TILD], BM_GETCHECK, 0, 0)) {
				pPSA->ltilt = DEF_SPD_CAM_TILT;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_TILU], BM_SETCHECK, BST_UNCHECKED, 0L);
			}
			else
				pPSA->ltilt = 0;

			pPSA->UpdateControl();
		}break;

		case BASE_ID_OTE_PB + ID_OTE_CHK_CAMERA_PANL: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_PANL], BM_GETCHECK, 0, 0)) {
				pPSA->lpan = -DEF_SPD_CAM_PAN;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_PANR], BM_SETCHECK, BST_UNCHECKED, 0L);
			}
			else
				pPSA->lpan = 0;

			pPSA->UpdateControl();
		}break;

		case BASE_ID_OTE_PB + ID_OTE_CHK_CAMERA_PANR: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_PANR], BM_GETCHECK, 0, 0)) {
				pPSA->lpan = DEF_SPD_CAM_PAN;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_PANL], BM_SETCHECK, BST_UNCHECKED, 0L);
			}
			else
				pPSA->lpan = 0;

			pPSA->UpdateControl();
		}break;

		case BASE_ID_OTE_PB + ID_OTE_CHK_CAMERA_ZOMW: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_ZOMW], BM_GETCHECK, 0, 0)) {
				pPSA->lzoom = -DEF_SPD_CAM_ZOOM;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_ZOMN], BM_SETCHECK, BST_UNCHECKED, 0L);
			}
			else
				pPSA->lzoom = 0;

			pPSA->UpdateControl();
		}break;

		case BASE_ID_OTE_PB + ID_OTE_CHK_CAMERA_ZOMN: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_ZOMN], BM_GETCHECK, 0, 0)) {
				pPSA->lzoom = DEF_SPD_CAM_ZOOM;
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][ID_OTE_CHK_CAMERA_ZOMW], BM_SETCHECK, BST_UNCHECKED, 0L);
			}
			else
				pPSA->lzoom = 0;

			pPSA->UpdateControl();
		}break;
		case BASE_ID_OTE_PB + ID_OTE_PB_CAMERA_STOP: {
			pPSA->CtrlStop();
			for (int i = ID_OTE_CHK_CAMERA_TILU; i <= ID_OTE_CHK_CAMERA_ZOMN; i++) {
				SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][i], BM_SETCHECK, BST_UNCHECKED, 0L);
			}
		}break;

		}
#endif
	}break;
	case OTE0_MSG_SWICH_CAMERA:
	{

		pPSA->LiveStop();
		switch (lParam) {
		case ID_OTE_RADIO_HOOK: {
			pPSA->SwitchCamera(hWnd, OTE_CAMERA_HOOK0_IP, OTE_CAMERA_USER, OTE_CAMERA_PASS, OTE_CAMERA_FORMAT_H264);
		}break;
		case ID_OTE_RADIO_OPE1:
		case ID_OTE_RADIO_OPE2:
		{
			pPSA->SwitchCamera(hWnd, OTE_CAMERA_FISH0_IP, OTE_CAMERA_USER, OTE_CAMERA_PASS, OTE_CAMERA_FORMAT_H265);
		}break;

		case ID_OTE_RADIO_WIDE:
		case ID_OTE_RADIO_ZOOM:
		default:
		{
			pPSA->SwitchCamera(hWnd, OTE_CAMERA_PTZ0_IP, OTE_CAMERA_USER, OTE_CAMERA_PASS, OTE_CAMERA_FORMAT_H264);
		}break;
		}
	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		//画面切取サンプル
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		id_cam = OTE_CAMERA_ID_NA;
		st_ipcam[OTE_CAMERA_WND_ID_OPT1].hwnd = NULL;
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
HWND open_status_Wnd(HWND hwnd) {

	InitCommonControls();//コモンコントロール初期化
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndStatusProc;
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
		OTE0_SUB_WND_X, OTE0_SUB_WND_Y, OTE0_SUB_WND_W, OTE0_SUB_WND_H,
		hwnd, nullptr, hInst, nullptr);

	HDC hdc = GetDC(hWnd_sub[ID_OTE0_SWY_WND]);
	st_work_wnd.hBmap[ID_OTE_HBMAP_SWY_MEM0] = CreateCompatibleBitmap(hdc, OTE0_SWY_WND_W, OTE0_SWY_WND_H);
	st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM0] = CreateCompatibleDC(hdc);
	SelectObject(st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM0], st_work_wnd.hBmap[ID_OTE_HBMAP_SWY_MEM0]);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_SWY_M0] = new Graphics(st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM0]);

	st_work_wnd.hBmap[ID_OTE_HBMAP_SWY_MEM_BK] = CreateCompatibleBitmap(hdc, OTE0_SWY_WND_W, OTE0_SWY_WND_H);
	st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM_BK] = CreateCompatibleDC(hdc);
	SelectObject(st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM_BK], st_work_wnd.hBmap[ID_OTE_HBMAP_SWY_MEM_BK]);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_SWY_BK] = new Graphics(st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM_BK]);

	st_work_wnd.hBmap[ID_OTE_HBMAP_SWY_MEM_GR] = CreateCompatibleBitmap(hdc, OTE0_SWY_WND_W, OTE0_SWY_WND_H);
	st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM_GR] = CreateCompatibleDC(hdc);
	SelectObject(st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM_GR], st_work_wnd.hBmap[ID_OTE_HBMAP_SWY_MEM_GR]);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_SWY_GRAPHIC] = new Graphics(st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM_GR]);

	PatBlt(st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM0], 0, 0, OTE0_SWY_WND_W, OTE0_SWY_WND_H, WHITENESS);
	PatBlt(st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM_BK], 0, 0, OTE0_SWY_WND_W, OTE0_SWY_WND_H, WHITENESS);
	PatBlt(st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM_GR], 0, 0, OTE0_SWY_WND_W, OTE0_SWY_WND_H, WHITENESS);

	//InvalidateRect(hWnd_sub[ID_OTE0_SWY_WND], NULL, TRUE);//表示更新

	ShowWindow(hWnd_sub[ID_OTE0_SWY_WND], SW_SHOW);
	UpdateWindow(hWnd_sub[ID_OTE0_SWY_WND]);

	return hWnd_sub[ID_OTE0_SWY_WND];
}
HWND open_camera_Wnd(HWND hwnd, int id_cam) {
		
	InitCommonControls();//コモンコントロール初期化
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndCamProc; //WndSwyProc2;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(0);;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;// TEXT("OTECON");
	wcex.lpszClassName = TEXT("Camera View");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	HWND hcamwnd=NULL;

	st_ipcam[OTE_CAMERA_WND_ID_BASE].icam = id_cam;
	hcamwnd = st_ipcam[OTE_CAMERA_WND_ID_BASE].hwnd = CreateWindowW(TEXT("Camera View"), TEXT("Camera View"), WS_POPUP | WS_BORDER ,//| WS_OVERLAPPEDWINDOW,
														OTE0_CAM_WND_X, OTE0_CAM_WND_Y, OTE0_CAM_WND_W, OTE0_CAM_WND_H,
														hwnd, nullptr, hInst, nullptr);

	if (hcamwnd != NULL) {
		if (id_cam == OTE_CAMERA_ID_PTZ0) {
			st_ipcam[OTE_CAMERA_WND_ID_BASE].pPSA = new CPsaMain(hcamwnd, OTE_CAMERA_PTZ0_IP, OTE_CAMERA_USER, OTE_CAMERA_PASS, DEF_STREAM_FORMAT_H264);
		}
		else if (id_cam == OTE_CAMERA_ID_FISH0) {
			st_ipcam[OTE_CAMERA_WND_ID_BASE].pPSA = new CPsaMain(hcamwnd, OTE_CAMERA_FISH0_IP, OTE_CAMERA_USER, OTE_CAMERA_PASS, DEF_STREAM_FORMAT_JPEG);
		}
		else if (id_cam == OTE_CAMERA_ID_HOOK0) {
			st_ipcam[OTE_CAMERA_WND_ID_BASE].pPSA = new CPsaMain(hcamwnd, OTE_CAMERA_HOOK0_IP, OTE_CAMERA_USER, OTE_CAMERA_PASS, DEF_STREAM_FORMAT_H265);
		}
		else {
			st_ipcam[OTE_CAMERA_WND_ID_BASE].pPSA = new CPsaMain(hcamwnd, OTE_CAMERA_PTZ0_IP, OTE_CAMERA_USER, OTE_CAMERA_PASS, DEF_STREAM_FORMAT_H264);
		}
		st_ipcam[OTE_CAMERA_WND_ID_BASE].pPSA->LiveStart();

		ShowWindow(hcamwnd, SW_SHOW);
		UpdateWindow(hcamwnd);
	}

	return hcamwnd;
}
HWND open_camera_Wnd2(HWND hwnd, int id_cam) {
	InitCommonControls();//コモンコントロール初期化
	HINSTANCE hInst = GetModuleHandle(0);
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndCam2Proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(0);;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;// TEXT("OTECON");
	wcex.lpszClassName = TEXT("Camera2 View");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	HWND hcamwnd;

	st_ipcam[OTE_CAMERA_WND_ID_OPT1].icam = id_cam;
	hcamwnd = st_ipcam[OTE_CAMERA_WND_ID_OPT1].hwnd = CreateWindowW(TEXT("Camera2 View"), TEXT("CAMERA VIEW"), WS_POPUP | WS_BORDER,//| WS_OVERLAPPEDWINDOW,
		OTE0_CAM2_WND_X, OTE0_CAM2_WND_Y, OTE0_CAM2_WND_W, OTE0_CAM2_WND_H,
		hwnd, nullptr, hInst, nullptr);
	if (id_cam == OTE_CAMERA_ID_PTZ0) {
		st_ipcam[OTE_CAMERA_WND_ID_OPT1].pPSA = new CPsaMain(hcamwnd, OTE_CAMERA_PTZ0_IP, OTE_CAMERA_USER, OTE_CAMERA_PASS, DEF_STREAM_FORMAT_H264);
	}
	else if (id_cam == OTE_CAMERA_ID_FISH0) {
		st_ipcam[OTE_CAMERA_WND_ID_OPT1].pPSA = new CPsaMain(hcamwnd, OTE_CAMERA_FISH0_IP, OTE_CAMERA_USER, OTE_CAMERA_PASS, DEF_STREAM_FORMAT_H265);
	}
	else if (id_cam == OTE_CAMERA_ID_HOOK0) {
		st_ipcam[OTE_CAMERA_WND_ID_OPT1].pPSA = new CPsaMain(hcamwnd, OTE_CAMERA_HOOK0_IP, OTE_CAMERA_USER, OTE_CAMERA_PASS, DEF_STREAM_FORMAT_H264);
	}
	else {
		st_ipcam[OTE_CAMERA_WND_ID_OPT1].pPSA = new CPsaMain(hcamwnd, OTE_CAMERA_PTZ0_IP, OTE_CAMERA_USER, OTE_CAMERA_PASS, DEF_STREAM_FORMAT_H264);
	}


	st_ipcam[OTE_CAMERA_WND_ID_OPT1].pPSA->m_psapi->SetH264Resolution(640);
	st_ipcam[OTE_CAMERA_WND_ID_OPT1].pPSA->m_psapi->SetJPEGResolution(640);

	st_ipcam[OTE_CAMERA_WND_ID_OPT1].pPSA->m_psapi->SetImageWidth(360);            //Image width
	st_ipcam[OTE_CAMERA_WND_ID_OPT1].pPSA->m_psapi->SetImageHeight(270);           //Imgae Height

	st_ipcam[OTE_CAMERA_WND_ID_OPT1].pPSA->LiveStart();


	ShowWindow(hcamwnd, SW_SHOW);
	UpdateWindow(hcamwnd);

	RECT rc;
	GetWindowRect(hWnd_work, &rc);
	SetWindowPos(st_ipcam[OTE_CAMERA_WND_ID_OPT1].hwnd, HWND_TOPMOST, rc.left + 350, rc.top + 35, OTE0_CAM2_WND_W, OTE0_CAM2_WND_H, true);

//	MoveWindow(hcamwnd, OTE0_CAM2_WND_X - 1280, OTE0_CAM2_WND_Y-3, OTE0_CAM2_WND_W, OTE0_CAM2_WND_H, true);

	return hcamwnd;
}

//*********************************************************************************************
/// <summary>
/// OTEウィンドウ上にオブジェクト配置
/// </summary>
void set_OTE_panel_objects(HWND hWnd) {

	HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

	//ラベル
	//for (int i = ID_OTE_LABEL_MH; i <= ID_OTE_INF_GT0; i++) { INF部はグラフィック表示
	for (int i = ID_OTE_LABEL_MH; i <= ID_OTE_LABEL_GT; i++) {
		st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][i] = CreateWindowW(TEXT("STATIC"), st_work_wnd.ctrl_text[ID_OTE_CTRL_STATIC][i], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][i].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][i].cy,
			hWnd, (HMENU)(BASE_ID_OTE_STATIC + i), hInst, NULL);
	}

	//PB
	{
		for (LONGLONG i = ID_OTE_PB_TEISHI; i <= ID_OTE_PB_FUREDOME; i++) {
			st_work_wnd.hctrl[ID_OTE_CTRL_PB][i] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][i], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE ,
				st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].y,
				st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cy,
				hWnd, (HMENU)(BASE_ID_OTE_PB + i), hInst, NULL);
		}

		int i = ID_OTE_PB_ARESET_ALL;
		st_work_wnd.hctrl[ID_OTE_CTRL_PB][i] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][i], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cy,
			hWnd, (HMENU)(BASE_ID_OTE_PB + i), hInst, NULL);

		//CHECK BOX
		for (LONGLONG i = ID_OTE_CHK_S1; i <= ID_OTE_CHK_N3; i++) {
				st_work_wnd.hctrl[ID_OTE_CTRL_PB][i] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][i], WS_CHILD | WS_VISIBLE  | BS_AUTOCHECKBOX | BS_PUSHLIKE,
					st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].y,
					st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cy,
					hWnd, (HMENU)(BASE_ID_OTE_PB + i), hInst, NULL);
		}

		//for (LONGLONG i = ID_OTE_CHK_ASET_MH; i <= ID_OTE_CHK_ASET_SL; i++) {
		for (LONGLONG i = ID_OTE_CHK_ASET_MH; i <=ID_OTE_CHK_CAMERA_WND; i++) {
			st_work_wnd.hctrl[ID_OTE_CTRL_PB][i] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][i], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
				st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].y,
				st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cy,
				hWnd, (HMENU)(BASE_ID_OTE_PB + i), hInst, NULL);
		}

		//RADIO
		for (LONGLONG i = ID_OTE_RADIO_WIDE; i <= ID_OTE_RADIO_SWAY; i++) {
			if ((i == ID_OTE_RADIO_WIDE) ||(i == ID_OTE_RADIO_COM)){
				st_work_wnd.hctrl[ID_OTE_CTRL_PB][i] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].y,
					st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cy,
					hWnd, (HMENU)(BASE_ID_OTE_PB + i), hInst, NULL);
			}
			else {
				st_work_wnd.hctrl[ID_OTE_CTRL_PB][i] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].y,
					st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cy,
					hWnd, (HMENU)(BASE_ID_OTE_PB + i), hInst, NULL);
			}
		}
	//	SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][st_work_wnd.subpanel_sel], BM_SETCHECK, BST_CHECKED, 0L);
		SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][st_work_wnd.camera_sel], BM_SETCHECK, BST_CHECKED, 0L);
	
	}

	//ノッチラジオボタン
	for (int j = 0; j <= ID_AHOIST;j++) {
		if (j == 2) {//グリップスイッチ
			for (int i = 0; i <= 9; i++) {
				st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_ESTOP + i] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_ESTOP + i], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE,
					st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_ESTOP + i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_ESTOP + i].y,
					st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_ESTOP + i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_ESTOP + i].cy,
					hWnd, (HMENU)(BASE_ID_OTE_NOTCH + ID_OTE_GRIP_ESTOP + i), hInst, NULL);
			};//TOROLLY IDはパス
		}
		else {
			for (LONGLONG i = 0; i <= 8; i++) {
				if (i == 0) {
					//st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][j* N_OTE_NOTCH_ARRAY] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][j * N_OTE_NOTCH_ARRAY], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][j * N_OTE_NOTCH_ARRAY] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][j * N_OTE_NOTCH_ARRAY], WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | BS_PUSHLIKE,
						st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][j * N_OTE_NOTCH_ARRAY + i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][j * N_OTE_NOTCH_ARRAY + i].y,
						st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][j * N_OTE_NOTCH_ARRAY].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][j * N_OTE_NOTCH_ARRAY].cy,
						hWnd, (HMENU)(BASE_ID_OTE_NOTCH + j * N_OTE_NOTCH_ARRAY + i), hInst, NULL);
				}
				else {
					//st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][j * N_OTE_NOTCH_ARRAY+i] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][j * N_OTE_NOTCH_ARRAY + i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE ,
					st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][j * N_OTE_NOTCH_ARRAY + i] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][j * N_OTE_NOTCH_ARRAY + i], WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | BS_PUSHLIKE,
						st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][j * N_OTE_NOTCH_ARRAY + i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][j * N_OTE_NOTCH_ARRAY + i].y,
						st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][j * N_OTE_NOTCH_ARRAY].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][j * N_OTE_NOTCH_ARRAY].cy,
						hWnd, (HMENU)(BASE_ID_OTE_NOTCH + j * N_OTE_NOTCH_ARRAY + i), hInst, NULL);
				}
			}
		}
	}

	//初期値セット
	SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_ESTOP], BM_SETCHECK, BST_CHECKED, 0L);
	st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_OTE_GRIP_SWITCH] |= ID_OTE_GRIP_ESTOP_POS;
	SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_NOTCH], BM_SETCHECK, BST_UNCHECKED, 0L);
	st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_OTE_GRIP_SWITCH] &= ~ID_OTE_GRIP_NOTCH_POS;
	SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_RMT], BM_SETCHECK, BST_CHECKED, 0L);
	st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_OTE_GRIP_SWITCH] |= ID_OTE_GRIP_RMT_POS;

	return;
}
void disp_msg_cnt() {
	msg_wos.str(L"");
	msg_wos << L"OTE0     " << L"SOU:" << pCOte0->cnt_snd_ote_u << L"  RPU:"<< pCOte0->cnt_rcv_pc_u << L"  SOMO:" << pCOte0->cnt_snd_ote_m_ote << L"  ROM:" << pCOte0->cnt_rcv_ote_m  << L"  SOMP:" << pCOte0->cnt_snd_ote_m_pc << L"  RPM:"<<pCOte0->cnt_rcv_pc_m;
	SetWindowText(hWnd_work, msg_wos.str().c_str());

	return;
}

//デバッグ用
static double rad_bh = 0.0, rad_sl = 0.0;
static double mh_r = 60.0, ah_r = 62.0, mh_lim_min = 21.0, mh_lim_max = 57.0;
static double rad_ah_off = spec.rad_mh_ah - PI180;
static double mhx,ahx, mhy = 10.0, ahy = 10.0;
static INT pix_level0 = OTE0_GR_AREA2_Y + OTE0_GR_AREA2_H - 50;//グラフィックの0高さ

void draw_graphic() {

	//MEM0に書き込み
	//AREA1
	rad_bh = pCOte0->data.deg_bh * RAD1DEG;
	rad_sl = pCOte0->data.pos[ID_SLEW];
	mhy = pCOte0->data.pos[ID_HOIST];
	ahy = pCOte0->data.pos[ID_AHOIST];

	double srad_bh = sin(rad_bh), crad_bh = cos(rad_bh);		//sin cos 使い回し用
	double srad_sl = sin(rad_sl), crad_sl = cos(rad_sl);		//sin cos 使い回し用
	double ah_r_add = spec.La_add * cos(rad_bh + rad_ah_off);	//主補巻シーブ相対高さ
	
	mh_r = spec.Lm * crad_bh;	//主巻吊点ジブ元との相対距離
	ah_r = mh_r + ah_r_add;		//補巻吊点ジブ元との相対距離

	//吊点
	//補巻PIX位置
	INT px_ah_x = OTE0_GR_AREA_CX + (INT)(ah_r * crad_sl * OTE0_GR_AREA_PIX1M), px_ah_y = OTE0_GR_AREA_CY - (INT)(ah_r * srad_sl * OTE0_GR_AREA_PIX1M);					//先端位置
	INT px_ah_x2 = OTE0_GR_AREA_CX + (INT)(ah_r * crad_sl * 0.5 * OTE0_GR_AREA_PIX1M), px_ah_y2 = OTE0_GR_AREA_CY - (INT)(ah_r * srad_sl * 0.5 * OTE0_GR_AREA_PIX1M);	//中間位置
	//主巻PIX位置
	INT px_mh_x = OTE0_GR_AREA_CX + (INT)(mh_r * crad_sl * OTE0_GR_AREA_PIX1M), px_mh_y = OTE0_GR_AREA_CY - (INT)(mh_r * srad_sl * OTE0_GR_AREA_PIX1M);

	INT px_ddx = -(INT)(8.0 * srad_sl), px_ddy = (INT)(8.0 * crad_sl);		//吊点描画オフセット量 　吊点幅分
	INT px_ddx0 = -(INT)(3.0 * srad_sl), px_ddy0 = (INT)(3.0 * crad_sl);	//ジブ描画オフセット量 　ジブ先幅分

	//AREA1
	// 
	
	//ガイド円
	if ((pCOte0->data.auto_mode == OTE_ID_AUTOSTAT_ACTIVE) && pCOte0->data.auto_sel[ID_BOOM_H] && pCOte0->data.auto_sel[ID_SLEW]) {
		st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->FillEllipse(st_work_wnd.pbrush[OTE0_COLOR_AUTO_ACTIVE], OTE0_GR_AREA_CX - OTE0_GR_AREA_R, OTE0_GR_AREA_CY - OTE0_GR_AREA_R, OTE0_GR_AREA_R * 2, OTE0_GR_AREA_R * 2);
	}
	else if ((pCOte0->data.auto_mode == OTE_ID_AUTOSTAT_STANDBY) && pCOte0->data.auto_sel[ID_BOOM_H] && pCOte0->data.auto_sel[ID_SLEW]){
		st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->FillEllipse(st_work_wnd.pbrush[OTE0_COLOR_AUTO_STANDBY], OTE0_GR_AREA_CX - OTE0_GR_AREA_R, OTE0_GR_AREA_CY - OTE0_GR_AREA_R, OTE0_GR_AREA_R * 2, OTE0_GR_AREA_R * 2);
	}
	else {
		st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->FillEllipse(st_work_wnd.pbrush[OTE0_COLOR_AUTO_OFF], OTE0_GR_AREA_CX - OTE0_GR_AREA_R, OTE0_GR_AREA_CY - OTE0_GR_AREA_R, OTE0_GR_AREA_R * 2, OTE0_GR_AREA_R * 2);
	}
	//極限描画
	st_work_wnd.ppen[OTE0_RED]->SetWidth(2.0); st_work_wnd.ppen[OTE0_RED]->SetDashStyle(DashStyleDash); st_work_wnd.ppen[OTE0_RED]->SetColor(Color(50,255,0,0));
	INT px_mhr = (INT)(mh_lim_min * OTE0_GR_AREA_PIX1M), px_mhd=2*px_mhr;	//極限半径をPIXに変換
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawEllipse(st_work_wnd.ppen[OTE0_RED],OTE0_GR_AREA_CX - px_mhr, OTE0_GR_AREA_CY - px_mhr, px_mhd, px_mhd);
	px_mhr = (INT)(mh_lim_max * OTE0_GR_AREA_PIX1M); px_mhd = 2 * px_mhr;
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawEllipse(st_work_wnd.ppen[OTE0_RED], OTE0_GR_AREA_CX - px_mhr, OTE0_GR_AREA_CY - px_mhr, px_mhd, px_mhd);

	//JC脚部ライン及びポータル部
	st_work_wnd.pbrush[OTE0_BLUE]->SetColor(Color(255, 200, 200, 255));
	st_work_wnd.ppen[OTE0_BLUE]->SetColor(Color(255, 150, 150, 255));
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->FillRectangle(st_work_wnd.pbrush[OTE0_BLUE], OTE0_GR_AREA_CX - 12, OTE0_GR_AREA_CY - 15, 24, 30);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawLine(st_work_wnd.ppen[OTE0_BLUE], OTE0_GR_AREA_CX - 45, OTE0_GR_AREA_CY - 14, OTE0_GR_AREA_CX + 45, OTE0_GR_AREA_CY - 14);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawLine(st_work_wnd.ppen[OTE0_BLUE], OTE0_GR_AREA_CX - 45, OTE0_GR_AREA_CY + 14, OTE0_GR_AREA_CX + 45, OTE0_GR_AREA_CY + 14);

	//JIB 6角形で表現　補巻位置まで描画
	Point pts[] = { Point(px_ah_x - px_ddx0, px_ah_y + px_ddy0),Point(px_ah_x + px_ddx0, px_ah_y - px_ddy0),Point(px_ah_x2 + px_ddx0*2, px_ah_y2 - px_ddy0*2),Point(OTE0_GR_AREA_CX + px_ddx0 *2, OTE0_GR_AREA_CY - px_ddy0 *2),Point(OTE0_GR_AREA_CX - px_ddx0 *2, OTE0_GR_AREA_CY + px_ddy0 *2),Point(px_ah_x2 - px_ddx0*2, px_ah_y2 + px_ddy0*2)};
	st_work_wnd.pbrush[OTE0_RED]->SetColor(Color(100, 255, 0, 0));//赤ブラシの色を仮変更
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawPolygon(st_work_wnd.ppen[OTE0_GREEN], pts,6);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->FillPolygon(st_work_wnd.pbrush[OTE0_RED], pts, 6);

	//JIB 先
	//PEN設定（幅6　Solid
	st_work_wnd.ppen[OTE0_BLUE]->SetWidth(6.0); st_work_wnd.ppen[OTE0_BLUE]->SetDashStyle(DashStyleSolid); st_work_wnd.ppen[OTE0_BLUE]->SetColor(Color(255, 0, 0, 255));
	//線で描画
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawLine(st_work_wnd.ppen[OTE0_BLUE], px_mh_x + px_ddx, px_mh_y - px_ddy, px_mh_x - px_ddx, px_mh_y + px_ddy);

	//ポスト
	st_work_wnd.pbrush[OTE0_BLUE]->SetColor(Color(255, 150, 150, 255));
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->FillEllipse(st_work_wnd.pbrush[OTE0_BLUE], OTE0_GR_AREA_CX - 10, OTE0_GR_AREA_CY - 10, 18, 18);

	//走行位置描画
	st_work_wnd.pbrush[OTE0_BLUE]->SetColor(Color(255, 200, 200, 255));
	INT posx = (INT)pCOte0->data.pos[ID_GANTRY];
	px_ddx = (INT)(pCOte0->data.pos[ID_BOOM_H] * crad_sl), px_ddy = (INT)(pCOte0->data.pos[ID_BOOM_H] * srad_sl);

	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->FillRectangle(st_work_wnd.pbrush[OTE0_BLUE], OTE0_GR_AREA_X + 10 + posx - 5, OTE0_GR_AREA_Y + OTE0_GR_AREA_H - 20, 10, 10);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawLine(st_work_wnd.ppen[OTE0_MAZENDA], OTE0_GR_AREA_X + 10 + posx, OTE0_GR_AREA_Y + OTE0_GR_AREA_H - 20 + 5
		, OTE0_GR_AREA_X + 10 + posx + px_ddx, OTE0_GR_AREA_Y + OTE0_GR_AREA_H - 20 + 5 - px_ddy);
	st_work_wnd.pbrush[OTE0_BLUE]->SetColor(Color(255, 0, 0, 255));

	st_work_wnd.pbrush[OTE0_BLUE]->SetColor(Color(255, 0, 0, 255));
	st_work_wnd.ppen[OTE0_BLUE]->SetColor(Color(255, 0, 0, 255));

	//走行レール(1m/pix)
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawLine(st_work_wnd.ppen[OTE0_GLAY], OTE0_GR_AREA_X + 10, OTE0_GR_AREA_Y + OTE0_GR_AREA_H - 20
		, OTE0_GR_AREA_X + 310, OTE0_GR_AREA_Y + OTE0_GR_AREA_H - 20);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawLine(st_work_wnd.ppen[OTE0_GLAY], OTE0_GR_AREA_X + 10, OTE0_GR_AREA_Y + OTE0_GR_AREA_H - 10
		, OTE0_GR_AREA_X + 310, OTE0_GR_AREA_Y + OTE0_GR_AREA_H - 10);

	//目標位置
	if ((pCOte0->data.auto_sel[ID_BOOM_H]) && (pCOte0->data.auto_sel[ID_SLEW]))
		st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->FillEllipse(st_work_wnd.pbrush[OTE0_MAZENDA], 
			(INT)pCOte0->data.pt_tgpos[OTE_ID_HOT_TARGET][ID_BOOM_H].x - 4, (INT)pCOte0->data.pt_tgpos[OTE_ID_HOT_TARGET][ID_BOOM_H].y -4,
			8, 8);

	//AREA2
	//JIB
	//転写先エリア計算
	//左上を中心に回転　右上のポイントシフト量を計算
	LONG dx1 = (LONG)((double)st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_DST_ARR].Width * crad_bh), dy1 = -(LONG)((double)st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_DST_ARR].Width * srad_bh);
	//左上を中心に回転　左下のポイントシフト量を計算
	LONG dx2 = (LONG)((double)st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_DST_ARR].Height * srad_bh), dy2 = (LONG)((double)st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_DST_ARR].Height * crad_bh);
	//PT0
	st_work_wnd.pt_imtg[OTE0_GRID_JC_JIB][0].x = st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_DST_ARR].X;
	st_work_wnd.pt_imtg[OTE0_GRID_JC_JIB][0].y = st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_DST_ARR].Y;
	//PT1
	st_work_wnd.pt_imtg[OTE0_GRID_JC_JIB][1].x = st_work_wnd.pt_imtg[OTE0_GRID_JC_JIB][0].x + dx1;
	st_work_wnd.pt_imtg[OTE0_GRID_JC_JIB][1].y = st_work_wnd.pt_imtg[OTE0_GRID_JC_JIB][0].y + dy1;
	//PT2
	st_work_wnd.pt_imtg[OTE0_GRID_JC_JIB][2].x = st_work_wnd.pt_imtg[OTE0_GRID_JC_JIB][0].x + dx2;
	st_work_wnd.pt_imtg[OTE0_GRID_JC_JIB][2].y = st_work_wnd.pt_imtg[OTE0_GRID_JC_JIB][0].y + dy2;

	PlgBlt(	st_work_wnd.hdc[ID_OTE_HDC_MEM0], st_work_wnd.pt_imtg[OTE0_GRID_JC_JIB],//転写先
			st_work_wnd.hdc[ID_OTE_HDC_MEM_GR], st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_SRC_ARR].X, st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_SRC_ARR].Y, st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_SRC_ARR].Width, st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_SRC_ARR].Height, 
			NULL, NULL, NULL);

	//フック描画

	//現在高さライン描画
	INT px_mh_h = (INT)(mhy * OTE0_GR_AREA2_PIX1M);
	INT px_ah_h = (INT)(ahy * OTE0_GR_AREA2_PIX1M);

	//st_work_wnd.ppen[OTE0_RED]->SetWidth(6.0); st_work_wnd.ppen[OTE0_RED]->SetDashStyle(DashStyleSolid); st_work_wnd.ppen[OTE0_RED]->SetColor(Color(255, 255, 0, 0));
	//線で描画
	st_work_wnd.ppen[OTE0_BLUE]->SetWidth(2.0);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawLine(st_work_wnd.ppen[OTE0_BLUE], OTE0_GR_AREA2_MH_SET_X-20, OTE0_GR_AREA2_LV0_Y - px_mh_h, OTE0_GR_AREA2_MH_SET_X-20 + OTE0_GR_AREA2_MH_SET_W, OTE0_GR_AREA2_LV0_Y - px_mh_h);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawLine(st_work_wnd.ppen[OTE0_BLUE], OTE0_GR_AREA2_AH_SET_X - 10, OTE0_GR_AREA2_LV0_Y - px_ah_h, OTE0_GR_AREA2_AH_SET_X - 10 + OTE0_GR_AREA2_AH_SET_W, OTE0_GR_AREA2_LV0_Y - px_ah_h);


	px_mh_x = OTE0_GR_AREA2_R0_X + (INT)(mh_r * OTE0_GR_AREA2_PIX1M) +5;//吊具画像幅の半分
	px_mh_y = OTE0_GR_AREA2_LV0_Y - px_mh_h;
	px_ah_x = OTE0_GR_AREA2_R0_X + (INT)(ah_r * OTE0_GR_AREA2_PIX1M) +3;//吊具画像幅の半分
	px_ah_y = OTE0_GR_AREA2_LV0_Y - px_ah_h;
		
	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].X = px_mh_x; st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].Y = px_mh_y;
	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].X = px_ah_x; st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].Y = px_ah_y;

	BitBlt(st_work_wnd.hdc[ID_OTE_HDC_MEM0], st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].X, st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].Y, st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].Width, st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].Height,
		st_work_wnd.hdc[ID_OTE_HDC_MEM_GR], st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_SRC_ARR].X, st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_SRC_ARR].Y, SRCCOPY);
	BitBlt(st_work_wnd.hdc[ID_OTE_HDC_MEM0], st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].X, st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].Y, st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].Width, st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].Height,
		st_work_wnd.hdc[ID_OTE_HDC_MEM_GR], st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_SRC_ARR].X, st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_SRC_ARR].Y, SRCCOPY);


	//巻目標位置設定ガイドライン

	if ((pCOte0->data.auto_mode == OTE_ID_AUTOSTAT_ACTIVE) && pCOte0->data.auto_sel[ID_HOIST]) {
		st_work_wnd.ppen[OTE0_COLOR_AUTO_ACTIVE]->SetWidth(OTE0_GR_AREA2_MH_SET_W);
		st_work_wnd.pgraphic[ID_OTE_HDC_MEM0]->DrawLine(st_work_wnd.ppen[OTE0_COLOR_AUTO_ACTIVE], OTE0_GR_AREA2_MH_SET_X, OTE0_GR_AREA2_MH_SET_Y, OTE0_GR_AREA2_MH_SET_X, OTE0_GR_AREA2_MH_SET_Y + OTE0_GR_AREA2_MH_SET_H - 5);
	}
	else if ((pCOte0->data.auto_mode == OTE_ID_AUTOSTAT_STANDBY) && pCOte0->data.auto_sel[ID_HOIST]) {
		st_work_wnd.ppen[OTE0_COLOR_AUTO_STANDBY]->SetWidth(OTE0_GR_AREA2_MH_SET_W);
		st_work_wnd.pgraphic[ID_OTE_HDC_MEM0]->DrawLine(st_work_wnd.ppen[OTE0_COLOR_AUTO_STANDBY], OTE0_GR_AREA2_MH_SET_X, OTE0_GR_AREA2_MH_SET_Y, OTE0_GR_AREA2_MH_SET_X, OTE0_GR_AREA2_MH_SET_Y + OTE0_GR_AREA2_MH_SET_H - 5);
	}
	else {
		st_work_wnd.ppen[OTE0_COLOR_AUTO_OFF]->SetWidth(OTE0_GR_AREA2_MH_SET_W);
		st_work_wnd.pgraphic[ID_OTE_HDC_MEM0]->DrawLine(st_work_wnd.ppen[OTE0_COLOR_AUTO_OFF], OTE0_GR_AREA2_MH_SET_X, OTE0_GR_AREA2_MH_SET_Y, OTE0_GR_AREA2_MH_SET_X, OTE0_GR_AREA2_MH_SET_Y + OTE0_GR_AREA2_MH_SET_H - 5);
	}


	if ((pCOte0->data.auto_mode == OTE_ID_AUTOSTAT_ACTIVE) && pCOte0->data.auto_sel[ID_AHOIST] ) {
		st_work_wnd.pgraphic[ID_OTE_HDC_MEM0]->DrawLine(st_work_wnd.ppen[OTE0_COLOR_AUTO_ACTIVE], OTE0_GR_AREA2_AH_SET_X, OTE0_GR_AREA2_AH_SET_Y, OTE0_GR_AREA2_AH_SET_X, OTE0_GR_AREA2_AH_SET_Y + OTE0_GR_AREA2_AH_SET_H - 5);
		st_work_wnd.ppen[OTE0_COLOR_AUTO_ACTIVE]->SetWidth(OTE0_GR_AREA2_AH_SET_W);
	}
	else if ((pCOte0->data.auto_mode == OTE_ID_AUTOSTAT_STANDBY) && pCOte0->data.auto_sel[ID_AHOIST]) {
		st_work_wnd.ppen[OTE0_COLOR_AUTO_STANDBY]->SetWidth(OTE0_GR_AREA2_AH_SET_W);
		st_work_wnd.pgraphic[ID_OTE_HDC_MEM0]->DrawLine(st_work_wnd.ppen[OTE0_COLOR_AUTO_STANDBY], OTE0_GR_AREA2_AH_SET_X, OTE0_GR_AREA2_AH_SET_Y, OTE0_GR_AREA2_AH_SET_X, OTE0_GR_AREA2_AH_SET_Y + OTE0_GR_AREA2_AH_SET_H - 5);
	}
	else {
		st_work_wnd.ppen[OTE0_COLOR_AUTO_OFF]->SetWidth(OTE0_GR_AREA2_AH_SET_W);
		st_work_wnd.pgraphic[ID_OTE_HDC_MEM0]->DrawLine(st_work_wnd.ppen[OTE0_COLOR_AUTO_OFF], OTE0_GR_AREA2_AH_SET_X, OTE0_GR_AREA2_AH_SET_Y, OTE0_GR_AREA2_AH_SET_X, OTE0_GR_AREA2_AH_SET_Y + OTE0_GR_AREA2_AH_SET_H - 5);
	}
	//巻目標位置ライン
	if(pCOte0->data.auto_sel[ID_HOIST])
		st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawLine(st_work_wnd.ppen[OTE0_MAZENDA], OTE0_GR_AREA2_MH_SET_X - 20, pCOte0->data.pt_tgpos[OTE_ID_HOT_TARGET][ID_HOIST].y, OTE0_GR_AREA2_MH_SET_X - 20 + OTE0_GR_AREA2_MH_SET_W/2, pCOte0->data.pt_tgpos[OTE_ID_HOT_TARGET][ID_HOIST].y);
	if (pCOte0->data.auto_sel[ID_AHOIST])
		st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawLine(st_work_wnd.ppen[OTE0_MAZENDA], OTE0_GR_AREA2_AH_SET_X - 10, pCOte0->data.pt_tgpos[OTE_ID_HOT_TARGET][ID_AHOIST].y, OTE0_GR_AREA2_AH_SET_X - 10 + OTE0_GR_AREA2_AH_SET_W/2, pCOte0->data.pt_tgpos[OTE_ID_HOT_TARGET][ID_AHOIST].y);
	//	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawLine(st_work_wnd.ppen[OTE0_MAZENDA], OTE0_GR_AREA2_MH_SET_X - 20, OTE0_GR_AREA2_LV0_Y - px_mh_h, OTE0_GR_AREA2_MH_SET_X - 20 + OTE0_GR_AREA2_MH_SET_W, OTE0_GR_AREA2_LV0_Y - px_mh_h);
	//	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawLine(st_work_wnd.ppen[OTE0_MAZENDA], OTE0_GR_AREA2_AH_SET_X - 10, OTE0_GR_AREA2_LV0_Y - px_ah_h, OTE0_GR_AREA2_AH_SET_X - 10 + OTE0_GR_AREA2_AH_SET_W, OTE0_GR_AREA2_LV0_Y - px_ah_h);
}
void init_graphic() {

	//クレーングラフィック素材描画 DC GRAPHIC
	//クレーンボディ
	long x = 0, y = 0, w = 80, h = 200;
	st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_SRC_ARR].X = x; st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_SRC_ARR].Y = y;
	st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_SRC_ARR].Width = w; st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_SRC_ARR].Height = h;

	st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_DST_ARR].X = OTE0_GR_AREA2_X + 30; st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_DST_ARR].Y = OTE0_GR_AREA2_Y + 40;
	st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_DST_ARR].Width = w; st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_DST_ARR].Height = h;
	st_work_wnd.pgraphic[OTE0_GDIP_GR_GRAPHIC]->DrawImage(st_work_wnd.pimg[OTE0_GRID_JC_BODY], st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_SRC_ARR]);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_BK]->DrawImage(st_work_wnd.pimg[OTE0_GRID_JC_BODY], st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_SRC_ARR]);

	//
	//クレーンジブ　フック
	y += h; w = 20, h = 25;
	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_SRC_ARR].X = x;		st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_SRC_ARR].Y = y;
	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_SRC_ARR].Width = w;	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_SRC_ARR].Height = h;

	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].X = OTE0_GR_AREA2_X + 200;	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].Y = OTE0_GR_AREA2_Y + 150;
	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].Width = w;					st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].Height = h;

	st_work_wnd.pgraphic[OTE0_GDIP_GR_GRAPHIC]->DrawImage(st_work_wnd.pimg[OTE0_GRID_JC_HOOK1], st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_SRC_ARR]);

	y += h; w = 12, h = 16;
	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_SRC_ARR].X = x;		st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_SRC_ARR].Y = y;
	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_SRC_ARR].Width = w;	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_SRC_ARR].Height = h;

	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].X = OTE0_GR_AREA2_X + 230;	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].Y = OTE0_GR_AREA2_Y + 180;
	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].Width = w;					st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].Height = h;

	st_work_wnd.pgraphic[OTE0_GDIP_GR_GRAPHIC]->DrawImage(st_work_wnd.pimg[OTE0_GRID_JC_HOOK2], st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_SRC_ARR]);

	//クレーンジブ　
	y += h; w = 189, h = 20;//ジブ長さ63→189PIX　1m→3PIX
	st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_SRC_ARR].X = x;						st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_SRC_ARR].Y = y;
	st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_SRC_ARR].Width = w;					st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_SRC_ARR].Height = h;

	st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_DST_ARR].X = OTE0_GR_AREA2_X + 100;	st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_DST_ARR].Y = OTE0_GR_AREA2_Y + 110;
	st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_DST_ARR].Width = w;					st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_DST_ARR].Height = h;

	st_work_wnd.pgraphic[OTE0_GDIP_GR_GRAPHIC]->DrawImage(st_work_wnd.pimg[OTE0_GRID_JC_JIB], st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_SRC_ARR]);

	//背景描画 DC BK
	//マップ背景ライン描画
	SelectObject(st_work_wnd.hdc[ID_OTE_HDC_MEM_BK], st_work_wnd.hpen[OTE0_GLAY]);
	SelectObject(st_work_wnd.hdc[ID_OTE_HDC_MEM_BK], GetStockBrush(NULL_BRUSH));
	Rectangle(st_work_wnd.hdc[ID_OTE_HDC_MEM_BK], OTE0_GR_AREA_X, OTE0_GR_AREA_Y, OTE0_GR_AREA_X + OTE0_GR_AREA_W, OTE0_GR_AREA_Y + OTE0_GR_AREA_H);
	Rectangle(st_work_wnd.hdc[ID_OTE_HDC_MEM_BK], OTE0_GR_AREA2_X, OTE0_GR_AREA2_Y, OTE0_GR_AREA2_X + OTE0_GR_AREA2_W, OTE0_GR_AREA2_Y + OTE0_GR_AREA2_H);

	//カメラ映像領域描画
	SelectObject(st_work_wnd.hdc[ID_OTE_HDC_MEM_BK], GetStockObject(GRAY_BRUSH));
	Rectangle(st_work_wnd.hdc[ID_OTE_HDC_MEM_BK], OTE0_CAM2_WND_X, OTE0_CAM2_WND_Y, OTE0_CAM2_WND_W + OTE0_CAM2_WND_X, OTE0_CAM2_WND_H);

	//AREA1 背景グラフィック


	//AREA2 背景グラフィック
	BitBlt(st_work_wnd.hdc[ID_OTE_HDC_MEM_BK], st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_DST_ARR].X, st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_DST_ARR].Y, st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_DST_ARR].Width, st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_DST_ARR].Height,
		st_work_wnd.hdc[ID_OTE_HDC_MEM_GR], st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_SRC_ARR].X, st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_SRC_ARR].Y, SRCCOPY);
	

	return;
}
void draw_info() {
	HDC hdc = st_work_wnd.hdc[ID_OTE_HDC_MEM0];
	wstring ws;
	wostringstream wo_msg;

	SetTextColor(hdc, RGB(100, 100, 100));
	SelectObject(hdc, st_work_wnd.hfont[ID_OTE_FONT12]);

	
	wo_msg.str(L""); wo_msg << L"主巻m   :" << pCOte0->data.pos[ID_HOIST]; TextOutW(hdc, OTE0_IF_AREA_X, OTE0_IF_AREA_Y, wo_msg.str().c_str(), (int)wo_msg.str().length());
	wo_msg.str(L""); wo_msg << L"補巻m   :" << pCOte0->data.pos[ID_AHOIST]; TextOutW(hdc, OTE0_IF_AREA_X, OTE0_IF_AREA_Y+15, wo_msg.str().c_str(), (int)wo_msg.str().length());

	wo_msg.str(L""); wo_msg << L"起伏角°:" << std::setprecision(4) << pCOte0->data.deg_bh; TextOutW(hdc, OTE0_IF_AREA_X, OTE0_IF_AREA_Y + 30, wo_msg.str().c_str(), (int)wo_msg.str().length());
	wo_msg.str(L""); wo_msg << L"旋回径m :" << pCOte0->data.pos[ID_BOOM_H]; TextOutW(hdc, OTE0_IF_AREA_X, OTE0_IF_AREA_Y + 45, wo_msg.str().c_str(), (int)wo_msg.str().length());
	wo_msg.str(L""); wo_msg << L"走行m   :" << pCOte0->data.pos[ID_GANTRY]; TextOutW(hdc, OTE0_IF_AREA_X, OTE0_IF_AREA_Y + 60, wo_msg.str().c_str(), (int)wo_msg.str().length());
	wo_msg.str(L"");
	wo_msg << L"主巻(t):" << pCOte0->data.load[ID_HOIST];
	TextOutW(hdc, OTE0_IF_AREA_X, OTE0_IF_AREA_Y + 80, wo_msg.str().c_str(), (int)wo_msg.str().length());
	wo_msg.str(L"");
	wo_msg << L"補巻(t):" << pCOte0->data.load[ID_AHOIST];
	TextOutW(hdc, OTE0_IF_AREA_X, OTE0_IF_AREA_Y + 95, wo_msg.str().c_str(), (int)wo_msg.str().length());
	
	wo_msg.str(L"");
	wo_msg << L"PAD Y:" << pad_data.lY;
	TextOutW(hdc, OTE0_IF_AREA_X, OTE0_IF_AREA_Y + 110, wo_msg.str().c_str(), (int)wo_msg.str().length());
	wo_msg.str(L"");
	wo_msg << L"PAD X:" << pad_data.lX;
	TextOutW(hdc, OTE0_IF_AREA_X, OTE0_IF_AREA_Y + 125, wo_msg.str().c_str(), (int)wo_msg.str().length());
	

	wo_msg.str(L""); wo_msg << L"自動目標";
	TextOutW(hdc, OTE0_GR_AREA2_X + 250, OTE0_GR_AREA2_Y + 5, wo_msg.str().c_str(), (int)wo_msg.str().length());
	wo_msg.str(L""); wo_msg  << L"主巻:"  << pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_HOIST];
	TextOutW(hdc, OTE0_GR_AREA2_X + 250, OTE0_GR_AREA2_Y + 20, wo_msg.str().c_str(), (int)wo_msg.str().length());
	wo_msg.str(L""); wo_msg << L"補巻 :" << pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_AHOIST];
	TextOutW(hdc, OTE0_GR_AREA2_X + 250, OTE0_GR_AREA2_Y + 35, wo_msg.str().c_str(), (int)wo_msg.str().length());

	wo_msg.str(L""); wo_msg << L"自動目標";
	TextOutW(hdc, OTE0_GR_AREA_X + 5, OTE0_GR_AREA_Y + 5, wo_msg.str().c_str(), (int)wo_msg.str().length());
	wo_msg.str(L""); wo_msg << L"半径:" << pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_BOOM_H] ;
	TextOutW(hdc, OTE0_GR_AREA_X + 5, OTE0_GR_AREA_Y + 20, wo_msg.str().c_str(), (int)wo_msg.str().length());
	wo_msg.str(L""); wo_msg << L"旋回:" << pCOte0->data.d_tgpos[OTE_ID_HOT_TARGET][ID_SLEW] * DEG1RAD;
	TextOutW(hdc, OTE0_GR_AREA_X + 5, OTE0_GR_AREA_Y + 35, wo_msg.str().c_str(), (int)wo_msg.str().length());



}

#define OTE_SWY_WND_PIX_X	5	//OTEの1PIXに対応するカメラの画素数
#define OTE_SWY_WND_PIX_Y	5	//OTEの1PIXに対応するカメラの画素数 
#define OTE_SWY_WND_PIX_X0	140	//OTE WINDOWのセンター位置
#define OTE_SWY_WND_PIX_Y0	120	//OTE WINDOWのセンター位置 
#define OTE_SWY_WND_TG_D	10	//OTEでのターゲット直径
#define OTE_SWY_WND_TG_R	5	//OTEでのターゲット半径
#define OTE_SWY_WND_TG1_X0  130
#define OTE_SWY_WND_TG2_X0  140
#define OTE_SWY_WND_TG1_Y0  115
#define OTE_SWY_WND_TG2_Y0  115

void draw_graphic_swy() {
	HDC hdc = st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM_GR];
	

	//主巻
	st_work_wnd.pgraphic[OTE0_GDIP_GR_SWY_M0]->FillEllipse(st_work_wnd.pbrush[OTE0_RED],
		OTE_SWY_WND_TG1_X0 + pCOte0->st_msg_pc_u_rcv.body.swy_cam_pix[OTE_ID_LOAD_MH][OTE_ID_SWY_AXIS_X]/ OTE_SWY_WND_PIX_X,
		OTE_SWY_WND_TG1_Y0 - pCOte0->st_msg_pc_u_rcv.body.swy_cam_pix[OTE_ID_LOAD_MH][OTE_ID_SWY_AXIS_Y]/ OTE_SWY_WND_PIX_Y,
		OTE_SWY_WND_TG_D, 
		OTE_SWY_WND_TG_D);

	st_work_wnd.pgraphic[OTE0_GDIP_GR_SWY_M0]->FillEllipse(st_work_wnd.pbrush[OTE0_GREEN], 
		OTE_SWY_WND_TG2_X0 + pCOte0->st_msg_pc_u_rcv.body.swy_cam_pix[OTE_ID_LOAD_MH][OTE_ID_SWY_AXIS_X]/ OTE_SWY_WND_PIX_X,
		OTE_SWY_WND_TG2_Y0 - pCOte0->st_msg_pc_u_rcv.body.swy_cam_pix[OTE_ID_LOAD_MH][OTE_ID_SWY_AXIS_Y]/ OTE_SWY_WND_PIX_Y,
		OTE_SWY_WND_TG_D, 
		OTE_SWY_WND_TG_D);

	//補巻

	st_work_wnd.pgraphic[OTE0_GDIP_GR_SWY_M0]->FillEllipse(st_work_wnd.pbrush[OTE0_BLUE],
		OTE_SWY_WND_TG1_X0 + pCOte0->st_msg_pc_u_rcv.body.swy_cam_pix[OTE_ID_LOAD_AH][OTE_ID_SWY_AXIS_X] / OTE_SWY_WND_PIX_X,
		OTE_SWY_WND_TG1_Y0 - pCOte0->st_msg_pc_u_rcv.body.swy_cam_pix[OTE_ID_LOAD_AH][OTE_ID_SWY_AXIS_Y] / OTE_SWY_WND_PIX_Y,
		OTE_SWY_WND_TG_D,
		OTE_SWY_WND_TG_D);

	st_work_wnd.pgraphic[OTE0_GDIP_GR_SWY_M0]->FillEllipse(st_work_wnd.pbrush[OTE0_YELLOW],
		OTE_SWY_WND_TG2_X0 + pCOte0->st_msg_pc_u_rcv.body.swy_cam_pix[OTE_ID_LOAD_AH][OTE_ID_SWY_AXIS_X] / OTE_SWY_WND_PIX_X,
		OTE_SWY_WND_TG2_Y0 - pCOte0->st_msg_pc_u_rcv.body.swy_cam_pix[OTE_ID_LOAD_AH][OTE_ID_SWY_AXIS_Y] / OTE_SWY_WND_PIX_Y,
		OTE_SWY_WND_TG_D,
		OTE_SWY_WND_TG_D);
}
void draw_bk_swy() {
	
	st_work_wnd.pgraphic[OTE0_GDIP_GR_SWY_BK]->DrawLine(st_work_wnd.ppen[OTE0_GLAY], 0, OTE0_SWY_WND_H / 2, OTE0_SWY_WND_W, OTE0_SWY_WND_H / 2);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_SWY_BK]->DrawLine(st_work_wnd.ppen[OTE0_GLAY], OTE0_SWY_WND_W/2, 0, OTE0_SWY_WND_W/ 2, OTE0_SWY_WND_H );
	
	
	HDC hdc = st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM_BK];
	wstring ws;
	ws = L"SWAY";
	TextOutW(hdc, 0, 0, ws.c_str(), (int)ws.length());

}
/// <summary>
/// ランプの描画
/// </summary>
void draw_lamp(HDC hdc,bool is_init) {

	SelectObject(hdc, GetStockObject(NULL_PEN));

	//PBランプ
	for (int i = ID_OTE_PB_TEISHI; i <= ID_OTE_CHK_N3; i++) {
		if (pCOte0->st_msg_pc_u_rcv.body.pb_lamp[i].com == OTE_LAMP_COM_ON) {
			SelectObject(hdc, st_work_wnd.hbrush[pCOte0->st_msg_pc_u_rcv.body.pb_lamp[i].color]);
		}
		else if (pCOte0->st_msg_pc_u_rcv.body.pb_lamp[i].com == OTE_LAMP_COM_FLICK) {
			if(disp_cnt & OTE_LAMP_FLICK_COUNT)SelectObject(hdc, st_work_wnd.hbrush[pCOte0->st_msg_pc_u_rcv.body.pb_lamp[i].color]);
			else SelectObject(hdc, st_work_wnd.hbrush[OTE0_GLAY]);		//OFF色
		}
		else {
			SelectObject(hdc, st_work_wnd.hbrush[OTE0_GLAY]);		//OFF色
		}

		RECT rc = st_work_wnd.pb_rect[i];
		Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

	}

	//NOTCHランプ
	for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
		for (int j = 0; j < N_OTE_NOTCH_ARRAY-1; j++) {
			int k = i * N_OTE_NOTCH_ARRAY + j;
			if ((k >= ID_OTE_GRIP_PP) && (k <= ID_OTE_GRIP_ZN)) continue;

			if (pCOte0->st_msg_pc_u_rcv.body.notch_lamp[k].com == OTE_LAMP_COM_ON) {
				SelectObject(hdc, st_work_wnd.hbrush[pCOte0->st_msg_pc_u_rcv.body.notch_lamp[k].color]);
			}
			else if (pCOte0->st_msg_pc_u_rcv.body.notch_lamp[k].com == OTE_LAMP_COM_FLICK) {
				if (disp_cnt & OTE_LAMP_FLICK_COUNT)SelectObject(hdc, st_work_wnd.hbrush[pCOte0->st_msg_pc_u_rcv.body.notch_lamp[k].color]);
				else SelectObject(hdc, st_work_wnd.hbrush[OTE0_GLAY]);		//OFF色
			}
			else {
				SelectObject(hdc, st_work_wnd.hbrush[OTE0_GLAY]);		//OFF色
			}

			RECT rc = st_work_wnd.notch_rect[i][j];
			Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
		}

	}
		
	return;
} 
void create_objects(HWND hWnd) {
	st_work_wnd.hbrush[OTE0_WHITE]				= CreateSolidBrush(RGB(255, 255, 255));
	st_work_wnd.hbrush[OTE0_GLAY]				= CreateSolidBrush(RGB(192, 192, 192));
	st_work_wnd.hbrush[OTE0_RED]				= CreateSolidBrush(RGB(255, 0, 0));
	st_work_wnd.hbrush[OTE0_BLUE]				= CreateSolidBrush(RGB(0, 0, 255));
	st_work_wnd.hbrush[OTE0_GREEN]				= CreateSolidBrush(RGB(0, 255, 0));
	st_work_wnd.hbrush[OTE0_YELLOW]				= CreateSolidBrush(RGB(255, 253, 85));
	st_work_wnd.hbrush[OTE0_MAZENDA]			= CreateSolidBrush(RGB(234, 63, 247));
	st_work_wnd.hbrush[OTE0_ORANGE]				= CreateSolidBrush(RGB(255, 142, 85));
	st_work_wnd.hbrush[OTE0_COLOR_AUTO_OFF]		= CreateSolidBrush(RGB(255, 142, 85));
	st_work_wnd.hbrush[OTE0_COLOR_AUTO_STANDBY] = CreateSolidBrush(RGB(255, 142, 85));
	st_work_wnd.hbrush[OTE0_COLOR_AUTO_ACTIVE]	= CreateSolidBrush(RGB(255, 142, 85));
	st_work_wnd.hbrush[OTE0_COLOR_BACK_GROUND]  = CreateSolidBrush(RGB(240, 240, 240));
	

	st_work_wnd.hpen[OTE0_WHITE]				= CreatePen(PS_SOLID,2,RGB(255, 255, 255));
	st_work_wnd.hpen[OTE0_GLAY]					= CreatePen(PS_SOLID, 2, RGB(192, 192, 192));
	st_work_wnd.hpen[OTE0_RED]					= CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	st_work_wnd.hpen[OTE0_BLUE]					= CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
	st_work_wnd.hpen[OTE0_GREEN]				= CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
	st_work_wnd.hpen[OTE0_YELLOW]				= CreatePen(PS_SOLID, 2, RGB(255, 253, 85));
	st_work_wnd.hpen[OTE0_MAZENDA]				= CreatePen(PS_SOLID, 2, RGB(234, 63, 247));
	st_work_wnd.hpen[OTE0_ORANGE]				= CreatePen(PS_SOLID, 2, RGB(255, 142, 85));

	st_work_wnd.pbrush[OTE0_WHITE]				= new SolidBrush(Color(255,255, 255, 255));
	st_work_wnd.pbrush[OTE0_GLAY]				= new SolidBrush(Color(255,192, 192, 192));
	st_work_wnd.pbrush[OTE0_RED]				= new SolidBrush(Color(255,255, 0, 0));
	st_work_wnd.pbrush[OTE0_BLUE]				= new SolidBrush(Color(255,0, 0, 255));
	st_work_wnd.pbrush[OTE0_GREEN]				= new SolidBrush(Color(255,0, 255, 0));
	st_work_wnd.pbrush[OTE0_YELLOW]				= new SolidBrush(Color(255,255, 253, 85));
	st_work_wnd.pbrush[OTE0_MAZENDA]			= new SolidBrush(Color(255,234, 63, 247));
	st_work_wnd.pbrush[OTE0_ORANGE]				= new SolidBrush(Color(255,255, 142, 85));
	st_work_wnd.pbrush[OTE0_COLOR_AUTO_OFF]		= new SolidBrush(Color(50, 182, 255, 0));
	st_work_wnd.pbrush[OTE0_COLOR_AUTO_STANDBY] = new SolidBrush(Color(50, 255, 216, 0));
	st_work_wnd.pbrush[OTE0_COLOR_AUTO_ACTIVE]	= new SolidBrush(Color(50, 234, 63, 247));
	st_work_wnd.pbrush[OTE0_COLOR_BACK_GROUND] = new SolidBrush(Color(50, 240, 240, 240));
		

	st_work_wnd.ppen[OTE0_WHITE]				= new Pen(Color(255, 255, 255, 255), 2);
	st_work_wnd.ppen[OTE0_GLAY]					= new Pen(Color(255, 192, 192, 192), 2);
	st_work_wnd.ppen[OTE0_RED]					= new Pen(Color(255, 255, 0, 0), 2);
	st_work_wnd.ppen[OTE0_BLUE]					= new Pen(Color(255, 0, 0, 255), 2);
	st_work_wnd.ppen[OTE0_GREEN]				= new Pen(Color(255, 0, 255, 0), 2);
	st_work_wnd.ppen[OTE0_YELLOW]				= new Pen(Color(255, 255, 253, 85), 2);
	st_work_wnd.ppen[OTE0_MAZENDA]				= new Pen(Color(255, 234, 63, 247), 2);
	st_work_wnd.ppen[OTE0_ORANGE]				= new Pen(Color(255, 255, 142, 85), 2);
	st_work_wnd.ppen[OTE0_COLOR_AUTO_OFF]		= new Pen(Color(50, 182, 255, 0),2);
	st_work_wnd.ppen[OTE0_COLOR_AUTO_STANDBY]	= new Pen(Color(50, 255, 216, 0),2);
	st_work_wnd.ppen[OTE0_COLOR_AUTO_ACTIVE]	= new Pen(Color(50, 234, 63, 247), 2);
	st_work_wnd.ppen[OTE0_COLOR_BACK_GROUND]	= new Pen(Color(50, 240, 240, 240), 2);

	//表示フォント設定
	st_work_wnd.hfont[ID_OTE_FONT8] = CreateFont(8, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));
	st_work_wnd.hfont[ID_OTE_FONT6] = CreateFont(6, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));
	st_work_wnd.hfont[ID_OTE_FONT12] = CreateFont(12, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));
	st_work_wnd.hfont[ID_OTE_FONT20] = CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));
	st_work_wnd.hfont[ID_OTE_FONT10] = CreateFont(10, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));


	HDC hdc = GetDC(hWnd);
	st_work_wnd.hBmap[ID_OTE_HBMAP_BK] = CreateCompatibleBitmap(hdc, st_work_wnd.area_w, st_work_wnd.area_h);
	st_work_wnd.hdc[ID_OTE_HDC_MEM_BK] = CreateCompatibleDC(hdc);
	SelectObject(st_work_wnd.hdc[ID_OTE_HDC_MEM_BK], st_work_wnd.hBmap[ID_OTE_HBMAP_BK]);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_BK] = new Graphics(st_work_wnd.hdc[ID_OTE_HDC_MEM_BK]);

	st_work_wnd.hBmap[ID_OTE_HBMAP_MEM0] = CreateCompatibleBitmap(hdc, st_work_wnd.area_w, st_work_wnd.area_h);
	st_work_wnd.hdc[ID_OTE_HDC_MEM0] = CreateCompatibleDC(hdc);
	SelectObject(st_work_wnd.hdc[ID_OTE_HDC_MEM0], st_work_wnd.hBmap[ID_OTE_HBMAP_MEM0]);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0] = new Graphics(st_work_wnd.hdc[ID_OTE_HDC_MEM0]);

	st_work_wnd.hBmap[ID_OTE_HBMAP_MEM_IF] = CreateCompatibleBitmap(hdc, st_work_wnd.area_w, st_work_wnd.area_h);
	st_work_wnd.hdc[ID_OTE_HDC_MEM_IF] = CreateCompatibleDC(hdc);
	SelectObject(st_work_wnd.hdc[ID_OTE_HDC_MEM_IF], st_work_wnd.hBmap[ID_OTE_HBMAP_MEM_IF]);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_INF] = new Graphics(st_work_wnd.hdc[ID_OTE_HDC_MEM_IF]);

	st_work_wnd.hBmap[ID_OTE_HBMAP_MEM_GR] = CreateCompatibleBitmap(hdc, st_work_wnd.area_w, st_work_wnd.area_h);
	st_work_wnd.hdc[ID_OTE_HDC_MEM_GR] = CreateCompatibleDC(hdc);
	SelectObject(st_work_wnd.hdc[ID_OTE_HDC_MEM_GR], st_work_wnd.hBmap[ID_OTE_HBMAP_MEM_GR]);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_GRAPHIC] = new Graphics(st_work_wnd.hdc[ID_OTE_HDC_MEM_GR]);

	PatBlt(st_work_wnd.hdc[ID_OTE_HDC_MEM_BK], 0, 0, st_work_wnd.area_w, st_work_wnd.area_h, WHITENESS);
	PatBlt(st_work_wnd.hdc[ID_OTE_HDC_MEM0], 0, 0, st_work_wnd.area_w, st_work_wnd.area_h, WHITENESS);
	PatBlt(st_work_wnd.hdc[ID_OTE_HDC_MEM_IF], 0, 0, st_work_wnd.area_w, st_work_wnd.area_h, WHITENESS);
	PatBlt(st_work_wnd.hdc[ID_OTE_HDC_MEM_GR], 0, 0, st_work_wnd.area_w, st_work_wnd.area_h, WHITENESS);

	ReleaseDC(hWnd_work, hdc);
}
void delete_objects(HWND hWnd) {
	for (int i = 0; i < N_OTE_BRUSH; i++) {
		DeleteObject(st_work_wnd.hbrush[i]);
		delete st_work_wnd.pbrush[i];
	}
	for (int i = 0; i < N_OTE_PEN; i++) {
		DeleteObject(st_work_wnd.hpen[i]);
		delete st_work_wnd.ppen[i];
	}
	for(int i = 0; i < N_OTE_HDC; i++) 	DeleteDC(st_work_wnd.hdc[i]);
	for (int i = 0; i < N_OTE_HBMAP; i++)	DeleteObject(st_work_wnd.hBmap[i]);
	for (int i = 0; i < N_OTE_FONT; i++)	DeleteObject(st_work_wnd.hfont[i]);
}

void COte::parse_auto_status() {

	if(pCOte0->st_msg_pc_u_rcv.body.pb_lamp[ID_OTE_PB_AUTO].com == OTE_LAMP_COM_FLICK)
		data.auto_mode = OTE_ID_AUTOSTAT_ACTIVE;
	else if(pCOte0->st_msg_pc_u_rcv.body.pb_lamp[ID_OTE_PB_AUTO].color != OTE0_GREEN)
		data.auto_mode = OTE_ID_AUTOSTAT_STANDBY;	
	else 
		data.auto_mode = OTE_ID_AUTOSTAT_OFF;
	
	if (pCOte0->st_msg_pc_u_rcv.body.pb_lamp[ID_OTE_PB_FUREDOME].com == OTE_LAMP_COM_FLICK)
		data.anti_sway_mode = OTE_ID_AUTOSTAT_ACTIVE;
	else if (pCOte0->st_msg_pc_u_rcv.body.pb_lamp[ID_OTE_PB_FUREDOME].color != OTE0_GREEN)
		data.anti_sway_mode = OTE_ID_AUTOSTAT_STANDBY;
	else
		data.anti_sway_mode = OTE_ID_AUTOSTAT_OFF;
	return;
}

void COte::update_auto_target_touch(int area, int x, int y) {
	if (data.auto_mode == OTE_ID_AUTOSTAT_STANDBY) {
		switch (area) {
		case OTE0_ID_AREA_GR_BHSL: {
			double d_x = (double)(x - OTE0_GR_AREA_CX);
			double d_y = (double)(OTE0_GR_AREA_CY - y );
			double r = sqrt(d_x * d_x + d_y * d_y);
			double rad =0.0;
			if (d_x == 0) {
				if (d_y >= 0) rad = PI90;
				else rad=-PI90;
			}
			else if (d_x >0.0) {
				rad = atan(d_y/d_x);
			}
			else {
				
				if (d_y < 0.0)	rad = -PI180 + atan(d_y / d_x);
				else			rad = PI180 + atan(d_y / d_x);
			}
			if ((data.auto_sel[ID_BOOM_H])&&(data.auto_sel[ID_SLEW])) {
				data.pt_tgpos[OTE_ID_HOT_TARGET][OTE0_ID_AREA_GR_BHSL] = { x,y };
				data.d_tgpos[OTE_ID_HOT_TARGET][ID_BOOM_H] = r * OTE0_GR_AREA_M1PIX;
				data.d_tgpos[OTE_ID_HOT_TARGET][ID_SLEW] = rad;
			}
		}break;
		case OTE0_ID_AREA_GR_MH: {
			if (data.auto_sel[ID_HOIST]) {
				data.pt_tgpos[OTE_ID_HOT_TARGET][OTE0_ID_AREA_GR_MH] = { x,y };
				data.d_tgpos[OTE_ID_HOT_TARGET][ID_HOIST] = (double)(OTE0_GR_AREA2_LV0_Y - y)* OTE0_GR_AREA2_M1PIX;
			}
		}break;
		case OTE0_ID_AREA_GR_AH: {
			if (data.auto_sel[ID_AHOIST]) {
				data.pt_tgpos[OTE_ID_HOT_TARGET][OTE0_ID_AREA_GR_AH] = { x,y };
				data.d_tgpos[OTE_ID_HOT_TARGET][ID_AHOIST] = (double)(OTE0_GR_AREA2_LV0_Y - y) * OTE0_GR_AREA2_M1PIX;
			}
		}break;
		default:break;
		}
	}
	return; 
}

POINT COte::cal_gr_pos_from_d_pos(int motion, double HorR, double Slew) {
	POINT ans = { 0,0 };
	if (motion == ID_HOIST) {
		ans.x = OTE0_GR_AREA2_MH_SET_X;
		ans.y = OTE0_GR_AREA2_LV0_Y - LONG(HorR * OTE0_GR_AREA2_PIX1M);
	}
	else if (motion == ID_AHOIST) {
		ans.x = OTE0_GR_AREA2_AH_SET_X;
		ans.y = OTE0_GR_AREA2_LV0_Y - LONG(HorR * OTE0_GR_AREA2_PIX1M);
	}
	else if ((motion == ID_BOOM_H) || (motion == ID_SLEW)) {
		double x = HorR * cos(Slew) * OTE0_GR_AREA_PIX1M;
		double y = HorR * sin(Slew) * OTE0_GR_AREA_PIX1M;
		ans.x = (LONG)x + OTE0_GR_AREA_CX;
		ans.y = -(LONG)y + OTE0_GR_AREA_CY;
	}
	else;

	return ans;
}
