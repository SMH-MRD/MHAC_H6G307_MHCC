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

	INT32 eventID;																		//�񓯊��C�x���gID
	INT32 protocol;																		//�v���g�R��
	INT32 access;																		//�N���C�A���g/�T�[�o�[���
	SOCKET s;																			//�\�P�b�g

	SOCKADDR_IN addr_in_rcv;															//��M�A�h���X
	SOCKADDR_IN addr_in_dst;															//���M��A�h���X
	SOCKADDR_IN addr_in_from;															//���M���A�h���X

	CRITICAL_SECTION csSck;																//��M�r���p�N���e�B�J���Z�N�V����

	HRESULT Initialize();																//WinSock������
	HRESULT init_sock_udp(HWND hwnd, SOCKADDR_IN addr_in);								//��M�\�P�b�g�̏�����
	HRESULT init_sock(HWND hwnd, SOCKADDR_IN addr_in) {
		if (protocol == UDP_PROTOCOL) return init_sock_udp(hwnd, addr_in);
		else return S_FALSE;
	}

	HRESULT Close();

	HRESULT set_dst_addr(SOCKADDR_IN addr_in) { addr_in_dst = addr_in; return S_OK; };	//���M��A�h���X�Z�b�g

	int snd_udp_msg(const char* pbuf, int len, SOCKADDR_IN addr);						//���b�Z�[�W���M
	int rcv_udp_msg(char* pbuf, int len);

	wostringstream err_msg;
	wstring wstr_sock_err;

	INT32 status = 0;

private:

	HRESULT GetSockMsg(int nError);
	HRESULT SetSockErr(int nError);	

};
