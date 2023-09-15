// PLC_IF.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "PLC_IF.h"
#include "CPLC_IF.h"

#include "CSharedMem.h"	    //# 共有メモリクラス

#include <windowsx.h>       //# コモンコントロール用
#include <commctrl.h>       //# コモンコントロール用

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

//# ステータスバーのウィンドウのハンドル
ST_KNL_MANAGE_SET    knl_manage_set;            //マルチスレッド管理用構造体
static ST_MAIN_WND stMainWnd;                   //メインウィンドウ操作管理用構造体
DWORD* psource_proc_counter=NULL;               //メインプロセスのヘルシーカウンタ


CPLC_IF* pProcObj;          //メイン処理オブジェクト:
ST_SPEC def_spec;

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//# ウィンドウにステータスバーを追加追加
HWND CreateStatusbarMain(HWND hWnd);

//# マルチメディアタイマイベントコールバック関数
VOID CALLBACK alarmHandlar(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);


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
    LoadStringW(hInstance, IDC_PLCIF, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PLCIF));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLCIF));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PLCIF);
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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
       MAIN_WND_INIT_POS_X, MAIN_WND_INIT_POS_Y,
       MAIN_WND_INIT_SIZE_W, MAIN_WND_INIT_SIZE_H,
       nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   
   psource_proc_counter = &(pProcObj->source_counter);  //ステータスバー表示用（ヘルシーカウント）
    
   // メインウィンドウのステータスバーに制御モード表示
   TCHAR tbuf[32];
   wsprintf(tbuf, L"mode:%04x", pProcObj->mode);
   SendMessage(stMainWnd.hWnd_status_bar, SB_SETTEXT, 0, (LPARAM)tbuf);

   // タスクループ処理起動マルチメディアタイマ起動
   {
       // --マルチメディアタイマ精度設定
       TIMECAPS wTc;//マルチメディアタイマ精度構造体
       if (timeGetDevCaps(&wTc, sizeof(TIMECAPS)) != TIMERR_NOERROR) 	return((DWORD)FALSE);
       knl_manage_set.mmt_resolution = MIN(MAX(wTc.wPeriodMin, TARGET_RESOLUTION), wTc.wPeriodMax);
       if (timeBeginPeriod(knl_manage_set.mmt_resolution) != TIMERR_NOERROR) return((DWORD)FALSE);

       _RPT1(_CRT_WARN, "MMTimer Period = %d\n", knl_manage_set.mmt_resolution);

       // --マルチメディアタイマセット
       knl_manage_set.KnlTick_TimerID = timeSetEvent(knl_manage_set.cycle_base, knl_manage_set.mmt_resolution, (LPTIMECALLBACK)alarmHandlar, 0, TIME_PERIODIC);

       // --マルチメディアタイマー起動失敗判定　メッセージBOX出してFALSE　returen
       if (knl_manage_set.KnlTick_TimerID == 0) {	 //失敗確認表示
           LPVOID lpMsgBuf;
           FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
               0, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language*/(LPTSTR)&lpMsgBuf, 0, NULL);
           MessageBox(NULL, (LPCWSTR)lpMsgBuf, L"MMT Failed!!", MB_OK | MB_ICONINFORMATION);// Display the string.
           LocalFree(lpMsgBuf);// Free the buffer.
           return((DWORD)FALSE);
       }
   }
      
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        InitCommonControls();//コモンコントロール初期化

        // メイン処理オブジェクトインスタンス化
        pProcObj = new CPLC_IF(hWnd);                        // メイン処理クラスのインスタンス化
        pProcObj->init_proc();                               // メイン処理クラスの初期化

        //メインウィンドウにステータスバー付加
        stMainWnd.hWnd_status_bar = CreateStatusbarMain(hWnd);
        SendMessage(stMainWnd.hWnd_status_bar, SB_SETTEXT, 0, (LPARAM)L"NORMAL");

        //メインウィンドウにコントロール追加
        stMainWnd.h_static0 = CreateWindowW(TEXT("STATIC"), L"-", WS_CHILD | WS_VISIBLE | SS_LEFT,
            10, 5, 180, 20, hWnd, (HMENU)IDC_STATIC_0,hInst, NULL);

        stMainWnd.h_pb_exit = CreateWindow(L"BUTTON", L"EXIT",  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            150, 65, 50, 25, hWnd, (HMENU)IDC_PB_EXIT, hInst, NULL);

        stMainWnd.h_chk_if = CreateWindow(L"BUTTON", L"IF CHK", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            5, 65, 80, 30, hWnd, (HMENU)IDC_CHK_IFCHK, hInst, NULL);
        SendMessage(stMainWnd.h_chk_if, BM_SETCHECK, BST_CHECKED, 0L);
        pProcObj->show_if_wnd();

        stMainWnd.h_redio_remote = CreateWindow(L"BUTTON", L"RMT", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
            5, 35, 40, 25, hWnd, (HMENU)IDC_RADIO_REMOTE, hInst, NULL);
        stMainWnd.h_redio_mon = CreateWindow(L"BUTTON", L"MON", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE ,
            50, 35, 40, 25, hWnd, (HMENU)IDC_RADIO_MON, hInst, NULL);
        stMainWnd.h_redio_sim = CreateWindow(L"BUTTON", L"SIM", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
            95, 35, 40, 25, hWnd, (HMENU)IDC_RADIO_SIM, hInst, NULL);
        pProcObj->set_mode(PLC_IF_REMOTE_MODE);
        SendMessage(stMainWnd.h_redio_remote, BM_SETCHECK, BST_CHECKED, 0L);
        SetWindowText(stMainWnd.h_static0, L"REMOTE MODE");
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 選択されたメニューの解析:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
        case IDC_PB_EXIT:
            DestroyWindow(hWnd);
            break;

        case IDC_RADIO_MON: {
            pProcObj->set_mode(PLC_IF_MONITOR_MODE);
            TCHAR tbuf[32];
            wsprintf(tbuf, L"mode:%04x", pProcObj->mode);
            SendMessage(stMainWnd.hWnd_status_bar, SB_SETTEXT, 0, (LPARAM)tbuf);
            SetWindowText(stMainWnd.h_static0, L"MONITOR MODE");
        }break;
        case IDC_RADIO_REMOTE: {
            pProcObj->set_mode(PLC_IF_REMOTE_MODE);
            TCHAR tbuf[32];
            wsprintf(tbuf, L"mode:%04x", pProcObj->mode);
            SendMessage(stMainWnd.hWnd_status_bar, SB_SETTEXT, 0, (LPARAM)tbuf);
            SetWindowText(stMainWnd.h_static0, L"REMOTE MODE");
        }break;
        case IDC_RADIO_SIM: {
            pProcObj->set_mode(PLC_IF_SIMULATOR_MODE);
            TCHAR tbuf[32];
            wsprintf(tbuf, L"mode:%04x", pProcObj->mode);
            SendMessage(stMainWnd.hWnd_status_bar, SB_SETTEXT, 0, (LPARAM)tbuf);
            SetWindowText(stMainWnd.h_static0, L"SIMULATOR MODE");
        }break;

        case IDC_CHK_IFCHK:

            if (BST_CHECKED == SendMessage(stMainWnd.h_chk_if, BM_GETCHECK, 0, 0)) pProcObj->show_if_wnd();
            else pProcObj->hide_if_wnd();

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
  
        timeKillEvent(knl_manage_set.KnlTick_TimerID);//マルチメディアタイマ停止
         
        Sleep(100);//100msec待機
        
        delete pProcObj;   //メイン処理オブジェクト削除
 
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
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

///#　******************************************************************************************
//  関数: CreateStatusbarMain(HWND)
//
//  目的: メイン ウィンドウ下部にアプリケーションの状態を表示用のステータスバーを配置します。
//　******************************************************************************************  
HWND CreateStatusbarMain(HWND hWnd)
{
    HWND hSBWnd;
    int sb_size[] = { 60,90,120,180};//ステータス区切り位置

    InitCommonControls();
    hSBWnd = CreateWindowEx(
        0,                          //拡張スタイル
        STATUSCLASSNAME,            //ウィンドウクラス
        NULL,                       //タイトル
        WS_CHILD | SBS_SIZEGRIP,    //ウィンドウスタイル
        0, 0, //位置
        0, 0, //幅、高さ
        hWnd, //親ウィンドウ
        (HMENU)ID_STATUS,           //ウィンドウのＩＤ
        hInst,                      //インスタンスハンドル
        NULL);
    SendMessage(hSBWnd, SB_SETPARTS, (WPARAM)4, (LPARAM)(LPINT)sb_size);//4枠で各枠の仕切り位置をパラーメータ指定
    ShowWindow(hSBWnd, SW_SHOW);
    return hSBWnd;
}

///#　****************************************************************************************
//  関数: alarmHandlar(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
//  目的: アプリケーションメイン処理
// 　マルチメディアタイマーイベント処理関数
//  ******************************************************************************************
VOID	CALLBACK    alarmHandlar(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
     knl_manage_set.sys_counter++;
      
     TCHAR tbuf[32];
     pProcObj->input();     //入力
     pProcObj->parse();      //データ解析処理
     pProcObj->output();    //出力
     //Statusバーにメインプロセスのカウンタ表示
     if (psource_proc_counter != NULL) {
         if (knl_manage_set.sys_counter % 40 == 0) {// 1000msec毎
             wsprintf(tbuf, L"%08d", knl_manage_set.sys_counter);
             SendMessage(stMainWnd.hWnd_status_bar, SB_SETTEXT, 3, (LPARAM)tbuf);
         }
     }
    return;
}
