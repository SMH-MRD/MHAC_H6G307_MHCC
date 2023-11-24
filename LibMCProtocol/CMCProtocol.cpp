#include <stdexcept>

#include "pch.h"
#include "framework.h"
#include "CMCProtocol.h"

UINT8 CMCProtocol::snd_buf[MAX_MC_DATA * 2];
UINT8 CMCProtocol::read_req_snd_buf[MAX_MC_DATA * 2];//読み込み要求送信用バッファ
UINT8 CMCProtocol::write_req_snd_buf[MAX_MC_DATA * 2];//書込み要求送信用バッファ
UINT8 CMCProtocol::rcv_buf[MAX_MC_DATA * 2];
ST_MC_WND CMCProtocol::st_work_wnd;
UINT16 CMCProtocol::D_no_r, CMCProtocol::D_no_w, CMCProtocol::n_D_read, CMCProtocol::n_D_write;
ST_XE_REQ CMCProtocol::mc_req_msg_r, CMCProtocol::mc_req_msg_w;
ST_XE_RES CMCProtocol::mc_res_msg_r, CMCProtocol::mc_res_msg_w, CMCProtocol::mc_res_msg_err;
CSockAlpha* CMCProtocol::pMCSock;
std::wostringstream CMCProtocol::msg_wos;

//グローバル
LARGE_INTEGER frequency;                //システムの周波数
LARGE_INTEGER start_count_w, end_count_w,start_count_r, end_count_r;   //システムカウント数
LONGLONG spancount_max_w, spancount_max_r;

//*********************************************************************************************　
/// <summary>
/// コンストラクタ
/// </summary>
CMCProtocol::CMCProtocol() {
	D_no_r= D_no_w = n_D_read = n_D_write=0;
	memset(&st_work_wnd, 0, sizeof(ST_MC_WND));
}

//*********************************************************************************************　
/// <summary>
/// デストラクタ
/// </summary>
CMCProtocol::~CMCProtocol() {
	close();
}
//*********************************************************************************************　
/// <summary>
/// 指定ソケット準備、読み書きデバイス指定
/// </summary>
/// <param name="hWnd">ソケットイベントを受信するウィンドウのハンドル</param>
/// <param name="paddrinc">	受信ソケットアドレス</param>
/// <param name="paddrins">	送信ソケットアドレス</param>
/// <param name="no_r">		読み出しDデバイス先頭番号</param>
/// <param name="num_r">	読み出しDデバイス数</param>
/// <param name="no_w">		書き込みDデバイス先頭番号</param>
/// <param name="num_w">	書き込みDデバイス数号</param>
/// <returns></returns>
HWND CMCProtocol::Initialize(HWND hwnd,SOCKADDR_IN* paddrinc, SOCKADDR_IN* paddrins,UINT16 no_r, UINT16 num_r, UINT16 no_w, UINT16 num_w) {
	
	st_work_wnd.hWnd_parent = hwnd;
	//WorkWindow生成、初期化
	st_work_wnd.hInst = (HINSTANCE)GetModuleHandle(0);

	//# ソケットアドレス設定
	memcpy(&st_work_wnd.addrins, paddrins, sizeof(SOCKADDR_IN));
	memcpy(&st_work_wnd.addrinc, paddrinc, sizeof(SOCKADDR_IN));

	//読み書きデバイス設定
	set_access_D_r(no_r, num_r);
	set_access_D_w(no_w, num_w);

	//送信バッファフォーマット設定
	set_sndbuf_read_D_3E();		//3E　Dデバイス読み込み用送信フォーマットセット
	set_sndbuf_write_D_3E();	//3E　Dデバイス書き込み用送信フォーマットセット

	//クライアントソケット
	memcpy(&st_work_wnd.addrinc, paddrinc, sizeof(SOCKADDR_IN));
	pMCSock = new CSockAlpha(UDP_PROTOCOL, ACCESS_TYPE_CLIENT, ID_SOCK_MC_CLIENT);

	//通信イベント処理用ウィンドウオープン
	st_work_wnd.hWnd_parent = hwnd;								//親ウィンドウセット
	st_work_wnd.hWnd = open_work_Wnd(st_work_wnd.hWnd_parent);	//作業ウィンドOPEN

	if (pMCSock->Initialize() != S_OK) {									//受信ソケット生成
		msg_wos.str() = pMCSock->err_msg.str();
		return NULL;
	}

	if (pMCSock->init_sock(st_work_wnd.hWnd, st_work_wnd.addrinc) != S_OK) {//受信ソケット設定
		msg_wos.str() = pMCSock->err_msg.str();
		return NULL;
	}

	return st_work_wnd.hWnd;
}
//**********************************************************************************************　
/// <summary>
/// クローズ処理
/// </summary>
/// <returns></returns>
HRESULT CMCProtocol::close() {

	delete pMCSock;

	return 0;
}
//**********************************************************************************************　
/// <summary>
/// 3Eフォーマット Wデバイス読み込み用送信バッファセット
/// 
/// </summary>
/// <param name="d_no"></param>
/// <param name="n_read"></param>
/// <returns></returns>
HRESULT CMCProtocol::set_sndbuf_read_D_3E() {
	UINT8* p8 = read_req_snd_buf;
	int size, len = 0;
	mc_req_msg_r.subcode = CODE_3E_FORMAT;
	memcpy(p8, &mc_req_msg_r.subcode, size = sizeof(mc_req_msg_r.subcode));	len += size; p8 += size;

	mc_req_msg_r.nNW = CODE_NW;
	memcpy(p8, &mc_req_msg_r.nNW, size = sizeof(mc_req_msg_r.nNW));			len += size; p8 += size;

	mc_req_msg_r.nPC = CODE_PC;
	memcpy(p8, &mc_req_msg_r.nPC, size = sizeof(mc_req_msg_r.nPC));			len += size; p8 += size;

	mc_req_msg_r.nUIO = CODE_UIO;
	memcpy(p8, &mc_req_msg_r.nUIO, size = sizeof(mc_req_msg_r.nUIO));	len += size; p8 += size;

	mc_req_msg_r.nUcode = CODE_UNODE;
	memcpy(p8, &mc_req_msg_r.nUcode, size = sizeof(mc_req_msg_r.nUcode));	len += size; p8 += size;

	mc_req_msg_r.len = 12; 	//以下のバイト数
	memcpy(p8, &mc_req_msg_r.len, size = sizeof(mc_req_msg_r.len));			len += size; p8 += size;

	mc_req_msg_r.timer = CODE_MON_TIMER;
	memcpy(p8, &mc_req_msg_r.timer, size = sizeof(mc_req_msg_r.timer));		len += size; p8 += size;

	mc_req_msg_r.com = CODE_COMMAND_READ;
	memcpy(p8, &mc_req_msg_r.com, size = sizeof(mc_req_msg_r.com));			len += size; p8 += size;

	mc_req_msg_r.scom = CODE_SUBCOM_READ;
	memcpy(p8, &mc_req_msg_r.scom, size = sizeof(mc_req_msg_r.scom));		len += size; p8 += size;

	mc_req_msg_r.d_no = D_no_r;
	memcpy(p8, &mc_req_msg_r.d_no, size = sizeof(mc_req_msg_r.d_no));		len += size; p8 += size;

	mc_req_msg_r.d_no0 = CODE_3E_NULL;	//簡単のため上位１byteは使わない
	memcpy(p8, &mc_req_msg_r.d_no0, size = sizeof(mc_req_msg_r.d_no0));		len += size; p8 += size;

	mc_req_msg_r.d_code = CODE_DEVICE_D;
	memcpy(p8, &mc_req_msg_r.d_code, size = sizeof(mc_req_msg_r.d_code));	len += size; p8 += size;

	mc_req_msg_r.n_device = n_D_read;
	memcpy(p8, &mc_req_msg_r.n_device, size = sizeof(mc_req_msg_r.n_device)); len += size; p8 += size;

	return S_OK; 
}							
//**********************************************************************************************　
/// <summary>
/// //3Eフォーマット Wデバイス書き込み用送信バッファセット
/// </summary>
/// <param name="d_no"></param>
/// <param name="n_write"></param>
/// <returns></returns>
HRESULT CMCProtocol::set_sndbuf_write_D_3E() { 
	UINT8* p8 = write_req_snd_buf;
	int size, len = 0;
	mc_req_msg_w.subcode = CODE_3E_FORMAT;
	memcpy(p8, &mc_req_msg_w.subcode, size = sizeof(mc_req_msg_w.subcode));	len += size; p8 += size;

	mc_req_msg_w.nNW = CODE_NW;
	memcpy(p8, &mc_req_msg_w.nNW, size = sizeof(mc_req_msg_w.nNW));			len += size; p8 += size;

	mc_req_msg_w.nPC = CODE_PC;
	memcpy(p8, &mc_req_msg_w.nPC, size = sizeof(mc_req_msg_w.nPC));			len += size; p8 += size;

	mc_req_msg_w.nUIO = CODE_UIO;
	memcpy(p8, &mc_req_msg_w.nUIO, size = sizeof(mc_req_msg_w.nUIO));		len += size; p8 += size;

	mc_req_msg_w.nUcode = CODE_UNODE;
	memcpy(p8, &mc_req_msg_w.nUcode, size = sizeof(mc_req_msg_w.nUcode));	len += size; p8 += size;


	mc_req_msg_w.len = 12 + n_D_write * 2; 	//以下のバイト数
	memcpy(p8, &mc_req_msg_w.len, size = sizeof(mc_req_msg_w.len));			len += size; p8 += size;

	mc_req_msg_w.timer = CODE_MON_TIMER;
	memcpy(p8, &mc_req_msg_w.timer, size = sizeof(mc_req_msg_w.timer));		len += size; p8 += size;

	mc_req_msg_w.com = CODE_COMMAND_WRITE;
	memcpy(p8, &mc_req_msg_w.com, size = sizeof(mc_req_msg_w.com));			len += size; p8 += size;

	mc_req_msg_w.scom = CODE_SUBCOM_WRITE;
	memcpy(p8, &mc_req_msg_w.scom, size = sizeof(mc_req_msg_w.scom));		len += size; p8 += size;

	mc_req_msg_w.d_no = D_no_w;
	memcpy(p8, &mc_req_msg_w.d_no, size = sizeof(mc_req_msg_w.d_no));		len += size; p8 += size;

	mc_req_msg_w.d_no0 = CODE_3E_NULL;//簡単のため上位１byteは使わない
	memcpy(p8, &mc_req_msg_w.d_no0, size = sizeof(mc_req_msg_w.d_no0));		len += size; p8 += size;

	mc_req_msg_w.d_code = CODE_DEVICE_D;
	memcpy(p8, &mc_req_msg_w.d_code, size = sizeof(mc_req_msg_w.d_code));	len += size; p8 += size;

	mc_req_msg_w.n_device = n_D_write;
	memcpy(p8, &mc_req_msg_w.n_device, size = sizeof(mc_req_msg_w.n_device)); len += size; p8 += size;

	for (int i = 0; i < n_D_write; i++) *(p8 + i)=0;

	return S_OK; 
}	

//**********************************************************************************************　
/// <summary>
///	3Eフォーマット Wデバイス書き込み要求送信
/// </summary>
/// <param name="p_data"></param>
/// <param name="n_write"></param>
/// <returns></returns>
HRESULT CMCProtocol::send_write_req_D_3E(void* p_data) { 

	UINT8* p8 = write_req_snd_buf;
	int len=0,size= 0;
	
	size = sizeof(mc_req_msg_w.subcode);	len += size; 
	size = sizeof(mc_req_msg_w.nNW);		len += size;
	size = sizeof(mc_req_msg_w.nPC);		len += size;
	size = sizeof(mc_req_msg_w.nUIO);		len += size;
	size = sizeof(mc_req_msg_w.nUcode);		len += size;
	size = sizeof(mc_req_msg_w.len);		len += size;
	size = sizeof(mc_req_msg_w.timer);		len += size;
	size = sizeof(mc_req_msg_w.com);		len += size;
	size = sizeof(mc_req_msg_w.scom);		len += size;
	size = sizeof(mc_req_msg_w.d_no);		len += size;
	size = sizeof(mc_req_msg_w.d_no0);		len += size;
	size = sizeof(mc_req_msg_w.d_code);		len += size;
	size = sizeof(mc_req_msg_w.n_device);	len += size;
	//len = 21;					
	memcpy(p8+len, p_data, size = n_D_write * 2); len += size;

	//	送信バッファのサーバーアドレスに送信 
	if (pMCSock->snd_udp_msg((char*)write_req_snd_buf, len, st_work_wnd.addrins) == SOCKET_ERROR) {
		msg_wos.str() = pMCSock->err_msg.str();
		return S_FALSE;
	}
	return S_OK; 
}
//**********************************************************************************************　
/// <summary>
/// 3Eフォーマット Wデバイス読み込み要求送信
/// </summary>
/// <param name="d_no"></param>
/// <param name="n_read"></param>
/// <returns></returns>
HRESULT CMCProtocol::send_read_req_D_3E() {

	int size, len = 0;
	size = sizeof(mc_req_msg_r.subcode);len += size;
	size = sizeof(mc_req_msg_r.nNW);	len += size;
	size = sizeof(mc_req_msg_r.nPC);	len += size;
	size = sizeof(mc_req_msg_r.nUIO);	len += size;
	size = sizeof(mc_req_msg_r.nUcode);	len += size;
	size = sizeof(mc_req_msg_r.len);	len += size;
	size = sizeof(mc_req_msg_r.timer);	len += size; 
	size = sizeof(mc_req_msg_r.com);	len += size; 
	size = sizeof(mc_req_msg_r.scom);	len += size;
	size = sizeof(mc_req_msg_r.d_no);	len += size;
	size = sizeof(mc_req_msg_r.d_no0);	len += size;
	size = sizeof(mc_req_msg_r.d_code);	len += size; 
	size = sizeof(mc_req_msg_r.n_device); len += size; 

	// len=21

	//	送信バッファのサーバーアドレスに送信 
	if (pMCSock->snd_udp_msg((char*)read_req_snd_buf, len, st_work_wnd.addrins) == SOCKET_ERROR) {
		msg_wos.str() = pMCSock->err_msg.str();
		return S_FALSE;
	}
	return S_OK;
}
//**********************************************************************************************　
/// <summary>
/// 3Eフォーマットでの書き込み要求送信関数
/// </summary>
/// <param name="d_no">先頭デバイス番号(D)</param>
/// <param name="n_write"></param>
/// <param name="pdata"></param>
/// <returns></returns>
HRESULT CMCProtocol::send_write_req(UINT16 d_no, UINT16 n_write, UINT16* pdata) {

	UINT8* p8 = snd_buf;
	int size, len = 0;
	mc_req_msg_w.subcode = CODE_3E_FORMAT;
	memcpy(p8, &mc_req_msg_w.subcode, size = sizeof(mc_req_msg_w.subcode));	len += size; p8 += size;

	mc_req_msg_w.nNW = CODE_NW;
	memcpy(p8, &mc_req_msg_w.nNW, size = sizeof(mc_req_msg_w.nNW));			len += size; p8 += size;

	mc_req_msg_w.nPC = CODE_PC;
	memcpy(p8, &mc_req_msg_w.nPC, size = sizeof(mc_req_msg_w.nPC));			len += size; p8 += size;

	mc_req_msg_w.nUIO = CODE_UIO;
	memcpy(p8, &mc_req_msg_w.nUIO, size = sizeof(mc_req_msg_w.nUIO));		len += size; p8 += size;

	mc_req_msg_w.nUcode = CODE_UNODE;
	memcpy(p8, &mc_req_msg_w.nUcode, size = sizeof(mc_req_msg_w.nUcode));	len += size; p8 += size;


	mc_req_msg_w.len = 12 + n_write * 2; 	//以下のバイト数
	memcpy(p8, &mc_req_msg_w.len, size = sizeof(mc_req_msg_w.len));			len += size; p8 += size;

	mc_req_msg_w.timer = CODE_MON_TIMER;
	memcpy(p8, &mc_req_msg_w.timer, size = sizeof(mc_req_msg_w.timer));		len += size; p8 += size;

	mc_req_msg_w.com = CODE_COMMAND_WRITE;
	memcpy(p8, &mc_req_msg_w.com, size = sizeof(mc_req_msg_w.com));			len += size; p8 += size;

	mc_req_msg_w.scom = CODE_SUBCOM_WRITE; 
	memcpy(p8, &mc_req_msg_w.scom, size = sizeof(mc_req_msg_w.scom));		len += size; p8 += size;

	mc_req_msg_w.d_no = d_no;
	memcpy(p8, &mc_req_msg_w.d_no, size = sizeof(mc_req_msg_w.d_no));		len += size; p8 += size;

	mc_req_msg_w.d_no0 = CODE_3E_NULL;//簡単のため上位１byteは使わない
	memcpy(p8, &mc_req_msg_w.d_no0, size = sizeof(mc_req_msg_w.d_no0));		len += size; p8 += size;

	mc_req_msg_w.d_code = CODE_DEVICE_D; 
	memcpy(p8, &mc_req_msg_w.d_code, size = sizeof(mc_req_msg_w.d_code));	len += size; p8 += size;

	mc_req_msg_w.n_device = n_write; 
	memcpy(p8, &mc_req_msg_w.n_device, size = sizeof(mc_req_msg_w.n_device));len += size; p8 += size;

	for (int i = 0; i < n_write; i++) mc_req_msg_w.req_data[i] = *(pdata + i); 
	size = n_write * sizeof(mc_req_msg_w.req_data[0]);
	memcpy(p8, &mc_req_msg_w.req_data, size);								len += size; p8 += size;


	//	送信バッファのサーバーアドレスに送信 
	if (pMCSock->snd_udp_msg((char*)snd_buf, len, st_work_wnd.addrins) == SOCKET_ERROR) {
		msg_wos.str() = pMCSock->err_msg.str();
		return S_FALSE;
	}

	
	return S_OK;
}
//*********************************************************************************************　
/// <summary>
/// Qシリーズ3Eフォーマットセット WORD読み込み用関数
/// </summary>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT CMCProtocol::send_read_req(UINT16 d_no, UINT16 n_read) {
	UINT8* p8 = snd_buf;
	int size,len = 0;
	mc_req_msg_r.subcode	= CODE_3E_FORMAT;	
	memcpy(p8, &mc_req_msg_r.subcode, size = sizeof(mc_req_msg_r.subcode));	len += size; p8 += size;

	mc_req_msg_r.nNW		= CODE_NW;
	memcpy(p8, &mc_req_msg_r.nNW, size = sizeof(mc_req_msg_r.nNW));			len += size; p8 += size;

	mc_req_msg_r.nPC		= CODE_PC;
	memcpy(p8, &mc_req_msg_r.nPC, size = sizeof(mc_req_msg_r.nPC));			len += size; p8 += size;

	mc_req_msg_r.nUIO		= CODE_UIO; 
	memcpy(p8, &mc_req_msg_r.nUIO, size = sizeof(mc_req_msg_r.nUIO));	len += size; p8 += size;

	mc_req_msg_r.nUcode		= CODE_UNODE; 
	memcpy(p8, &mc_req_msg_r.nUcode, size = sizeof(mc_req_msg_r.nUcode));	len += size; p8 += size;

	mc_req_msg_r.len		= 12; 	//以下のバイト数
	memcpy(p8, &mc_req_msg_r.len, size = sizeof(mc_req_msg_r.len));			len += size; p8 += size;
	
	mc_req_msg_r.timer		= CODE_MON_TIMER; 
	memcpy(p8, &mc_req_msg_r.timer, size = sizeof(mc_req_msg_r.timer));		len += size; p8 += size;

	mc_req_msg_r.com		= CODE_COMMAND_READ; 
	memcpy(p8, &mc_req_msg_r.com, size = sizeof(mc_req_msg_r.com));			len += size; p8 += size;

	mc_req_msg_r.scom		= CODE_SUBCOM_READ; 
	memcpy(p8, &mc_req_msg_r.scom, size = sizeof(mc_req_msg_r.scom));		len += size; p8 += size;

	mc_req_msg_r.d_no		= d_no;	
	memcpy(p8, &mc_req_msg_r.d_no, size = sizeof(mc_req_msg_r.d_no));		len += size; p8 += size;

	mc_req_msg_r.d_no0		= CODE_3E_NULL;	//簡単のため上位１byteは使わない
	memcpy(p8, &mc_req_msg_r.d_no0, size = sizeof(mc_req_msg_r.d_no0));		len += size; p8 += size;

	mc_req_msg_r.d_code		= CODE_DEVICE_D; 
	memcpy(p8, &mc_req_msg_r.d_code, size = sizeof(mc_req_msg_r.d_code));	len += size; p8 += size;

	mc_req_msg_r.n_device	= n_read; 
	memcpy(p8, &mc_req_msg_r.n_device, size = sizeof(mc_req_msg_r.n_device));len += size; p8 += size;

	//	送信バッファのサーバーアドレスに送信 
	if (pMCSock->snd_udp_msg((char*)snd_buf, len, st_work_wnd.addrins) == SOCKET_ERROR) {
		msg_wos.str() = pMCSock->err_msg.str();
		return S_FALSE;
	}

	return S_OK;
}

//*********************************************************************************************
/// <summary>
/// ソケット受信イベント受信データ分析
/// </summary>
/// <returns></returns>
UINT CMCProtocol::rcv_msg_3E() {

	//ソケット受信処理
	int nRtn = pMCSock->rcv_udp_msg((char*)rcv_buf, sizeof(ST_XE_RES));
	if (nRtn == SOCKET_ERROR) {
		msg_wos.str() = pMCSock->err_msg.str();
		return S_FALSE;
	}
	st_work_wnd.addrfrom = pMCSock->addr_in_from;

	//受信データ分析
	ST_XE_RES res;
	UINT8* p8 = rcv_buf;

	int size, len = 0;

	memcpy(&res.subcode, rcv_buf, size = sizeof(res.subcode));	len += size; p8 += size;
	if (res.subcode == CODE_4E_FORMAT) 	return S_FALSE;
	memcpy(&res.nNW, p8, size = sizeof(res.nNW));			len += size; p8 += size;
	memcpy(&res.nPC, p8, size = sizeof(res.nPC));			len += size; p8 += size;
	memcpy(&res.nUIO, p8, size = sizeof(res.nUIO));			len += size; p8 += size;
	memcpy(&res.nUcode, p8, size = sizeof(res.nUcode));		len += size; p8 += size;
	memcpy(&res.len, p8, size = sizeof(res.len));			len += size; p8 += size;
	memcpy(&res.endcode, p8, size = sizeof(res.endcode));	len += size; p8 += size;

	if (res.endcode) {
		memcpy(res.res_data, p8, size = res.len);	len += size;
		memcpy(&mc_res_msg_err, &res, len);	
		return MC_RES_ERR;
	}
	else {
		if (res.len == 2) {
			memcpy(&mc_res_msg_w, &res, len);
			return MC_RES_WRITE;
		}
		else {
			memcpy(res.res_data, p8, size = res.len);	len += size;
			memcpy(&mc_res_msg_r, &res, len);
			return MC_RES_READ;
		}
	}

	return S_FALSE;

}

//*********************************************************************************************　
/// <summary>
/// 受信バッファを構造体に展開
/// </summary>
/// <param name="pbuf">受信内容展開先構造体のポインタ</param>
/// <returns>HRESULT</returns>
HRESULT CMCProtocol::parse_rcv_buf(UINT8* p8, LPST_XE_RES pbuf, int bufsize) {

	int size, len = 0;
	memcpy(&pbuf->subcode, p8, size = sizeof(pbuf->subcode));	len += size; p8 += size;
	if (pbuf->subcode == CODE_4E_FORMAT) {
		memcpy(&pbuf->serial, p8, size = sizeof(pbuf->serial));	len += size; p8 += size;
		memcpy(&pbuf->blank, p8, size = sizeof(pbuf->blank));	len += size; p8 += size;
	}
	memcpy(&pbuf->nNW, p8, size = sizeof(pbuf->nNW));			len += size; p8 += size;
	memcpy(&pbuf->nPC, p8, size = sizeof(pbuf->nPC));			len += size; p8 += size;
	memcpy(&pbuf->nUIO, p8, size = sizeof(pbuf->nUIO));			len += size; p8 += size;
	memcpy(&pbuf->nUcode, p8, size = sizeof(pbuf->nUcode));		len += size; p8 += size;
	memcpy(&pbuf->len, p8, size = sizeof(pbuf->len));			len += size; p8 += size;
	memcpy(&pbuf->endcode, p8, size = sizeof(pbuf->endcode));	len += size; p8 += size;

	size = bufsize - len;
	if (size < 0) return S_FALSE;
	memcpy(&pbuf->res_data, p8, size);							len += size; p8 += size;

	if (pbuf->endcode != 0) return S_FALSE;

	return S_OK;
}

//*********************************************************************************************　
/// <summary>
/// PLCからの受信メッセージを構造体に展開
/// </summary>
/// <param name="pbuf">受信内容展開先構造体のポインタ</param>
/// <returns>HRESULT</returns>
HRESULT CMCProtocol::rcv_res(LPST_XE_RES pbuf) {
	int nRtn = pMCSock->rcv_udp_msg((char*)rcv_buf, sizeof(ST_XE_RES));
	if (nRtn == SOCKET_ERROR) {
		msg_wos.str() = pMCSock->err_msg.str();
		return S_FALSE;
	}

	st_work_wnd.addrfrom = pMCSock->addr_in_from;

	return parse_rcv_buf((UINT8*)rcv_buf, pbuf, nRtn);
}

//*********************************************************************************************　
/// <summary>
/// 送信バッファの内容を構造体に展開
/// </summary>
/// <param name="p8"></param>
/// <param name="pbuf"></param>
/// <returns></returns>
HRESULT CMCProtocol::parse_snd_buf(UINT8* p8, LPST_XE_REQ pbuf) {
	int size, len = 0;
	memcpy(&pbuf->subcode, p8, size = sizeof(pbuf->subcode));	len += size; p8 += size;
	memcpy(&pbuf->nNW, p8, size = sizeof(pbuf->nNW));			len += size; p8 += size;
	memcpy(&pbuf->nPC, p8, size = sizeof(pbuf->nPC));			len += size; p8 += size;
	memcpy(&pbuf->nUIO, p8, size = sizeof(pbuf->nUIO));			len += size; p8 += size;
	memcpy(&pbuf->nUcode, p8, size = sizeof(pbuf->nUcode));		len += size; p8 += size;
	memcpy(&pbuf->len, p8, size = sizeof(pbuf->len));			len += size; p8 += size;
	memcpy(&pbuf->timer, p8, size = sizeof(pbuf->timer));		len += size; p8 += size;
	memcpy(&pbuf->com, p8, size = sizeof(pbuf->com));			len += size; p8 += size;
	memcpy(&pbuf->scom, p8, size = sizeof(pbuf->scom));			len += size; p8 += size;
	memcpy(&pbuf->d_no, p8, size = sizeof(pbuf->d_no));			len += size; p8 += size;//3バイト指定だが2バイトセット0～65535の範囲
	memcpy(&pbuf->d_no0, p8, size = sizeof(pbuf->d_no0));		len += size; p8 += size;//3バイトの残り１バイトは無視
	memcpy(&pbuf->d_code, p8, size = sizeof(pbuf->d_code));		len += size; p8 += size;
	memcpy(&pbuf->n_device, p8, size = sizeof(pbuf->n_device)); len += size; p8 += size;

	if ((pbuf->n_device > 0) && (pbuf->n_device <= 300)) {
		for (int i = 0; i < pbuf->n_device; i++) pbuf->req_data[i] = *((INT16*)p8 + i);
	}
	else return S_FALSE;

	return S_OK;
}
//*********************************************************************************************　
/// <summary>
/// シミュレータ用：PLCへのコマンドへ応答を返す関数
/// </summary>
/// <param name="pbuf">PLC受信（PC送信）バッファ</param>
/// <param name="pdata"> 
/// Read時：PLC内　PC用読み出し用デバイス模擬メモリへのポインタ
/// Write時：PLC内　PC書き込みデバイス模擬メモリへのポインタ
/// </param>
/// <returns></returns>
UINT16 CMCProtocol::snd_responce(ST_XE_REQ st_com,UINT16* pdata ) {

	UINT8 buf_PLCsnd[MAX_MC_DATA*2];
	UINT8* p8 = buf_PLCsnd;
	int len = 0; 
	*p8 = 0xD0; p8++; len++;//応答サブコード
	*p8 = 0x00; p8++; len++;//応答サブコード
	*p8 = 0x00; p8++; len++;//ネットワーク番号
	*p8 = 0xFF; p8++; len++;//PC番号
	*p8 = 0xFF; p8++; len++;//ユニットIO番号
	*p8 = 0x03; p8++; len++;//ユニットIO番号
	*p8 = 0x00; p8++; len++;//ユニット局番号
	
	if (st_com.com == CODE_COMMAND_READ) {
		*((UINT16*)p8) = 2 + st_com.n_device*2; //終了コード長＋読み出しデバイス長
		 p8+=2; len+=2;//応答データ長
		*p8 = 0x00; p8++; len++;//終了コード正常
		*p8 = 0x00; p8++; len++;//終了コード正常

		for (int i = 0; i < st_com.n_device; i++)
			((UINT16*)p8)[i] = pdata[i];

		len += st_com.n_device * 2;

		//	送信バッファをクライアントアドレスに送信 
		if (pMCSock->snd_udp_msg((char*)buf_PLCsnd, len, st_work_wnd.addrinc) == SOCKET_ERROR) {
			msg_wos.str() = pMCSock->err_msg.str();
			return S_FALSE;
		}
		return st_com.com;
	}
	else if (st_com.com == CODE_COMMAND_WRITE) {
		*p8 = 0x02; p8++; len++;//応答データ長
		*p8 = 0x00; p8++; len++;//応答データ長
		*p8 = 0x00; p8++; len++;//終了コード正常
		*p8 = 0x00; p8++; len++;//終了コード正常

		for (int i = 0; i < st_com.n_device; i++)
			*(pdata + i) = st_com.req_data[i];

		//	送信バッファをクライアントアドレスに送信 
		if (pMCSock->snd_udp_msg((char*)buf_PLCsnd, len, st_work_wnd.addrinc) == SOCKET_ERROR) {
			msg_wos.str() = pMCSock->err_msg.str();
			return S_FALSE;
		}

		return st_com.com;
	}
	else {
		return SOCKET_ERROR;
	}

	return st_com.com;
}
//*********************************************************************************************　
/// <summary>
/// 
/// </summary>
/// <param name="hwnd"></param>
/// <returns></returns>
HWND CMCProtocol::open_work_Wnd(HWND hwnd) {

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
	wcex.lpszMenuName = TEXT("PLC IF");
	wcex.lpszClassName = TEXT("MCProtocol");
	wcex.hIconSm = NULL;

	ATOM fb=RegisterClassExW(&wcex);

	st_work_wnd.hWnd = CreateWindowW(TEXT("MCProtocol"), TEXT("PLC IF(MC)"), WS_OVERLAPPEDWINDOW,
		WORK_WND_INIT_POS_X, WORK_WND_INIT_POS_Y,
		WORK_WND_INIT_SIZE_W, WORK_WND_INIT_SIZE_H,
		nullptr, nullptr, hInst, nullptr);

	RECT rc;
	GetClientRect(st_work_wnd.hWnd, &rc);
	st_work_wnd.area_w = rc.right - rc.left;
	st_work_wnd.area_h = rc.bottom - rc.top;

	//表示フォント設定
	st_work_wnd.hfont_inftext = CreateFont(12, 0, 0, 0, 0, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));
	//デバイスコンテキスト
	HDC hdc = GetDC(st_work_wnd.hWnd);
	st_work_wnd.hBmap_mem0 = CreateCompatibleBitmap(hdc, st_work_wnd.area_w, st_work_wnd.area_h);
	st_work_wnd.hdc_mem0 = CreateCompatibleDC(hdc);
	SelectObject(st_work_wnd.hdc_mem0, st_work_wnd.hBmap_mem0);
	PatBlt(st_work_wnd.hdc_mem0, 0, 0, st_work_wnd.area_w, st_work_wnd.area_h, WHITENESS);

	st_work_wnd.hBmap_inf = CreateCompatibleBitmap(hdc, st_work_wnd.area_w, st_work_wnd.area_h);
	st_work_wnd.hdc_mem_inf = CreateCompatibleDC(hdc);
	SelectObject(st_work_wnd.hdc_mem_inf, st_work_wnd.hBmap_inf);
	PatBlt(st_work_wnd.hdc_mem_inf, 0, 0, st_work_wnd.area_w, st_work_wnd.area_h, WHITENESS);
	TextOutW(st_work_wnd.hdc_mem_inf, 0, 0, L"<<Information>>", 15);

	ReleaseDC(st_work_wnd.hWnd, hdc);

	InvalidateRect(st_work_wnd.hWnd, NULL, TRUE);//表示更新

	ShowWindow(st_work_wnd.hWnd, SW_SHOW);
	UpdateWindow(st_work_wnd.hWnd);

	return st_work_wnd.hWnd;
};

/// <summary>
/// wstringの文字列をメインウィンドウに出力
/// </summary>
/// <param name="srcw"></param>
void CMCProtocol::wstr_out_inf(const std::wstring& srcw) {
	TextOutW(st_work_wnd.hdc_mem_inf, st_work_wnd.x_org, st_work_wnd.w_ptr_inf, srcw.c_str(), (int)srcw.length());

	//次書き込み位置更新
	st_work_wnd.w_ptr_inf += st_work_wnd.h_row;
	if (st_work_wnd.w_ptr_inf > st_work_wnd.h_row * st_work_wnd.n_max_row_inf)st_work_wnd.w_ptr_inf = MC_PRM_INFTXT_ORG_Y;
	//次の行クリア
	PatBlt(st_work_wnd.hdc_mem_inf, st_work_wnd.x_org, st_work_wnd.w_ptr_inf, st_work_wnd.area_w, st_work_wnd.h_row, WHITENESS);
	InvalidateRect(st_work_wnd.hWnd, NULL, TRUE);
	return;
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
static bool				is_next_write_req = true;
static bool				is_slowmode = false, disp_sock_info = false, disp_msg = false, disp_infomation = false,is_disp_hex=false;
static wostringstream	wos;
static ostringstream	os;
static wstring			wstr;
static string			str;

LRESULT CALLBACK CMCProtocol::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE hInst = st_work_wnd.hInst;

	switch (message)
	{
	case WM_CREATE: {
		InitCommonControls();//コモンコントロール初期化

		//タイマ起動
		SetTimer(hWnd, ID_UPDATE_TIMER, MC_TIMER_PERIOD, NULL);

		//ウィンドウにコントロール追加

		st_work_wnd.h_static_snd_cnt_w = CreateWindowW(TEXT("STATIC"), L"SND CNT W: -", WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 35, 150, 20, hWnd, (HMENU)IDC_MC_STATIC_SND0, hInst, NULL);
		st_work_wnd.h_static_snd_cnt_r = CreateWindowW(TEXT("STATIC"), L" R: -", WS_CHILD | WS_VISIBLE | SS_LEFT,
			160, 35, 80, 20, hWnd, (HMENU)IDC_MC_STATIC_SND0, hInst, NULL);
		st_work_wnd.h_static_rcv_cnt_w = CreateWindowW(TEXT("STATIC"), L"RCV CNT W: -", WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 55, 150, 20, hWnd, (HMENU)IDC_MC_STATIC_SND0, hInst, NULL);
		st_work_wnd.h_static_rcv_cnt_r = CreateWindowW(TEXT("STATIC"), L" R: -", WS_CHILD | WS_VISIBLE | SS_LEFT,
			160, 55, 80, 20, hWnd, (HMENU)IDC_MC_STATIC_SND0, hInst, NULL);
		st_work_wnd.h_static_rcv_cnt_err = CreateWindowW(TEXT("STATIC"), L" ERR: -", WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 110, 80, 20, hWnd, (HMENU)IDC_MC_STATIC_SND0, hInst, NULL);
		st_work_wnd.h_static_res_w = CreateWindowW(TEXT("STATIC"), L"RES TIME W: -", WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 80, 150, 20, hWnd, (HMENU)IDC_MC_STATIC_SND0, hInst, NULL);
		st_work_wnd.h_static_res_r = CreateWindowW(TEXT("STATIC"), L" R: -", WS_CHILD | WS_VISIBLE | SS_LEFT,
			160, 80, 80, 20, hWnd, (HMENU)IDC_MC_STATIC_SND0, hInst, NULL);

		st_work_wnd.h_socket_inf = CreateWindowW(TEXT("STATIC"), L"SOCK INF", WS_CHILD | WS_VISIBLE | SS_LEFT,
			250, 35, 280, 100, hWnd, (HMENU)IDC_MC_STATIC_SOCK_INF, hInst, NULL);

		st_work_wnd.h_static_snd_msg_w = CreateWindowW(TEXT("STATIC"), L"SNDw >> -", WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 140, 520, 40, hWnd, (HMENU)IDC_MC_STATIC_SND0, hInst, NULL);
		st_work_wnd.h_static_rcv_msg_w = CreateWindowW(TEXT("STATIC"), L"RCVw >> -", WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 185, 520, 40, hWnd, (HMENU)IDC_MC_STATIC_SND, hInst, NULL);
		st_work_wnd.h_static_snd_msg_r = CreateWindowW(TEXT("STATIC"), L"SNDr >> -", WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 235, 520, 40, hWnd, (HMENU)IDC_MC_STATIC_RCV0, hInst, NULL);
		st_work_wnd.h_static_rcv_msg_r = CreateWindowW(TEXT("STATIC"), L"RCVr >> -", WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 280, 520, 40, hWnd, (HMENU)IDC_MC_STATIC_SND, hInst, NULL);

		st_work_wnd.h_chkSlow = CreateWindow(L"BUTTON", L"Slow", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			5, 5, 60, 25, hWnd, (HMENU)IDC_CHK_IS_SLOW_MODE, hInst, NULL);
		SendMessage(st_work_wnd.h_chkSlow, BM_SETCHECK, BST_UNCHECKED, 0L);

		st_work_wnd.h_chk_inf = CreateWindow(L"BUTTON", L"Info", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			10, 325, 60, 25, hWnd, (HMENU)IDC_CHK_INF, hInst, NULL);
		SendMessage(st_work_wnd.h_chk_inf, BM_SETCHECK, BST_UNCHECKED, 0L);

		st_work_wnd.h_chkSockinf = CreateWindow(L"BUTTON", L"Sock", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			90, 5, 60, 25, hWnd, (HMENU)IDC_CHK_DISP_SOCK, hInst, NULL);
		SendMessage(st_work_wnd.h_chkSockinf, BM_SETCHECK, BST_UNCHECKED, 0L);

		st_work_wnd.h_chk_msg = CreateWindow(L"BUTTON", L"Msg", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			170, 5, 60, 25, hWnd, (HMENU)IDC_CHK_MSG, hInst, NULL);
		SendMessage(st_work_wnd.h_chk_msg, BM_SETCHECK, BST_UNCHECKED, 0L);

		st_work_wnd.h_chk_hex = CreateWindow(L"BUTTON", L"HEX", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			75, 325, 60, 25, hWnd, (HMENU)IDC_MC_INF_CHK_HEX, hInst, NULL);
		SendMessage(st_work_wnd.h_chk_hex, BM_SETCHECK, BST_UNCHECKED, 0L);
				
		st_work_wnd.h_static_inf = CreateWindowW(TEXT("STATIC"), L"INFO:-", WS_CHILD | WS_VISIBLE | SS_LEFT,
			5, 350, 520, 60, hWnd, (HMENU)IDC_MC_STATIC_INF, hInst, NULL);
		st_work_wnd.h_static_Dr = CreateWindowW(TEXT("STATIC"), L"DR:-", WS_CHILD | WS_VISIBLE | SS_LEFT,
			40, 420, 520, 60, hWnd, (HMENU)IDC_MC_STATIC_DW, hInst, NULL);
		st_work_wnd.h_static_Dw = CreateWindowW(TEXT("STATIC"), L"DW:-", WS_CHILD | WS_VISIBLE | SS_LEFT,
			40, 490, 520, 60, hWnd, (HMENU)IDC_MC_STATIC_DW, hInst, NULL);
		
		st_work_wnd.h_pb_dr_minus = CreateWindow(L"BUTTON", L"R-", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			5, 420, 30, 25, hWnd, (HMENU)IDC_PB_DR_MINUS, hInst, NULL);
		st_work_wnd.h_pb_dr_plus = CreateWindow(L"BUTTON", L"R+", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			5, 450, 30, 25, hWnd, (HMENU)IDC_PB_DR_PLUS, hInst, NULL);

		st_work_wnd.h_pb_dw_minus = CreateWindow(L"BUTTON", L"W-", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			5, 490, 30, 25, hWnd, (HMENU)IDC_PB_DW_MINUS, hInst, NULL);
		st_work_wnd.h_pb_dw_plus = CreateWindow(L"BUTTON", L"W+", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			5, 520, 30, 25, hWnd, (HMENU)IDC_PB_DW_PLUS, hInst, NULL);
	}
	case WM_TIMER: {
		
		if (is_slowmode) {
			break;
		}

		if (is_next_write_req) {//書き込み要求送信
			//3Eフォーマット Dデバイス書き込み要求送信
			if (send_write_req_D_3E(mc_req_msg_w.req_data) != S_OK) {
				wstr = L"ERROR : send_read_req_D_3E()";
				wstr_out_inf(wstr);
			}
			else {
				wstr = L"SUCCESS : send_read_req_D_3E()";
				st_work_wnd.count_snd_w++;
			}
			if (disp_msg) {
				wos.str(L"");
				wos << L"SNDw>>" << L"#sub:" << std::hex << mc_req_msg_w.subcode << L" #serial:" << mc_req_msg_w.serial << L" #NW:" << mc_req_msg_w.nNW << L" #PC:" << mc_req_msg_w.nPC << L" #UIO:" << mc_req_msg_w.nUIO << L" #Ucd:" << mc_req_msg_w.nUcode
					<< L"#len:" << mc_req_msg_w.len << L" #tm:" << mc_req_msg_w.timer << L"\n    " << L" #com:" << mc_req_msg_w.com << L" #scom:" << mc_req_msg_w.scom << L" #d_no:" << mc_req_msg_w.d_no << L"#d_code:" << mc_req_msg_w.d_code << L" #n_dev:" << mc_req_msg_w.n_device << L" #data0:" << mc_req_msg_w.req_data[0];
				SetWindowText(st_work_wnd.h_static_snd_msg_w, wos.str().c_str());
			}

			QueryPerformanceCounter(&start_count_w);  // 現在のカウント数

			is_next_write_req = false;
		}
		else {
			//読み出し要求送信
			if (send_read_req_D_3E() != S_OK) {
				wstr = L"ERROR : send_read_req_D_3E()";
				wstr_out_inf(wstr);
			}
			else {
				wstr = L"SUCCESS : send_read_req_D_3E()";
				st_work_wnd.count_snd_r++;
			}
			if (disp_msg) {
				wos.str(L"");
				wos << L"SNDr>>" << L"#sub:" << std::hex << mc_req_msg_r.subcode << L" #serial:" << mc_req_msg_r.serial << L" #NW:" << mc_req_msg_r.nNW << L" #PC:" << mc_req_msg_r.nPC << L" #UIO:" << mc_req_msg_r.nUIO << L" #Ucd:" << mc_req_msg_r.nUcode
					<< L"#len:" << mc_req_msg_r.len << L" #tm:" << mc_req_msg_r.timer << L"\n    " << L" #com:" << mc_req_msg_r.com << L" #scom:" << mc_req_msg_r.scom << L" #d_no:" << mc_req_msg_r.d_no << L"#d_code:" << mc_req_msg_r.d_code << L" #n_dev:" << mc_req_msg_r.n_device << L" #data0:" << L"-";
				SetWindowText(st_work_wnd.h_static_snd_msg_r, wos.str().c_str());

				QueryPerformanceCounter(&start_count_r);  // 現在のカウント数
			}

			is_next_write_req = true;
		}
		//カウンタ表示
		wos.str(L""); wos << L"SND CNT W: " << std::dec << st_work_wnd.count_snd_w;
		SetWindowText(st_work_wnd.h_static_snd_cnt_w, wos.str().c_str());
		wos.str(L""); wos << L" R: " << std::dec << st_work_wnd.count_snd_r;
		SetWindowText(st_work_wnd.h_static_snd_cnt_r, wos.str().c_str());

		if (disp_infomation) {
	

			wos.str(L""); wos << L"DW" << st_work_wnd.i_dw_disp << L"\n";

			for (int i = 0; i < 20; i++) {
				if (is_disp_hex) wos << hex << setw(4) << setfill(L'0');
				wos << mc_req_msg_w.req_data[st_work_wnd.i_dw_disp + i] << L" ";
				if (i == 9)wos << L"\n";
			}
			SetWindowText(st_work_wnd.h_static_Dw, wos.str().c_str());

			wos.str(L""); wos << L"DR" << dec << st_work_wnd.i_dr_disp << L"\n";

			for (int i = 0; i < 20; i++) {
				if (is_disp_hex) wos << hex << setw(4) << setfill(L'0');
				wos << mc_res_msg_r.res_data[st_work_wnd.i_dr_disp + i] << L" ";
				if (i == 9)wos << L"\n";
			}
			SetWindowText(st_work_wnd.h_static_Dr, wos.str().c_str());

		}




	}break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case IDC_CHK_DISP_SOCK: {
			if (IsDlgButtonChecked(hWnd, IDC_CHK_DISP_SOCK) == BST_CHECKED) disp_sock_info = true;
			else  disp_sock_info = false;
		}break;
		case IDC_CHK_IS_SLOW_MODE: {
			if (IsDlgButtonChecked(hWnd, IDC_CHK_IS_SLOW_MODE) == BST_CHECKED) is_slowmode = true;
			else  is_slowmode = false;
		}break;
		case  IDC_CHK_INF: {
			if (IsDlgButtonChecked(hWnd, IDC_CHK_INF) == BST_CHECKED) disp_infomation=true;
			else  disp_infomation = false;
		}break;
		case  IDC_CHK_MSG: {
			if (IsDlgButtonChecked(hWnd, IDC_CHK_MSG) == BST_CHECKED) disp_msg = true;
			else  disp_msg = false;
		}break;
		case  IDC_MC_INF_CHK_HEX: {
			if (IsDlgButtonChecked(hWnd, IDC_MC_INF_CHK_HEX) == BST_CHECKED) is_disp_hex = true;
			else  is_disp_hex = false;
		}break;
			

		case IDC_PB_DW_PLUS: {
			st_work_wnd.i_dw_disp+=20;
			if (st_work_wnd.i_dw_disp > 80)st_work_wnd.i_dw_disp = 0;
		}break;
		case IDC_PB_DW_MINUS: {
			st_work_wnd.i_dw_disp-=20;
			if (st_work_wnd.i_dw_disp < 0)st_work_wnd.i_dw_disp = 80;
		}break;
		case IDC_PB_DR_PLUS: {
			st_work_wnd.i_dr_disp+=20;
			if (st_work_wnd.i_dr_disp > 80)st_work_wnd.i_dr_disp = 0;
		}break;
		case IDC_PB_DR_MINUS: {
			st_work_wnd.i_dr_disp -= 20;
			if (st_work_wnd.i_dr_disp < 0)st_work_wnd.i_dr_disp = 80;
		}break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}break;
	case ID_SOCK_MC_SERVER:
	{
		int nEvent = WSAGETSELECTEVENT(lParam);
		switch (nEvent) {
		case FD_READ: {
			UINT nRtn = rcv_msg_3E();

			if (nRtn == MC_RES_READ) {
				wstr = L"PLC READ SUCCESS";
			}
			else if (nRtn == MC_RES_WRITE) {
				wstr = L"PLC WRITE SUCCESS";
			}
			else {
				wstr = L"server rcv error";
				int err_code = WSAGetLastError();
			}

			wstr_out_inf(wstr);

		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;
	case ID_SOCK_MC_CLIENT:
	{
		int nEvent = WSAGETSELECTEVENT(lParam);
		switch (nEvent) {
		case FD_READ: {
			UINT nRtn = rcv_msg_3E();

			if (nRtn == MC_RES_READ) {
				wstr = L"PLC READ SUCCESS";
				st_work_wnd.count_rcv_r++;
				wos.str(L""); wos << L" R: " << std::dec << st_work_wnd.count_rcv_r;
				SetWindowText(st_work_wnd.h_static_rcv_cnt_r, wos.str().c_str());

				if (disp_msg) {
					wos.str(L"");
					wos << L"RCVr>>" << L"#sub:" << std::hex << mc_res_msg_r.subcode << L" #serial:" << mc_res_msg_r.serial << L" #NW:" << mc_res_msg_r.nNW << L" #PC:" << mc_res_msg_r.nPC << L" #UIO:" << mc_res_msg_r.nUIO << L" #Ucd:" << mc_res_msg_r.nUcode
						<< L"#len:" << mc_res_msg_r.len<< L"\n    " << L" #end:" << mc_res_msg_r.endcode  << L" #data0:" << mc_res_msg_r.res_data[0] << L" #1:" << mc_res_msg_r.res_data[1] << L" #2:" << mc_res_msg_r.res_data[2] << L" #3:" << mc_res_msg_r.res_data[3];
					SetWindowText(st_work_wnd.h_static_rcv_msg_r, wos.str().c_str());

					QueryPerformanceFrequency(&frequency);  // システムの周波数
					QueryPerformanceCounter(&end_count_r);    // 現在のカウント数
					LONGLONG lspan = (end_count_r.QuadPart - start_count_r.QuadPart) * 1000000L / frequency.QuadPart;// 時間の間隔[usec]
					if (spancount_max_r < lspan) spancount_max_r = lspan;
					if (st_work_wnd.count_snd_r % 40 == 0) {
						wos.str(L""); wos << L" R: " << std::dec << spancount_max_r;
						SetWindowText(st_work_wnd.h_static_res_r, wos.str().c_str());
						spancount_max_r = 0;
					}
				}
			}
			else if (nRtn == MC_RES_WRITE) {
				wstr = L"PLC WRITE SUCCESS";
				st_work_wnd.count_rcv_w++;
				wos.str(L""); wos << L"RCV CNT W: " << std::dec << st_work_wnd.count_rcv_w;
				SetWindowText(st_work_wnd.h_static_rcv_cnt_w, wos.str().c_str());
				if (disp_msg) {
					wos.str(L"");
					wos << L"RCVr>>" << L"#sub:" << std::hex << mc_res_msg_w.subcode << L" #serial:" << mc_res_msg_w.serial << L" #NW:" << mc_res_msg_w.nNW << L" #PC:" << mc_res_msg_w.nPC << L" #UIO:" << mc_res_msg_w.nUIO << L" #Ucd:" << mc_res_msg_w.nUcode
						<< L"#len:" << mc_res_msg_w.len << L"\n    " << L" #end:" << mc_res_msg_w.endcode << L" #data0:" << mc_res_msg_w.res_data[0];
					SetWindowText(st_work_wnd.h_static_rcv_msg_w, wos.str().c_str());

					QueryPerformanceFrequency(&frequency);  // システムの周波数
					QueryPerformanceCounter(&end_count_w);    // 現在のカウント数
					LONGLONG lspan = (end_count_w.QuadPart - start_count_w.QuadPart) * 1000000L / frequency.QuadPart;// 時間の間隔[usec]
					if (spancount_max_w < lspan) spancount_max_w = lspan;
					if (st_work_wnd.count_snd_w % 40 == 0) {
						wos.str(L""); wos << L"RES W μs:" << std::dec << spancount_max_w;
						SetWindowText(st_work_wnd.h_static_res_w, wos.str().c_str());
						spancount_max_w = 0;
					}
				}
			}
			else {
				wstr = L"PLC ERR RESPONCE";
				int err_code = WSAGetLastError();
				st_work_wnd.count_rcv_err++;
				wos.str(L""); wos << L"ERR: " << std::dec << st_work_wnd.count_rcv_err;
				SetWindowText(st_work_wnd.h_static_rcv_cnt_err, wos.str().c_str());

				if (disp_msg) {
					wos.str(L"");
					wos << L"RCVr>>" << L"#sub:" << std::hex << mc_res_msg_r.subcode << L" #serial:" << mc_res_msg_r.serial << L" #NW:" << mc_res_msg_r.nNW << L" #PC:" << mc_res_msg_r.nPC << L" #UIO:" << mc_res_msg_r.nUIO << L" #Ucd:" << mc_res_msg_r.nUcode
						<< L"#len:" << mc_res_msg_r.len << L"\n    " << L" #end:" << mc_res_msg_r.endcode << L" #data0:" << mc_res_msg_r.res_data[0] << L" #1:" << mc_res_msg_r.res_data[1] << L" #2:" << mc_res_msg_r.res_data[2] << L" #3:" << mc_res_msg_r.res_data[3];
					SetWindowText(st_work_wnd.h_static_rcv_msg_r, wos.str().c_str());
				}
			}

			if (disp_sock_info) {
				wos.str(L"");
				wos << L"SOCK INF\n  From IP:  " << std::dec << st_work_wnd.addrfrom.sin_addr.S_un.S_un_b.s_b1 << L"." << st_work_wnd.addrfrom.sin_addr.S_un.S_un_b.s_b2 << L"." << st_work_wnd.addrfrom.sin_addr.S_un.S_un_b.s_b3 << L"." << st_work_wnd.addrfrom.sin_addr.S_un.S_un_b.s_b4 << L"  PORT: " << ntohs(st_work_wnd.addrfrom.sin_port) << L"\n";
				wos << L"  MCC IP:  " << std::dec << st_work_wnd.addrinc.sin_addr.S_un.S_un_b.s_b1 << L"." << st_work_wnd.addrinc.sin_addr.S_un.S_un_b.s_b2 << L"." << st_work_wnd.addrinc.sin_addr.S_un.S_un_b.s_b3 << L"." << st_work_wnd.addrinc.sin_addr.S_un.S_un_b.s_b4 << L"  PORT: " << ntohs(st_work_wnd.addrinc.sin_port) << L"\n";
				wos << L"  PLC IP :  " << std::dec << st_work_wnd.addrins.sin_addr.S_un.S_un_b.s_b1 << L"." << st_work_wnd.addrins.sin_addr.S_un.S_un_b.s_b2 << L"." << st_work_wnd.addrins.sin_addr.S_un.S_un_b.s_b3 << L"." << st_work_wnd.addrins.sin_addr.S_un.S_un_b.s_b4 << L"  PORT: " << ntohs(st_work_wnd.addrins.sin_port);
				SetWindowText(st_work_wnd.h_socket_inf, wos.str().c_str());
			}
			wstr_out_inf(wstr);

		}break;
		case FD_WRITE: break;
		case FD_CLOSE: break;
		}
	}break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		BOOL brtn = TransparentBlt(st_work_wnd.hdc_mem0, 0, 0, WORK_WND_INIT_SIZE_W, WORK_WND_INIT_SIZE_H,//DST
			st_work_wnd.hdc_mem_inf, 0, 0, WORK_WND_INIT_SIZE_W, WORK_WND_INIT_SIZE_H,//SORCE
			RGB(255, 255, 255));

		BitBlt(hdc, 20, MC_PRM_MEM0_POS_Y, WORK_WND_INIT_SIZE_W, MC_PRM_MEM0_POS_H, st_work_wnd.hdc_mem0, 0, 0, SRCCOPY);
	//	BitBlt(hdc, 50, MC_PRM_INFMEM_POS_Y, WORK_WND_INIT_SIZE_W, MC_PRM_INFMEM_POS_H, st_work_wnd.hdc_mem_inf, 0, 0, SRCCOPY);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY: {
		delete pMCSock;
		PostQuitMessage(0);
	}break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

