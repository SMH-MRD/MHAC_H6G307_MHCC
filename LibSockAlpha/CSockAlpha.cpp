#include "pch.h"
#include "CSockAlpha.h"

#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <TCHAR.H>

//****************************************************************************
/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="_protocol">　プロトコル TCP/UDP　</param>
/// <param name="_access">	　サーバー/クライアント種別</param>
CSockAlpha::CSockAlpha(INT32 _protocol, INT32 _access, INT32 _eventID)
{
	addr_in_rcv.sin_port = htons(DEFAULT_MY_PORT);
	addr_in_rcv.sin_family = AF_INET;
	inet_pton(AF_INET, DEFAULT_MY_IP, &addr_in_rcv.sin_addr.s_addr);
	addr_in_dst = addr_in_from = addr_in_rcv;
	for (int i = 0; i < MAX_N_MULTICAST; i++) addr_in_multi[i] = addr_in_rcv;

	eventID = _eventID;
	protocol = _protocol;
	access	= _access;		//準備するソケットの接続タイプ

	InitializeCriticalSection(&csSck); 
	s = -1;
}

/// <summary>
/// コンストラクタ(指定無いときUDPクライアント　イベントID：10600
/// </summary>
CSockAlpha::CSockAlpha()
{
	addr_in_rcv.sin_port = htons(DEFAULT_MY_PORT);
	addr_in_rcv.sin_family = AF_INET;
	inet_pton(AF_INET, DEFAULT_MY_IP, &addr_in_rcv.sin_addr.s_addr);
	addr_in_dst = addr_in_from  = addr_in_rcv;
	for(int i=0;i<MAX_N_MULTICAST;i++) addr_in_multi[i] = addr_in_rcv;

	eventID		= ID_MC_DEFAULT_EVENT;
	protocol	= UDP_PROTOCOL;
	access		= ACCESS_TYPE_CLIENT;		//準備するソケットの接続タイプ
	n_multicast = 0;

	InitializeCriticalSection(&csSck);
	s = -1;
}

//****************************************************************************
/// <summary>
/// デストラクタ
/// </summary>
/// <param name=""></param>
CSockAlpha::~CSockAlpha(void)
{
	Close();
}

/******************************************************************************/
/// <summary>
/// WinSockの初期化　WSAStartup(MAKEWORD(1, 1))
/// </summary>
/// <returns>HRESULT</returns>
HRESULT CSockAlpha::Initialize()
{
	HRESULT	hr = S_OK;
	WSADATA	WsaData;

	if (WSAStartup(MAKEWORD(1, 1), &WsaData) == SOCKET_ERROR) {
		SetSockErr(WSAGetLastError());
		hr = E_FAIL;
	}	// ver1.01

	DeleteCriticalSection(&csSck);
	InitializeCriticalSection(&csSck);

	return hr;
}

/******************************************************************************/
/// <summary>
/// クラスメンバにソケットのエラーコードに応じた表示文字列をセットする
/// </summary>
/// <param name="nError">GetLastErrorで取得したエラーコード</param>
/// <returns>HRESULT</returns>
HRESULT CSockAlpha::GetSockMsg(int nError)
{
	wchar_t	pszError[128];
	swprintf_s(pszError, sizeof(pszError)/2, L"Err>> %d:", nError);					//エラーコード書き込み

	switch (nError)
	{
	//lstrcat ある文字列の末尾に別の文字列を結合する。
	case WSAEINTR:				lstrcat(pszError, L"Interrupted system call");							break;
	case WSAEBADF:				lstrcat(pszError, L"Bad file number");									break;
	case WSAEACCES:				lstrcat(pszError, L"Permission denied");								break;
	case WSAEFAULT:				lstrcat(pszError, L"Bad address");										break;
	case WSAEINVAL:				lstrcat(pszError, L"Invalid argument");									break;
	case WSAEMFILE:				lstrcat(pszError, L"Too many open files");								break;
	case WSAEWOULDBLOCK:		lstrcat(pszError, L"Operation would block");							break;
	case WSAEINPROGRESS:		lstrcat(pszError, L"Operation now in progress");						break;
	case WSAEALREADY:			lstrcat(pszError, L"Operation already in progress");					break;
	case WSAENOTSOCK:			lstrcat(pszError, L"Socket operation on non-socket");					break;
	case WSAEDESTADDRREQ:		lstrcat(pszError, L"Destination address required");						break;
	case WSAEMSGSIZE:			lstrcat(pszError, L"Message too long");									break;
	case WSAEPROTOTYPE:			lstrcat(pszError, L"Protocol wrong type for socket");					break;
	case WSAENOPROTOOPT:		lstrcat(pszError, L"Protocol not available");							break;
	case WSAEPROTONOSUPPORT:	lstrcat(pszError, L"Protocol not supported");							break;
	case WSAESOCKTNOSUPPORT:	lstrcat(pszError, L"Socket type not supported");						break;
	case WSAEOPNOTSUPP:			lstrcat(pszError, L"Operation not supported on socket");				break;
	case WSAEPFNOSUPPORT:		lstrcat(pszError, L"Protocol family not supported");					break;
	case WSAEAFNOSUPPORT:		lstrcat(pszError, L"Address family not supported by protocol family");	break;
	case WSAEADDRINUSE:			lstrcat(pszError, L"Address already in use");							break;
	case WSAEADDRNOTAVAIL:		lstrcat(pszError, L"Can't assign requested address");					break;
	case WSAENETDOWN:			lstrcat(pszError, L"Network is down");									break;
	case WSAENETUNREACH:		lstrcat(pszError, L"Network is unreachable");							break;
	case WSAENETRESET:			lstrcat(pszError, L"Network dropped connection on reset");				break;
	case WSAECONNABORTED:		lstrcat(pszError, L"Software caused connection abort");					break;
	case WSAECONNRESET:			lstrcat(pszError, L"Connection reset by peer");							break;
	case WSAENOBUFS:			lstrcat(pszError, L"No buffer space available");						break;
	case WSAEISCONN:			lstrcat(pszError, L"Socket is already connected");						break;
	case WSAENOTCONN:			lstrcat(pszError, L"Socket is not connected");							break;
	case WSAESHUTDOWN:			lstrcat(pszError, L"Can't send after socket shutdown");					break;
	case WSAETOOMANYREFS:		lstrcat(pszError, L"Too many references: can't splice");				break;
	case WSAETIMEDOUT:			lstrcat(pszError, L"Connection timed out");								break;
	case WSAECONNREFUSED:		lstrcat(pszError, L"Connection refused");								break;
	case WSAELOOP:				lstrcat(pszError, L"Too many levels of symbolic links");				break;
	case WSAENAMETOOLONG:		lstrcat(pszError, L"File name too long");								break;
	case WSAEHOSTDOWN:			lstrcat(pszError, L"Host is down");										break;
	case WSAEHOSTUNREACH:		lstrcat(pszError, L"No route to host");									break;
	case WSAENOTEMPTY:			lstrcat(pszError, L"Directory not empty");								break;
	case WSAEPROCLIM:			lstrcat(pszError, L"Too many processes");								break;
	case WSAEUSERS:				lstrcat(pszError, L"Too many users");									break;
	case WSAEDQUOT:				lstrcat(pszError, L"Disc quota exceeded");								break;
	case WSAESTALE:				lstrcat(pszError, L"Stale NFS file handle");							break;
	case WSAEREMOTE:			lstrcat(pszError, L"Too many levels of remote in path");				break;
#ifdef _WIN32
	case WSAEDISCON:			lstrcat(pszError, L"Disconnect");										break;
#endif
	case WSASYSNOTREADY:		lstrcat(pszError, L"Network sub-system is unusable");					break;
	case WSAVERNOTSUPPORTED:	lstrcat(pszError, L"WinSock DLL cannot support this application");		break;
	case WSANOTINITIALISED:		lstrcat(pszError, L"WinSock not initialized");							break;
	case WSAHOST_NOT_FOUND:		lstrcat(pszError, L"Host not found");									break;
	case WSATRY_AGAIN:			lstrcat(pszError, L"Non-authoritative host not found");					break;
	case WSANO_RECOVERY:		lstrcat(pszError, L"Non-recoverable error");							break;
	case WSANO_DATA:			lstrcat(pszError, L"Valid name, no data record of requested type");		break;
//	default:					lstrcpy(pszError, L"Not a WinSock error");								break;
	default:					lstrcat(pszError, L"Not a WinSock error");								break;
	}

	wstr_sock_err = pszError;

	return S_OK;
}

/******************************************************************************/
/// <summary>
/// 
/// </summary>
/// <param name="nError">GetLastErrorで取得したエラーコード</param>
/// <returns>HRESULT</returns>
HRESULT CSockAlpha::SetSockErr(int nError) {

	SYSTEMTIME st; TCHAR tbuf[32];
	::GetLocalTime(&st);
	wsprintf(tbuf, L"%02d:%02d:%02d.%01d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds / 100);

	GetSockMsg(nError);
	err_msg.str(L"");
	err_msg << tbuf << L"::" << wstr_sock_err << L"@SockLib";

	//	err_msg << wstr_sock_err << L" <<" << _T(__FILE__) << L"@" << __LINE__ << L" >>";

	return S_OK;
}

/******************************************************************************/
/// <summary>
/// ソケット（受信）の初期化
/// </summary>
/// <param name="hwnd">非同期受信イベントを受け取るウィンドウハンドル</param>
/// <param name="addr_in">ソケット（受信）のアドレス/ポート</param>
/// <returns></returns>
HRESULT CSockAlpha::init_sock_udp(HWND hwnd, SOCKADDR_IN addr_in) {

	//Socketオープン
	s = socket(AF_INET, protocol, 0);
	if (s < 0) {
		SetSockErr(WSAGetLastError());
		return E_FAIL;
	}

	//ソケットに名前を付ける bind
	addr_in_rcv = addr_in;
	int nRtn = bind(s, (LPSOCKADDR)&addr_in_rcv, (int)sizeof(addr_in_rcv)); 
	if (nRtn == SOCKET_ERROR) {
		SetSockErr(WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return E_FAIL;
	}

	//非同期化
	nRtn = WSAAsyncSelect(s, hwnd, eventID, FD_READ | FD_WRITE | FD_CLOSE);
	if (nRtn == SOCKET_ERROR) {
		SetSockErr(WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return E_FAIL;
	}

	return S_OK;
}
/******************************************************************************/
/// <summary>
/// ソケット（受信）の初期化　マルチキャスト
/// </summary>
/// <param name="hwnd">非同期受信イベントを受け取るウィンドウハンドル</param>
/// <param name="addr_in">ソケット（受信）のアドレス/ポート</param>
/// <returns></returns>
HRESULT CSockAlpha::init_sock_udp_m(HWND hwnd, SOCKADDR_IN addr_in, SOCKADDR_IN addr_in_m) {

	//Socketオープン
	s = socket(AF_INET, protocol, 0);
	if (s < 0) {
		SetSockErr(WSAGetLastError());
		return E_FAIL;
	}

	//ソケットに名前を付ける bind
	addr_in_rcv = addr_in;
	addr_in_multi[0] = addr_in_m;
	int nRtn = bind(s, (LPSOCKADDR)&addr_in_rcv, (int)sizeof(addr_in_rcv));
	if (nRtn == SOCKET_ERROR) {
		SetSockErr(WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return E_FAIL;
	}

	//非同期化
	nRtn = WSAAsyncSelect(s, hwnd, eventID, FD_READ | FD_WRITE | FD_CLOSE);
	if (nRtn == SOCKET_ERROR) {
		SetSockErr(WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return E_FAIL;
	}

	//TTL設定
	int ttl = 10;
	if (setsockopt(s,IPPROTO_IP,IP_MULTICAST_TTL,(char *)&ttl,sizeof(ttl) !=0)) {
		SetSockErr(WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return E_FAIL;
	}

	addr_in_multi[0]=addr_in_m;						//マルチキャスト送信先アドレス設定
	static struct ip_mreq mreq;                     //マルチキャスト受信設定用構造体
	//マルチキャストグループ参加登録
	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_interface.S_un.S_addr = addr_in_rcv.sin_addr.S_un.S_addr;       //利用ネットワーク
	mreq.imr_multiaddr.S_un.S_addr = addr_in_multi[0].sin_addr.S_un.S_addr; 	//マルチキャストIPアドレス
	if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) != 0) {
		perror("setopt受信設定失敗\n");
		return E_FAIL;
	}
	n_multicast++;
	return S_OK;
}

/******************************************************************************/
/// <summary>
/// ソケット（受信）の初期化　マルチキャスト
/// </summary>
/// <param name="hwnd">非同期受信イベントを受け取るウィンドウハンドル</param>
/// <param name="addr_in">ソケット（受信）のアドレス/ポート</param>
/// <returns></returns>
HRESULT CSockAlpha::add_multi_to_sock(SOCKADDR_IN addr_in_m) {
	
	if (n_multicast > MAX_N_MULTICAST) return E_FAIL;
	
	addr_in_multi[n_multicast] = addr_in_m;						//マルチキャスト送信先アドレス設定
	static struct ip_mreq mreq;                     //マルチキャスト受信設定用構造体
	//マルチキャストグループ参加登録
	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_interface.S_un.S_addr = addr_in_rcv.sin_addr.S_un.S_addr;       //利用ネットワーク
	mreq.imr_multiaddr.S_un.S_addr = addr_in_multi[n_multicast].sin_addr.S_un.S_addr; 	//マルチキャストIPアドレス
	if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) != 0) {
		perror("setopt受信設定失敗\n");
		return E_FAIL;
	}
	n_multicast++;
	return S_OK;
}
/******************************************************************************/
/// <summary>
/// メッセージ送信
/// </summary>
/// <param name="pbuf">送信データバッファポインタ</param>
/// <param name="len">送信データ長さ</param>
/// <param name="to">送信先アドレスポインタ</param>
/// <returns></returns>
int CSockAlpha::snd_udp_msg(const char* pbuf, int len, SOCKADDR_IN addr) {
	addr_in_dst = addr;
	int nRtn = sendto(s, pbuf, len, 0, (LPSOCKADDR)&addr_in_dst, sizeof(addr_in_dst));
	if (nRtn == SOCKET_ERROR) SetSockErr(WSAGetLastError());
	return nRtn;
}

/******************************************************************************/
/// <summary>
/// メッセージ受信
/// </summary>
/// <param name="pbuf">受信データバッファポインタ</param>
/// <param name="len">受信データバッファサイズ</param>
/// <returns></returns>
int CSockAlpha::rcv_udp_msg(char* pbuf, int len) {

	SOCKADDR from_addr;                             //送信元アドレス取り込みバッファ
	int from_addr_size = (int)sizeof(from_addr);    //送信元アドレスサイズバッファ

	int nRtn = recvfrom(s, pbuf, len, 0, (SOCKADDR*)&addr_in_from, &from_addr_size);
	if (nRtn == SOCKET_ERROR) SetSockErr(WSAGetLastError());
	return nRtn;
}

/******************************************************************************/
/// <summary>
/// ソケットクローズ
/// </summary>
/// <returns></returns>
HRESULT CSockAlpha::Close()
{
	closesocket(s);
	s = NULL;
	DeleteCriticalSection(&csSck);
	return (S_OK);
}

