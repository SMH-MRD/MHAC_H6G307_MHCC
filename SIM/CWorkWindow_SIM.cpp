

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <ws2tcpip.h>
//#include <winsock2.h>
#include <windowsx.h>       //# コモンコントロール

#include <iostream>
#include <iomanip>
#include <sstream>

#include "CWorkWindow_SIM.h"
#include "CSIM.h"

extern CSIM* pProcObj;

CWorkWindow::CWorkWindow() { hInst = NULL; }
CWorkWindow::~CWorkWindow() {}
HWND CWorkWindow::hWorkWnd;
HWND CWorkWindow::hwndSTATMSG;
HWND CWorkWindow::hwndRCVMSG;
HWND CWorkWindow::hwndSNDMSG;
HWND CWorkWindow::hwndChkBoxSwAct;
HWND CWorkWindow::hwndStaticSimInf;
bool CWorkWindow::is_sway_packet_send = false;

LPST_SIMULATION_STATUS CWorkWindow::pSimStat;

//Sway Sensor模擬用 SIMULATOR側
static WSADATA wsaData;
static SOCKET s;
static SOCKADDR_IN addrin;	//自局
static SOCKADDR_IN client;	//制御PC
static int fromlen;
static int nRtn=0,nRcv=0,nSnd=0;
static u_short port = SWAY_IF_IP_SWAY_PORT_S;
static char szBuf[256];

std::wostringstream woMSG;
std::wstring wsMSG;


//# #######################################################################
HWND CWorkWindow::open_WorkWnd(HWND hwnd_parent) {

	InitCommonControls();//コモンコントロール初期化
		
	WNDCLASSEX wc;

	hInst = GetModuleHandle(0);

	ZeroMemory(&wc, sizeof(wc));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WorkWndProc;// !CALLBACKでreturnを返していないとWindowClassの登録に失敗する
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("WorkWnd");
	wc.hIconSm = NULL;
	ATOM fb = RegisterClassExW(&wc);

	hWorkWnd = CreateWindow(TEXT("WorkWnd"),
		TEXT("SIM_WORK"),
		WS_POPUPWINDOW | WS_VISIBLE | WS_CAPTION, WORK_WND_X, WORK_WND_Y, WORK_WND_W, WORK_WND_H,
		hwnd_parent,
		0,
		hInst,
		NULL);

	ShowWindow(hWorkWnd, SW_SHOW);
	UpdateWindow(hWorkWnd);

	return hWorkWnd;
};

//# Window 終了処理 ###################################################################################
int CWorkWindow::close_WorkWnd() {

	closesocket(s);
	WSACleanup();
	DestroyWindow(hWorkWnd);  //ウィンドウ破棄
	hWorkWnd = NULL;
	return 0;
}

//###### Sway IF用 #######################3
int CWorkWindow::init_sock(HWND hwnd) {
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {    //WinSockの初期化
		woMSG << L"WSAStartup Error";
		return -1;
	}

	s = socket(AF_INET, SOCK_DGRAM, 0);                  //Socketオープン
	if (s < 0) {
		woMSG << L"socket失敗\n";
		return -2;
	}
	memset(&addrin, 0, sizeof(addrin));
	addrin.sin_port = htons(port);
	addrin.sin_family = AF_INET;
//	inet_pton(AF_INET, SWAY_SENSOR_IP_ADDR, &addrin.sin_addr.s_addr);CTRL_PC_IP_ADDR
	inet_pton(AF_INET, CTRL_PC_IP_ADDR_SWAY, &addrin.sin_addr.s_addr);

	nRtn = bind(s, (LPSOCKADDR)&addrin, (int)sizeof(addrin)); //ソケットに名前を付ける
	if (nRtn == SOCKET_ERROR) {
		woMSG << L"bindエラーです";
		closesocket(s);
		WSACleanup();
		return -3;
	}

	nRtn = WSAAsyncSelect(s, hwnd, ID_UDP_EVENT,FD_READ| FD_WRITE|FD_CLOSE);

	if (nRtn == SOCKET_ERROR) {
		woMSG << L"非同期化失敗";
		closesocket(s);
		WSACleanup();
		return -4;
	}

	//送信先アドレス初期値設定
	memset(&client, 0, sizeof(client));
	client.sin_port = htons(SWAY_IF_IP_SWAY_PORT_C);
	client.sin_family = AF_INET;
	inet_pton(AF_INET, CTRL_PC_IP_ADDR_SWAY, &client.sin_addr.s_addr);

	return 0;
}

//# ウィンドウへのメッセージ表示　wstring
void CWorkWindow::tweet2statusMSG(const std::wstring& srcw) {
	SetWindowText(hwndSTATMSG, srcw.c_str()); return;
}; 
void CWorkWindow::tweet2rcvMSG(const std::wstring& srcw) {
	SetWindowText(hwndRCVMSG, srcw.c_str()); return;
};
void CWorkWindow::tweet2sndMSG(const std::wstring& srcw) {
	SetWindowText(hwndSNDMSG, srcw.c_str()); return;
};

//# コールバック関数 ########################################################################	

LRESULT CALLBACK CWorkWindow::WorkWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	HDC hdc;
	int nEvent,clientlen;
	switch (msg) {
	case WM_DESTROY: {
		hWorkWnd = NULL;
		KillTimer(hwnd, ID_WORK_WND_TIMER);

	}return 0;
	case WM_CREATE: {

		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = GetModuleHandle(0);

		CreateWindowW(TEXT("STATIC"), L"STATUS", WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 400, 55, 20, hwnd, (HMENU)ID_STATIC_SWAY_IF_LABEL_RCV, hInst, NULL);
		hwndSTATMSG = CreateWindowW(TEXT("STATIC"), L"-", WS_CHILD | WS_VISIBLE | SS_LEFT,
			70, 400, 300, 20, hwnd, (HMENU)ID_STATIC_SWAY_IF_LABEL_RCV, hInst, NULL);

		hwndChkBoxSwAct = CreateWindow(L"BUTTON", L"SWY SND", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			400, 400, 100, 20, hwnd, (HMENU)ID_CHK_PACKET_SND, hInst, NULL);
		SendMessage(hwndChkBoxSwAct, BM_SETCHECK, BST_CHECKED, 0L);
		is_sway_packet_send = true;

		CreateWindowW(TEXT("STATIC"), L"RCV  ", WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 425, 55, 20, hwnd, (HMENU)ID_STATIC_SWAY_IF_LABEL_RCV, hInst, NULL);
		hwndRCVMSG = CreateWindowW(TEXT("STATIC"), L"-", WS_CHILD | WS_VISIBLE | SS_LEFT,
			70, 425, 500, 40, hwnd, (HMENU)ID_STATIC_SWAY_IF_LABEL_RCV, hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"SND  ", WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 470, 55, 20, hwnd, (HMENU)ID_STATIC_SWAY_IF_LABEL_SND, hInst, NULL);
		hwndSNDMSG = CreateWindowW(TEXT("STATIC"), L"-", WS_CHILD | WS_VISIBLE | SS_LEFT,
			70, 470, 500, 40, hwnd, (HMENU)ID_STATIC_SWAY_IF_LABEL_RCV, hInst, NULL);

		hwndStaticSimInf = CreateWindowW(TEXT("STATIC"), L"SIM INF", WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 5, 500, 200, hwnd, (HMENU)ID_STATIC_SIM_INF, hInst, NULL);

		if (init_sock(hwnd) == 0) {
			woMSG << L"SOCK OK";
			tweet2statusMSG(woMSG.str()); woMSG.str(L""); woMSG.clear();
			wsMSG = L"No RCV MSG";
			tweet2rcvMSG(wsMSG); wsMSG.clear();
			wsMSG = L"No SND MSG";
			tweet2sndMSG(wsMSG); wsMSG.clear();
		}
		else {
			tweet2statusMSG(woMSG.str()); woMSG.str(L""); woMSG.clear();
			wsMSG = L"No RCV MSG";
			tweet2rcvMSG(wsMSG); wsMSG.clear();
			wsMSG = L"No SND MSG";
			tweet2sndMSG(wsMSG); wsMSG.clear();
			//close_WorkWnd();
		}

		//タイマ起動
		SetTimer(hwnd, ID_WORK_WND_TIMER, WORK_SCAN_TIME, NULL);

	}break;
	case WM_TIMER: {

		//振れセンサ模擬データをSWAY_IFへ送信
		if (is_sway_packet_send) {
			int n = sizeof(ST_SWAY_RCV_MSG);
			nRtn = sendto(s, reinterpret_cast<const char*> (&pProcObj->pSIM_work->rcv_msg), n, 0, (LPSOCKADDR)&client, sizeof(client));//reinterpret_cast 強制的な型変換
			woMSG.str(L""); woMSG.clear();
			if (nRtn == n) {
				nSnd++;
				woMSG << L" SND len: " << nRtn << L"  Count > " << nSnd;
			}
			else {
				woMSG << L" sendto ERROR ";
			}
			tweet2sndMSG(woMSG.str()); woMSG.str(L""); woMSG.clear();
		}

		update_siminf();//シミュレーション状態表示

	}break;
	case ID_UDP_EVENT: {
		nEvent = WSAGETSELECTEVENT(lp);
		switch (nEvent) {
		case FD_READ: {
			nRcv++;
			clientlen = (int)sizeof(client);

			nRtn = recvfrom(s, (char*)(&pProcObj->pSIM_work->snd_msg), (int)sizeof(ST_SWAY_SND_MSG), 0, (SOCKADDR*)&client, &clientlen);
			woMSG.str(L""); woMSG.clear();
			if (nRtn == SOCKET_ERROR) {
				woMSG << L" recvfrom ERROR";
			}
			else {
				woMSG << L" RCV len : " << nRtn << L" Count :" << nRcv;
				woMSG << L" >>> ID : " << pProcObj->pSIM_work->snd_msg.head.id[0] << pProcObj->pSIM_work->snd_msg.head.id[1] << pProcObj->pSIM_work->snd_msg.head.id[2] << pProcObj->pSIM_work->snd_msg.head.id[3];
				woMSG << L"\n  IP : " << pProcObj->pSIM_work->snd_msg.head.sockaddr.sin_addr.S_un.S_un_b.s_b1 << L".";
				woMSG << pProcObj->pSIM_work->snd_msg.head.sockaddr.sin_addr.S_un.S_un_b.s_b2 << L".";
				woMSG << pProcObj->pSIM_work->snd_msg.head.sockaddr.sin_addr.S_un.S_un_b.s_b3 << L".";
				woMSG << pProcObj->pSIM_work->snd_msg.head.sockaddr.sin_addr.S_un.S_un_b.s_b4;
				woMSG << L"  PORT : " << ntohs(pProcObj->pSIM_work->snd_msg.head.sockaddr.sin_port);

			}
			tweet2rcvMSG(woMSG.str()); 

		}break;
		case FD_WRITE: {

		}break;
		case FD_CLOSE: {
			;
		}break;

		}
	}break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
	}break;
	case WM_COMMAND: {
		switch (LOWORD(wp)) {
		case ID_WORK_WND_CLOSE_PB: {
		}break;
		case ID_CHK_PACKET_SND: {
			if (BST_CHECKED == SendMessage(hwndChkBoxSwAct, BM_GETCHECK, 0, 0)) {
				is_sway_packet_send = true;
			}
			else {
				is_sway_packet_send = false;
			}
			break;
		}
		default:break;
		}
	}
	default:
		return DefWindowProc(hwnd, msg, wp, lp);
	}
	return 0;
}

void CWorkWindow::update_siminf() {

	woMSG.str(L"SIM STAT\n"); woMSG.clear();
	woMSG << L"nd[].p>> MH:" << pSimStat->nd[ID_HOIST].p << L" AH:" << pSimStat->nd[ID_AHOIST].p << L" BH:" << pSimStat->nd[ID_BOOM_H].p << L" SL:" << pSimStat->nd[ID_SLEW].p << L" GT" << pSimStat->nd[ID_GANTRY].p;
	woMSG<<L"\n";
	woMSG << L"nd[].a>> MH:" << pSimStat->nd[ID_HOIST].a << L" AH:" << pSimStat->nd[ID_AHOIST].a << L" BH:" << pSimStat->nd[ID_BOOM_H].a << L" SL:" << pSimStat->nd[ID_SLEW].a << pSimStat->nd[ID_GANTRY].a;
	woMSG << L"\n";
	woMSG << L"pos[]>>  MH" << pSimStat->pos[ID_HOIST] << L" AH:" << pSimStat->pos[ID_AHOIST] << L" BH:" << pSimStat->pos[ID_BOOM_H] << L" SL:" << pSimStat->pos[ID_SLEW] << L" GT:" << pSimStat->pos[ID_GANTRY];

	SetWindowText(hwndStaticSimInf, woMSG.str().c_str());
	return;
}
