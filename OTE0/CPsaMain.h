#pragma once

//Include header files of PS-API
#include "psapidef.h"
#include "ipsapi.h"
#include "isearchresult.h"

#include "CPsaCB.h"
#include <string>

//--------------------------------------------------------------------
//define
//--------------------------------------------------------------------
#define PLAYSTART		1
#define PLAYSTOP		0

#define DEF_SPD_CAM_TILT 150
#define DEF_SPD_CAM_PAN  150
#define DEF_SPD_CAM_ZOOM  1
#define DEF_SPD_CAM_IRIS  1

#define DEF_STREAM_FORMAT_H264	3
#define DEF_STREAM_FORMAT_JPEG	0


class CPsaMain
{

public:
	CPsaMain(HWND hWnd, const char* _ip, const char* _user, const char* _pass,long format);
	CPsaMain();

	~CPsaMain();

	HWND hwork_wnd;
	HRESULT init_psa(HWND hWnd, const char* _ip, const char* _user, const char* _pass, long format);

	void OnClose();
	void OnPaint();

	void LiveStart();
	void LiveStop();
	void CtrlStop();
	void UpdateControl();

	void SwitchCamera(HWND hWnd, const char* _ip, const char* _user, const char* _pass, long format);


	
	//-----------------------------------------------------
	// Define variables
	//-----------------------------------------------------
	IPSAPI* m_psapi;
	CPsaCB	m_pcallback;
	std::wstring m_csLog;
	long	PlayStatus;

	long	lRet = 0;
	long	lChannel = 0;
	long	lStatus = 0;
	long	lBlocking = 0;
	long	lCommand = 0;
	long	lSpeed = 0;

	long	lpan = 0;
	long	ltilt = 0;
	long	lzoom = 0;
	long	lfocus = 0;
	long	liris = 0;


	void SetImageSize(long w, long h) {
		m_psapi->SetImageWidth(640);            //Image width
		m_psapi->SetImageHeight(480);           //Imgae Height
		return;
	}

};

