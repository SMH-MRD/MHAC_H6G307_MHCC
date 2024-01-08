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


class CPsaMain
{

public:
	CPsaMain(HWND hWnd); 
	CPsaMain();

	~CPsaMain();

	HWND hwork_wnd;
	HRESULT init_psa(HWND hWnd);

	void OnClose();
	void OnPaint();

	void LiveStart();
	void LiveStop();
	
	//-----------------------------------------------------
	// Define variables
	//-----------------------------------------------------
	IPSAPI* m_psapi;
	CPsaCB	m_pcallback;
	std::wstring m_csLog;
	long		PlayStatus;


};

