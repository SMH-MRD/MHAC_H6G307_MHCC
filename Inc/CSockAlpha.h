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

#define MAX_N_MULTICAST			4

class CSockAlpha
{
public:
	CSockAlpha();
	CSockAlpha(INT32 protocol, INT32 access, INT32 eventID);
	virtual ~CSockAlpha(void);

	INT32 eventID;																		//非同期イベントID
	INT32 protocol;																		//プロトコル
	INT32 access;																		//クライアント/サーバー種別
	INT32 n_multicast;																	//マルチキャストアドレス設定数
	SOCKET s;																			//ソケット

	SOCKADDR_IN addr_in_rcv;															//受信アドレス
	SOCKADDR_IN addr_in_dst;															//送信先アドレス
	SOCKADDR_IN addr_in_from;															//送信元アドレス
	SOCKADDR_IN addr_in_multi[MAX_N_MULTICAST];											//マルチキャストアドレス

	CRITICAL_SECTION csSck;																//受信排他用クリティカルセクション

	HRESULT Initialize();																//WinSock初期化
	HRESULT init_sock_udp(HWND hwnd, SOCKADDR_IN addr_in);								//受信ソケットの初期化
	HRESULT init_sock_udp_m(HWND hwnd, SOCKADDR_IN addr_in, SOCKADDR_IN addr_in_m);		//受信ソケットの初期化(マルチキャスト）
	HRESULT init_sock(HWND hwnd, SOCKADDR_IN addr_in) {
		if (protocol == UDP_PROTOCOL) return init_sock_udp(hwnd, addr_in);
		else return S_FALSE;
	}
	HRESULT init_sock_m(HWND hwnd, SOCKADDR_IN addr_in, SOCKADDR_IN addr_in_m) {
		if (protocol == UDP_PROTOCOL) return init_sock_udp_m(hwnd, addr_in, addr_in_m);
		else return S_FALSE;
	}
	
	HRESULT add_multi_to_sock(SOCKADDR_IN addr_in_m); //マルチキャストグループへの参加登録

	HRESULT Close();

	HRESULT set_dst_addr(SOCKADDR_IN addr_in) { addr_in_dst = addr_in; return S_OK; };		//送信先アドレスセット

	int snd_udp_msg(const char* pbuf, int len, SOCKADDR_IN addr);						//メッセージ送信
	int rcv_udp_msg(char* pbuf, int len);												//メッセージ受信

	wostringstream err_msg;
	wstring wstr_sock_err;

	INT32 status = 0;

private:

	HRESULT GetSockMsg(int nError);
	HRESULT SetSockErr(int nError);	

};
