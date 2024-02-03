﻿// OTE0.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "OTE0.h"
#include "OTE0panel.h"
#include "CPsaMain.h"
#include "PLC_DEF.h"
#include "COTE0_GR.h"
#include <Xinput.h>
#include "spec.h"

#include <windowsx.h>       //# コモンコントロール
#include <winsock2.h>
#include <iostream>
#include <iomanip>
#include <sstream>

//@ GDI+
#include <objbase.h>//gdiplus.hのコンパイルを通すために必要
#include <gdiplus.h>
using namespace Gdiplus;

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
HWND hwnd_camera;

COte* pCOte0;					//OTE0オブジェクト
CPsaMain* pPSA;					//PSApi処理用オブジェクト

//@GDI+
GdiplusStartupInput gdiSI;
ULONG_PTR           gdiToken;


static INT16 disp_cnt=0;
static bool is_init_disp = true;

static std::wostringstream msg_wos;

static ST_SPEC spec;

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);

HWND open_connect_Wnd(HWND hwnd);
HWND open_mode_Wnd(HWND hwnd);
HWND open_fault_Wnd(HWND hwnd);
HWND open_status_Wnd(HWND hwnd);
HWND open_auto_Wnd(HWND hwnd);
HWND open_swy_Wnd(HWND hwnd); 
HWND open_camera_Wnd(HWND hwnd);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndConnectProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndAutoProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndModeProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndFaultProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndStatusProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndSwyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndCamProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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
	   if (i == ID_SLEW) {
		   rc_add.left = 0; rc_add.top = PRM_OTE_DEF_PB_H; rc_add.right = PRM_OTE_DEF_PB_W2; rc_add.bottom = PRM_OTE_DEF_PB_H/2;
	   }
	   else if (i == ID_GANTRY) {
		   rc_add.left = 0; rc_add.top = -PRM_OTE_DEF_PB_H/2; rc_add.right = PRM_OTE_DEF_PB_W2; rc_add.bottom = PRM_OTE_DEF_PB_H/2;
	   }
	   else if (i == ID_OTE_GRIP_SWITCH) {
			rc_add.left = 0; rc_add.top = PRM_OTE_DEF_PB_H; rc_add.right = PRM_OTE_DEF_PB_W; rc_add.bottom = PRM_OTE_DEF_PB_H / 2;

	   }
	   else {
		   rc_add.left = -PRM_OTE_DEF_PB_W2/3; rc_add.top = 0; rc_add.right = PRM_OTE_DEF_PB_W2 / 3; rc_add.bottom = PRM_OTE_DEF_PB_H;
	   }
	   for (int j = 0; j < N_OTE_NOTCH_ARRAY; j++) {
		   if ((i == ID_OTE_GRIP_SWITCH) && (j > 2))continue;
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
		hwnd_current_subwnd = open_fault_Wnd(hWnd);			//故障表示子ウィンドウ

		open_swy_Wnd(hWnd);									//振れウィンドウ追加

		//マルチキャストタイマ起動 
		SetTimer(hWnd, ID_OTE_MULTICAST_TIMER, OTE_MULTICAST_SCAN_MS, NULL);
		//ユニキャストタイマ起動
		SetTimer(hWnd, ID_OTE_UNICAST_TIMER, OTE_UNICAST_SCAN_MS, NULL);

	}break;
	case WM_TIMER: {

		pCOte0->parse();//受信データ展開

		if (wParam == ID_OTE_UNICAST_TIMER) {
			set_lamp();

			//ON PAINT　呼び出し　表示更新
			InvalidateRect(hWnd, NULL, FALSE);
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

		//操作入力状態取り込み更新（グリップスイッチetc）
		INT16 mask = 1;
		for (int i = 0; i < N_OTE_NOTCH_ARRAY; i++) {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_ESTOP + i], BM_GETCHECK, 0, 0)) {
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_OTE_GRIP_SWITCH] |= mask;
				st_work_wnd.notch_pb_stat[ID_OTE_GRIP_ESTOP + i] = OTE0_PB_OFF_DELAY_COUNT;
			}
			else {
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_OTE_GRIP_SWITCH] &= ~mask;
				st_work_wnd.notch_pb_stat[ID_OTE_GRIP_ESTOP + i] = L_OFF;
			}
			mask=mask << 1;
		}
		//リモート無効時
		if (BST_CHECKED != SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_OTE_GRIP_RMT], BM_GETCHECK, 0, 0)) {
			for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
				if (i == ID_OTE_GRIP_SWITCH)continue;//	グリップスイッチは対象外
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][i] = st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_TRIG][i] = ID_OTE_0NOTCH_POS;	//0ノッチで初期化
			}
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
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_HOIST] = wmId - ID_OTE_NOTCH_MH_MIN;
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_TRIG][ID_HOIST] = wmId - ID_OTE_NOTCH_MH_MIN;
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_CNT][ID_HOIST] = OTE0_PB_OFF_DELAY_COUNT;
			}
			else if (wmId < ID_OTE_NOTCH_GT_MAX) {
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_GANTRY] = wmId - ID_OTE_NOTCH_GT_MIN;
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_TRIG][ID_GANTRY] = wmId - ID_OTE_NOTCH_GT_MIN;
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_CNT][ID_GANTRY] = OTE0_PB_OFF_DELAY_COUNT;
			}
			else if (wmId < ID_OTE_NOTCH_GR_MAX);//グリップスイッチはスルー チェックボックスを定周期チェック
			else if (wmId < ID_OTE_NOTCH_BH_MAX) {
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_BOOM_H] = wmId - ID_OTE_NOTCH_BH_MIN;
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_TRIG][ID_BOOM_H] = wmId - ID_OTE_NOTCH_BH_MIN;
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_CNT][ID_BOOM_H] = OTE0_PB_OFF_DELAY_COUNT;
			}
			else if (wmId < ID_OTE_NOTCH_SL_MAX) {
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_SLEW] = wmId - ID_OTE_NOTCH_SL_MIN;
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_TRIG][ID_SLEW] = wmId - ID_OTE_NOTCH_SL_MIN;
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_CNT][ID_SLEW] = OTE0_PB_OFF_DELAY_COUNT;
			}
			else {
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_AHOIST] = wmId - ID_OTE_NOTCH_AH_MIN;
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_TRIG][ID_AHOIST] = wmId - ID_OTE_NOTCH_AH_MIN;
				st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_CNT][ID_AHOIST] = OTE0_PB_OFF_DELAY_COUNT;
			}
		}
		//SUB WINDOW選択　RADIO PB
		if ((wmId >= BASE_ID_OTE_PB + ID_OTE_RADIO_COM) && (wmId <= BASE_ID_OTE_PB + ID_OTE_RADIO_STAT)) {
			DestroyWindow(hwnd_current_subwnd);
			switch (st_work_wnd.subpanel_sel = wmId - BASE_ID_OTE_PB) {
			case ID_OTE_RADIO_COM:hwnd_current_subwnd = open_connect_Wnd(hWnd); break;
			case ID_OTE_RADIO_MODE:hwnd_current_subwnd = open_mode_Wnd(hWnd); break;
			case ID_OTE_RADIO_FAULT:hwnd_current_subwnd = open_fault_Wnd(hWnd); break;
			case ID_OTE_RADIO_STAT:hwnd_current_subwnd = open_status_Wnd(hWnd); break;
			default:break;
			}
		}
		//表示カメラ選択　RADIO PB
		if ((wmId >= BASE_ID_OTE_PB + ID_OTE_RADIO_WIDE) && (wmId <= BASE_ID_OTE_PB + ID_OTE_RADIO_OPE2)) {
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
			}
			break;
		}
		case BASE_ID_OTE_PB + ID_OTE_CHK_ASET_MH:
		case BASE_ID_OTE_PB + ID_OTE_CHK_ASET_AH:
		case BASE_ID_OTE_PB + ID_OTE_CHK_ASET_BH:
		case BASE_ID_OTE_PB + ID_OTE_CHK_ASET_SL: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][wmId - BASE_ID_OTE_PB], BM_GETCHECK, 0, 0))
				st_work_wnd.pb_lamp[wmId - BASE_ID_OTE_PB].com = L_ON;
			else
				st_work_wnd.pb_lamp[wmId - BASE_ID_OTE_PB].com = L_OFF;
			break;
		}
		//カメラウィンドウ表示
		case BASE_ID_OTE_PB + ID_OTE_CHK_CAMERA_WND: {
			if (BST_UNCHECKED == SendMessage(st_work_wnd.hctrl[ID_OTE_CTRL_PB][wmId - BASE_ID_OTE_PB], BM_GETCHECK, 0, 0)) 
				DestroyWindow(hwnd_camera);
			else 
				open_camera_Wnd(hWnd);
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
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);


		PatBlt(st_work_wnd.hdc[ID_OTE_HDC_MEM0], 0, 0, st_work_wnd.area_w, st_work_wnd.area_h, WHITENESS);
		draw_graphic();
		draw_info();
	
		if(disp_cnt== 1)init_graphic();
		draw_lamp(hdc, is_init_disp);
		
		//Windowに合成画像を書き込み
		LONG cpyw = OTE0_GR_AREA2_X + OTE0_GR_AREA2_W - OTE0_GR_AREA_X;//MEM DCからのコピー幅
		BitBlt(hdc, OTE0_GR_AREA_X, OTE0_GR_AREA_Y, cpyw, OTE0_GR_AREA_H, st_work_wnd.hdc[ID_OTE_HDC_MEM_BK], OTE0_GR_AREA_X, OTE0_GR_AREA_Y, SRCCOPY);
		TransparentBlt(hdc, OTE0_GR_AREA_X, OTE0_GR_AREA_Y, cpyw, OTE0_GR_AREA_H, st_work_wnd.hdc[ID_OTE_HDC_MEM0], OTE0_GR_AREA_X, OTE0_GR_AREA_Y, cpyw, OTE0_GR_AREA_H, RGB(255, 255, 255));
		BitBlt(hdc, OTE0_IF_AREA_X, OTE0_IF_AREA_Y, OTE0_IF_AREA_W, OTE0_IF_AREA_H,
			st_work_wnd.hdc[ID_OTE_HDC_MEM0], OTE0_IF_AREA_X, OTE0_IF_AREA_Y,SRCCOPY);

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

		draw_bk_swy();
		draw_graphic_swy();

		BitBlt(hdc, 0, 0, OTE0_SWY_WND_W, OTE0_SWY_WND_H, st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM_BK], 0, 0, SRCCOPY);
		//グラフィックを重ね合わせ
		TransparentBlt(	hdc, 0, 0, OTE0_SWY_WND_W, OTE0_SWY_WND_H,
			st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM0], 0, 0, OTE0_SWY_WND_W, OTE0_SWY_WND_H,
						RGB(255, 255, 255));

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

static HDC hCaptureDC;
static HBITMAP hCaptureBitmap;

LRESULT CALLBACK WndCamProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	int id;
	HINSTANCE hInst = GetModuleHandle(0);
	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		//マルチキャストタイマ起動 
		SetTimer(hWnd, ID_OTE_CAMERA_TIMER, OTE_CAMERA_SCAN_MS,NULL);

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

		// PSAPI処理オブジェクト
		pPSA = new CPsaMain();
		pPSA->init_psa(hWnd);

	}break;
	case WM_TIMER: {
			InvalidateRect(hWnd, NULL, FALSE);
		}

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
			else
				pPSA->ltilt = 0;
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
	}break;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
//画面切取サンプル
#if 0 
		// ウィンドウを透明にする
		SetLayeredWindowAttributes(hwnd_camera, RGB(0, 0, 0), 0, LWA_COLORKEY);

		// デスクトップ画面の一部を切り取る
		st_work_wnd.hdc[ID_OTE_HDC_CAMERA_VIEW] = GetDC(NULL);
		hCaptureDC = CreateCompatibleDC(st_work_wnd.hdc[ID_OTE_HDC_CAMERA_VIEW]);
		hCaptureBitmap = CreateCompatibleBitmap(st_work_wnd.hdc[ID_OTE_HDC_CAMERA_VIEW], 640, 480);
		SelectObject(hCaptureDC, hCaptureBitmap);
		BitBlt(hCaptureDC, 0, 0, 640, 480, st_work_wnd.hdc[ID_OTE_HDC_CAMERA_VIEW], OTE0_CAM_WND_TG_X, OTE0_CAM_WND_TG_Y, SRCCOPY);

		// 切り取った画像をウィンドウに表示する
	//	HDC hWindowDC = GetDC(hwnd_camera);
		BitBlt(hdc, 0, 0, 640, 480, hCaptureDC, 0, 0, SRCCOPY);

		// 後始末
//		SelectObject(hCaptureDC, hOldBitmap);

#endif
		EndPaint(hWnd, &ps);
	}break;
	case WM_DESTROY: {
		KillTimer(hWnd_work, ID_OTE_CAMERA_TIMER);
		DeleteObject(hCaptureBitmap);
		pPSA->OnClose();
		delete pPSA;
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
		OTE0_SWY_WND_X, OTE0_SWY_WND_Y, OTE0_SWY_WND_W, OTE0_SWY_WND_H,
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

	InvalidateRect(hWnd_sub[ID_OTE0_SWY_WND], NULL, TRUE);//表示更新

	ShowWindow(hWnd_sub[ID_OTE0_SWY_WND], SW_SHOW);
	UpdateWindow(hWnd_sub[ID_OTE0_SWY_WND]);

	return hWnd_sub[ID_OTE0_SWY_WND];
}
#if 1
HWND open_camera_Wnd(HWND hwnd) {
	InitCommonControls();//コモンコントロール初期化
	HINSTANCE hInst = GetModuleHandle(0);
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndCamProc;
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
	
	hwnd_camera = CreateWindowW(TEXT("CAMERA VIEW"), TEXT("CAMERA VIEW"), WS_POPUP | WS_BORDER | WS_OVERLAPPEDWINDOW,
			OTE0_CAM_WND_X, OTE0_CAM_WND_Y, OTE0_CAM_WND_W, OTE0_CAM_WND_H,
			hwnd, nullptr, hInst, nullptr);

	//デバイスコンテキスト
	HDC hdc = GetDC(hwnd_camera);
	TextOutW(hdc, 10, 10, L"<<Camera>>", 15);
	ReleaseDC(hwnd_camera, hdc);

	InvalidateRect(hwnd_camera, NULL, TRUE);//表示更新

	ShowWindow(hwnd_camera, SW_SHOW);
	UpdateWindow(hwnd_camera);

	return hwnd_camera;
}
#else
HWND open_camera_Wnd(HWND hwnd) {
	InitCommonControls();//コモンコントロール初期化
	HINSTANCE hInst = GetModuleHandle(0);
	// ウィンドウを作成
	hwnd_camera = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,

		L"STATIC",
		L"Desktop Capture",
		WS_POPUP,
		900, 650, 640, 480,
		NULL, NULL, NULL, NULL);

	// ウィンドウを透明にする
	SetLayeredWindowAttributes(hwnd_camera, RGB(0, 0, 255), 0, LWA_COLORKEY);

	// デスクトップ画面の一部を切り取る
	HDC hDC = GetDC(NULL);
	HDC hCaptureDC = CreateCompatibleDC(hDC);
	HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDC, 640, 480);
	HGDIOBJ hOldBitmap = SelectObject(hCaptureDC, hCaptureBitmap);
	BitBlt(hCaptureDC, 0, 0, 640, 480, hDC, 0, 0, SRCCOPY);

	// 切り取った画像をウィンドウに表示する
	HDC hWindowDC = GetDC(hwnd_camera);
	BitBlt(hWindowDC, 0, 0, 640, 480, hCaptureDC, 0, 0, SRCCOPY);

	// 後始末
	SelectObject(hCaptureDC, hOldBitmap);
	DeleteObject(hCaptureBitmap);
	DeleteDC(hCaptureDC);
	ReleaseDC(NULL, hDC);
	ReleaseDC(hwnd_camera, hWindowDC);


	// ウィンドウを表示
	ShowWindow(hwnd_camera, SW_SHOW);

	return hwnd_camera;
}
#endif

//*********************************************************************************************
/// <summary>
/// OTEウィンドウ上にオブジェクト配置
/// </summary>
void set_OTE_panel_objects(HWND hWnd) {

	HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

#if 0 グラフィック描画に変更
	//ランプ
	for (int i = ID_OTE_LAMP_HIJYOU; i <= ID_OTE_LAMP_N3; i++) {
		st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][i] = CreateWindowW(TEXT("STATIC"), st_work_wnd.ctrl_text[ID_OTE_CTRL_STATIC][i], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][i].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][i].cy,
			hWnd, (HMENU)(BASE_ID_OTE_STATIC + i), hInst, NULL);
	}
#endif
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
		for (LONGLONG i = ID_OTE_RADIO_WIDE; i <= ID_OTE_RADIO_STAT; i++) {
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
			for (int i = 0; i < 9; i++) {
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
	st_work_wnd.notch_pos[ID_OTE_NOTCH_POS_HOLD][ID_OTE_GRIP_RMT] |= ID_OTE_GRIP_RMT_POS;

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
#if 0 //デバッグ
	rad_bh = rad_bh + 0.01; if (rad_bh > 1.4)rad_bh = -0.8;
	rad_sl += 0.02; if (rad_sl > 6.28)rad_sl = 0.0;
	//mhy -= 0.5; if (mhy < -5.0)mhy = 50.0; ahy += 0.2; if (ahy > 50.0)ahy = -5.0;
#else
	rad_bh = pCOte0->data.deg_bh * DEG1RAD;
	rad_sl = pCOte0->data.pos[ID_SLEW];
	mhy = pCOte0->data.pos[ID_HOIST];
	ahy = pCOte0->data.pos[ID_AHOIST];

#endif
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
	//極限描画
	st_work_wnd.ppen[OTE0_RED]->SetWidth(2.0); st_work_wnd.ppen[OTE0_RED]->SetDashStyle(DashStyleDash); st_work_wnd.ppen[OTE0_RED]->SetColor(Color(50,255,0,0));
	INT px_mhr = (INT)(mh_lim_min * OTE0_GR_AREA_PIX1M), px_mhd=2*px_mhr;	//極限半径をPIXに変換
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawEllipse(st_work_wnd.ppen[OTE0_RED],OTE0_GR_AREA_CX - px_mhr, OTE0_GR_AREA_CY - px_mhr, px_mhd, px_mhd);
	px_mhr = (INT)(mh_lim_max * OTE0_GR_AREA_PIX1M); px_mhd = 2 * px_mhr;
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawEllipse(st_work_wnd.ppen[OTE0_RED], OTE0_GR_AREA_CX - px_mhr, OTE0_GR_AREA_CY - px_mhr, px_mhd, px_mhd);

	//JIB 6角形で表現　補巻位置まで描画
	Point pts[] = { Point(px_ah_x - px_ddx0, px_ah_y + px_ddy0),Point(px_ah_x + px_ddx0, px_ah_y - px_ddy0),Point(px_ah_x2 + px_ddx0*2, px_ah_y2 - px_ddy0*2),Point(OTE0_GR_AREA_CX + px_ddx0 *2, OTE0_GR_AREA_CY - px_ddy0 *2),Point(OTE0_GR_AREA_CX - px_ddx0 *2, OTE0_GR_AREA_CY + px_ddy0 *2),Point(px_ah_x2 - px_ddx0*2, px_ah_y2 + px_ddy0*2)};
	st_work_wnd.pbrush[OTE0_RED]->SetColor(Color(100, 255, 0, 0));//赤ブラシの色を仮変更
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawPolygon(st_work_wnd.ppen[OTE0_GREEN], pts,6);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->FillPolygon(st_work_wnd.pbrush[OTE0_RED], pts, 6);

	//JIB 先
	//PEN設定（幅6　Solid
	st_work_wnd.ppen[OTE0_RED]->SetWidth(6.0); st_work_wnd.ppen[OTE0_RED]->SetDashStyle(DashStyleSolid); st_work_wnd.ppen[OTE0_RED]->SetColor(Color(255, 255, 0, 0));
	//線で描画
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawLine(st_work_wnd.ppen[OTE0_RED], px_mh_x + px_ddx, px_mh_y - px_ddy, px_mh_x - px_ddx, px_mh_y + px_ddy);

	//ポスト
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->FillEllipse(st_work_wnd.pbrush[OTE0_BLUE], OTE0_GR_AREA_CX - 10, OTE0_GR_AREA_CY - 10, 18, 18);

	//走行位置描画
	st_work_wnd.pbrush[OTE0_BLUE]->SetColor(Color(255, 200, 200, 255));
	INT posx =(INT) pCOte0->data.pos[ID_GANTRY];
	px_ddx = (INT)(pCOte0->data.pos[ID_BOOM_H] * crad_sl), px_ddy = (INT)(pCOte0->data.pos[ID_BOOM_H] * srad_sl);

	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->FillRectangle(st_work_wnd.pbrush[OTE0_BLUE], OTE0_GR_AREA_X+ 10 + posx-5, OTE0_GR_AREA_Y + OTE0_GR_AREA_H - 20, 10, 10);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_M0]->DrawLine(st_work_wnd.ppen[OTE0_MAZENDA], OTE0_GR_AREA_X + 10 + posx, OTE0_GR_AREA_Y + OTE0_GR_AREA_H - 20 +5	
																					, OTE0_GR_AREA_X + 10 + posx + px_ddx, OTE0_GR_AREA_Y + OTE0_GR_AREA_H - 20 + 5 - px_ddy);
	st_work_wnd.pbrush[OTE0_BLUE]->SetColor(Color(255, 0, 0, 255));

	
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
	px_mh_x = OTE0_GR_AREA2_R0_X + (INT)(mh_r * OTE0_GR_AREA2_PIX1M) -3;
	px_mh_y = OTE0_GR_AREA2_LV0_Y - (INT)(mhy * OTE0_GR_AREA2_PIX1M);
	px_ah_x = OTE0_GR_AREA2_R0_X + (INT)(ah_r * OTE0_GR_AREA2_PIX1M) -0;
	px_ah_y = OTE0_GR_AREA2_LV0_Y - (INT)(ahy * OTE0_GR_AREA2_PIX1M);
	
	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].X = px_mh_x; st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].Y = px_mh_y;
	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].X = px_ah_x; st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].Y = px_ah_y;

	BitBlt(st_work_wnd.hdc[ID_OTE_HDC_MEM0], st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].X, st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].Y, st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].Width, st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].Height,
		st_work_wnd.hdc[ID_OTE_HDC_MEM_GR], st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_SRC_ARR].X, st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_SRC_ARR].Y, SRCCOPY);
	BitBlt(st_work_wnd.hdc[ID_OTE_HDC_MEM0], st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].X, st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].Y, st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].Width, st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].Height,
		st_work_wnd.hdc[ID_OTE_HDC_MEM_GR], st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_SRC_ARR].X, st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_SRC_ARR].Y, SRCCOPY);

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

	//
	//クレーンジブ　フック
	y += h; w = 20, h = 25;
	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_SRC_ARR].X = x; st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_SRC_ARR].Y = y;
	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_SRC_ARR].Width = w; st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_SRC_ARR].Height = h;

	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].X = OTE0_GR_AREA2_X + 200; st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].Y = OTE0_GR_AREA2_Y + 150;
	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].Width = w; st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_DST_ARR].Height = h;

	st_work_wnd.pgraphic[OTE0_GDIP_GR_GRAPHIC]->DrawImage(st_work_wnd.pimg[OTE0_GRID_JC_HOOK1], st_work_wnd.im_rect[OTE0_GRID_JC_HOOK1][OTE0_ID_GR_SRC_ARR]);

	y += h; w = 12, h = 16;
	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_SRC_ARR].X = x; st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_SRC_ARR].Y = y;
	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_SRC_ARR].Width = w; st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_SRC_ARR].Height = h;

	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].X = OTE0_GR_AREA2_X + 230; st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].Y = OTE0_GR_AREA2_Y + 180;
	st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].Width = w; st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_DST_ARR].Height = h;

	st_work_wnd.pgraphic[OTE0_GDIP_GR_GRAPHIC]->DrawImage(st_work_wnd.pimg[OTE0_GRID_JC_HOOK2], st_work_wnd.im_rect[OTE0_GRID_JC_HOOK2][OTE0_ID_GR_SRC_ARR]);

	//クレーンジブ　
	y += h; w = 189, h = 20;//ジブ長さ63→189PIX　1m→3PIX
	st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_SRC_ARR].X = x; st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_SRC_ARR].Y = y;
	st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_SRC_ARR].Width = w; st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_SRC_ARR].Height = h;

	st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_DST_ARR].X = OTE0_GR_AREA2_X + 100; st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_DST_ARR].Y = OTE0_GR_AREA2_Y + 110;
	st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_DST_ARR].Width = w; st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_DST_ARR].Height = h;

	st_work_wnd.pgraphic[OTE0_GDIP_GR_GRAPHIC]->DrawImage(st_work_wnd.pimg[OTE0_GRID_JC_JIB], st_work_wnd.im_rect[OTE0_GRID_JC_JIB][OTE0_ID_GR_SRC_ARR]);

	//背景描画 DC BK
	//マップ背景ライン描画
	SelectObject(st_work_wnd.hdc[ID_OTE_HDC_MEM_BK], st_work_wnd.hpen[OTE0_GLAY]);
	SelectObject(st_work_wnd.hdc[ID_OTE_HDC_MEM_BK], GetStockObject(NULL_BRUSH));
	Rectangle(st_work_wnd.hdc[ID_OTE_HDC_MEM_BK], OTE0_GR_AREA_X, OTE0_GR_AREA_Y, OTE0_GR_AREA_X + OTE0_GR_AREA_W, OTE0_GR_AREA_Y + OTE0_GR_AREA_H);
	Rectangle(st_work_wnd.hdc[ID_OTE_HDC_MEM_BK], OTE0_GR_AREA2_X, OTE0_GR_AREA2_Y, OTE0_GR_AREA2_X + OTE0_GR_AREA2_W, OTE0_GR_AREA2_Y + OTE0_GR_AREA2_H);

	//AREA1 背景グラフィック
	//ガイド円
	SolidBrush mySolidBrush(Color(255, 255, 247, 214));
	INT r = 120;INT dia = 2 * r;
	st_work_wnd.pgraphic[OTE0_GDIP_GR_BK]->FillEllipse(&mySolidBrush, OTE0_GR_AREA_CX-r, OTE0_GR_AREA_CY-r, dia,dia);

	//JC脚部ライン及びポータル部
	st_work_wnd.pbrush[OTE0_BLUE]->SetColor(Color(255, 200, 200, 255));
	st_work_wnd.ppen[OTE0_BLUE]->SetColor(Color(255, 150, 150, 255));

	st_work_wnd.pgraphic[OTE0_GDIP_GR_BK]->FillRectangle(st_work_wnd.pbrush[OTE0_BLUE], OTE0_GR_AREA_CX - 12, OTE0_GR_AREA_CY - 15, 24, 30);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_BK]->DrawLine(st_work_wnd.ppen[OTE0_BLUE], OTE0_GR_AREA_CX - 45, OTE0_GR_AREA_CY - 14, OTE0_GR_AREA_CX + 45, OTE0_GR_AREA_CY - 14);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_BK]->DrawLine(st_work_wnd.ppen[OTE0_BLUE], OTE0_GR_AREA_CX - 45, OTE0_GR_AREA_CY + 14, OTE0_GR_AREA_CX + 45, OTE0_GR_AREA_CY + 14);

	st_work_wnd.pbrush[OTE0_BLUE]->SetColor(Color(255, 0, 0, 255));
	st_work_wnd.ppen[OTE0_BLUE]->SetColor(Color(255, 0, 0, 255));

	//走行レール(1m/pix)
	st_work_wnd.pgraphic[OTE0_GDIP_GR_BK]->DrawLine(st_work_wnd.ppen[OTE0_GLAY],  OTE0_GR_AREA_X + 10, OTE0_GR_AREA_Y + OTE0_GR_AREA_H - 20
																				, OTE0_GR_AREA_X + 310, OTE0_GR_AREA_Y + OTE0_GR_AREA_H - 20);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_BK]->DrawLine(st_work_wnd.ppen[OTE0_GLAY], OTE0_GR_AREA_X + 10, OTE0_GR_AREA_Y + OTE0_GR_AREA_H - 10
																				, OTE0_GR_AREA_X + 310, OTE0_GR_AREA_Y + OTE0_GR_AREA_H - 10);
	//AREA2 背景グラフィック
	BitBlt(st_work_wnd.hdc[ID_OTE_HDC_MEM_BK], st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_DST_ARR].X, st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_DST_ARR].Y, st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_DST_ARR].Width, st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_DST_ARR].Height,
		st_work_wnd.hdc[ID_OTE_HDC_MEM_GR], st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_SRC_ARR].X, st_work_wnd.im_rect[OTE0_GRID_JC_BODY][OTE0_ID_GR_SRC_ARR].Y, SRCCOPY);
	
	//巻目標位置設定ガイドライン
	Pen Mypen(Color(100, 255, 216, 0), 40);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_BK]->DrawLine(&Mypen, OTE0_GR_AREA2_X + 150, OTE0_GR_AREA2_Y + 10, OTE0_GR_AREA2_X + 150, OTE0_GR_AREA2_Y + 240);
	Mypen.SetColor(Color(100, 182, 255, 0)); Mypen.SetWidth(20);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_BK]->DrawLine(&Mypen, OTE0_GR_AREA2_X + 200, OTE0_GR_AREA2_Y + 10, OTE0_GR_AREA2_X + 200, OTE0_GR_AREA2_Y + 240);

	return;
}
void draw_info() {
	HDC hdc = st_work_wnd.hdc[ID_OTE_HDC_MEM0];
	wstring ws;
	wostringstream wo_msg;

	SetTextColor(hdc, RGB(100, 100, 100));
	SelectObject(hdc, st_work_wnd.hfont[ID_OTE_FONT12]);
	wo_msg.str(L"");
	wo_msg << L"旋回角(deg):"<<pCOte0->data.deg_sl;
	TextOutW(hdc, OTE0_GR_AREA_X+5, OTE0_GR_AREA_Y+5, wo_msg.str().c_str(), (int)wo_msg.str().length());
	wo_msg.str(L"");
	wo_msg << L"旋回径(m):" << pCOte0->data.pos[ID_BOOM_H];
	TextOutW(hdc, OTE0_GR_AREA_X+5, OTE0_GR_AREA_Y+20, wo_msg.str().c_str(), (int)wo_msg.str().length());
	wo_msg.str(L"");
	wo_msg << L"走行位置(m):" << pCOte0->data.pos[ID_GANTRY];
	TextOutW(hdc, OTE0_GR_AREA_X + 200, OTE0_GR_AREA_Y + 215, wo_msg.str().c_str(), (int)wo_msg.str().length());

	wo_msg.str(L"");
	wo_msg << L"自動目標";
	TextOutW(hdc, OTE0_IF_AREA_X+5, OTE0_IF_AREA_Y+5, wo_msg.str().c_str(), (int)wo_msg.str().length());
	wo_msg.str(L"");
	wo_msg <<std::setprecision(4) << L"主巻:" << 50.0 << L" 半径:" << 36.3 << L" 旋回:" <<75.0 << L" 補巻:" << 70.0;
	TextOutW(hdc, OTE0_IF_AREA_X + 5, OTE0_IF_AREA_Y + 20, wo_msg.str().c_str(), (int)wo_msg.str().length());

	wo_msg.str(L"");
	wo_msg << L"主巻m:" << pCOte0->data.pos[ID_HOIST] << L" 補巻m:" << pCOte0->data.pos[ID_AHOIST];
	TextOutW(hdc, OTE0_GR_AREA2_X + 180, OTE0_GR_AREA2_Y + 185, wo_msg.str().c_str(), (int)wo_msg.str().length());
	wo_msg.str(L"");
	wo_msg <<  L"起伏角(deg):" << pCOte0->data.deg_bh;
	TextOutW(hdc, OTE0_GR_AREA2_X + 180, OTE0_GR_AREA2_Y + 200, wo_msg.str().c_str(), (int)wo_msg.str().length());

	wo_msg.str(L"");
	wo_msg << L"主巻荷重(t):" << pCOte0->data.load[ID_HOIST];
	TextOutW(hdc, OTE0_GR_AREA2_X + 180, OTE0_GR_AREA2_Y + 215, wo_msg.str().c_str(), (int)wo_msg.str().length());

	wo_msg.str(L"");
	wo_msg << L"補巻荷重(t):" << pCOte0->data.load[ID_AHOIST];
	TextOutW(hdc, OTE0_GR_AREA2_X + 180, OTE0_GR_AREA2_Y + 230, wo_msg.str().c_str(), (int)wo_msg.str().length());
}
void draw_graphic_swy() {
	HDC hdc = st_work_wnd.hdc[ID_OTE_HDC_SWY_MEM_GR];
	
	st_work_wnd.pgraphic[OTE0_GDIP_GR_SWY_M0]->FillEllipse(st_work_wnd.pbrush[OTE0_RED], OTE0_SWY_WND_W / 2-10, OTE0_SWY_WND_H / 2-5, 10, 10);
	st_work_wnd.pgraphic[OTE0_GDIP_GR_SWY_M0]->FillEllipse(st_work_wnd.pbrush[OTE0_GREEN], OTE0_SWY_WND_W / 2, OTE0_SWY_WND_H / 2-5, 10, 10);
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
/// ランプセット
/// </summary>
void set_lamp() {
	return;
}
/// <summary>
/// ランプの描画
/// </summary>
void draw_lamp(HDC hdc,bool is_init) {

	UINT32* plamp_com_hold, *plamp_com_now;
	UINT32  lamp_com_delta,icolor;
	
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

	st_work_wnd.pbrush[OTE0_WHITE]	= new SolidBrush(Color(255,255, 255, 255));
	st_work_wnd.pbrush[OTE0_GLAY]	= new SolidBrush(Color(255,192, 192, 192));
	st_work_wnd.pbrush[OTE0_RED]	= new SolidBrush(Color(255,255, 0, 0));
	st_work_wnd.pbrush[OTE0_BLUE]	= new SolidBrush(Color(255,0, 0, 255));
	st_work_wnd.pbrush[OTE0_GREEN]	= new SolidBrush(Color(255,0, 255, 0));
	st_work_wnd.pbrush[OTE0_YELLOW] = new SolidBrush(Color(255,255, 253, 85));
	st_work_wnd.pbrush[OTE0_MAZENDA]= new SolidBrush(Color(255,234, 63, 247));
	st_work_wnd.pbrush[OTE0_ORANGE] = new SolidBrush(Color(255,255, 142, 85));

	st_work_wnd.ppen[OTE0_WHITE]	= new Pen(Color(255, 255, 255, 255), 2);
	st_work_wnd.ppen[OTE0_GLAY]		= new Pen(Color(255, 192, 192, 192), 2);
	st_work_wnd.ppen[OTE0_RED]		= new Pen(Color(255, 255, 0, 0), 2);
	st_work_wnd.ppen[OTE0_BLUE]		= new Pen(Color(255, 0, 0, 255), 2);
	st_work_wnd.ppen[OTE0_GREEN]	= new Pen(Color(255, 0, 255, 0), 2);
	st_work_wnd.ppen[OTE0_YELLOW]	= new Pen(Color(255, 255, 253, 85), 2);
	st_work_wnd.ppen[OTE0_MAZENDA]	= new Pen(Color(255, 234, 63, 247), 2);
	st_work_wnd.ppen[OTE0_ORANGE]	= new Pen(Color(255, 255, 142, 85), 2);

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
