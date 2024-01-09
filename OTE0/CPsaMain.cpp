#include "CPsaMain.h"
#include "framework.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//****************************************************************
//* Function Name   : CPsaMain
//****************************************************************/
CPsaMain::CPsaMain(HWND hWnd){
	hwork_wnd = hWnd;
	PlayStatus = PLAYSTOP;
	m_psapi = NULL;
}
CPsaMain::CPsaMain() {
	hwork_wnd = NULL;
	PlayStatus = PLAYSTOP;
	m_psapi = NULL;
}
CPsaMain::~CPsaMain(){}

//****************************************************************
//* Function Name   : OnInitDialog
//****************************************************************/
HRESULT CPsaMain::init_psa(HWND hWnd)
{
	PlayStatus = PLAYSTOP;
	m_csLog = L"";

	m_pcallback.SetLogWndHandle(NULL);				//Set message handle
	m_pcallback.SetProcHandle(hWnd);				//Set proc handle

	//-----------------------------------------------------
	//Create an instance
	//-----------------------------------------------------	
	m_psapi = NULL;
	m_psapi = GetIPSAPI();

	//-----------------------------------------------------
	//Set properties
	//-----------------------------------------------------
	//Set properties to connect the device
	char ip[64] = "192.168.1.81";
	char username[64] = "SHI";
	char password[64] = "Shimh001";
	m_psapi->SetIPAddr(ip); //IP address of the device
	m_psapi->SetDeviceType(2);          //Device type:0-3
	m_psapi->SetHttpPort(80);           //Port:0-65535
	m_psapi->SetUserName(username);      //User name to access the device
	m_psapi->SetPassword(password);      //Password to access the device

	//Set properties for display area
	m_psapi->SetVideoWindow(hWnd);  //Set the window handle to display
	m_psapi->SetImageWidth(640);            //Image width
	m_psapi->SetImageHeight(480);           //Imgae Height

	//Set properties for image format
	m_psapi->SetStreamFormat(3);        //Image format - JPEG:0,MPEG4:1
	m_psapi->SetJPEGResolution(640);    //JPEG Resolution(It works in case of StreamFormat=0)
	m_psapi->SetMPEG4Resolution(640);   //MPEG-4 Resolution(It works in case of StreamFormat=1)
	m_psapi->SetH264Resolution(640);    //H.264 Resolution(It works in case of StreamFormat=3)

	//-----------------------------------------------------
	//Set Listener
	//-----------------------------------------------------
	m_psapi->SetErrListener((IAppListener*)&m_pcallback);
	m_psapi->SetDevListener(NULL);
	m_psapi->SetRecListener(NULL);
	m_psapi->SetPlayListener((IAppListener*)&m_pcallback);
	m_psapi->SetImageListener(NULL, 1);
	m_psapi->SetRecordListener(NULL);
	m_psapi->SetOpListener(NULL);
	m_psapi->SetAlmListener(NULL);

	//Paint background color
	m_psapi->ClearImage();

	//CHAR csbuf[256];
	//sprintf(csbuf, "[Function] ClearImage");
	//m_dlog.Logging(m_csLog);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

//****************************************************************
//* Function Name   : OnClose
//****************************************************************/
void CPsaMain::OnClose()
{
	//-----------------------------------------------------
	//Delete the instance
	//-----------------------------------------------------
	m_psapi->SetVideoWindow(NULL);  //Set the window handle to display

	//-----------------------------------------------------
	//Set Listener
	//-----------------------------------------------------
	m_psapi->SetErrListener(NULL);
	m_psapi->SetDevListener(NULL);
	m_psapi->SetRecListener(NULL);
	m_psapi->SetPlayListener(NULL);
	m_psapi->SetImageListener(NULL, 1);
	m_psapi->SetRecordListener(NULL);
	m_psapi->SetOpListener(NULL);
	m_psapi->SetAlmListener(NULL);

	DeleteIPSAPI(m_psapi);
	m_psapi = NULL;

	//	CDialog::OnClose();
}

//****************************************************************
//* Function Name   : OnPaint
//****************************************************************/
void CPsaMain::OnPaint()
{

}

//****************************************************************
//* Function Name   : LiveStart
//****************************************************************/
void CPsaMain::LiveStart()
{
	//-----------------------------------------------------
	//Define variables
	//-----------------------------------------------------
	//long	lRet = 0;
	//long	lChannel = 0;
	//long	lStatus = 0;
	//long	lBlocking = 0;

	//-----------------------------------------------------
	//Connect to the device
	//-----------------------------------------------------
	lRet = m_psapi->Open();
//	m_csLog.Format("[Function] Open:%d", lRet);
//	m_dlog.Logging(m_csLog);


	if (lRet > -1) {
		//Success to connect
		//-----------------------------------------------------
		//Start Live
		//-----------------------------------------------------
		lChannel = 1;                      //Connect to 1ch
		lBlocking = 0;                      //0:Blocking mode
		lRet = m_psapi->PlayLive(lChannel, lStatus, lBlocking, NULL);
//		m_csLog.Format("[Function] PlayLive(Start):%d", lRet);
//		m_dlog.Logging(m_csLog);

		//Change status
		if (lRet == 0) {
			//Status:Live
			PlayStatus = PLAYSTART;
		}
		else {
			//Status:Stop
			m_psapi->Close();
//			m_csLog.Format("[Function] Close");
//			m_dlog.Logging(m_csLog);
		}
	}
}

//****************************************************************
//* Function Name   : LiveStop
//****************************************************************/
void CPsaMain::LiveStop()
{
	//-----------------------------------------------------
	//Define variables
	//-----------------------------------------------------
	//long    lRet = 0;
	//long	lCommand = 0;
	//long	lSpeed = 0;
	//long    lStatus = 0;
	//long    lBlocking = 0;


	if (PlayStatus == PLAYSTART) {
		//-----------------------------------------------------
		//Stop Live
		//-----------------------------------------------------
		lCommand = 1;
		lSpeed = 1;	//If you want to stop play, you set zero.
		lBlocking = 0;	//0:Blocking
		lRet = m_psapi->PlayControl(lCommand, lSpeed, lStatus, lBlocking, NULL);
//		m_csLog.Format("[Function] PlayLive(Stop):%d", lRet);
//		m_dlog.Logging(m_csLog);

		//-----------------------------------------------------
		//Close connection to the device
		//-----------------------------------------------------
		m_psapi->Close();
//		m_csLog.Format("[Function] Close");
//		m_dlog.Logging(m_csLog);

		m_psapi->ClearImage();
//		m_csLog.Format("[Function] ClearImage");
//		m_dlog.Logging(m_csLog);

		//Change status
		PlayStatus = PLAYSTOP;
	}
	else {
//		m_csLog.Format("[Message] No live.");
//		m_dlog.Logging(m_csLog);
	}
}

//****************************************************************
//* Function Name   : OnStop
//****************************************************************/
void CPsaMain::CtrlStop()
{
	lpan = ltilt = lzoom = lfocus = liris = 0;
	if (PlayStatus == PLAYSTART) {
		lRet = m_psapi->CameraControl(lChannel, lpan, ltilt, lzoom, lfocus, liris);
		//m_csLog.Format("[Function] CameraControl(Stop):%d", lRet);
		//m_dlog.Logging(m_csLog);
	}
	else {
		//m_csLog.Format("[Message] No live.");
		//m_dlog.Logging(m_csLog);
	}
}

//****************************************************************
//* Function Name   : Update Control
//****************************************************************/
void CPsaMain::UpdateControl()
{
	if (PlayStatus == PLAYSTART) {
		lRet = m_psapi->CameraControl(lChannel, lpan, ltilt, lzoom, lfocus, liris);
		//m_csLog.Format("[Function] CameraControl(Stop):%d", lRet);
		//m_dlog.Logging(m_csLog);
	}
	else {
		//m_csLog.Format("[Message] No live.");
		//m_dlog.Logging(m_csLog);
	}
}


