#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <ws2tcpip.h>

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

//////////////////////////////////////////////////////////////////////////////
// CSock

#define ACCESS_TYPE_CLIENT	1
#define ACCESS_TYPE_SERVER	2

#define DEFAULT_MY_IP		"127.0.0.1"
#define DEFAULT_MY_PORT		10050

#define TCP_PROTOCOL	SOCK_STREAM
#define UDP_PROTOCOL	SOCK_DGRAM

#define ID_MC_DEFAULT_EVENT		10600

class CSockAlpha
{
public:
	CSockAlpha();
	CSockAlpha(INT32 protocol, INT32 access, INT32 eventID);
	virtual ~CSockAlpha(void);

	INT32 eventID;																		//非同期イベントID
	INT32 protocol;																		//プロトコル
	INT32 access;																		//クライアント/サーバー種別
	SOCKET s;																			//ソケット

	SOCKADDR_IN addr_in_rcv;															//受信アドレス
	SOCKADDR_IN addr_in_dst;															//送信先アドレス
	SOCKADDR_IN addr_in_from;															//送信元アドレス

	CRITICAL_SECTION csSck;																//受信排他用クリティカルセクション

	HRESULT Initialize();																//WinSock初期化
	HRESULT init_sock_udp(HWND hwnd, SOCKADDR_IN addr_in);								//受信ソケットの初期化
	HRESULT init_sock(HWND hwnd, SOCKADDR_IN addr_in) {
		if (protocol == UDP_PROTOCOL) return init_sock_udp(hwnd, addr_in);
		else return S_FALSE;
	}

	HRESULT Close();

	HRESULT set_dst_addr(SOCKADDR_IN addr_in) { addr_in_dst = addr_in; return S_OK; };	//送信先アドレスセット

	int snd_udp_msg(const char* pbuf, int len, SOCKADDR_IN addr);						//メッセージ送信
	int rcv_udp_msg(char* pbuf, int len);

	wostringstream err_msg;
	wstring wstr_sock_err;

	INT32 status = 0;

private:

	HRESULT GetSockMsg(int nError);
	HRESULT SetSockErr(int nError);	

};
