#include "framework.h"
#include "OTE0.h"
#include <windowsx.h>       //# コモンコントロール
#include <winsock2.h>

#include <iostream>
#include <iomanip>
#include <sstream>

#include "OTE0panel.h"


/*****************************************************************************/
/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="hWnd">親ウィンドウのハンドル</param>
COte::COte(HWND hWnd) {
	hWnd_parent = hWnd;
	cnt_snd_ote_u = cnt_snd_ote_m_ote = cnt_snd_ote_m_pc = 0;
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
int COte::init_proc(LPST_OTE_WORK_WND pst) {
	//ウィンド処理用構造体アドレスセット
	pst_work_wnd = pst;

	//### ソケットアドレスセット
	memset(&addrin_pc_u_ote, 0, sizeof(SOCKADDR_IN));	memset(&addrin_pc_u_ote, 0, sizeof(SOCKADDR_IN)); memset(&addrin_ote_m_ote, 0, sizeof(SOCKADDR_IN));
	memset(&addrin_ote_m_pc_snd, 0, sizeof(SOCKADDR_IN));	memset(&addrin_ote_m_pc_snd, 0, sizeof(SOCKADDR_IN));

	set_sock_addr(&addrin_pc_u_ote, OTE_IF_UNICAST_IP_OTE0, OTE_IF_UNICAST_PORT_OTE);
	set_sock_addr(&addrin_ote_m_ote, OTE_IF_UNICAST_IP_OTE0, OTE_IF_MULTICAST_PORT_OTE2OTE);
	set_sock_addr(&addrin_pc_m_ote, OTE_IF_UNICAST_IP_OTE0, OTE_IF_MULTICAST_PORT_PC2OTE);

	set_sock_addr(&addrin_ote_u_snd, OTE_IF_UNICAST_IP_PC, OTE_IF_UNICAST_PORT_PC);
	set_sock_addr(&addrin_ote_m_ote_snd, OTE_IF_MULTICAST_IP_OTE, OTE_IF_MULTICAST_PORT_OTE2OTE);
	set_sock_addr(&addrin_ote_m_pc_snd, OTE_IF_MULTICAST_IP_OTE, OTE_IF_MULTICAST_PORT_OTE2PC);

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
	if (pSockPcUniCastOte->init_sock(hWnd_parent, addrin_pc_u_ote) != S_OK) { msg_wos.str() = pSockPcUniCastOte->err_msg.str(); return NULL; }

	//マルチキャスト用

#if 0
	SOCKADDR_IN addr_tmp;
	set_sock_addr(&addr_tmp, OTE_IF_MULTICAST_IP_PC, NULL);
	if (pSockPcMultiCastOte->init_sock_m(hWnd_parent, addrin_pc_m_ote, addr_tmp) != S_OK) { msg_wos.str() = pSockPcMultiCastOte->err_msg.str(); return NULL; }
	set_sock_addr(&addr_tmp, OTE_IF_MULTICAST_IP_OTE, NULL);
	if (pSockOteMultiCastOte->init_sock_m(hWnd_parent, addrin_ote_m_ote, addr_tmp) != S_OK) { msg_wos.str() = pSockOteMultiCastOte->err_msg.str(); return NULL; }
#else
	set_sock_addr(&addrin_pc_m_rcv, OTE_IF_MULTICAST_IP_PC, NULL);
	if (pSockPcMultiCastOte->init_sock_m(hWnd_parent, addrin_pc_m_ote, addrin_pc_m_rcv) != S_OK) { msg_wos.str() = pSockPcMultiCastOte->err_msg.str(); return NULL; }
	set_sock_addr(&addrin_ote_m_rcv, OTE_IF_MULTICAST_IP_OTE, NULL);
	if (pSockOteMultiCastOte->init_sock_m(hWnd_parent, addrin_ote_m_ote, addrin_ote_m_rcv) != S_OK) { msg_wos.str() = pSockOteMultiCastOte->err_msg.str(); return NULL; }
#endif


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
	for (int i = 0; i < 6; i++) {
		data.pos[i] = st_msg_pc_u_rcv.body.pos[i];
		data.v_fb[i] = st_msg_pc_u_rcv.body.v_fb[i];
		data.v_ref[i] = st_msg_pc_u_rcv.body.v_ref[i];
	}

	data.deg_sl=DEG1RAD * (double)data.pos[ID_SLEW];
	data.deg_bh= (double)st_msg_pc_u_rcv.body.plc_in.cab_ai[PLC_IF_CAB_AI_BH_ANGLE] * 0.018 + 4.462;//PLCソフト計算式
	data.load[ID_HOIST] = st_msg_pc_u_rcv.body.plc_in.cab_ai[PLC_IF_CAB_AI_MH_LOAD]*0.206;
	data.load[ID_AHOIST] = st_msg_pc_u_rcv.body.plc_in.cab_ai[PLC_IF_CAB_AI_AH_LOAD]*0.0312;

	return 0;
}
/*****************************************************************************/
/// <summary>
/// OTEユニキャストメッセージ送信バッファセット
/// </summary>
/// <returns></returns>
LPST_OTE_U_MSG COte::set_msg_ote_u() {
	//Header部
	st_msg_ote_u_snd.head.addr = addrin_pc_u_ote;
	st_msg_ote_u_snd.head.myid =ID_OTE0;
	st_msg_ote_u_snd.head.tgid = st_msg_pc_u_rcv.head.myid;
	st_msg_ote_u_snd.head.code = 0x01;
	st_msg_ote_u_snd.head.status = 0x2;

	//Body部
	//操作卓PB入力　UINT16		pb_ope[128];
	memcpy(st_msg_ote_u_snd.body.pb_ope, pst_work_wnd->pb_stat, 256);//OFF DELAYカウンタ

	//ノッチデータ　UINT16　pb_notch[128];
	memcpy(st_msg_ote_u_snd.body.pb_notch, pst_work_wnd->notch_pb_stat, 128);

	//ノッチ入力位置　INT16		notch_pos[2][MOTION_ID_MAX];
	memcpy(st_msg_ote_u_snd.body.notch_pos[ID_OTE_NOTCH_POS_HOLD], pst_work_wnd->notch_pos[ID_OTE_NOTCH_POS_HOLD], 16);
	memcpy(st_msg_ote_u_snd.body.notch_pos[ID_OTE_NOTCH_POS_TRIG], pst_work_wnd->notch_pos[ID_OTE_NOTCH_POS_TRIG], 16);

	//自動目標位置,自動選択　	double		auto_tg_pos[MOTION_ID_MAX];	
	for (int i = 0; i < MOTION_ID_MAX; i++) {
		st_msg_ote_u_snd.body.auto_tg_pos[i] = data.d_tgpos[OTE_ID_HOT_TARGET][i];
		st_msg_ote_u_snd.body.auto_sel[i] = data.auto_sel[i];
	}

	//その他状態
	//INT32		ope_mode;						//0:モニタのみ　1:運転入力有効
	//INT32		grip_status;
	st_msg_ote_u_snd.body.ope_mode = data.ope_mode;
	st_msg_ote_u_snd.body.grip_status = data.grip_stat.ui32;
	  
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
	if (pSockPcMultiCastOte->snd_udp_msg((char*)pbuf, sizeof(ST_OTE_M_MSG), addrin_ote_m_pc_snd) == SOCKET_ERROR) {
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
	if (pSockOteMultiCastOte->snd_udp_msg((char*)pbuf, sizeof(ST_OTE_M_MSG), addrin_ote_m_ote_snd) == SOCKET_ERROR) {
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

