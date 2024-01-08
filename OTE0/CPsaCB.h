#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する
// Windows ヘッダー ファイル
#include <windows.h>
#include <string>

#include "psapidef.h"
#include "iapplistener.h"
#include "iappcallback.h"

class CPsaCB :
    public IAppListener,
	public IAppCallBack
{
public:
	//Constructor/Destructor
	CPsaCB();
	virtual ~CPsaCB();

	HWND GetLogWndHandle();
	void SetLogWndHandle(HWND LogWnd);

	HANDLE GetProcHandle();
	void SetProcHandle(HWND hwnd);

	void SendMsg(std::string msg);
	void SendParm(long wParam, long lParam);

	//Override -Listener
	void	OnError(long error,
		LPCSTR description);

	void	OnDevStatus(long channel,
		long status);

	void	OnRecStatus(long channel,
		long status);

	void	OnRecordStatus(long type,
		LPCSTR lpcszTD,
		LPCSTR lpcszNextTD);

	void	OnPlayStatus(long channel,
		long status);

	void	OnImage(long type,
		BYTE* pBuffer,
		long size);

	void	OnOpStatus(long channel,
		long status);

	void	OnAlmStatus(long channel,
		long lType,
		LPCSTR lpcszTD,
		long status);

	//Override -CallBack
	void	OnRecCB(long status);

	void	OnSearchCB();

	void	OnSearchExCB();

	void	OnPlayStatusCB(long status);

	void	OnOpStatusCB(long status);

	void	OnAlmStatusCB(long status);

	void	OnFtpStatusCB(long status);

private:
	HWND	m_hLogWnd;
	HWND	m_hProcWnd;
};

