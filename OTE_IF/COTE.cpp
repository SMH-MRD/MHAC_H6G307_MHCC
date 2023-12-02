#include "COTE.h"
#include <iostream>
#include <iomanip>
#include <sstream>

std::wstring COte::msg_ws;
std::wostringstream COte::msg_wos;

HWND COte::hWnd_parent;				//親ウィンドウのハンドル
HWND COte::hWnd_work;				//操作端末メインウィンドウハンドル
HWND COte::hWnd_sub[OTE0_N_SUB_WND];	//通信イベント処理用ウィンドウハンドル
HWND COte::hwnd_current_subwnd;		//表示中のサブウィンドハンドル

ST_OTE_WORK_WND	COte::st_work_wnd;

CSockAlpha* COte::pSockPcUniCastOte;	//OTEユニキャスト用ソケット
CSockAlpha* COte::pSockPcMultiCastOte;	//PCマルチキャストOTE受信用ソケット
CSockAlpha* COte::pSockOteMultiCastOte;	//OTEマルチキャストOTE受信用ソケット

SOCKADDR_IN COte::addrin_pc_u_ote;		//OTEユニキャスト用アドレス(OTE受信用)
SOCKADDR_IN COte::addrin_pc_m_ote;		//PCマルチキャスト受信アドレス(OTE受信用)
SOCKADDR_IN COte::addrin_ote_m_ote;		//OTEマルチキャスト受信アドレス(OTE受信用)

SOCKADDR_IN COte::addrin_ote_m_snd;		//OTEマルチキャスト送信先アドレス
SOCKADDR_IN COte::addrin_ote_u_snd;		//OTEマルチキャスト受信アドレス

LONG COte::cnt_snd_ote_u, COte::cnt_snd_ote_m;
LONG COte::cnt_rcv_pc_u, COte::cnt_rcv_pc_m, COte::cnt_rcv_ote_m;

ST_OTE_U_MSG COte::st_msg_ote_u_snd;
ST_OTE_M_MSG COte::st_msg_ote_m_snd;

ST_PC_M_MSG COte::st_msg_pc_m_ote_rcv;
ST_OTE_M_MSG COte::st_msg_ote_m_ote_rcv;
ST_PC_U_MSG COte::st_msg_ote_u_rcv;

/*****************************************************************************/
/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="hWnd">親ウィンドウのハンドル</param>
COte::COte(HWND hWnd) {
	hWnd_parent = hWnd;
	cnt_snd_ote_u = cnt_snd_ote_m = 0;
	cnt_rcv_pc_u = cnt_rcv_pc_m = cnt_rcv_ote_m = 0;
};
/*****************************************************************************/
/// <summary>
/// デストラクタ
/// </summary>
COte::~COte() {
};
/*****************************************************************************/
/// <summary>
/// 初期化処理（オーバーライド関数）
/// </summary>
/// <returns></returns>
int COte::init_proc() {

	//操作端末、通信イベント処理用ウィンドウオープン
	hWnd_work = open_work_Wnd(hWnd_parent);					//操作端末ウィンドOPEN
	hwnd_current_subwnd = open_connect_Wnd(hWnd_work);		//接続表示子ウィンドウ　
	
	//### ソケットアドレスセット
	memset(&addrin_pc_u_ote, 0, sizeof(SOCKADDR_IN));	memset(&addrin_pc_m_ote, 0, sizeof(SOCKADDR_IN)); memset(&addrin_ote_m_ote, 0, sizeof(SOCKADDR_IN));
	memset(&addrin_ote_m_snd, 0, sizeof(SOCKADDR_IN));	memset(&addrin_ote_u_snd, 0, sizeof(SOCKADDR_IN)); 

	set_sock_addr(&addrin_pc_u_ote, OTE_IF_UNICAST_IP_OTE0, OTE_IF_UNICAST_PORT_OTE);
	set_sock_addr(&addrin_ote_m_ote, OTE_IF_UNICAST_IP_OTE0, OTE_IF_MULTICAST_PORT_OTE);
	set_sock_addr(&addrin_pc_m_ote, OTE_IF_UNICAST_IP_OTE0, OTE_IF_MULTICAST_PORT_PC);

	set_sock_addr(&addrin_ote_u_snd, OTE_IF_UNICAST_IP_PC, OTE_IF_UNICAST_PORT_PC);
	set_sock_addr(&addrin_ote_m_snd, OTE_IF_MULTICAST_IP_OTE, OTE_IF_MULTICAST_PORT_OTE);

	//### ソケット設定
	pSockPcUniCastOte = new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_PC_UNI_OTE);
	pSockPcMultiCastOte = new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_PC_MULTI_OTE);
	pSockOteMultiCastOte = new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_OTE_MULTI_OTE);

	//### オブジェクト初期化

	if (pSockPcUniCastOte->Initialize() != S_OK) { msg_wos.str() = pSockPcUniCastOte->err_msg.str();	return NULL; }
	if (pSockPcMultiCastOte->Initialize() != S_OK) { msg_wos.str() = pSockPcMultiCastOte->err_msg.str();	return NULL; }
	if (pSockOteMultiCastOte->Initialize() != S_OK) { msg_wos.str() = pSockOteMultiCastOte->err_msg.str(); return NULL; }

	//### ソケット設定
	//ユニキャスト用
	if (pSockPcUniCastOte->init_sock(hWnd_work, addrin_pc_u_ote) != S_OK) { msg_wos.str() = pSockPcUniCastOte->err_msg.str(); return NULL; }

	//マルチキャスト用
	SOCKADDR_IN addr_tmp;
	set_sock_addr(&addr_tmp, OTE_IF_MULTICAST_IP_OTE, NULL);
	if (pSockPcMultiCastOte->init_sock_m(hWnd_work, addrin_pc_m_ote, addr_tmp) != S_OK) { msg_wos.str() = pSockPcMultiCastOte->err_msg.str(); return NULL; }
	if (pSockOteMultiCastOte->init_sock_m(hWnd_work, addrin_ote_m_ote, addr_tmp) != S_OK) { msg_wos.str() = pSockOteMultiCastOte->err_msg.str(); return NULL; }

	//送信メッセージヘッダ設定（送信元受信アドレス：受信先の折り返し用）
	st_msg_ote_u_snd.head.addr = addrin_pc_u_ote;
	st_msg_ote_m_snd.head.addr = addrin_ote_m_ote;

	return 0;
}

/*****************************************************************************/
/// <summary>
///  解析処理（表示更新処理　WM_TIMERで呼び出し）
/// </summary>
/// <returns></returns>
int COte::parse() {
	return 0;
}
/*****************************************************************************/
/// <summary>
/// OTEユニキャストメッセージ送信バッファセット
/// </summary>
/// <returns></returns>
LPST_OTE_U_MSG COte::set_msg_ote_u() {
#if 0 
	//Header部
	if (mode) {
		ote_io_workbuf.ote_io.snd_msg_u.head.addr = addrin_u;
		ote_io_workbuf.ote_io.snd_msg_u.head.myid = pCraneStat->spec.device_code.no;
	}
	ote_io_workbuf.ote_io.snd_msg_u.head.tgid = ote_io_workbuf.id_connected_te;;
	ote_io_workbuf.ote_io.snd_msg_u.head.code = code;
	ote_io_workbuf.ote_io.snd_msg_u.head.status = ote_io_workbuf.status_connected_te;

	//Body部
	//ランプ
	for (int i = 0; i < N_UI_LAMP; i++) ote_io_workbuf.ote_io.snd_msg_u.body.lamp[i] = pCSInf->ui_lamp[i];
	//ノッチ指令
	for (int i = 0; i < MOTION_ID_MAX; i++) ote_io_workbuf.ote_io.snd_msg_u.body.notch_pos[i] = pPLCio->status.notch_ref[i];
	//各軸位置
	for (int i = 0; i < MOTION_ID_MAX; i++) ote_io_workbuf.ote_io.snd_msg_u.body.pos[i] = (INT32)(pPLCio->status.pos[i] * 1000.0);
	//各軸速度FB
	for (int i = 0; i < MOTION_ID_MAX; i++) ote_io_workbuf.ote_io.snd_msg_u.body.v_fb[i] = (INT32)(pPLCio->status.v_fb[i] * 1000.0);
	//各軸速度指令
	for (int i = 0; i < MOTION_ID_MAX; i++) ote_io_workbuf.ote_io.snd_msg_u.body.v_ref[i] = (INT32)(pPLCio->status.v_ref[i] * 1000.0);

	//吊点位置
	ote_io_workbuf.ote_io.snd_msg_u.body.hp_pos[0] = pCSInf->hunging_point_for_view[0];
	ote_io_workbuf.ote_io.snd_msg_u.body.hp_pos[1] = pCSInf->hunging_point_for_view[1];
	ote_io_workbuf.ote_io.snd_msg_u.body.hp_pos[2] = pCSInf->hunging_point_for_view[2];

	//吊荷位置(吊点との相対位置）
	ote_io_workbuf.ote_io.snd_msg_u.body.ld_pos[0] = (INT32)(pSway_IO->th[ID_SLEW] * 1000.0);
	ote_io_workbuf.ote_io.snd_msg_u.body.ld_pos[1] = (INT32)(pSway_IO->th[ID_BOOM_H] * 1000.0);
	ote_io_workbuf.ote_io.snd_msg_u.body.ld_pos[2] = (INT32)(pCraneStat->mh_l * 1000.0);

	//吊荷速度
	ote_io_workbuf.ote_io.snd_msg_u.body.ld_v_fb[0] = (INT32)(pSway_IO->dth[ID_SLEW] * 1000.0);
	ote_io_workbuf.ote_io.snd_msg_u.body.ld_v_fb[1] = (INT32)(pSway_IO->dth[ID_BOOM_H] * 1000.0);
	ote_io_workbuf.ote_io.snd_msg_u.body.ld_v_fb[2] = (INT32)(pPLCio->status.v_fb[ID_HOIST] * 1000.0);

	//自動目標位置
	double tg_x_rad, tg_x_m, tg_y_rad, tg_y_m, h;

	h = pCSInf->ote_camera_height_m;
	tg_x_m = pCSInf->semi_auto_selected_target.pos[ID_BOOM_H] * cos(pCSInf->semi_auto_selected_target.pos[ID_SLEW]);
	tg_x_rad = tg_x_m / h;
	tg_y_m = pCSInf->semi_auto_selected_target.pos[ID_BOOM_H] * sin(pCSInf->semi_auto_selected_target.pos[ID_SLEW]);
	tg_y_rad = tg_y_m / h;

	ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos[0] = (INT32)(tg_x_rad * 1000.0);
	ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos[1] = (INT32)(tg_y_rad * 1000.0);
	ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos[2] = (INT32)(pCSInf->semi_auto_selected_target.pos[ID_HOIST] * 1000.0);

	//半自動目標位置
	for (int i = 0; i < 6; i++) {
		ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos_semi[i][0] = (INT32)(pCSInf->semi_auto_setting_target[i].pos[ID_BOOM_H] * 1000.0);
		ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos_semi[i][1] = (INT32)(pCSInf->semi_auto_setting_target[i].pos[ID_SLEW] * 1000.0);
		ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos_semi[i][2] = (INT32)(pCSInf->semi_auto_setting_target[i].pos[ID_HOIST] * 1000.0);
	}

	//VIEWカメラセット高さ
	ote_io_workbuf.ote_io.snd_msg_u.body.cam_inf[ID_OTE_CAMERA_HEIGHT] = (INT16)(pCraneStat->spec.boom_high * 1000.0);

	ote_io_workbuf.ote_io.snd_msg_u.body.lamp[ID_LAMP_OTE_NOTCH_MODE] = ote_io_workbuf.ote_io.rcv_msg_u.body.pb[ID_LAMP_OTE_NOTCH_MODE];

	//PLCデータ
	for (int i = 0; i < PLC_IO_MONT_WORD_NUM; i++) ote_io_workbuf.ote_io.snd_msg_u.body.plc_data[i] = pPLCio->plc_data[i];
#endif  
	return &st_msg_ote_u_snd;
}

/*****************************************************************************/
/// <summary>
/// OTEマルチキャストメッセージ送信バッファセット
/// </summary>
/// <returns></returns>
LPST_OTE_M_MSG COte::set_msg_ote_m() {
#if 0
	//送信メッセージセット
	int n = sizeof(ST_MOTE_SND_MSG);
	nRtn = sendto(s_m_snd, reinterpret_cast<const char*> (&ote_io_workbuf.ote_io.snd_msg_m), n, 0, (LPSOCKADDR)&addrin_m_snd, sizeof(addrin_m_snd));
	nRtn = sendto(s_m_snd_dbg, reinterpret_cast<const char*> (&ote_io_workbuf.ote_io.snd_msg_m), n, 0, (LPSOCKADDR)&addrin_m_snd, sizeof(addrin_m_snd));
	woMSG.str(L"");
	if (nRtn == n) {
		nSnd_m++;
		lSnd_m = nRtn;
		woMSG << L"Rcv n:" << nRcv_u << L" l:" << lRcv_u << L"  Snd n:" << nSnd_u << L" l:" << lSnd_u;
		tweet2infMSG(woMSG.str(), ID_SOCK_CODE_CR); woMSG.str(L""); woMSG.clear();

		woMSG << L"ID:" << ote_io_workbuf.ote_io.snd_msg_m.head.myid << L" CD:" << ote_io_workbuf.ote_io.snd_msg_m.head.code;
		sockaddr_in* psockaddr = (sockaddr_in*)&ote_io_workbuf.ote_io.snd_msg_m.head.addr;
		woMSG << L" IP:" << psockaddr->sin_addr.S_un.S_un_b.s_b1 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b2 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b3 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b4;
		woMSG << L" PORT: " << htons(psockaddr->sin_port);
		woMSG << L" ST:" << ote_io_workbuf.ote_io.snd_msg_m.head.status << L" ID:" << ote_io_workbuf.ote_io.snd_msg_m.head.tgid;
		tweet2sndMSG(woMSG.str(), ID_SOCK_CODE_CR); woMSG.str(L""); woMSG.clear();

	}
	else if (nRtn == SOCKET_ERROR) {
		woMSG << L"ERR CODE ->" << WSAGetLastError();
		tweet2sndMSG(woMSG.str(), ID_SOCK_CODE_CR); woMSG.str(L""); woMSG.clear();
	}
	else {
		woMSG << L" sendto size ERROR ";
		tweet2sndMSG(woMSG.str(), ID_SOCK_CODE_CR); woMSG.str(L""); woMSG.clear();
	}
#endif
	return &st_msg_ote_m_snd;
}

//****************************************************************************　
/// <summary>
/// クローズ処理
/// </summary>
/// <returns></returns>
HRESULT COte::close() {

	delete pSockPcUniCastOte;		//PC->OTEユニキャスト受信用ソケット
	delete pSockPcMultiCastOte;		//PC->OTEマルチキャスト受信用ソケット
	delete pSockOteMultiCastOte;	//OTE→OTEマルチキャスト受信用ソケット

	KillTimer(hWnd_work, ID_OTE_MULTICAST_TIMER);
	KillTimer(hWnd_work, ID_OTE_UNICAST_TIMER);

	return S_OK;
}

//****************************************************************************
/// <summary>
/// OTE実行タイマー起動,停止
/// </summary>
/// <param name="is_activate_req"></param>
void COte::activate_ote(bool is_activate_req) {
	if (is_activate_req) {
		//マルチキャストタイマ起動
		SetTimer(hWnd_work, ID_OTE_MULTICAST_TIMER, OTE_MULTICAST_SCAN_MS, NULL);
		//ユニキャストタイマ起動
		SetTimer(hWnd_work, ID_OTE_UNICAST_TIMER, OTE_UNICAST_SCAN_MS, NULL);
	}
	else {
		KillTimer(hWnd_work, ID_OTE_MULTICAST_TIMER);
		KillTimer(hWnd_work, ID_OTE_UNICAST_TIMER);
	}
	return;
}

//****************************************************************************
/// <summary>
/// OTE操作ウィンドウオープン
/// </summary>
/// <param name="hwnd"></param>
/// <returns></returns>
HWND COte::open_work_Wnd(HWND hwnd) {
	InitCommonControls();//コモンコントロール初期化
	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)GetModuleHandle(0);;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = TEXT("OTE0");
	wcex.lpszClassName = TEXT("OTE0");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//メインウィンドウ
	hWnd_work = CreateWindowW(TEXT("OTE0"), TEXT("OTE0"), WS_OVERLAPPEDWINDOW,
		OTE0_WND_X, OTE0_WND_Y, OTE0_WND_W, OTE0_WND_H,
		nullptr, nullptr, hInst, nullptr);

	RECT rc;
	GetClientRect(hWnd_work, &rc);
	st_work_wnd.area_w = rc.right - rc.left;
	st_work_wnd.area_h = rc.bottom - rc.top;

	//表示フォント設定
	st_work_wnd.hfont_inftext = CreateFont(6, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));
	//デバイスコンテキスト
	HDC hdc = GetDC(hWnd_work);
	st_work_wnd.hBmap[ID_OTE_HBMAP_MEM0] = CreateCompatibleBitmap(hdc, st_work_wnd.area_w, st_work_wnd.area_h);
	st_work_wnd.hdc[ID_OTE_HDC_MEM0] = CreateCompatibleDC(hdc);
	SelectObject(st_work_wnd.hdc[ID_OTE_HDC_MEM0], st_work_wnd.hBmap[ID_OTE_HBMAP_MEM0]);
	PatBlt(st_work_wnd.hdc[ID_OTE_HDC_MEM0], 0, 0, st_work_wnd.area_w, st_work_wnd.area_h, WHITENESS);

	st_work_wnd.hBmap[ID_OTE_HBMAP_MEM_IF] = CreateCompatibleBitmap(hdc, st_work_wnd.area_w, st_work_wnd.area_h);
	st_work_wnd.hdc[ID_OTE_HDC_MEM_IF] = CreateCompatibleDC(hdc);
	SelectObject(st_work_wnd.hdc[ID_OTE_HDC_MEM_IF], st_work_wnd.hBmap[ID_OTE_HBMAP_MEM_IF]);
	PatBlt(st_work_wnd.hdc[ID_OTE_HDC_MEM_IF], 0, 0, st_work_wnd.area_w, st_work_wnd.area_h, WHITENESS);
	TextOutW(st_work_wnd.hdc[ID_OTE_HDC_MEM_IF], 0, 0, L"<<Information>>", 15);

	ReleaseDC(hWnd_work, hdc);

	InvalidateRect(hWnd_work, NULL, TRUE);//表示更新

	ShowWindow(hWnd_work, SW_SHOW);
	UpdateWindow(hWnd_work);

	return hWnd_work;
}
HWND COte::open_connect_Wnd(HWND hwnd) {

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
	wcex.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
	wcex.lpszMenuName = TEXT("OTE CONNECT");
	wcex.lpszClassName = TEXT("OTE CONNECT");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//メインウィンドウ

	hWnd_sub[ID_OTE0_SUB_WND_CONNECT] = CreateWindowW(TEXT("OTE0 CONNECT"), TEXT("OTE0 CONNECT"), WS_CHILD | WS_BORDER,
		OTE0_SUB_WND_X, OTE0_SUB_WND_Y, OTE0_SUB_WND_W, OTE0_SUB_WND_H,
		hwnd, nullptr, hInst, nullptr);

	InvalidateRect(hWnd_sub[ID_OTE0_SUB_WND_CONNECT], NULL, TRUE);//表示更新

	ShowWindow(hWnd_sub[ID_OTE0_SUB_WND_CONNECT], SW_SHOW);
	UpdateWindow(hWnd_sub[ID_OTE0_SUB_WND_CONNECT]);

	return hWnd_sub[ID_OTE0_SUB_WND_CONNECT];
}
HWND COte::open_auto_Wnd(HWND hwnd) {

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
	wcex.lpszMenuName = TEXT("OTE AUTO");
	wcex.lpszClassName = TEXT("OTE AUTO");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//メインウィンドウ
	hWnd_sub[ID_OTE0_SUB_WND_AUTO] = CreateWindowW(TEXT("OTE AUTO"), TEXT("OTE AUTO"), WS_CHILD | WS_BORDER,//WS_OVERLAPPEDWINDOW,
		OTE0_SUB_WND_X, OTE0_SUB_WND_Y, OTE0_SUB_WND_W, OTE0_SUB_WND_H,
		hwnd, nullptr, hInst, nullptr);

	InvalidateRect(hWnd_sub[ID_OTE0_SUB_WND_AUTO], NULL, TRUE);//表示更新

	ShowWindow(hWnd_sub[ID_OTE0_SUB_WND_AUTO], SW_SHOW);
	UpdateWindow(hWnd_sub[ID_OTE0_SUB_WND_AUTO]);

	return hWnd_sub[ID_OTE0_SUB_WND_AUTO];
}
HWND COte::open_mode_Wnd(HWND hwnd) {

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
HWND COte::open_fault_Wnd(HWND hwnd) {

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
HWND COte::open_moment_Wnd(HWND hwnd) {

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
HWND COte::open_swy_Wnd(HWND hwnd) {

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
	wcex.lpszMenuName = TEXT("SWAY STAT");
	wcex.lpszClassName = TEXT("SWAY STAT");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//メインウィンドウ
	hWnd_sub[ID_OTE0_SWY_WND] = CreateWindowW(TEXT("SWAY STAT"), TEXT("SWAY STAT"), WS_CHILD | WS_BORDER,//WS_OVERLAPPEDWINDOW,
		OTE0_SUB_WND_X, OTE0_SUB_WND_Y, OTE0_SUB_WND_W, OTE0_SUB_WND_H,
		hwnd, nullptr, hInst, nullptr);

	InvalidateRect(hWnd_sub[ID_OTE0_SWY_WND], NULL, TRUE);//表示更新

	ShowWindow(hWnd_sub[ID_OTE0_SWY_WND], SW_SHOW);
	UpdateWindow(hWnd_sub[ID_OTE0_SWY_WND]);

	return hWnd_sub[ID_OTE0_SWY_WND];
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
static int tmp_counter=0;

LRESULT CALLBACK COte::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	int id;
	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		//ウィンドウにコントロール追加
		set_OTE_panel_objects(hWnd);
		//サブウィンドウ追加
		 open_connect_Wnd(hWnd);
		 //振れウィンドウ追加
		 open_swy_Wnd(hWnd);
	}

	case WM_TIMER: {

		//PBの表示更新
		draw_pb();

		if (wParam == ID_OTE_MULTICAST_TIMER) {
			if (S_OK == snd_ote_m_pc(set_msg_ote_m())) {//OTEマルチキャスト送信
				cnt_snd_ote_m++;
			}
		}
		if (wParam == ID_OTE_UNICAST_TIMER) {
			if (S_OK == snd_ote_u_pc(set_msg_ote_u(), &addrin_ote_u_snd)) {//OTEユニキャスト送信
				cnt_snd_ote_m++;
			}
		}
		disp_msg_cnt();//カウント表示更新
	}break;

	case WM_COMMAND:{
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
			if (rcv_pc_u_ote(&st_msg_ote_u_rcv) == S_OK) {				//PCからのユニキャストメッセージ受信
				cnt_rcv_pc_u++;
				disp_msg_cnt();
			}
			else {
				msg_ws = L"ERROR : rcv_ote_u_pc()";	wstr_out_inf(msg_ws);
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
			if (rcv_pc_m_ote(&st_msg_pc_m_ote_rcv) == S_OK) {				//PCからのユニキャストメッセージ受信
				cnt_rcv_pc_m++;
				disp_msg_cnt();
			}
			else {
				msg_ws = L"ERROR : rcv_ote_u_pc()";	wstr_out_inf(msg_ws);
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
			if (rcv_ote_m_ote(&st_msg_ote_m_ote_rcv) == S_OK) {				//PCからのユニキャストメッセージ受信
				cnt_rcv_ote_m++;
				disp_msg_cnt();
			}
			else {
				msg_ws = L"ERROR : rcv_ote_m_ote()";	wstr_out_inf(msg_ws);
			}
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case WM_PAINT:{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		BOOL brtn = TransparentBlt(st_work_wnd.hdc[ID_OTE_HDC_MEM0], 0, 0, OTE0_WND_W, OTE0_WND_H,		//DST
									st_work_wnd.hdc[ID_OTE_HDC_MEM0], 0, 0, OTE0_WND_W, OTE0_WND_H,	//SORCE
									RGB(255, 255, 255));

		BitBlt(hdc, 0, 0, OTE0_WND_W, OTE0_WND_H, st_work_wnd.hdc[ID_OTE_HDC_MEM0], 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
	}break;

	case WM_DESTROY:{
		close();
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
LRESULT CALLBACK COte::WndConnectProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		CreateWindowW(TEXT("STATIC"), L"HEAD:", WS_CHILD | WS_VISIBLE | SS_LEFT,
			0, 0, 0, 25, hWnd, (HMENU)(BASE_ID_OTE_STATIC + ID_OTE_SUB_CONNECT_HEAD), hInst, NULL);
		CreateWindowW(TEXT("STATIC"), L"BODY:", WS_CHILD | WS_VISIBLE | SS_LEFT,
			0, 0, 0, 25, hWnd, (HMENU)(BASE_ID_OTE_STATIC + ID_OTE_SUB_CONNECT_BODY), hInst, NULL);

		//メッセージ表示切替ラジオ釦
		for (int i = ID_OTE_RADIO_SOU; i <= ID_OTE_RADIO_RPM; i++) {
			if (i == ID_OTE_RADIO_SOU)// | WS_GROUP あり
				st_work_wnd.hctrl[ID_OTE_CTRL_PB][i] = CreateWindow(L"BUTTON",
					st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].y,
					st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cy,
					hWnd, (HMENU)(BASE_ID_OTE_PB + (LONG64)i), hInst, NULL);
			else
				st_work_wnd.hctrl[ID_OTE_CTRL_PB][i] = CreateWindow(L"BUTTON",
					st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
					st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_PB][i].y,
					st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_PB][i].cy,
					hWnd, (HMENU)(BASE_ID_OTE_PB + (LONG64)i), hInst, NULL);
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
LRESULT CALLBACK COte::WndAutoProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
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
LRESULT CALLBACK COte::WndModeProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

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
LRESULT CALLBACK COte::WndFaultProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

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
LRESULT CALLBACK COte::WndMomentProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

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
//*********************************************************************************************
/// <summary>
/// 振れウィンドウコールバック関数
/// </summary>
/// <param name="hWnd"></param>
/// <param name="message"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
/// <returns></returns>
LRESULT CALLBACK COte::WndSwyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

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

//*********************************************************************************************
/// <summary>
/// OTEマルチキャスト電文受信処理（OTEマルチキャストメッセージを受信）
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COte::rcv_pc_u_ote(LPST_PC_U_MSG pbuf) {
	int nRtn = pSockPcUniCastOte->rcv_udp_msg((char*)pbuf, sizeof(ST_PC_U_MSG));
	if (nRtn == SOCKET_ERROR) {
		msg_wos.str() = pSockPcUniCastOte->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}
//*********************************************************************************************
/// <summary>
/// OTEマルチキャスト電文受信処理（OTEマルチキャストメッセージを受信）
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COte::rcv_ote_m_ote(LPST_OTE_M_MSG pbuf) {
	int nRtn = pSockOteMultiCastOte->rcv_udp_msg((char*)pbuf, sizeof(ST_OTE_M_MSG));
	if (nRtn == SOCKET_ERROR) {
		msg_wos.str() = pSockOteMultiCastOte->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}
//*********************************************************************************************
/// <summary>
/// OTEマルチキャスト電文受信処理（OTEマルチキャストメッセージを受信）
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COte::rcv_pc_m_ote(LPST_PC_M_MSG pbuf) {
	int nRtn = pSockPcMultiCastOte->rcv_udp_msg((char*)pbuf, sizeof(ST_PC_M_MSG));
	if (nRtn == SOCKET_ERROR) {
		msg_wos.str() = pSockPcMultiCastOte->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}

//*********************************************************************************************
/// <summary>
///  OTE→PC　Unicast　【OTEのPCからのユニキャスト受信ソケットで送信】
/// </summary>
/// <param name="pbuf"></param>
/// <param name="pto_addrin"></param>
/// <returns></returns>
HRESULT COte::snd_ote_u_pc(LPST_OTE_U_MSG pbuf, SOCKADDR_IN* p_addrin_to) {
	if (pSockPcUniCastOte->snd_udp_msg((char*)pbuf, sizeof(ST_OTE_U_MSG), *p_addrin_to) == SOCKET_ERROR) {
		msg_wos.str() = pSockPcUniCastOte->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}
//*********************************************************************************************
/// <summary>
/// OTE->PC マルチキャスト　【OTEのPCからのマルチキャスト受信ソケットで送信】
/// </summary>
/// <param name="pbuf"></param>
/// <param name="pto_addrin"></param>
/// <returns></returns>
HRESULT COte::snd_ote_m_pc(LPST_OTE_M_MSG pbuf) {
	if (pSockPcMultiCastOte->snd_udp_msg((char*)pbuf, sizeof(ST_OTE_M_MSG), addrin_ote_m_snd) == SOCKET_ERROR) {
		msg_wos.str() = pSockPcMultiCastOte->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}//OTEユニキャスト送信処理
//*********************************************************************************************
/// <summary>
/// OTE->OTE マルチキャスト　【OTEのOTEからのマルチキャスト受信ソケットで送信】
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COte::snd_ote_m_ote(LPST_OTE_M_MSG pbuf) {
	if (pSockOteMultiCastOte->snd_udp_msg((char*)pbuf, sizeof(ST_OTE_M_MSG), addrin_ote_m_snd) == SOCKET_ERROR) {
		msg_wos.str() = pSockOteMultiCastOte->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}//OTEマルチキャスト送信処理
//*********************************************************************************************
/// <summary>
/// 
/// </summary>
/// <param name="srcw"></param>
void COte::wstr_out_inf(const std::wstring& srcw) {
	return;
}

//*********************************************************************************************
/// <summary>
/// OTEウィンドウ上にオブジェクト配置
/// </summary>
void COte::set_OTE_panel_objects(HWND hWnd) {

	HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

	//ラベル
	for (int i = ID_OTE_LABEL_MH; i <= ID_OTE_INF_GT0;i++) {
		st_work_wnd.hctrl[ID_OTE_CTRL_STATIC][i] = CreateWindowW(TEXT("STATIC"), st_work_wnd.ctrl_text[ID_OTE_CTRL_STATIC][i], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_STATIC][i].y,
			st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_STATIC][i].cy,
			hWnd, (HMENU)(BASE_ID_OTE_STATIC + i), hInst, NULL);
	}

	//PB
	{
		for (LONGLONG i = ID_OTE_PB_HIJYOU; i <= ID_OTE_PB_N3; i++) {
			st_work_wnd.hctrl[ID_OTE_CTRL_PB][i] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_PB][i], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_PUSHLIKE,
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
		hWnd, (HMENU)(ID_OTE_NOTCH_MH0 -4), hInst, NULL);

		for (LONGLONG i = 1; i <= 8; i++) {
			st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_HOIST] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][ID_HOIST * N_OTE_NOTCH_ARRAY + i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
				st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_HOIST * N_OTE_NOTCH_ARRAY + i].x, st_work_wnd.pt_ctrl[ID_OTE_CTRL_NOTCH][ID_HOIST * N_OTE_NOTCH_ARRAY + i].y,
				st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_HOIST * N_OTE_NOTCH_ARRAY + i].cx, st_work_wnd.size_ctrl[ID_OTE_CTRL_NOTCH][ID_HOIST * N_OTE_NOTCH_ARRAY + i].cy,
				hWnd, (HMENU)(ID_OTE_NOTCH_MH0 - 4 + i), hInst, NULL);
		}
	}
	//補巻
	{	st_work_wnd.hctrl[ID_OTE_CTRL_NOTCH][ID_AHOIST] = CreateWindowW(TEXT("BUTTON"), st_work_wnd.ctrl_text[ID_OTE_CTRL_NOTCH][ID_AHOIST * N_OTE_NOTCH_ARRAY], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
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

/// <summary>
/// PBのオーナードロー
/// </summary>
void COte::draw_pb() {
	HBITMAP hBitmap;
	HDC hmdc;
	HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

	//主幹PB

//	if (st_msg_ote_u_rcv.body.lamp[OTE_INDEX_PB_CTR_SOURCE])	hBitmap = (HBITMAP)LoadBitmap(hInst, L"IDB_PB_CTR_SOURCE_ON");
//	else				hBitmap = (HBITMAP)LoadBitmap(hInst, L"IDB_PB_CTR_SOURCE_OFF");

//	hmdc = CreateCompatibleDC(st_work_wnd.hdc_pb_ctr_source);
//	SelectObject(hmdc, hBitmap); BitBlt(st_work_wnd.hdc_pb_ctr_source, 0, 0, 50, 20, hmdc, 0, 0, SRCCOPY);
	DeleteDC(hmdc);
	DeleteObject(hBitmap);

	return;
}