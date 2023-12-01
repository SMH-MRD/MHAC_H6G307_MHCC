#include "framework.h"
#include "COTE_IF.h"
#include <windowsx.h>       //# コモンコントロール
#include <winsock2.h>

#include <iostream>
#include <iomanip>
#include <sstream>


std::wstring COteIF::msg_ws;
std::wostringstream COteIF::msg_wos;

ST_OTE_IO COteIF::ote_io_workbuf;

LPST_OTE_IO COteIF::pOTEio;
LPST_CRANE_STATUS COteIF::pCraneStat;
LPST_PLC_IO COteIF::pPLCio;
LPST_CS_INFO COteIF::pCSInf;
LPST_AGENT_INFO COteIF::pAgentInf;
LPST_SWAY_IO COteIF::pSway_IO;

HWND COteIF::hWnd_parent;					//親ウィンドウのハンドル
HWND COteIF::hWnd_work; 					//通信イベント処理用ウィンドウハンドル

ST_OTEIF_WORK_WND COteIF::st_work_wnd;

CSockAlpha* COteIF::pSockOteUniCastPc;		//PCユニキャスト用ソケット
CSockAlpha* COteIF::pSockPcMultiCastPc;		//PCマルチキャストPC受信用ソケット
CSockAlpha* COteIF::pSockOteMultiCastPc;	//OTEマルチキャストPC受信用ソケット

SOCKADDR_IN COteIF::addrin_ote_u_pc;		//PCユニキャスト用アドレス(PC受信用)
SOCKADDR_IN COteIF::addrin_pc_m_pc;			//PCマルチキャスト受信アドレス(PC受信用)
SOCKADDR_IN COteIF::addrin_ote_m_pc;		//OTEマルチキャスト受信アドレス(PC受信用)

SOCKADDR_IN COteIF::addrin_pc_u_snd;		//PCユニキャスト送信先アドレス
SOCKADDR_IN COteIF::addrin_pc_m_snd;		//PCマルチキャスト送信先アドレス


SOCKADDR_IN COteIF::addr_active_ote;		//操作信号が有効な端末のアドレス

LONG COteIF::cnt_snd_pc_u, COteIF::cnt_snd_pc_m;
LONG COteIF::cnt_rcv_ote_u, COteIF::cnt_rcv_ote_m,COteIF::cnt_rcv_pc_m;

ST_PC_U_MSG COteIF::st_msg_pc_u_snd;
ST_PC_M_MSG COteIF::st_msg_pc_m_snd;

ST_PC_M_MSG COteIF::st_msg_pc_m_pc_rcv;
ST_PC_M_MSG COteIF::st_msg_pc_m_ote_rcv;

ST_OTE_M_MSG COteIF::st_msg_ote_m_pc_rcv;
ST_OTE_M_MSG COteIF::st_msg_ote_m_ote_rcv;
ST_OTE_U_MSG COteIF::st_msg_ote_u_rcv;

ST_OTE_U_MSG COteIF::st_ote_active_msg;	//操作信号が有効な現メッセージ

/*****************************************************************************/
/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="hWnd">親ウィンドウのハンドル</param>
COteIF::COteIF(HWND hWnd) {
    hWnd_parent = hWnd;
    // 共有メモリオブジェクトのインスタンス化
    pOteIOObj = new CSharedMem;
    pCraneStatusObj = new CSharedMem;
    pSimulationStatusObj = new CSharedMem;
    pPLCioObj = new CSharedMem;
    pCSInfObj = new CSharedMem;
    pAgentInfObj = new CSharedMem;
    pSwayIO_Obj = new CSharedMem;

	cnt_snd_pc_u = cnt_snd_pc_m= 0;
	cnt_rcv_ote_u = cnt_rcv_ote_m = cnt_rcv_pc_m = 0;
 };
/*****************************************************************************/
/// <summary>
/// デストラクタ
/// </summary>
COteIF::~COteIF() {
    // 共有メモリオブジェクトの解放
    delete pOteIOObj;
    delete pCraneStatusObj;
    delete pSimulationStatusObj;
    delete pPLCioObj;
    delete pCSInfObj;
    delete pAgentInfObj;
    delete pSwayIO_Obj;
};
/*****************************************************************************/
/// <summary>
/// 出力バッファのアドレスをセット（オーバーライド関数）
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
int COteIF::set_outbuf(LPVOID pbuf) { 
    poutput = pbuf;return 0;
    return 0; 

}    //出力バッファセット
/*****************************************************************************/
/// <summary>
/// 初期化処理（オーバーライド関数）
/// </summary>
/// <returns></returns>
int COteIF::init_proc() {
    // 共有メモリ取得
	{
		// 出力用共有メモリ取得
		out_size = sizeof(ST_OTE_IO);
		if (OK_SHMEM != pOteIOObj->create_smem(SMEM_OTE_IO_NAME, out_size, MUTEX_OTE_IO_NAME)) {
			mode |= OTE_IF_OTE_IO_MEM_NG;
		}
		set_outbuf(pOteIOObj->get_pMap());

		// 入力用共有メモリ取得
		if (OK_SHMEM != pSimulationStatusObj->create_smem(SMEM_SIMULATION_STATUS_NAME, sizeof(ST_SIMULATION_STATUS), MUTEX_SIMULATION_STATUS_NAME)) {
			mode |= OTE_IF_SIM_MEM_NG;
		}

		if (OK_SHMEM != pCraneStatusObj->create_smem(SMEM_CRANE_STATUS_NAME, sizeof(ST_CRANE_STATUS), MUTEX_CRANE_STATUS_NAME)) {
			mode |= OTE_IF_CRANE_MEM_NG;
		}

		if (OK_SHMEM != pPLCioObj->create_smem(SMEM_PLC_IO_NAME, sizeof(ST_PLC_IO), MUTEX_PLC_IO_NAME)) {
			mode |= OTE_IF_PLC_MEM_NG;
		}

		if (OK_SHMEM != pCSInfObj->create_smem(SMEM_CS_INFO_NAME, sizeof(ST_CS_INFO), MUTEX_CS_INFO_NAME)) {
			mode |= OTE_IF_PLC_MEM_NG;
		}

		if (OK_SHMEM != pAgentInfObj->create_smem(SMEM_AGENT_INFO_NAME, sizeof(ST_AGENT_INFO), MUTEX_AGENT_INFO_NAME)) {
			mode |= OTE_IF_PLC_MEM_NG;
		}

		pCraneStat = (LPST_CRANE_STATUS)(pCraneStatusObj->get_pMap());
		pPLCio = (LPST_PLC_IO)(pPLCioObj->get_pMap());
		pOTEio = (LPST_OTE_IO)pOteIOObj->get_pMap();
		pCSInf = (LPST_CS_INFO)pCSInfObj->get_pMap();
		pAgentInf = (LPST_AGENT_INFO)pAgentInfObj->get_pMap();
		pSway_IO = (LPST_SWAY_IO)(pSwayIO_Obj->get_pMap());
	}
    
	//操作端末、通信イベント処理用ウィンドウオープン
	hWnd_work = open_work_Wnd(hWnd_parent);					//操作端末ウィンドOPEN

	//### ソケットアドレスセット
	memset(&addrin_ote_u_pc	, 0, sizeof(SOCKADDR_IN));	memset(&addrin_pc_m_pc, 0, sizeof(SOCKADDR_IN));memset(&addrin_ote_m_pc, 0, sizeof(SOCKADDR_IN));
	memset(&addrin_pc_m_snd, 0, sizeof(SOCKADDR_IN)); memset(&addrin_pc_u_snd, 0, sizeof(SOCKADDR_IN));
	
	//受信アドレス
	set_sock_addr(&addrin_ote_u_pc, OTE_IF_UNICAST_IP_PC, OTE_IF_UNICAST_PORT_PC);
	set_sock_addr(&addrin_ote_m_pc, OTE_IF_UNICAST_IP_PC, OTE_IF_MULTICAST_PORT_OTE);
	set_sock_addr(&addrin_pc_m_pc,	OTE_IF_UNICAST_IP_PC, OTE_IF_MULTICAST_PORT_PC);

	//送信先アドレス	
	set_sock_addr(&addrin_pc_m_snd , OTE_IF_MULTICAST_IP_PC, OTE_IF_MULTICAST_PORT_PC);
	
 	
	//### ソケット設定
	pSockOteUniCastPc	= new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_SERVER, ID_SOCK_EVENT_OTE_UNI_PC);
	pSockPcMultiCastPc	= new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_PC_MULTI_PC);
	pSockOteMultiCastPc = new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_CLIENT, ID_SOCK_EVENT_OTE_MULTI_PC);

	//### オブジェクト初期化
	if (pSockOteUniCastPc->Initialize()		!= S_OK) { msg_wos.str() = pSockOteUniCastPc->err_msg.str();	return NULL; }
	if (pSockPcMultiCastPc->Initialize()	!= S_OK) { msg_wos.str() = pSockPcMultiCastPc->err_msg.str();	return NULL; }
	if (pSockOteMultiCastPc->Initialize()	!= S_OK) { msg_wos.str() = pSockOteMultiCastPc->err_msg.str();	return NULL; }

	//### ソケット設定
	//ユニキャスト用 init_sock():bind()→非同期化まで実施
	if (pSockOteUniCastPc->init_sock(hWnd_work,	addrin_ote_u_pc	)	!= S_OK) { msg_wos.str() = pSockOteUniCastPc->err_msg.str(); return NULL;}

	//マルチキャスト用 init_sock_m():bind()まで実施 + マルチキャストグループへ登録
	SOCKADDR_IN addr_tmp;
	set_sock_addr(&addr_tmp, OTE_IF_MULTICAST_IP_PC, NULL);//マルチキャスト受信IPセット,PORTはネットワーク設定（第2引数）のポート
	if (pSockPcMultiCastPc->init_sock_m(hWnd_work, addrin_pc_m_pc, addr_tmp) != S_OK) { msg_wos.str() = pSockPcMultiCastPc->err_msg.str(); return NULL; }
	if (pSockOteMultiCastPc->init_sock_m(hWnd_work, addrin_ote_m_pc, addr_tmp) != S_OK) { msg_wos.str() = pSockOteMultiCastPc->err_msg.str(); return NULL; }

	//送信メッセージヘッダ設定（送信元受信アドレス：受信先の折り返し用）
	st_msg_pc_u_snd.head.addr = addrin_ote_u_pc;
	st_msg_pc_m_snd.head.addr = addrin_ote_m_pc;

	return 0; 
}
/*****************************************************************************/
/// <summary>
/// 入力処理（オーバーライド関数）
/// </summary>
/// <returns></returns>
int COteIF::input() {

      return 0; 
} 
/*****************************************************************************/
/// <summary>
///  解析処理（オーバーライド関数）
/// </summary>
/// <returns></returns>
int COteIF::parse() { 
	//ST_OTE_IO workbuf設定
    return 0;
}    
/*****************************************************************************/
/// <summary>
/// 出力解析処理（オーバーライド関数）
/// </summary>
/// <returns></returns>
int COteIF::output() {                          //出力処理
	//workbuf　→　ST_OTE_IO
   if (out_size) memcpy_s(poutput, out_size, &ote_io_workbuf, out_size);
   return 0; 
}
/*****************************************************************************/
/// <summary>
/// PCユニキャストメッセージ送信バッファセット
/// </summary>
/// <returns></returns>
LPST_PC_U_MSG COteIF:: set_msg_pc_u() {
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
    for (int i = 0;i < N_UI_LAMP;i++) ote_io_workbuf.ote_io.snd_msg_u.body.lamp[i] = pCSInf->ui_lamp[i];
    //ノッチ指令
    for (int i = 0;i < MOTION_ID_MAX;i++) ote_io_workbuf.ote_io.snd_msg_u.body.notch_pos[i] = pPLCio->status.notch_ref[i];
    //各軸位置
    for (int i = 0;i < MOTION_ID_MAX;i++) ote_io_workbuf.ote_io.snd_msg_u.body.pos[i] = (INT32)(pPLCio->status.pos[i] * 1000.0);
    //各軸速度FB
    for (int i = 0;i < MOTION_ID_MAX;i++) ote_io_workbuf.ote_io.snd_msg_u.body.v_fb[i] = (INT32)(pPLCio->status.v_fb[i] * 1000.0);
    //各軸速度指令
    for (int i = 0;i < MOTION_ID_MAX;i++) ote_io_workbuf.ote_io.snd_msg_u.body.v_ref[i] = (INT32)(pPLCio->status.v_ref[i] * 1000.0);

    //吊点位置
    ote_io_workbuf.ote_io.snd_msg_u.body.hp_pos[0] = pCSInf->hunging_point_for_view[0];
    ote_io_workbuf.ote_io.snd_msg_u.body.hp_pos[1] = pCSInf->hunging_point_for_view[1];
    ote_io_workbuf.ote_io.snd_msg_u.body.hp_pos[2] = pCSInf->hunging_point_for_view[2];

    //吊荷位置(吊点との相対位置）
    ote_io_workbuf.ote_io.snd_msg_u.body.ld_pos[0] = (INT32)(pSway_IO->th[ID_SLEW]* 1000.0);
    ote_io_workbuf.ote_io.snd_msg_u.body.ld_pos[1] = (INT32)(pSway_IO->th[ID_BOOM_H] * 1000.0);
    ote_io_workbuf.ote_io.snd_msg_u.body.ld_pos[2] = (INT32)(pCraneStat->mh_l*1000.0);

    //吊荷速度
    ote_io_workbuf.ote_io.snd_msg_u.body.ld_v_fb[0] = (INT32)(pSway_IO->dth[ID_SLEW] * 1000.0);
    ote_io_workbuf.ote_io.snd_msg_u.body.ld_v_fb[1] = (INT32)(pSway_IO->dth[ID_BOOM_H] * 1000.0);
    ote_io_workbuf.ote_io.snd_msg_u.body.ld_v_fb[2] = (INT32)(pPLCio->status.v_fb[ID_HOIST] * 1000.0);

    //自動目標位置
    double tg_x_rad, tg_x_m, tg_y_rad, tg_y_m,h;

        h = pCSInf->ote_camera_height_m;
        tg_x_m = pCSInf->semi_auto_selected_target.pos[ID_BOOM_H] * cos(pCSInf->semi_auto_selected_target.pos[ID_SLEW]);
        tg_x_rad = tg_x_m / h;
        tg_y_m = pCSInf->semi_auto_selected_target.pos[ID_BOOM_H] * sin(pCSInf->semi_auto_selected_target.pos[ID_SLEW]);
        tg_y_rad = tg_y_m / h;

        ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos[0] = (INT32)(tg_x_rad * 1000.0);
        ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos[1] = (INT32)(tg_y_rad * 1000.0);
        ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos[2] = (INT32)(pCSInf->semi_auto_selected_target.pos[ID_HOIST] * 1000.0);
  
    //半自動目標位置
    for (int i = 0;i < 6;i++) {
        ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos_semi[i][0] = (INT32)(pCSInf->semi_auto_setting_target[i].pos[ID_BOOM_H] * 1000.0);
        ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos_semi[i][1] = (INT32)(pCSInf->semi_auto_setting_target[i].pos[ID_SLEW] * 1000.0);
        ote_io_workbuf.ote_io.snd_msg_u.body.tg_pos_semi[i][2] = (INT32)(pCSInf->semi_auto_setting_target[i].pos[ID_HOIST] * 1000.0);
    }

    //VIEWカメラセット高さ
    ote_io_workbuf.ote_io.snd_msg_u.body.cam_inf[ID_OTE_CAMERA_HEIGHT] = (INT16)(pCraneStat->spec.boom_high * 1000.0);

    ote_io_workbuf.ote_io.snd_msg_u.body.lamp[ID_LAMP_OTE_NOTCH_MODE] = ote_io_workbuf.ote_io.rcv_msg_u.body.pb[ID_LAMP_OTE_NOTCH_MODE];

    //PLCデータ
    for (int i = 0;i < PLC_IO_MONT_WORD_NUM;i++) ote_io_workbuf.ote_io.snd_msg_u.body.plc_data[i] = pPLCio->plc_data[i];
#endif  
    return &st_msg_pc_u_snd;
} 
/*****************************************************************************/
/// <summary>
/// PCマルチキャストメッセージ送信バッファセット
/// </summary>
/// <returns></returns>
LPST_PC_M_MSG COteIF::set_msg_pc_m() {
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
        tweet2infMSG(woMSG.str(), ID_SOCK_CODE_CR); woMSG.str(L"");woMSG.clear();

        woMSG << L"ID:" << ote_io_workbuf.ote_io.snd_msg_m.head.myid << L" CD:" << ote_io_workbuf.ote_io.snd_msg_m.head.code;
        sockaddr_in* psockaddr = (sockaddr_in*)&ote_io_workbuf.ote_io.snd_msg_m.head.addr;
        woMSG << L" IP:" << psockaddr->sin_addr.S_un.S_un_b.s_b1 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b2 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b3 << L"." << psockaddr->sin_addr.S_un.S_un_b.s_b4;
        woMSG << L" PORT: " << htons(psockaddr->sin_port);
        woMSG << L" ST:" << ote_io_workbuf.ote_io.snd_msg_m.head.status << L" ID:" << ote_io_workbuf.ote_io.snd_msg_m.head.tgid;
        tweet2sndMSG(woMSG.str(), ID_SOCK_CODE_CR); woMSG.str(L"");woMSG.clear();

    }
    else if (nRtn == SOCKET_ERROR) {
        woMSG << L"ERR CODE ->" << WSAGetLastError();
        tweet2sndMSG(woMSG.str(), ID_SOCK_CODE_CR); woMSG.str(L"");woMSG.clear();
    }
    else {
        woMSG << L" sendto size ERROR ";
        tweet2sndMSG(woMSG.str(), ID_SOCK_CODE_CR); woMSG.str(L"");woMSG.clear();
    }
#endif
    return &st_msg_pc_m_snd;
}

//****************************************************************************　
/// <summary>
/// クローズ処理
/// </summary>
/// <returns></returns>
HRESULT COteIF::close() {
	delete  pSockOteUniCastPc;		//PCユニキャスト用ソケット
	delete  pSockPcMultiCastPc;		//PCマルチキャストPC受信用ソケット
	delete  pSockOteMultiCastPc;	//OTEマルチキャストPC受信用ソケット
	return S_OK;
}

//****************************************************************************
/// <summary>
/// 受信処理用ウィンドウオープン
/// </summary>
/// <param name="hwnd"></param>
/// <returns></returns>
HWND COteIF::open_work_Wnd(HWND hwnd) {

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
	wcex.lpszMenuName = TEXT("OTE IF");
	wcex.lpszClassName = TEXT("OTE IF");
	wcex.hIconSm = NULL;

	ATOM fb = RegisterClassExW(&wcex);
	//メインウィンドウ
	hWnd_work = CreateWindowW(TEXT("OTE IF"), TEXT("OTE IF"), WS_OVERLAPPEDWINDOW,
				OTEIF_WORK_WND_X, OTEIF_WORK_WND_Y, OTEIF_WORK_WND_W0, OTEIF_WORK_WND_H0,
				hwnd, nullptr, hInst, nullptr);

	RECT rc;
	GetClientRect(hWnd_work, &rc);
	st_work_wnd.area_w = rc.right - rc.left;
	st_work_wnd.area_h = rc.bottom - rc.top;

	//表示フォント設定
	st_work_wnd.hfont_inftext = CreateFont(6, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));

	InvalidateRect(hWnd_work, NULL, TRUE);//表示更新

	ShowWindow(hWnd_work, SW_SHOW);
	UpdateWindow(hWnd_work);

	return hWnd_work;
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

LRESULT CALLBACK COteIF::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	int id;
	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		//ウィンドウにコントロール追加
		set_OTEIF_panel_objects(hWnd);

	}break;
	case WM_TIMER: {
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
	case ID_SOCK_EVENT_OTE_UNI_PC: {
		int nEvent = WSAGETSELECTEVENT(lParam);
		switch (nEvent) {
		case FD_READ: {
			if (rcv_ote_u_pc(&st_msg_ote_u_rcv) == S_OK) {				//PCからのユニキャストメッセージ受信
				cnt_rcv_ote_u++;

				if (S_OK == snd_pc_u_ote(set_msg_pc_u(), &addrin_pc_u_snd)) {//PC->OTMへユニキャスト送信
					cnt_snd_pc_u++;
				}
			}
			else {
				msg_ws = L"ERROR : rcv_ote_u_pc()";	wstr_out_inf(msg_ws);
			}
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case ID_SOCK_EVENT_PC_MULTI_PC: {
		int nEvent = WSAGETSELECTEVENT(lParam);
		switch (nEvent) {
		case FD_READ: {
			if (rcv_pc_m_pc(&st_msg_pc_m_pc_rcv) == S_OK) {				//PCからのユニキャストメッセージ受信
				cnt_rcv_pc_m++;
			}
			else {
				msg_ws = L"ERROR : rcv_ote_u_pc()";	wstr_out_inf(msg_ws);
			}
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case ID_SOCK_EVENT_OTE_MULTI_PC: {
		int nEvent = WSAGETSELECTEVENT(lParam);
		switch (nEvent) {
		case FD_READ: {
			if (rcv_ote_m_pc(&st_msg_ote_m_pc_rcv) == S_OK) {				//PCからのユニキャストメッセージ受信
				cnt_rcv_ote_m++;
			}
			else {
				msg_ws = L"ERROR : rcv_ote_u_pc()";	wstr_out_inf(msg_ws);
			}
		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;

	case WM_PAINT:{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
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

LRESULT CALLBACK COteIF::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化
		HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);
		//ウィンドウにコントロール追加
		set_OTEIF_panel_objects(hWnd);
		break;
	}
	case WM_TIMER: {
		break;
	}
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case BASE_ID_OTEIF_PB + ID_OTEIF_CHK_MSG: {
			if(BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_MSG], BM_GETCHECK, 0, 0)) st_work_wnd.is_disp_msg = true;
			else st_work_wnd.is_disp_msg = false;
		}break;

		case BASE_ID_OTEIF_PB + ID_OTEIF_CHK_HOLD: {
			if (BST_CHECKED == SendMessage(st_work_wnd.hctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_HOLD], BM_GETCHECK, 0, 0)) st_work_wnd.is_hold_disp = true;
			else st_work_wnd.is_hold_disp = false;
		}break;

		case BASE_ID_OTEIF_PB + ID_OTEIF_RADIO_SPU: {
			st_work_wnd.id_disp_body = ID_OTEIF_RADIO_SPU;
		}break;

		case BASE_ID_OTEIF_PB + ID_OTEIF_RADIO_ROU: {
			st_work_wnd.id_disp_body = ID_OTEIF_RADIO_ROU;
		}break;

		case BASE_ID_OTEIF_PB + ID_OTEIF_RADIO_SPM: {
			st_work_wnd.id_disp_body = ID_OTEIF_RADIO_SPM;
		}break;

		case BASE_ID_OTEIF_PB + ID_OTEIF_RADIO_RPM: {
			st_work_wnd.id_disp_body = ID_OTEIF_RADIO_RPM;
		}break;

		case BASE_ID_OTEIF_PB + ID_OTEIF_RADIO_ROM: {
			st_work_wnd.id_disp_body = ID_OTEIF_RADIO_ROM;
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
		PostQuitMessage(0);
	}break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return S_OK;
}
//*********************************************************************************************
/// <summary>
/// OTEウィンドウ上にオブジェクト配置
/// </summary>
void COteIF::set_OTEIF_panel_objects(HWND hWnd) {

	HINSTANCE hInst = (HINSTANCE)GetModuleHandle(0);

	//LABEL
	{
		st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_LABEL_0] = CreateWindowW(TEXT("STATIC"),
			st_work_wnd.ctrl_text[ID_OTEIF_CTRL_STATIC][ID_OTEIF_LABEL_0], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_LABEL_0].x, st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_LABEL_0].y,
			st_work_wnd.size_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_LABEL_0].cx, st_work_wnd.size_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_LABEL_0].cy,
			hWnd, (HMENU)(BASE_ID_OTEIF_STATIC + ID_OTEIF_LABEL_0), hInst, NULL);

		st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_LABEL_1] = CreateWindowW(TEXT("STATIC"),
			st_work_wnd.ctrl_text[ID_OTEIF_CTRL_STATIC][ID_OTEIF_LABEL_1], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_LABEL_1].x, st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_LABEL_1].y,
			st_work_wnd.size_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_LABEL_1].cx, st_work_wnd.size_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_LABEL_1].cy,
			hWnd, (HMENU)(BASE_ID_OTEIF_STATIC + ID_OTEIF_LABEL_1), hInst, NULL);
	}

	//IF STATIC
	{
		st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_SPU] = CreateWindowW(TEXT("STATIC"),
			st_work_wnd.ctrl_text[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_SPU], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_SPU].x, st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_SPU].y,
			st_work_wnd.size_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_SPU].cx, st_work_wnd.size_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_SPU].cy,
			hWnd, (HMENU)(BASE_ID_OTEIF_STATIC + ID_OTEIF_INF_SPU), hInst, NULL);
		st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_ROU] = CreateWindowW(TEXT("STATIC"),
			st_work_wnd.ctrl_text[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_ROU], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_ROU].x, st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_ROU].y,
			st_work_wnd.size_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_ROU].cx, st_work_wnd.size_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_ROU].cy,
			hWnd, (HMENU)(BASE_ID_OTEIF_STATIC + ID_OTEIF_INF_ROU), hInst, NULL);
		st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_SPM] = CreateWindowW(TEXT("STATIC"),
			st_work_wnd.ctrl_text[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_SPM], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_SPM].x, st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_SPM].y,
			st_work_wnd.size_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_SPM].cx, st_work_wnd.size_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_SPM].cy,
			hWnd, (HMENU)(BASE_ID_OTEIF_STATIC + ID_OTEIF_INF_SPM), hInst, NULL);
		st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_RPM] = CreateWindowW(TEXT("STATIC"),
			st_work_wnd.ctrl_text[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_RPM], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_RPM].x, st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_RPM].y,
			st_work_wnd.size_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_RPM].cx, st_work_wnd.size_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_RPM].cy,
			hWnd, (HMENU)(BASE_ID_OTEIF_STATIC + ID_OTEIF_INF_RPM), hInst, NULL);
		st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_ROM] = CreateWindowW(TEXT("STATIC"),
			st_work_wnd.ctrl_text[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_ROM], WS_CHILD | WS_VISIBLE | SS_LEFT,
			st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_ROM].x, st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_ROM].y,
			st_work_wnd.size_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_ROM].cx, st_work_wnd.size_ctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_ROM].cy,
			hWnd, (HMENU)(BASE_ID_OTEIF_STATIC + ID_OTEIF_INF_ROM), hInst, NULL);
	}

	
	//CHK BOX
	{
		st_work_wnd.hctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_MSG] = CreateWindow(L"BUTTON",
			st_work_wnd.ctrl_text[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_MSG], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_MSG].x, st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_MSG].y,
			st_work_wnd.size_ctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_MSG].cx, st_work_wnd.size_ctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_MSG].cy,
			hWnd, (HMENU)(BASE_ID_OTEIF_STATIC + ID_OTEIF_CHK_MSG), hInst, NULL);

		st_work_wnd.is_disp_msg = false;
		SendMessage(st_work_wnd.hctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_MSG], BM_SETCHECK, BST_UNCHECKED, 0L);


		st_work_wnd.hctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_HOLD] = CreateWindow(L"BUTTON",
			st_work_wnd.ctrl_text[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_HOLD], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_HOLD].x, st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_HOLD].y,
			st_work_wnd.size_ctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_HOLD].cx, st_work_wnd.size_ctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_HOLD].cy,
			hWnd, (HMENU)(BASE_ID_OTEIF_STATIC + ID_OTEIF_CHK_HOLD), hInst, NULL);

		st_work_wnd.is_hold_disp = false;
		SendMessage(st_work_wnd.hctrl[ID_OTEIF_CTRL_PB][ID_OTEIF_CHK_HOLD], BM_SETCHECK, BST_UNCHECKED, 0L);
	}

	//RADIO BUTTON
	{
		for (int i = ID_OTEIF_RADIO_SPU; i <= ID_OTEIF_RADIO_ROM; i++) {
			if (i == ID_OTEIF_RADIO_SPU)// | WS_GROUP あり
				st_work_wnd.hctrl[ID_OTEIF_CTRL_PB][i] = CreateWindow(L"BUTTON", 
					st_work_wnd.ctrl_text[ID_OTEIF_CTRL_PB][i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE | WS_GROUP,
					st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_PB][i].x, st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_PB][i].y,
					st_work_wnd.size_ctrl[ID_OTEIF_CTRL_PB][i].cx, st_work_wnd.size_ctrl[ID_OTEIF_CTRL_PB][i].cy,
					hWnd, (HMENU)(BASE_ID_OTEIF_PB + (LONG64)i), hInst, NULL);
			else
				st_work_wnd.hctrl[ID_OTEIF_CTRL_PB][i] = CreateWindow(L"BUTTON",
					st_work_wnd.ctrl_text[ID_OTEIF_CTRL_PB][i], WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE ,
					st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_PB][i].x, st_work_wnd.pt_ctrl[ID_OTEIF_CTRL_PB][i].y,
					st_work_wnd.size_ctrl[ID_OTEIF_CTRL_PB][i].cx, st_work_wnd.size_ctrl[ID_OTEIF_CTRL_PB][i].cy,
					hWnd, (HMENU)(BASE_ID_OTEIF_PB + (LONG64)i), hInst, NULL);
	}
	return;
}

//*********************************************************************************************
/// <summary>
/// 
/// </summary>
/// <param name="srcw"></param>
void COteIF::wstr_out_inf(const std::wstring& srcw) {
	return;
}


//*********************************************************************************************
/// <summary>
/// PC→OTE　Unicast　【PCのOTEからのユニキャスト受信ソケットで送信】
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COteIF::snd_pc_u_ote(LPST_PC_U_MSG pbuf, SOCKADDR_IN* p_addrin_to) {

	pbuf->body.lamp[ID_OTE_PB_CTRL_SOURCE] = pPLCio->input.rbuf.erm_bo[ID_MC_ERM_BO_160] & MC_SETBIT_LAMP_CONTROL_SOURCE;

	if (pSockOteUniCastPc->snd_udp_msg((char*)pbuf, sizeof(ST_PC_U_MSG), *p_addrin_to) == SOCKET_ERROR) {
		msg_wos.str() = pSockOteUniCastPc->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}
//*********************************************************************************************
/// <summary>
/// PC->PC マルチキャスト　【PCのPCからのマルチキャスト受信ソケットで送信】
/// </summary>
/// <param name="pbuf"></param>
/// <param name="pto_addrin"></param>
/// <returns></returns>
HRESULT COteIF::snd_pc_m_pc(LPST_PC_M_MSG pbuf) {
	if (pSockPcMultiCastPc->snd_udp_msg((char*)pbuf, sizeof(ST_PC_M_MSG), addrin_pc_m_snd) == SOCKET_ERROR) {
		msg_wos.str() = pSockPcMultiCastPc->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}

//*********************************************************************************************
/// <summary>
/// PC->OTE マルチキャスト　【PCのOTEからのマルチキャスト受信ソケットで送信】
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COteIF::snd_pc_m_ote(LPST_PC_M_MSG pbuf) {
	if (pSockOteMultiCastPc->snd_udp_msg((char*)pbuf, sizeof(ST_PC_M_MSG), addrin_pc_m_snd) == SOCKET_ERROR) {
		msg_wos.str() = pSockOteMultiCastPc->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}//OTEマルチキャスト送信処理
//受信処理用
//*********************************************************************************************
/// <summary>
///PCユニキャスト電文受信処理 (OTEユニキャストメッセージを受信）
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COteIF::rcv_ote_u_pc(LPST_OTE_U_MSG pbuf) {
	int nRtn = pSockOteUniCastPc->rcv_udp_msg((char*)pbuf, sizeof(ST_OTE_U_MSG));
	if (nRtn == SOCKET_ERROR) {
		msg_wos.str() = pSockOteUniCastPc->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}
//*********************************************************************************************
/// <summary>
///PCマルチキャスト電文受信処理  (PCマルチキャストメッセージを受信）
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COteIF::rcv_pc_m_pc(LPST_PC_M_MSG pbuf) {
	int nRtn = pSockPcMultiCastPc->rcv_udp_msg((char*)pbuf, sizeof(ST_PC_M_MSG));
	if (nRtn == SOCKET_ERROR) {
		msg_wos.str() = pSockPcMultiCastPc->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}
//*********************************************************************************************
/// <summary>
/// OTEユニキャスト電文受信処理(PCユニキャストメッセージを受信）
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT COteIF::rcv_ote_m_pc(LPST_OTE_M_MSG pbuf) {
	int nRtn = pSockOteMultiCastPc->rcv_udp_msg((char*)pbuf, sizeof(ST_PC_U_MSG));
	if (nRtn == SOCKET_ERROR) {
		msg_wos.str() = pSockOteUniCastPc->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}

/// <summary>
/// 通信状態表示テキスト更新
/// </summary>
/// <param name="is_msg"></param>
void COteIF::disp_update_spu(bool is_hold, bool is_msg, bool is_body) {
	
	if (is_hold)return;
	
	//送信カウント　送信先アドレス
	msg_wos.str(L"");
	msg_wos << L"SPU #:" << cnt_snd_pc_u << L" : " << addrin_pc_u_snd.sin_addr.S_un.S_un_b.s_b1 << L"." << addrin_pc_u_snd.sin_addr.S_un.S_un_b.s_b2 << L"." << addrin_pc_u_snd.sin_addr.S_un.S_un_b.s_b3 << L"." << addrin_pc_u_snd.sin_addr.S_un.S_un_b.s_b4 << L" "
		<< htons(addrin_pc_u_snd.sin_port) << L" : ";
	
	//ヘッダ部情報追加
	if (is_msg) {
		msg_wos << L" (PC)" << st_msg_pc_u_snd.head.myid << L" (EVENT)" << st_msg_pc_u_snd.head.code
			<< L" (IP)" << st_msg_pc_u_snd.head.addr.sin_addr.S_un.S_un_b.s_b1 << L"." << st_msg_pc_u_snd.head.addr.sin_addr.S_un.S_un_b.s_b2 << L"." << st_msg_pc_u_snd.head.addr.sin_addr.S_un.S_un_b.s_b3 << L"." << st_msg_pc_u_snd.head.addr.sin_addr.S_un.S_un_b.s_b4
			<< L" (PORT) " << htons(st_msg_pc_u_snd.head.addr.sin_port)
			<< L" (COM)" << st_msg_pc_u_snd.head.status << L" (接続中OTE)" << st_msg_pc_u_snd.head.tgid;
	}

	//テキスト出力（基本情報部）
	SetWindowText(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_SPU], msg_wos.str().c_str());

	//ボディ部情報
	if (is_body) {
		msg_wos.str(L"");
		//後で追加　msg_wos << L"SND BODY:";
		SetWindowText(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_BODY], msg_wos.str().c_str());
	}

	InvalidateRect(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_SPU], NULL, TRUE);//表示更新

	return;
}
void COteIF::disp_update_rou(bool is_hold, bool is_msg, bool is_body) {

	if (is_hold)return;

	//受信カウント　送信元アドレス
	msg_wos.str(L"");
	msg_wos << L"SPU #:" << cnt_rcv_ote_u << L" : " << addrin_ote_u_from.sin_addr.S_un.S_un_b.s_b1 << L"." << addrin_ote_u_from.sin_addr.S_un.S_un_b.s_b2 << L"." << addrin_ote_u_from.sin_addr.S_un.S_un_b.s_b3 << L"." << addrin_ote_u_from.sin_addr.S_un.S_un_b.s_b4 << L" "
		<< htons(addrin_ote_u_from.sin_port) << L" : ";

	//ヘッダ部情報追加
	if (is_msg) {
		msg_wos << L" (OTE)" << st_msg_ote_u_rcv.head.myid << L" (EVENT)" << st_msg_ote_u_rcv.head.code
			<< L" (IP)" << st_msg_ote_u_rcv.head.addr.sin_addr.S_un.S_un_b.s_b1 << L"." << st_msg_ote_u_rcv.head.addr.sin_addr.S_un.S_un_b.s_b2 << L"." << st_msg_ote_u_rcv.head.addr.sin_addr.S_un.S_un_b.s_b3 << L"." << st_msg_ote_u_rcv.head.addr.sin_addr.S_un.S_un_b.s_b4
			<< L" (PORT) " << htons(st_msg_ote_u_rcv.head.addr.sin_port)
			<< L" (COM)" << st_msg_ote_u_rcv.head.status << L" (接続中PC)" << st_msg_ote_u_rcv.head.tgid;
	}

	//テキスト出力（基本情報部）
	SetWindowText(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_ROU], msg_wos.str().c_str());

	//ボディ部情報
	if (is_body) {
		msg_wos.str(L"");
		//OTEユニキャスト受信　BODY部
		msg_wos <<L"#PB(主幹)" << st_msg_ote_u_rcv.body.pb_ope[ID_OTE_PB_CTRL_SOURCE] << L"(非停)" << st_msg_ote_u_rcv.body.pb_ope[ID_OTE_PB_ESTOP]
			<< L" #ﾉｯﾁ(MH)" << st_msg_ote_u_rcv.body.notch_pos[ID_HOIST] << L" (BH)" << st_msg_ote_u_rcv.body.notch_pos[ID_BOOM_H] << L" (SL)" << st_msg_ote_u_rcv.body.notch_pos[ID_SLEW];
		SetWindowText(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_BODY], msg_wos.str().c_str());
	}

	InvalidateRect(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_ROU], NULL, TRUE);//表示更新

	return;
}
void COteIF::disp_update_spm(bool is_hold, bool is_msg, bool is_body) {

	if (is_hold)return;

	//送信カウント　送信先アドレス
	msg_wos.str(L"");
	msg_wos << L"SPU #:" << cnt_snd_pc_m << L" : " << addrin_pc_m_snd.sin_addr.S_un.S_un_b.s_b1 << L"." << addrin_pc_m_snd.sin_addr.S_un.S_un_b.s_b2 << L"." << addrin_pc_m_snd.sin_addr.S_un.S_un_b.s_b3 << L"." << addrin_pc_m_snd.sin_addr.S_un.S_un_b.s_b4 << L" "
		<< htons(addrin_pc_m_snd.sin_port) << L" : ";

	//ヘッダ部情報追加
	if (is_msg) {
		msg_wos << L" (PC)" << st_msg_pc_m_snd.head.myid << L" (EVENT)" << st_msg_pc_m_snd.head.code
			<< L" (IP)" << st_msg_pc_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b1 << L"." << st_msg_pc_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b2 << L"." << st_msg_pc_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b3 << L"." << st_msg_pc_m_snd.head.addr.sin_addr.S_un.S_un_b.s_b4
			<< L" (PORT) " << htons(st_msg_pc_m_snd.head.addr.sin_port)
			<< L" (COM)" << st_msg_pc_m_snd.head.status << L" (接続中OTE)" << st_msg_pc_m_snd.head.tgid;
	}

	//テキスト出力（基本情報部）
	SetWindowText(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_SPM], msg_wos.str().c_str());

	//ボディ部情報
	if (is_body) {
		msg_wos.str(L"");
		//後で追加　msg_wos << L"SND BODY:";
		SetWindowText(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_BODY], msg_wos.str().c_str());
	}

	InvalidateRect(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_SPM], NULL, TRUE);//表示更新

	return;
}
void COteIF::disp_update_rpm(bool is_hold, bool is_msg, bool is_body) {
	if (is_hold)return;

	//送信カウント　送信先アドレス
	msg_wos.str(L"");
	msg_wos << L"RPU #:" << cnt_rcv_pc_m << L" : " << addrin_pc_m_from.sin_addr.S_un.S_un_b.s_b1 << L"." << addrin_pc_m_from.sin_addr.S_un.S_un_b.s_b2 << L"." << addrin_pc_m_from.sin_addr.S_un.S_un_b.s_b3 << L"." << addrin_pc_m_from.sin_addr.S_un.S_un_b.s_b4 << L" "
		<< htons(addrin_pc_m_from.sin_port) << L" : ";

	//ヘッダ部情報追加
	if (is_msg) {
		msg_wos << L" (PC)" << st_msg_pc_m_pc_rcv.head.myid << L" (EVENT)" << st_msg_pc_m_pc_rcv.head.code
			<< L" (IP)" << st_msg_pc_m_pc_rcv.head.addr.sin_addr.S_un.S_un_b.s_b1 << L"." << st_msg_pc_m_pc_rcv.head.addr.sin_addr.S_un.S_un_b.s_b2 << L"." << st_msg_pc_m_pc_rcv.head.addr.sin_addr.S_un.S_un_b.s_b3 << L"." << st_msg_pc_m_pc_rcv.head.addr.sin_addr.S_un.S_un_b.s_b4
			<< L" (PORT) " << htons(st_msg_pc_m_pc_rcv.head.addr.sin_port)
			<< L" (COM)" << st_msg_pc_m_pc_rcv.head.status << L" (接続中OTE)" << st_msg_pc_m_pc_rcv.head.tgid;
	}

	//テキスト出力（基本情報部）
	SetWindowText(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_RPM], msg_wos.str().c_str());

	//ボディ部情報
	if (is_body) {
		msg_wos.str(L"");
		//後で追加　msg_wos << L"RCV BODY:";
		SetWindowText(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_BODY], msg_wos.str().c_str());
	}

	InvalidateRect(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_RPM], NULL, TRUE);//表示更新

	return;
}
void COteIF::disp_update_rom(bool is_hold, bool is_msg, bool is_body) {

	if (is_hold)return;

	//送信カウント　送信先アドレス
	msg_wos.str(L"");
	msg_wos << L"ROM #:" << cnt_rcv_ote_m << L" : " << addrin_ote_m_from.sin_addr.S_un.S_un_b.s_b1 << L"." << addrin_ote_m_from.sin_addr.S_un.S_un_b.s_b2 << L"." << addrin_ote_m_from.sin_addr.S_un.S_un_b.s_b3 << L"." << addrin_ote_m_from.sin_addr.S_un.S_un_b.s_b4 << L" "
		<< htons(addrin_ote_m_from.sin_port) << L" : ";

	//ヘッダ部情報追加
	if (is_msg) {
		msg_wos << L" (PC)" << st_msg_ote_m_pc_rcv.head.myid << L" (EVENT)" << st_msg_ote_m_pc_rcv.head.code
			<< L" (IP)" << st_msg_ote_m_pc_rcv.head.addr.sin_addr.S_un.S_un_b.s_b1 << L"." << st_msg_ote_m_pc_rcv.head.addr.sin_addr.S_un.S_un_b.s_b2 << L"." << st_msg_ote_m_pc_rcv.head.addr.sin_addr.S_un.S_un_b.s_b3 << L"." << st_msg_ote_m_pc_rcv.head.addr.sin_addr.S_un.S_un_b.s_b4
			<< L" (PORT) " << htons(st_msg_ote_m_pc_rcv.head.addr.sin_port)
			<< L" (COM)" << st_msg_ote_m_pc_rcv.head.status << L" (接続中OTE)" << st_msg_ote_m_pc_rcv.head.tgid;
	}

	//テキスト出力（基本情報部）
	SetWindowText(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_ROM], msg_wos.str().c_str());

	//ボディ部情報
	if (is_body) {
		msg_wos.str(L"");
		//後で追加　msg_wos << L"RCV BODY:";
		SetWindowText(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_BODY], msg_wos.str().c_str());
	}
	
	InvalidateRect(st_work_wnd.hctrl[ID_OTEIF_CTRL_STATIC][ID_OTEIF_INF_ROM], NULL, TRUE);//表示更新
	return;
}


