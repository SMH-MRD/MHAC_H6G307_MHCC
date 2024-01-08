#include "framework.h"
#include "CPsaCB.h"

#pragma warning(disable : 4996)

CPsaCB::CPsaCB()
{
	m_hLogWnd = NULL;
	m_hProcWnd = NULL;
}

CPsaCB::~CPsaCB()
{
	m_hLogWnd = NULL;
	m_hProcWnd = NULL;
}
/****************************************************************
 * function			: LogWndHandle
****************************************************************/
void CPsaCB::SetLogWndHandle(HWND LogWnd)
{
	if (LogWnd == NULL) return;

	m_hLogWnd = LogWnd;
}

HWND CPsaCB::GetLogWndHandle()
{
	return m_hLogWnd;

}

/****************************************************************
 * function			: ProcHandle
****************************************************************/
void CPsaCB::SetProcHandle(HWND hwnd)
{
	if (hwnd == NULL) return;

	m_hProcWnd = hwnd;

}

HANDLE CPsaCB::GetProcHandle()
{
	return m_hProcWnd;

}

/****************************************************************
 * function			: SendMsg
 ****************************************************************/
void CPsaCB::SendMsg(std::string msg)
{
	CHAR* l_pcStr = NULL;
	long	l_lMsglen = msg.length();
	if (l_lMsglen == 0)return;
	if (m_hLogWnd == NULL)return;

	l_pcStr = new CHAR[l_lMsglen + 1];
	sprintf(l_pcStr,"%s", msg);
	::PostMessage(m_hLogWnd, (WM_USER + 1), (UINT)l_pcStr, 0);
}

/****************************************************************
 * function			: SendParm
****************************************************************/
void CPsaCB::SendParm(long wParam, long lParam)
{
	if (m_hProcWnd == NULL)return;

	::PostMessage(m_hProcWnd, (WM_USER + 2), wParam, lParam);
}


//----------------------------------------------------------------
//Listener
//----------------------------------------------------------------
/****************************************************************
 * function			: OnError
****************************************************************/
void CPsaCB::OnError(long error, LPCSTR description)
{
	CHAR csbuf[256];
	sprintf(csbuf, "[OnError] error[%d] description[%s]", error, description);
	std::string	msg = csbuf;
	SendMsg(msg);
}

/****************************************************************
 * function			: OnDevStatus
****************************************************************/
void CPsaCB::OnDevStatus(long channel, long status)
{
	CHAR csbuf[256];
	sprintf(csbuf, "[OnDevStatus] channel[%d] status[%d]", channel, status);
	std::string	msg = csbuf;
	SendMsg(msg);

}

/****************************************************************
 * function			: OnRecStatus
****************************************************************/
void CPsaCB::OnRecStatus(long channel, long status)
{
	CHAR csbuf[256];
	sprintf(csbuf, "[OnRecStatus] channel[%d] status[%d]", channel, status);
	std::string	msg = csbuf;
	SendMsg(msg);
}

/****************************************************************
 * function			: OnRecordStatus
****************************************************************/
void CPsaCB::OnRecordStatus(long  type,
	LPCSTR  lpcszTD,
	LPCSTR lpcszNextTD)
{

	CHAR csbuf[256];
	sprintf(csbuf, "[OnRecordStatus] type[%d] lpcszTD[%s] lpcszTD[%s]", type, lpcszTD, lpcszNextTD);
	std::string	msg = csbuf;
	SendMsg(msg);

}

/****************************************************************
 * function			: OnPlayStatus
****************************************************************/
void CPsaCB::OnPlayStatus(long channel, long status)
{

	CHAR csbuf[256];
	sprintf(csbuf, "[OnPlayStatus] channel[%d] status[%d]", channel, status);
	std::string	msg = csbuf;
	SendMsg(msg);

}

/****************************************************************
 * function			: OnImage
****************************************************************/
void CPsaCB::OnImage(long type, BYTE* pBuffer, long size)
{
	CHAR csbuf[256];
	sprintf(csbuf, "[OnImage] type[%d] size[%d byte])", type, size);
	std::string	msg = csbuf;
	SendMsg(msg);
}

/****************************************************************
 * function			: OnOpStatus
****************************************************************/
void CPsaCB::OnOpStatus(long channel, long status)
{
	
	CHAR csbuf[256];
	sprintf(csbuf, "[OnOpStatus] channel[%d] status[%d]", channel, status);
	std::string	msg = csbuf;
	SendMsg(msg);
}

/****************************************************************
 * function			: OnAlarmStatus
****************************************************************/
void CPsaCB::OnAlmStatus(long  channel,
	long  type,
	LPCSTR lpcszTD,
	long  status)
{
	CHAR csbuf[256];
	sprintf(csbuf, "[OnAlmStatus] channel[%d] type[%d] lpcszTD[%s] status[%d]", channel, type, lpcszTD, status);
	std::string	msg = csbuf;
	SendMsg(msg);
}


//----------------------------------------------------------------
//CallBack
//----------------------------------------------------------------
/****************************************************************
 * function			: OnRecCB
****************************************************************/
void CPsaCB::OnRecCB(long status)
{
	CHAR csbuf[256];
	sprintf(csbuf, "[OnRecCB] status[%d])", status);
	std::string	msg = csbuf;
	SendMsg(msg);
}

/****************************************************************
 * function			: OnSearchCB
****************************************************************/
void CPsaCB::OnSearchCB()
{
	//Logging

	CHAR csbuf[256];
	sprintf(csbuf, "[OnSearchCB] Show Search result.");
	std::string	msg = csbuf;
	SendMsg(msg);

	//Proc
	SendParm(0, 0);
}

/****************************************************************
 * function			: OnSearchExCB
****************************************************************/
void CPsaCB::OnSearchExCB()
{
	//Logging
	CHAR csbuf[256];
	sprintf(csbuf, "[OnSearchExCB] Show Search result.");
	std::string	msg = csbuf;
	SendMsg(msg);

	//Proc
	SendParm(1, 0);
}

/****************************************************************
 * function			: OnPlayStatusCB
****************************************************************/
//PlayCallBack
void CPsaCB::OnPlayStatusCB(long status)
{
	CHAR csbuf[256];
	sprintf(csbuf, "[OnPlayStatusCB] status[%d])", status);
	std::string	msg = csbuf;
	SendMsg(msg);
}

/****************************************************************
 * function			: OnOPStatusCB
****************************************************************/
void CPsaCB::OnOpStatusCB(long status)
{
	CHAR csbuf[256];
	sprintf(csbuf, "[OnOpStatusCB] status[%d])", status);
	std::string	msg = csbuf;
	SendMsg(msg);
}

/****************************************************************
 * function			: OnAlarmStatusCB
****************************************************************/
void CPsaCB::OnAlmStatusCB(long  status)
{
	CHAR csbuf[256];
	sprintf(csbuf, "[OnAlmStatusCB] status[%d])", status);
	std::string	msg = csbuf;
	SendMsg(msg);
}

/****************************************************************
 * function			: OnFtpStatusCB
****************************************************************/
void CPsaCB::OnFtpStatusCB(long status)
{
	CHAR csbuf[256];
	sprintf(csbuf, "[OnFtpStatusCB] status[%d])", status);
	std::string	msg = csbuf;
	SendMsg(msg);
}

