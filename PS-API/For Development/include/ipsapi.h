#ifndef _IPSAPI_
#define _IPSAPI_
/******************************************************************************************
 *  INTERFACE MODULE	PS API DLL
 ******************************************************************************************
 *
 *	UNIT:			PS API DLL Interface
 *
 *	FILE NAME:		ipsapi.h
 *
 *	DESCRIPTION:	PS API DLL Interface
 *
 *	INCLUDES:
 */

					#include	"psapidef.h"
					#include	"isearchresult.h"
					#include	"ipsapipicture.h"
					#include	"ipsapifileinfo.h"
					#include	"ipsapiftpfilelist.h"
					#include	"iapplistener.h"
					#include	"iappcallback.h"


/*
 *	NAMESPACE:
 */

/*
 *	NOTES:
 *
 *  REV HIST:
 * 
 *	Name		Date		Reason
 *	---------------------------------------------------------------------------------------
 *	S.Ramaswamy 09/17/08	Created
 *	S.Ramaswamy 12/21/08	Modified SetXXX function to return error value
 *	S.Ramaswamy 12/22/08	Moved PSAPIDECLSPEC defintion to psapidef.h
 *	S.Ramaswamy 05/13/09	Added DrawRawData() and GetLastError()
 *	S.Ramaswamy 06/09/09	Renamed DrawRawData() to DecodeImage()
 *	S.Ramaswamy	06/09/09	Modified GetLastError() to return error description
 *	S.Ramaswamy 08/25/09	Added SearchEx(), GetDevTZ(), Set/GetTFrmRate(), 
 *							Set/GetTotalDecRate(), Set/GetH264Port(), Set/GetStreamNumber()
 *							Set/GetH264Resolution() and Set/GetMulticastAutoConf()
 *	S.Ramaswamy 08/29/09	Added FtpGet(), FtpCancel(), FtpServerClose(), 
 *							GetFtpStatus(), Set/GetFtpTransMode(), Set/GetFtpPort(),
 *							GetFtpTransRate() and GetFtpTransByte()
 *	S.Ramaswamy 02/24/10	Added VMDSearchEx(), SearchCancel(), SetSearchMultiChMask()
 *	S.Ramaswamy 02/24/10	Added GetDeviceLog(), GetDevCurrentInfo(), GetInfoString(), , 
 *							GetSearchMultiChMask(), SaveFile(), SnapShot(), 
 *							TitleOperation(), BoxOperation(), GetTitle() and SetBackColor(), 
 *	S.Ramaswamy 02/24/10	Added functions to get #of waiting functions or to clear them.
 *	S.Ramaswamy 02/24/10	Added Intelligent view, digital zoom, skiprecordgap functions
 *	S.Ramaswamy 02/24/10	Added Audio functions
 *	PSN         04/05/10	Add SetSaveFileListener()
 *  PSN         04/06/10    Added GetMultiScreenChannel(), SetMultiScreenChannel()
 *  PSN         07/15/10    ChgNo.060 - Added isDst flag to PlayControlByTime().
 *  PSN         07/15/10    ChgNo.062 - Added MultiSyncPause() and MultiSyncTime() method.
 *  PSN         08/31/10    Added SetImageFixToView() and GetImageFixToView()
 *  PSN		    09/30/10    Changed interface name from FixToView to PictureFitMode
 *  PSN         09/30/10    Added GetPicturePosition(), ConvertFile(), Set/GetPictureFitMode(),
 *                          Set/GetDecResolutionMode(), Set/GetFilePassword(),
 *                          and SetConvertFileListener()
 *  PSN         11/18/10    Added SetMultiAutoStMode()
 *  PSN         04/22/11    Added SetCameraTime(), SetPersonalID(), GetFileInfo(),
 *                                SetFileInfo(), DetectAlteration(), DistortionAdjust(),
 *                                SetCorrectDistortionParam(), FtpGetFileList(),
 *                                FtpGetCam(), FtpDeleteCamFile(), FtpDeleteCam(),
 *                                SetFtpPermissionCam().
 *                          Added SetCorrectDistortionMode() / GetCorrectDistortionMode(),
 *                                SetStabilizationMode() / GetStabilizationMode().
 *  PSN         05/25/11    Added an argument of DetectAlteration (&RESULT)
 *  PSN         06/10/11    Added CameraLEDControl()
 *  PSN         07/14/11    Added SetRollingDegree()/GetRollingDegree()
 *  PSN         08/04/11    Added SetInternetMode() / GetInternetMode()
 *  PSN         09/01/11    Added TitleOperationEx(), BoxOperationEx(), BitmapOperationEx(),
 *                                SetInternetModeCam()
 *                                SetFastPlayMode() / GetFastPlayMode(),
 *                                SetRtpPortMode() / GetRtpPortMode(),
 *                                SetRtpPortRange() / GetRtpPortRange()
 *  PSN         09/01/11    Added SetAudioListener()
 *  PSN         11/18/11    ChgNo.WCAM_SDK_016 Change DetectAlteration interface
 *  PSN         11/18/11    ChgNo.WCAM_SDK_016 Added SetDetectAlterationListener interface
 *  PSN         02/01/12    Added GetLoginStatus() / GetSIDInfo() / GetUIDInfo()
 *                                GetStatisticsData() / DevResutart() / SetUIDPriority()
 *                                SetSIDMode() / GetSIDMode()
 *  PSN         05/17/12    Added GetFileInfoEx()
 *  PSN         10/08/12    Added SetFCropSize() / GetFCropSize() / SetFCropPos() / GetFCropPos()
 *                                SetFCropZoom() / GetFCropZoom()
 *                                SetFPanoramaAngle() / GetFPanoramaAngle()
 *                                SetFWPanoramaAngle() / GetFWPanoramaAngle()
 *                                CamSnapShot() / CameraShutterControl()
 *                                SetFCropStyle()/ GetFCropStyle()
 *                                SetDevMaxChannel()/ GetDevMaxChannel()
 *  PSN         11/26/12    Added SetTransIntervalMode() / GetTransIntervalMode()
 *                                SetDecBufferNum() / GetDecBufferNum()
 *  PSN         01/16/13    Added SetFCropForcedMode() / GetFCropForcedMode()
 *                                SetFCropForcedParam() / GetFCropForcedParam()
 *  PSN         02/23/13    Added SetCameraImageCap()
 *  PSN         09/16/13    Added SetPlayBufferInterval() / GetPlayBufferInterval()
 *  PSN         06/21/14    Added SnapShotLive() / SnapShotPlay() / SnapShotFile() / GetFisheyeImageStyle()
 *  PSN         01/06/15    Added SetH264DecMaxRate() / Set&Get_H264DecType() / Set&Get_DispMode()
 *                                Set&Get_H264ColorRangeMode() / Set&Get_ResourceReductionMode
 *  PSN         01/07/14    Added SetConvertClipStart() / GetConvertClipStart
 *                                SetConvertClipEnd() / GetConvertClipEnd()
 *  PSN         02/15/15    Added SetPCColor(), GetPCColor(), SetPCVisibleEnable(), GetPCVisibleEnable()
 *                                SetHMMask(), GetAnalyticsSrcFiles(), AddAnalyticsTargetFile()
 *                                ClearAnalyticsTargetFile(), DrawAnalyticsImage(), SetHMRange(), GetHMRange()
 *  PSN         03/22/15    Added GetMicDevCurrentInfo() / GetMicInfoString() / PlayFileByTime()
 *                                GetAngleFromPos() / GetPosFromAngle()
 *                                SetAudioExtractionAngle() / GetAudioExtractionAngle()
 *                                SetMicIpAddr() / GetMicIpAddr() / SetMicHttpPort() / GetMicHttpPort()
 *                                SetMicUserName() / GetMicUserName() / SetMicPassword() / GetMicPassword()
 *                                MicDeviceModel() / SetMCDAPort() / GetMCDAPort()
 *                                SetMicMulticastAddr() / GetMicMulticastAddr()
 *                                SetRcvAudioDec() / GetRcvAudioDec()
 *                                SetMicAVTimeAlignment() / GetMicAVTimeAlignment()
 *                                SetOutputGainCoef() / GetOutputGainCoef()
 *                                SetVoiceClearMode() / GetVoiceClearMode()
 *                                SetVoiceClearFrequency() / GetNoiseReductionMode() 
 *                                SetNoiseReductionMode() / GetVoiceClearFrequency()
 *                                SetAudioExtractGainCoef() / GetAudioExtractGainCoef()
 *  PSN         07/08/15    Added QueryFunction()
 *  PSN         07/16/15    Added SetVESnowParameter() / GetVESnowParameter()
 *                                SetVEDtccParameter() / GetVEDtccParameter()
 *                                SetVEFrmsynParameter() / GetVEFrmsynParameter()
 *                                ResetVEParameter() / VEStart() / VEStop() / GetVEStatus()
 *                                SetVEMaskArea() / DeleteVEMaskArea() / GetVEMaskArea() / SnapshotEx()
 *  PSN         07/16/15    Added SetVESnowEnable() / GetVESnowEnable()
 *                                SetVEDtccEnable() / GetVEDtccEnable()
 *                                SetVEFrmsynEnable() / GetVEFrmsynEnable()
 *                                SetVEDecMaxRate() / GetVEDecMaxRate()
 *                                SetVEProcMode() / GetVEProcMode()
 *  PSN         10/02/15    Added GetAnalyticsSrcFilesByHours()
 *                                SetCroppingRect() / GetCroppingRect()
 *                                SetCroppingDrawRect() / GetCroppingDrawRect()
 *                                SetCroppingDrawEnabled() / GetCroppingDrawEnabled()
 *                                SetCroppingMarker() / GetCroppingMarker()
 *                                SetCroppingEnabled() / GetCroppingEnabled()
 *                                SetMaxDrawRate() / GetMaxDrawRate()
 *  PSN         10/08/15    Changed SetCroppingMarker() / GetCroppingMarker()
 *  PSN         12/15/15    Added SetSecureCommunicationMode()/GetSecureCommunicationMode()
 *  PSN		    04/22/16    Added RecStream() / GetRecStreamStatus() / SetRecStreamListener()
 *  PSN		    12/01/16    Added CameraWiperControl()
 *  PSN		    10/23/17    Added SetNXStreamNumber() / GetNXStreamNumber()
 *  CMS         01/17/18    Added SetHEVCDecMaxRate() / Set&Get_FCropPosAdjustType()
 *  Comsis      10/15/20    Added SetFigureDrawEnable(), GetFigureDrawEnable(), SetFigureDrawType(), GetFigureDrawType()
 *                                DeleteFigureDraw(), SetFigureDrawImagePos(), GetFigureDrawImagePos(), OnFigureAreaID()
 *								  SetVideoRcvBitRateEnable(), GetVideoRcvBitRateEnable()
 *	Comsis		12/11/20	Added SetAreaID(), SetAudioDetectListener()
 *	Comsis		02/12/21	Added SetFigureMaxAreaID(), GetFigureMaxAreaID(), SetAreaLineColor()
 *	Comsis		02/08/21	Added SetCertificateVerifyEnable(), GetCertificateVerifyEnable()
 *	Comsis		03/25/21	Added HttpDownload()
 *	Comsis		09/01/21	Added CircleFrameDraw()
 *	Comsis		09/15/21	Added SetLineDirectionChg()
 *	Comsis		06/13/22	Added SetRectZoomMode(), GetRectZoomMode()
 *
 ******************************************************************************************
 *  COPYRIGHT:		Panasonic System Networks Co., Ltd. 2008 - 2015
 *****************************************************************************************/

/*----------------------------------------------------------------------------------------
 |  PS API Interface Class
 +--------------------------------------------------------------------------------------*/

class PSAPIDECLSPEC IPSAPI
{
public:
	virtual ~IPSAPI() {};

//---------------------------------------------------------------------
// IPSBuilder Interface
//---------------------------------------------------------------------
public:
	virtual SPSAPITYPE::UID			Open( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	Connect( SPSAPITYPE::UID ) = 0;

	virtual void					Close( void ) = 0;

	virtual void					Disconnect( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	ClearWaitingFunc( void ) = 0;

	virtual SPSAPITYPE::COUNT		GetWaitingFuncCount( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetLastError ( SPSAPITYPE::ERRVALUE*, 
												   SPSAPITYPE::DESC, 
												   SPSAPITYPE::COUNT ) = 0;

	virtual SPSAPITYPE::STATUS		GetLoginStatus( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetUIDInfo(	SUID_INFO*	) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetSIDInfo(	SSID_INFO*	) = 0;

public:
	virtual SPSAPITYPE::ERRVALUE	SetDeviceType( SPSAPITYPE::DEVTYPE ) = 0;
	virtual SPSAPITYPE::DEVTYPE		GetDeviceType( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetIPAddr( SPSAPITYPE::IPADDR ) = 0;
	virtual void					GetIPAddr( SPSAPITYPE::IPADDR, 
											   SPSAPITYPE::COUNT ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetHttpPort( SPSAPITYPE::PORT ) = 0;
	virtual SPSAPITYPE::PORT		GetHttpPort( void ) = 0;
	
	virtual SPSAPITYPE::ERRVALUE	SetHttpTimeout( SPSAPITYPE::TIME ) = 0;
	virtual SPSAPITYPE::TIME		GetHttpTimeout( void ) = 0;
	
	virtual SPSAPITYPE::ERRVALUE	SetProxyName( SPSAPITYPE::PROXY ) = 0;
	virtual void					GetProxyName( SPSAPITYPE::PROXY,
												  SPSAPITYPE::COUNT ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetProxyPort( SPSAPITYPE::PORT ) = 0;
	virtual SPSAPITYPE::PORT		GetProxyPort( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetAccessType( SPSAPIPSB::ACCESSTYPE ) = 0;
	virtual SPSAPIPSB::ACCESSTYPE	GetAccessType( void ) = 0;

	virtual SPSAPITYPE::UID			GetUID( void ) = 0;


	virtual SPSAPITYPE::ERRVALUE	SetUserName( SPSAPITYPE::UNAME ) = 0;
	virtual void					GetUserName( SPSAPITYPE::UNAME,
												 SPSAPITYPE::COUNT ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetPassword( SPSAPITYPE::PSSWD ) = 0;
	virtual void					GetPassword( SPSAPITYPE::PSSWD,
												 SPSAPITYPE::COUNT ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetDevMaxChannel( SPSAPITYPE::CHANNEL ) = 0;
	virtual SPSAPITYPE::CHANNEL		GetDevMaxChannel( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetMicIpAddr( SPSAPITYPE::IPADDR ) = 0;
	virtual void					GetMicIpAddr( SPSAPITYPE::IPADDR, 
												  SPSAPITYPE::COUNT ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetMicHttpPort( SPSAPITYPE::PORT ) = 0;
	virtual SPSAPITYPE::PORT		GetMicHttpPort( void ) = 0;
	
	virtual SPSAPITYPE::ERRVALUE	SetMicUserName( SPSAPITYPE::UNAME ) = 0;
	virtual void					GetMicUserName( SPSAPITYPE::UNAME,
													SPSAPITYPE::COUNT ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetMicPassword( SPSAPITYPE::PSSWD ) = 0;
	virtual void					GetMicPassword( SPSAPITYPE::PSSWD,
													SPSAPITYPE::COUNT ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetSecureCommunicationMode( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetSecureCommunicationMode ( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetCertificateVerifyEnable ( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetCertificateVerifyEnable ( void ) = 0;

public:
	virtual SPSAPITYPE::ERRVALUE	SetErrListener( IAppListener* ) = 0;


//---------------------------------------------------------------------
// Device Interface
//---------------------------------------------------------------------
public:
	virtual SPSAPITYPE::STATUS		GetDevStatus( SPSAPITYPE::CHANNEL ) = 0;

	virtual SPSAPITYPE::ERRVALUE	RecCtrl( SPSAPITYPE::CHANNEL,
											 SPSAPITYPE::COMMAND,
											 SPSAPITYPE::STATE&,
											 SPSAPITYPE::MODE,
											 IAppCallBack*) = 0;

	virtual SPSAPITYPE::STATUS		GetRecStatus( SPSAPITYPE::CHANNEL ) = 0;

	virtual SPSAPITYPE::ERRVALUE	Search( SPSAPITYPE::CHANNEL,
											SPSAPITYPE::TIMEDATE,
											SPSAPITYPE::TIMEDATE,
											SPSAPIDEV::EVNTTYPE,
											ISearchResult*,
											SPSAPITYPE::MODE,
											IAppCallBack* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SearchEx( SPSAPITYPE::CHANNEL,
											  SPSAPITYPE::TIMEDATE,
											  SPSAPITYPE::TIMEDATE,
											  SPSAPIDEV::EVNTTYPE,
											  ISearchResultEx*,
											  SPSAPITYPE::MODE,
											  IAppCallBack* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	VMDSearchEx ( SPSAPITYPE::CHANNEL,
												  SPSAPITYPE::TIMEDATE,
												  SPSAPITYPE::TIMEDATE,
												  SPSAPITYPE::VALUE,
												  SPSAPIVID::SENSITIVITY,
												  SPSAPIVID::IMGPOS,
												  SPSAPIVID::IMGPOS,
												  SPSAPIVID::IMGPOS,
												  SPSAPIVID::IMGPOS,
												  SPSAPIVID::SENSITIVITY,
												  SPSAPIVID::IMGPOS,
												  SPSAPIVID::IMGPOS,
												  SPSAPIVID::IMGPOS,
												  SPSAPIVID::IMGPOS,
												  SPSAPIVID::SENSITIVITY,
												  SPSAPIVID::IMGPOS,
												  SPSAPIVID::IMGPOS,
												  SPSAPIVID::IMGPOS,
												  SPSAPIVID::IMGPOS,
												  SPSAPIVID::SENSITIVITY,
												  SPSAPIVID::IMGPOS,
												  SPSAPIVID::IMGPOS,
												  SPSAPIVID::IMGPOS,
												  SPSAPIVID::IMGPOS,
												  SPSAPIVID::IMGSIZE,
												  SPSAPIVID::IMGSIZE,
												  ISearchResultEx*,
												  SPSAPITYPE::MODE,
												  IAppCallBack* ) = 0;

	virtual	SPSAPITYPE::ERRVALUE	SearchCancel ( void ) = 0;

	virtual	SPSAPITYPE::ERRVALUE	GetDeviceLog( SPSAPITYPE::TYPE,
												  SLOGRESULT*,
												  SPSAPITYPE::COUNT,
												  SPSAPITYPE::COUNT& ) = 0;

	virtual void					GetDevTimeZone( STIMEZONE_INFO* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetDevCurrentInfo( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetInfoString( SPSAPITYPE::CHARPTR,
												   SPSAPITYPE::CHARPTR,
												   SPSAPITYPE::COUNT ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetCameraTime( SPSAPITYPE::TIMEDATE,
												   SPSAPITYPE::ISDST ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetPersonalID( SPSAPITYPE::PERSONID ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetStatisticsData(	SPSAPITYPE::TIMEDATE,
														SPSAPITYPE::TIMEDATE,
														SPSAPIVID::FILENAME	) = 0;

	virtual SPSAPITYPE::ERRVALUE	DevRestart( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetUIDPriority(	SPSAPITYPE::COMMAND	) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetMicDevCurrentInfo( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetMicInfoString( SPSAPITYPE::CHARPTR,
													  SPSAPITYPE::CHARPTR,
													  SPSAPITYPE::COUNT ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetRecordingMode( SPSAPITYPE::COMMAND ) = 0;

public:
	virtual void					GetDevModel( SPSAPITYPE::MODELPTR, 
												 SPSAPITYPE::COUNT ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetSearchMultiChMask ( SPSAPITYPE::CHARPTR ) = 0;
	virtual SPSAPITYPE::ERRVALUE	GetSearchMultiChMask ( SPSAPITYPE::CHARPTR,
														   SPSAPITYPE::COUNT ) = 0;

	virtual void					GetMicDevModel( SPSAPITYPE::MODELPTR, 
													SPSAPITYPE::COUNT ) = 0;
public:
	virtual	SPSAPITYPE::ERRVALUE	SetDevListener ( IAppListener* ) = 0;

	virtual	SPSAPITYPE::ERRVALUE	SetRecListener ( IAppListener* ) = 0;


//---------------------------------------------------------------------
// Video Interface
//---------------------------------------------------------------------
public:
	virtual void					GetFrameTime( SVIDEO_FRAMETIME* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	PlayLive( SPSAPITYPE::CHANNEL,
											  SPSAPITYPE::STATUS&,
											  SPSAPITYPE::MODE,
											  IAppCallBack* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	Play( SPSAPITYPE::CHANNEL,
										  SPSAPITYPE::TIMEDATE,
										  SPSAPITYPE::STATUS&,
										  SPSAPITYPE::MODE,
										  IAppCallBack* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	PlayFile( SPSAPIVID::FILENAME,
											  SPSAPITYPE::STATUS&,
											  SPSAPITYPE::MODE,
											  IAppCallBack* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	PlayControl( SPLAYCTRL_CMD::CMD,
												 SPSAPIVID::SPEED,
												 SPSAPITYPE::STATUS&,
												 SPSAPITYPE::MODE,
												 IAppCallBack* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	PlayControlByTime( SPSAPITYPE::TIMEDATE,
													   SPSAPITYPE::ISDST,
													   SPSAPITYPE::STATUS&,
													   SPSAPITYPE::MODE,
													   IAppCallBack* ) = 0;

	virtual SPSAPITYPE::STATUS		GetPlayStatus( void ) = 0;

	virtual SPSAPIVID::SPEED		GetPlaySpeed( void ) = 0;

	virtual SPSAPIVID::FRAMERATE 	GetFrameRate( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetImageResolution( SPSAPIVID::IMGSIZE&,
														SPSAPIVID::IMGSIZE& ) = 0;

	virtual SPSAPITYPE::ERRVALUE 	SetVideoWindow( SPSAPIVID::VIDWINDOW ) = 0;

	virtual SPSAPITYPE::ERRVALUE	DecodeImage( SPSAPIVID::IMGBUFFER*,
												 SPSAPIVID::IMGSIZE ) = 0;

	virtual void					ClearImage( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SnapShot ( IPSAPIPicture* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	TitleOperation ( SPSAPITYPE::ID,
													 SPSAPITYPE::COMMAND,
													 SPSAPITYPE::CHARPTR,
													 SPSAPIVID::IMGPOS,
													 SPSAPIVID::IMGPOS,
													 SPSAPITEXT::ALIGN,
													 SPSAPITEXT::FONT,
													 SPSAPITYPE::COUNT,
													 SPSAPITYPE::COLOR,
													 SPSAPITYPE::COLOR,
													 SPSAPITYPE::STYLE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	TitleOperationEx ( SPSAPITYPE::ID,
													   SPSAPITYPE::COMMAND,
													   SPSAPITYPE::CHARPTR,
													   SPSAPIVID::IMGPOS,
													   SPSAPIVID::IMGPOS,
													   SPSAPITEXT::ALIGN,
													   SPSAPITEXT::FONT,
													   SPSAPITYPE::COUNT,
													   SPSAPITYPE::COLOR,
													   SPSAPITYPE::COLOR,
													   SPSAPITYPE::STYLE,
													   SPSAPITYPE::TRNSMISSIV) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetTitle( SPSAPITYPE::ID,
											  SPSAPITYPE::CHARPTR,
											  SPSAPITYPE::COUNT ) = 0;

	virtual SPSAPITYPE::ERRVALUE	BoxOperation ( SPSAPITYPE::ID,
												   SPSAPITYPE::COMMAND,
												   SPSAPITYPE::COLOR,
												   SPSAPITYPE::COUNT,
												   SPSAPIVID::IMGPOS,
												   SPSAPIVID::IMGPOS,
												   SPSAPIVID::IMGPOS,
												   SPSAPIVID::IMGPOS ) = 0;

	virtual SPSAPITYPE::ERRVALUE	BoxOperationEx ( SPSAPITYPE::ID,
													 SPSAPITYPE::COMMAND,
													 SPSAPITYPE::COLOR,
													 SPSAPITYPE::COUNT,
													 SPSAPIVID::IMGPOS,
													 SPSAPIVID::IMGPOS,
													 SPSAPIVID::IMGPOS,
													 SPSAPIVID::IMGPOS,
													 SPSAPITYPE::TRNSMISSIV ) = 0;

	virtual SPSAPITYPE::ERRVALUE	BitmapOperationEx ( SPSAPITYPE::ID,
														SPSAPITYPE::COMMAND,
														SPSAPIVID::FILENAME,
														SPSAPIVID::IMGPOS,
														SPSAPIVID::IMGPOS,
														SPSAPITYPE::COLOR,
														SPSAPITYPE::TRNSMISSIV ) = 0;

	virtual SPSAPITYPE::ERRVALUE	DigitalZoomMove ( SPSAPIVID::IMGPOS,
													  SPSAPIVID::IMGPOS ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetDigitalZoomPosition( SPSAPIVID::IMGPOS&,
															SPSAPIVID::IMGPOS& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetPicturePosition( SPSAPIVID::IMGPOS&,
														SPSAPIVID::IMGPOS&,
														SPSAPIVID::IMGPOS&,
														SPSAPIVID::IMGPOS& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetIntelligentView ( SPSAPITYPE::TYPE,
														 SPSAPITYPE::VALUE ) = 0;

	virtual SPSAPITYPE::VALUE		GetIntelligentView	( SPSAPITYPE::TYPE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetIntelligentViewColor ( SPSAPITYPE::TYPE,
															  SPSAPITYPE::COLOR ) = 0;

	virtual SPSAPITYPE::COLOR		GetIntelligentViewColor( SPSAPITYPE::TYPE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetIntelligentViewSize ( SPSAPITYPE::TYPE,
															 SPSAPITYPE::COUNT ) = 0;

	virtual SPSAPITYPE::COUNT		GetIntelligentViewSize( SPSAPITYPE::TYPE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetIntelligentViewTrackTime ( SPSAPITYPE::TYPE,
																  SPSAPITYPE::TIME ) = 0;
	virtual SPSAPITYPE::TIME		GetIntelligentViewTrackTime( SPSAPITYPE::TYPE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	MultiSyncPause ( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	MultiSyncTime( SPSAPITYPE::TIMEDATE,
												   SPSAPITYPE::ISDST) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetMultiAutoStMode( SPSAPITYPE::CHANNEL,
														SPSAPITYPE::COMMAND,
														SPSAPIVID::FORMAT,
														SPSAPIVID::STRMNO) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetInternetModeCam( SPSAPITYPE::CHANNEL,
														SPSAPITYPE::COMMAND,
														SPSAPIVID::FORMAT,
														SPSAPIVID::STRMNO) = 0;

	virtual SPSAPITYPE::ERRVALUE	SaveFile( SPSAPITYPE::COMMAND,
											  SPSAPITYPE::FOLDER,
											  SPSAPIVID::FORMAT,
											  SPSAPITYPE::MODE,
											  SPSAPITYPE::TIME ) = 0;

	virtual SPSAPITYPE::ERRVALUE	ConvertFile( SPSAPITYPE::COMMAND,
												 SPSAPIVID::FILENAME,
												 SFILELIST*,
												 SPSAPITYPE::COUNT,
												 SPSAPIVID::FORMAT,
												 SPSAPIVID::IMGSIZE,
												 SPSAPIVID::IMGSIZE,
												 SPSAPITYPE::MODE,
												 SPSAPITYPE::TIME,
												 SPSAPITYPE::TIME,
												 SPSAPITYPE::QUALITY) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetFileInfo( SPSAPIVID::FILENAME,
												 IFileInfo* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetFileInfo( SPSAPIVID::FILENAME,
												 SPSAPITYPE::TYPE,
												 SPSAPITYPE::CHARPTR ) = 0;

	virtual SPSAPITYPE::ERRVALUE	DetectAlteration ( SPSAPITYPE::COMMAND,
													   SPSAPIVID::FILENAME ) = 0;

	virtual SPSAPITYPE::ERRVALUE	DistortionAdjust( SPSAPIVID::IMGPOS,
													  SPSAPIVID::IMGPOS ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetCorrectDistortionParam( SPSAPIVID::ZOOMR,
															   SPSAPIVID::FOCUSLEN ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetFileInfoEx( SPSAPIVID::FILENAME,
												   SPSAPITYPE::ID,
												   SPSAPITYPE::CHARPTR,
												   SPSAPITYPE::COUNT ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetFCropSize( SPSAPIVID::IMGPOS,
												  SPSAPIVID::IMGPOS,
												  SPSAPIVID::IMGPOS,
												  SPSAPIVID::IMGPOS ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetFCropSize( SPSAPIVID::IMGPOS&,
												  SPSAPIVID::IMGPOS&,
												  SPSAPIVID::IMGPOS&,
												  SPSAPIVID::IMGPOS& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetFCropPos( SPSAPITYPE::ID,
												 SPSAPIVID::IMGPOS,
												 SPSAPIVID::IMGPOS,
												 SPSAPIVID::IMGPOS,
												 SPSAPIVID::IMGPOS ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetFCropPos( SPSAPITYPE::ID,
												 SPSAPIVID::IMGPOS&,
												 SPSAPIVID::IMGPOS&,
												 SPSAPIVID::IMGPOS&,
												 SPSAPIVID::IMGPOS& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetFCropZoom( SPSAPITYPE::ID,
												  SPSAPIOP::ZOOM ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetFCropZoom( SPSAPITYPE::ID,
												  SPSAPIOP::ZOOM& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetFPanoramaAngle( SPSAPITYPE::ANGLE,
													   SPSAPITYPE::ANGLE,
													   SPSAPITYPE::ANGLE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetFPanoramaAngle( SPSAPITYPE::ANGLE&,
													   SPSAPITYPE::ANGLE&,
													   SPSAPITYPE::ANGLE& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetFWPanoramaAngle( SPSAPITYPE::ANGLE,
														SPSAPITYPE::ANGLE,
														SPSAPITYPE::ANGLE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetFWPanoramaAngle( SPSAPITYPE::ANGLE&,
														SPSAPITYPE::ANGLE&,
														SPSAPITYPE::ANGLE& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	CamSnapShot( SPSAPITYPE::CHANNEL,
												 SPSAPITYPE::MODE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetFCropForcedParam( SPSAPIVID::INST,
														 SPSAPIVID::CAPTURE,
														 SPSAPIVID::FOCUSLEN,
														 SPSAPIVID::IMGPOS,
														 SPSAPIVID::IMGPOS,
														 SPSAPIVID::IMGPOS,
														 SPSAPIVID::IMGPOS ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetFCropForcedParam( SPSAPIVID::INST&,
														 SPSAPIVID::CAPTURE&,
														 SPSAPIVID::FOCUSLEN&,
														 SPSAPIVID::IMGPOS&,
														 SPSAPIVID::IMGPOS&,
														 SPSAPIVID::IMGPOS&,
														 SPSAPIVID::IMGPOS& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetCameraImageCap( SPSAPITYPE::MODE,
													   SPSAPIVID::INST ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SnapShotLive( SPSAPITYPE::CHANNEL,
												  SPSAPITYPE::MODE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SnapShotPlay( SPSAPITYPE::CHANNEL,
												  SPSAPITYPE::TIMEDATE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SnapShotFile( SPSAPIVID::FILENAME,
												  SPSAPITYPE::TIMEDATE ) = 0;

	virtual SPSAPITYPE::STYLE		GetFisheyeImageStyle( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetH264DecMaxRate ( SPSAPITYPE::ID,
														SPSAPIVID::RESOLUTION,
														SPSAPIVID::FRAMERATE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetConvertClipStart( SPSAPITYPE::TIMEDATE,
														 SPSAPITYPE::TIMEZONE,
														 SPSAPITYPE::ISDST) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetConvertClipStart( SPSAPITYPE::CHARPTR,
														 SPSAPITYPE::COUNT,
														 SPSAPITYPE::TIMEZONE&,
														 SPSAPITYPE::ISDST&) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetConvertClipEnd( SPSAPITYPE::TIMEDATE,
													   SPSAPITYPE::TIMEZONE,
													   SPSAPITYPE::ISDST) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetConvertClipEnd( SPSAPITYPE::CHARPTR,
													   SPSAPITYPE::COUNT,
													   SPSAPITYPE::TIMEZONE&,
													   SPSAPITYPE::ISDST&) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetPCColor ( SPSAPITYPE::ID,
												 SPSAPITYPE::COLOR,
												 SPSAPITYPE::COLOR ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetPCColor ( SPSAPITYPE::ID,
												 SPSAPITYPE::COLOR&,
												 SPSAPITYPE::COLOR& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetPCVisibleEnable ( SPSAPITYPE::ID,
														 SPSAPITYPE::MODE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetPCVisibleEnable ( SPSAPITYPE::ID,
														 SPSAPITYPE::MODE& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetHMMask ( SPSAPITYPE::CHARPTR ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetAnalyticsSrcFiles ( SPSAPITYPE::MODE,
														   SPSAPIVID::FILENAME,
														   SPSAPITYPE::TIMEDATE,
														   SPSAPITYPE::TIMEZONE,
														   SPSAPITYPE::ISDST,
														   SPSAPITYPE::TIME ) = 0;

	virtual SPSAPITYPE::ERRVALUE	AddAnalyticsTargetFile ( SPSAPITYPE::MODE,
															 SPSAPIVID::FILENAME ) = 0;

	virtual SPSAPITYPE::ERRVALUE	ClearAnalyticsTargetFile ( SPSAPITYPE::MODE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	DrawAnalyticsImage ( SPSAPITYPE::MODE,
														 SPSAPIVID::FILENAME ) = 0;

	virtual SPSAPITYPE::ERRVALUE	PlayFileByTime( SPSAPIVID::FILENAME,
													SPSAPITYPE::TIMEDATE,
													SPSAPITYPE::TIME,
													SPSAPITYPE::ISDST,
													SPSAPITYPE::STATUS&,
													SPSAPITYPE::MODE,
													IAppCallBack* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetAngleFromPos( SPSAPIVID::IMGPOS,
													 SPSAPIVID::IMGPOS,
													 SPSAPITYPE::TYPE,
													 SPSAPITYPE::ANGLE&,
													 SPSAPITYPE::ANGLE& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetPosFromAngle( SPSAPITYPE::ANGLE,
													 SPSAPITYPE::ANGLE,
													 SPSAPITYPE::TYPE,
													 SPSAPIVID::IMGPOS&,
													 SPSAPIVID::IMGPOS& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetAudioExtractionAngle( SPSAPITYPE::ID,
															 SPSAPITYPE::ANGLE,
															 SPSAPITYPE::ANGLE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetAudioExtractionAngle( SPSAPITYPE::ID,
															 SPSAPITYPE::ANGLE&,
															 SPSAPITYPE::ANGLE& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	QueryFunction ( SPSAPITYPE::TYPE,
													SPSAPITYPE::STATUS& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SnapShotEx( SPSAPITYPE::MODE,
												IPSAPIPicture* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetVESnowParameter( SPSAPITYPE::MODE,
														SPSAPITYPE::LEVEL ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetVESnowParameter( SPSAPITYPE::MODE&,
														SPSAPITYPE::LEVEL& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetVEDtccParameter( SPSAPITYPE::MODE,
														SPSAPITYPE::LEVEL,
														SPSAPITYPE::BRIGHTNESS,
														SPSAPITYPE::LEVEL,
														SPSAPITYPE::SIZE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetVEDtccParameter( SPSAPITYPE::MODE&,
														SPSAPITYPE::LEVEL&,
														SPSAPITYPE::BRIGHTNESS&,
														SPSAPITYPE::LEVEL&,
														SPSAPITYPE::SIZE& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetVEFrmsynParameter( SPSAPITYPE::TYPE,
														  SPSAPITYPE::PRECISION,
														  SPSAPITYPE::RANGE,
														  SPSAPITYPE::MODE,
														  SPSAPITYPE::LEVEL,
														  SPSAPITYPE::LEVEL ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetVEFrmsynParameter( SPSAPITYPE::TYPE&,
														  SPSAPITYPE::PRECISION&,
														  SPSAPITYPE::RANGE&,
														  SPSAPITYPE::MODE&,
														  SPSAPITYPE::LEVEL&,
														  SPSAPITYPE::LEVEL& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	ResetVEParameter( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	VEStart( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	VEStop( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetVEMaskArea( SPSAPITYPE::ID,
												   SPSAPITYPE::POS,
												   SPSAPITYPE::POS,
												   SPSAPITYPE::RESOLUTION,
												   SPSAPITYPE::RESOLUTION,
												   SPSAPITYPE::RESOLUTION,
												   SPSAPITYPE::RESOLUTION ) = 0;

	virtual SPSAPITYPE::ERRVALUE	DeleteVEMaskArea( SPSAPITYPE::ID ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetVEMaskArea( SPSAPITYPE::ID,
												   SPSAPITYPE::POS&,
												   SPSAPITYPE::POS&,
												   SPSAPITYPE::RESOLUTION&,
												   SPSAPITYPE::RESOLUTION&,
												   SPSAPITYPE::RESOLUTION&,
												   SPSAPITYPE::RESOLUTION& ) = 0;

	virtual SPSAPITYPE::STATUS	GetVEStatus( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetAnalyticsSrcFilesByHours ( SPSAPITYPE::MODE,
																  SPSAPIVID::FILENAME,
																  SPSAPITYPE::TIMEDATE,
																  SPSAPITYPE::TIMEZONE,
																  SPSAPITYPE::ISDST,
																  SPSAPITYPE::TIME ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetCroppingRect( SPSAPITYPE::ID,
													 SPSAPITYPE::POS,
													 SPSAPITYPE::POS,
													 SPSAPITYPE::POS,
													 SPSAPITYPE::POS ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetCroppingRect( SPSAPITYPE::ID,
													 SPSAPITYPE::POS&,
													 SPSAPITYPE::POS&,
													 SPSAPITYPE::POS&,
													 SPSAPITYPE::POS& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetCroppingDrawRect( SPSAPITYPE::ID,
														 SPSAPITYPE::POS,
														 SPSAPITYPE::POS,
														 SPSAPITYPE::POS,
														 SPSAPITYPE::POS ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetCroppingDrawRect( SPSAPITYPE::ID,
														 SPSAPITYPE::POS&,
														 SPSAPITYPE::POS&,
														 SPSAPITYPE::POS&,
														 SPSAPITYPE::POS& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetCroppingDrawEnabled( SPSAPITYPE::ID,
															SPSAPITYPE::MODE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetCroppingDrawEnabled( SPSAPITYPE::ID,
															SPSAPITYPE::MODE& ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetCroppingMarker( SPSAPITYPE::ID,
													   SPSAPITYPE::MODE,
													   SPSAPITYPE::POS,
													   SPSAPITYPE::POS,
													   SPSAPITYPE::POS,
													   SPSAPITYPE::POS,
													   SPSAPITYPE::SIZE,
													   SPSAPITYPE::COLOR,
													   SPSAPITYPE::SIZE,
													   SPSAPITYPE::COLOR ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetCroppingMarker( SPSAPITYPE::ID,
													   SPSAPITYPE::MODE&,
													   SPSAPITYPE::POS&,
													   SPSAPITYPE::POS&,
													   SPSAPITYPE::POS&,
													   SPSAPITYPE::POS&,
													   SPSAPITYPE::SIZE&,
													   SPSAPITYPE::COLOR&,
													   SPSAPITYPE::SIZE&,
													   SPSAPITYPE::COLOR& ) = 0;

	virtual void					Convert( void ) = 0;

	
	virtual SPSAPITYPE::ERRVALUE	RecStream(	SPSAPITYPE::CHANNEL,
												SPSAPITYPE::COMMAND,
												SPSAPIVID::FILENAME,
												SPSAPITYPE::TIME,
												SPSAPITYPE::SIZE ) = 0;
	
	virtual SPSAPITYPE::STATUS		GetRecStreamStatus ( void ) = 0;
	
	virtual SPSAPITYPE::ERRVALUE	SetRecStreamListener( IAppListener* ) = 0;
	
	virtual SPSAPITYPE::INDEX		QueryHevcDec( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetHevcHWAdapterInfo(	SPSAPITYPE::INDEX,
															SPSAPITYPE::INAME,
															SPSAPITYPE::SIZE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetHevcHWAdapter( SPSAPITYPE::INDEX ) = 0;

	virtual SPSAPITYPE::ERRVALUE	HttpMP4Download(	SPSAPITYPE::CHANNEL,
														SPSAPITYPE::COMMAND,
														SPSAPITYPE::TIMEDATE,
														SPSAPITYPE::TIMEDATE,
														SPSAPITYPE::MODE,
														SPSAPIVID::FILENAME ) = 0;

	virtual SPSAPITYPE::STATUS		GetMP4DownloadStatus ( void ) = 0;

	virtual SPSAPITYPE::RATE		GetMP4DownloadTransRate( void ) = 0;
	
	virtual SPSAPITYPE::ERRVALUE	SetMP4DownloadListener( IAppListener* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetHEVCDecMaxRate ( SPSAPITYPE::ID,
														SPSAPIVID::RESOLUTION,
														SPSAPIVID::FRAMERATE ) = 0;

public:
	virtual SPSAPITYPE::ERRVALUE	SetMPEG4Port( SPSAPITYPE::PORT ) = 0;
	virtual SPSAPITYPE::PORT		GetMPEG4Port( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetH264Port( SPSAPITYPE::PORT ) = 0;
	virtual SPSAPITYPE::PORT		GetH264Port( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetMulticastAddr( SPSAPITYPE::IPADDR ) = 0;
	virtual void					GetMulticastAddr( SPSAPITYPE::IPADDR,
													  SPSAPITYPE::COUNT ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetMPEG4Resolution( SPSAPIVID::RESOLUTION ) = 0;
	virtual SPSAPIVID::RESOLUTION	GetMPEG4Resolution( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetH264Resolution( SPSAPIVID::RESOLUTION ) = 0;
	virtual SPSAPIVID::RESOLUTION	GetH264Resolution( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetJPEGResolution( SPSAPIVID::RESOLUTION ) = 0;
	virtual SPSAPIVID::RESOLUTION	GetJPEGResolution( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetStreamFormat( SPSAPIVID::FORMAT ) = 0;
	virtual SPSAPIVID::FORMAT		GetStreamFormat( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetImageHeight( SPSAPIVID::IMGSIZE ) = 0;
	virtual SPSAPIVID::IMGSIZE		GetImageHeight( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetImageWidth( SPSAPIVID::IMGSIZE ) = 0;
	virtual SPSAPIVID::IMGSIZE		GetImageWidth( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetMulticastAutoConf( SPSAPITYPE::VALUE ) = 0;
	virtual SPSAPITYPE::VALUE		GetMulticastAutoConf( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetStreamNumber( SPSAPIVID::STRMNO ) = 0;
	virtual SPSAPIVID::STRMNO		GetStreamNumber( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetNXStreamNumber( SPSAPIVID::STRMNO ) = 0;
	virtual SPSAPIVID::STRMNO		GetNXStreamNumber( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetTransFrameRate( SPSAPIVID::FRAMERATE ) = 0;
	virtual SPSAPIVID::FRAMERATE 	GetTransFrameRate( void )  = 0;

	virtual SPSAPITYPE::ERRVALUE	SetTotalDecRate( SPSAPITYPE::RATE ) = 0;
	virtual SPSAPITYPE::RATE	 	GetTotalDecRate( void )  = 0;

	virtual SPSAPITYPE::ERRVALUE	SetDigitalZoom ( SPSAPIOP::ZOOM ) = 0;
	virtual SPSAPIOP::ZOOM			GetDigitalZoom( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetDigitalZoomMode( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetDigitalZoomMode ( void ) = 0;

	virtual	SPSAPITYPE::ERRVALUE	SetSkipRecordGap( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetSkipRecordGap( void ) = 0;

	virtual	SPSAPITYPE::ERRVALUE	SetMultiScreenChannel( SPSAPITYPE::CHARPTR ) = 0;
	virtual void					GetMultiScreenChannel( SPSAPITYPE::CHARPTR,
														   SPSAPITYPE::COUNT ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetBackColor ( SPSAPITYPE::COLOR ) = 0;
	virtual SPSAPITYPE::COLOR		GetBackColor ( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetPictureFitMode( SPSAPITYPE::VIEWTYPE ) = 0;
	virtual SPSAPITYPE::VIEWTYPE	GetPictureFitMode( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetDecResolutionMode( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetDecResolutionMode( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetFilePassword( SPSAPITYPE::PSSWD ) = 0;
	virtual void					GetFilePassword( SPSAPITYPE::PSSWD,
													 SPSAPITYPE::COUNT ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetCorrectDistortionMode( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetCorrectDistortionMode( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetStabilizationMode( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetStabilizationMode( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetRollingDegree( SPSAPITYPE::DEGREE ) = 0;
	virtual SPSAPITYPE::DEGREE		GetRollingDegree( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetInternetMode( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetInternetMode( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetFastPlayMode( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetFastPlayMode( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetRtpPortMode( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetRtpPortMode( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetRtpPortRange( SPSAPITYPE::RANGE ) = 0;
	virtual SPSAPITYPE::RANGE		GetRtpPortRange( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetSIDMode( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetSIDMode( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetFCropStyle( SPSAPITYPE::ID ) = 0;
	virtual SPSAPITYPE::ID			GetFCropStyle( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetTransIntervalMode( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetTransIntervalMode( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetDecBufferNum( SPSAPITYPE::COUNT ) = 0;
	virtual SPSAPITYPE::COUNT		GetDecBufferNum( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetFCropForcedMode( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetFCropForcedMode( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetPlayBufferInterval( SPSAPITYPE::TIME ) = 0;
	virtual SPSAPITYPE::TIME		GetPlayBufferInterval( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetH264DecType( SPSAPITYPE::TYPE ) = 0;
	virtual SPSAPITYPE::TYPE		GetH264DecType( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetDispMode( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetDispMode( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetH264ColorRangeMode( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetH264ColorRangeMode( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetResourceReductionMode( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetResourceReductionMode( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetHMRange ( SPSAPITYPE::RANGE ) = 0;
	virtual SPSAPITYPE::RANGE		GetHMRange( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetRcvAudioDec( SPSAPIVID::FORMAT ) = 0;
	virtual SPSAPIVID::FORMAT		GetRcvAudioDec( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetMCDAPort( SPSAPITYPE::PORT ) = 0;
	virtual SPSAPITYPE::PORT		GetMCDAPort( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetMicMulticastAddr( SPSAPITYPE::IPADDR ) = 0;
	virtual void					GetMicMulticastAddr( SPSAPITYPE::IPADDR,
														 SPSAPITYPE::COUNT ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetMicAVTimeAlignment( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetMicAVTimeAlignment( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetOutputGainCoef( SPSAPITYPE::GAIN ) = 0;
	virtual SPSAPITYPE::GAIN		GetOutputGainCoef( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetVoiceClearMode( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetVoiceClearMode( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetVoiceClearFrequency( SPSAPITYPE::FREQUENCY ) = 0;
	virtual SPSAPITYPE::FREQUENCY	GetVoiceClearFrequency( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetNoiseReductionMode( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetNoiseReductionMode( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetAudioExtractGainCoef( SPSAPITYPE::GAIN ) = 0;
	virtual SPSAPITYPE::GAIN		GetAudioExtractGainCoef( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetVESnowEnable( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetVESnowEnable( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetVEDtccEnable( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetVEDtccEnable( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetVEFrmsynEnable( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetVEFrmsynEnable( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetVEDecMaxRate( SPSAPITYPE::RATE ) = 0;
	virtual SPSAPITYPE::RATE		GetVEDecMaxRate( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetVEProcMode( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetVEProcMode( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetCroppingEnabled( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetCroppingEnabled( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetMaxDrawRate( SPSAPITYPE::RATE ) = 0;
	virtual SPSAPITYPE::RATE		GetMaxDrawRate( void ) = 0;
	
	virtual SPSAPITYPE::ERRVALUE	SetHEVCDecType( SPSAPITYPE::TYPE ) = 0;
	virtual SPSAPITYPE::TYPE		GetHEVCDecType( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetFCropPosAdjustType( SPSAPITYPE::TYPE ) = 0;
	virtual SPSAPITYPE::TYPE		GetFCropPosAdjustType( void ) = 0;

public:
	virtual SPSAPITYPE::ERRVALUE	SetPlayListener( IAppListener* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetRecordListener ( IAppListener* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetImageListener( IAppListener*,
													  SPSAPIVID::IMGTYPE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetAudioListener( IAppListener*,
													  SPSAPIVID::IMGTYPE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetSaveFileListener ( IAppListener* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetConvertFileListener ( IAppListener*,
															 SPSAPITYPE::TIME ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetDetectAlterationListener ( IAppListener* ) = 0;

public:	// なめらかディワープ対応
	virtual SPSAPITYPE::ERRVALUE	SetJoystickFCropPos ( SPSAPIVIDSMOOTH::XPOS,
												SPSAPIVIDSMOOTH::YPOS,
												SPSAPIVIDSMOOTH::IMAGEWIDTH,
												SPSAPIVIDSMOOTH::IMAGEHEIGHT) = 0;

	virtual SPSAPITYPE::ERRVALUE	JoystickFCropPTZ ( SPSAPIVIDSMOOTH::XPOS,
												SPSAPIVIDSMOOTH::YPOS,
												SPSAPIVIDSMOOTH::LZOOM) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetFCropMovePos ( SPSAPIVIDSMOOTH::STYLE&,
												SPSAPIVIDSMOOTH::FISHEYEROTATE&,
												SPSAPIVIDSMOOTH::XPOS&,
												SPSAPIVIDSMOOTH::YPOS&,
												SPSAPIVIDSMOOTH::ZOOM,
												SPSAPIVIDSMOOTH::ZOOMSIZE,
												SPSAPIVIDSMOOTH::SEAMLESSZOOM,
												SPSAPIVIDSMOOTH::ZOOMSIZE,
												SPSAPIVIDSMOOTH::IMAGEWIDTH&,
												SPSAPIVIDSMOOTH::IMAGEHEIGHT&) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetFCropMovePos ( SPSAPIVIDSMOOTH::STYLE,
												SPSAPIVIDSMOOTH::FISHEYEROTATE,
												SPSAPIVIDSMOOTH::XPOS,
												SPSAPIVIDSMOOTH::YPOS,
												SPSAPIVIDSMOOTH::ZOOM,
												SPSAPIVIDSMOOTH::SEAMLESSZOOM,
												SPSAPIVIDSMOOTH::IMAGEWIDTH,
												SPSAPIVIDSMOOTH::IMAGEHEIGHT) = 0;

	virtual SPSAPITYPE::ERRVALUE		SetFCropMoveMode( SPSAPIVIDSMOOTH::MODE mode ) = 0;
	virtual SPSAPIVIDSMOOTH::MODE		GetFCropMoveMode( void ) = 0;
	virtual SPSAPITYPE::ERRVALUE		SetFCropReDrawInterval( SPSAPIVIDSMOOTH::INTERVAL interval ) = 0;
	virtual SPSAPIVIDSMOOTH::INTERVAL	GetFCropReDrawInterval( void ) = 0;
	virtual SPSAPITYPE::ERRVALUE		SetFocalLengthAdjust( SPSAPIVIDSMOOTH::MODE mode ) = 0;
	virtual SPSAPIVIDSMOOTH::MODE		GetFocalLengthAdjust( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE		SetFigureDrawEnable ( SPSAPITYPE::MODE mode ) = 0;
	virtual SPSAPITYPE::MODE			GetFigureDrawEnable ( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE		SetFigureDrawType ( SPSAPITYPE::TYPE type) = 0;
	virtual SPSAPITYPE::TYPE			GetFigureDrawType ( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE		DeleteFigureDraw ( SPSAPITYPE::ID id ) = 0;
	virtual SPSAPITYPE::ERRVALUE		SetFigureDrawImagePos ( SPSAPITYPE::ID id, SPSAPITYPE::CHARPTR pStr) = 0;
	virtual void						GetFigureDrawImagePos ( SPSAPITYPE::ID id, SPSAPITYPE::CHARPTR pStr, SPSAPITYPE::COUNT cnt) = 0;

	virtual SPSAPITYPE::ERRVALUE		SetVideoRcvBitRateEnable ( SPSAPITYPE::MODE mode ) = 0;
	virtual SPSAPITYPE::MODE			GetVideoRcvBitRateEnable ( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE		SetFigureAreaIDListener( IAppListener*) = 0;

	virtual SPSAPITYPE::ERRVALUE		SetAreaID( SPSAPITYPE::ID id, SPSAPITYPE::LINETYPE lineType ) = 0;
	virtual SPSAPITYPE::ERRVALUE		SetAudioDetectListener( IAppListener* ) = 0;

	virtual SPSAPITYPE::ERRVALUE		SetFigureMaxAreaID( SPSAPITYPE::ID id ) = 0;
	virtual SPSAPITYPE::ID				GetFigureMaxAreaID( void ) = 0;
	virtual SPSAPITYPE::ERRVALUE		SetAreaLineColor( SPSAPITYPE::ID id, SPSAPITYPE::COLOR lineColor ) = 0;

	virtual SPSAPITYPE::ERRVALUE	HttpDownload(	SPSAPITYPE::CHANNEL,
													SPSAPITYPE::COMMAND,
													SPSAPITYPE::TIMEDATE,
													SPSAPITYPE::ISDST,
													SPSAPITYPE::TIMEDATE,
													SPSAPITYPE::ISDST,
													SPSAPITYPE::TYPE,
													SPSAPIVID::FILENAME ) = 0;

	virtual	SPSAPITYPE::ERRVALUE	CircleFrameDraw ( SPSAPITYPE::RADIUS, SPSAPITYPE::RADIUS, SPSAPITYPE::TYPE, SPSAPITYPE::DIRECTION ) = 0;
	virtual	SPSAPITYPE::ERRVALUE	SetLineDirectionChg ( SPSAPITYPE::ID, SPSAPITYPE::TYPE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetRectZoomMode ( SPSAPITYPE::MODE mode ) = 0;
	virtual SPSAPITYPE::MODE		GetRectZoomMode ( void ) = 0;

//---------------------------------------------------------------------
// Audio Interface
//---------------------------------------------------------------------
public:
	virtual SPSAPITYPE::ERRVALUE	AudioSend ( SPSAPITYPE::COMMAND ) = 0;

	virtual SPSAPITYPE::STATUS		GetAudioSendStatus( void ) = 0;

public:
	virtual SPSAPITYPE::ERRVALUE	SetAudioRcvEnable( SPSAPITYPE::MODE ) = 0;
	virtual SPSAPITYPE::MODE		GetAudioRcvEnable( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetAudioRcvVolume ( SPSAPIAUDIO::VOLUME ) = 0;
	virtual SPSAPIAUDIO::VOLUME		GetAudioRcvVolume( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetAudioRcvMute ( SPSAPIAUDIO::MUTE ) = 0;
	virtual SPSAPIAUDIO::MUTE		GetAudioRcvMute( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetAudioSendVolume ( SPSAPIAUDIO::VOLUME ) = 0;
	virtual SPSAPIAUDIO::VOLUME		GetAudioSendVolume( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetAudioSendMute ( SPSAPIAUDIO::MUTE ) = 0;
	virtual SPSAPIAUDIO::MUTE		GetAudioSendMute( void ) = 0;


//---------------------------------------------------------------------
// Operation Interface
//---------------------------------------------------------------------
public:
	virtual SPSAPITYPE::ERRVALUE	CameraControl( SPSAPITYPE::CHANNEL,
												   SPSAPIOP::PAN,
												   SPSAPIOP::TILT,
												   SPSAPIOP::ZOOM,
												   SPSAPIOP::FOCUS,
												   SPSAPIOP::IRIS ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetCameraPosition( SPSAPITYPE::CHANNEL,
													   SPSAPIOP::PAN,
													   SPSAPIOP::TILT,
													   SPSAPIOP::ZOOM,
													   SPSAPIOP::FOCUS ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetCameraPosition( SPSAPITYPE::CHANNEL,
													   SPSAPIOP::PAN&,
													   SPSAPIOP::TILT&,
													   SPSAPIOP::ZOOM&,
													   SPSAPIOP::FOCUS& ) = 0;

	virtual SPSAPITYPE::ERRVALUE 	CameraOperation( SPSAPITYPE::CHANNEL,
													 SPSAPITYPE::COMMAND,
													 SPSAPITYPE::DATA,
													 SPSAPITYPE::STATUS&,
													 SPSAPITYPE::MODE,
													 IAppCallBack* ) = 0;

	virtual SPSAPITYPE::STATUS		GetCamOpStatus( SPSAPITYPE::CHANNEL ) = 0;

	virtual SPSAPITYPE::ERRVALUE	CameraCentering( SPSAPITYPE::CHANNEL,
													 SPSAPIVID::IMGPOS,
													 SPSAPIVID::IMGPOS,
													 SPSAPIVID::IMGSIZE,
													 SPSAPIVID::IMGSIZE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	CameraAuxControl ( SPSAPITYPE::CHANNEL,
													   SPSAPIALM::TERMINAL,
													   SPSAPITYPE::COMMAND ) = 0;

	virtual SPSAPITYPE::STATUS		GetCameraAuxStatus( SPSAPITYPE::CHANNEL,
														SPSAPIALM::TERMINAL ) = 0;

	virtual SPSAPITYPE::ERRVALUE	CameraLEDControl( SPSAPITYPE::COMMAND ) = 0;

	virtual SPSAPITYPE::ERRVALUE	CameraShutterControl( SPSAPITYPE::COMMAND ) = 0;

	virtual SPSAPITYPE::ERRVALUE	PortControl( SPSAPITYPE::PORT,
   												 SPSAPITYPE::COMMAND,
												 SPSAPITYPE::STATUS&,
												 SPSAPITYPE::MODE,
												 IAppCallBack* ) = 0;

	virtual SPSAPITYPE::STATUS		GetPortStatus( void  ) = 0;

	virtual SPSAPITYPE::STATUS		CameraWiperControl( SPSAPITYPE::CHANNEL,
														SPSAPITYPE::COMMAND ) = 0;

public:
	virtual SPSAPITYPE::ERRVALUE	SetOpListener( IAppListener* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetPortListener( IAppListener* ) = 0;


//---------------------------------------------------------------------
// Alarm Interface
//---------------------------------------------------------------------
public:
	virtual SPSAPITYPE::ERRVALUE 	AlmOperation ( SPSAPITYPE::CHANNEL,
												   SPSAPITYPE::COMMAND,
												   SPSAPITYPE::STATUS&,
												   SPSAPITYPE::MODE,
												   IAppCallBack* ) = 0;

	virtual SPSAPITYPE::STATUS		GetAlarmStatus( void ) = 0;

public:
	virtual SPSAPITYPE::ERRVALUE	SetAlmListener( IAppListener* ) = 0;


//---------------------------------------------------------------------
// FTP Interface
//---------------------------------------------------------------------
public:
	virtual SPSAPITYPE::ERRVALUE	FtpGet( SPSAPITYPE::CHANNEL,
											SPSAPITYPE::TIMEDATE,
											SPSAPITYPE::TIMEDATE,
											SPSAPITYPE::DATA,
											SPSAPIDEV::EVNTTYPE,
											SPSAPIVID::FILENAME,
											SPSAPITYPE::STATUS&,
											SPSAPITYPE::MODE,
											IAppCallBack* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	FtpGetFileList( SPSAPITYPE::TYPE,
													IFtpFileList* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	FtpGetCam( SPSAPIVID::FILENAME,
											   SPSAPIVID::FILENAME,
											   SPSAPITYPE::STATUS&,
											   SPSAPITYPE::MODE,
											   IAppCallBack* ) = 0;

	virtual SPSAPITYPE::ERRVALUE	FtpDeleteCamFile ( SPSAPIVID::FILENAME ) = 0;

	virtual SPSAPITYPE::ERRVALUE	FtpDeleteCam ( SPSAPITYPE::TYPE ) = 0;

	virtual SPSAPITYPE::ERRVALUE	SetFtpPermissionCam ( SPSAPITYPE::COMMAND ) = 0;

	virtual SPSAPITYPE::ERRVALUE	FtpCancel( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	FtpServerClose( void ) = 0;

	virtual	SPSAPITYPE::STATUS		GetFtpStatus( void ) = 0;

	virtual	SPSAPITYPE::RATE		GetFtpTransRate( void ) = 0;

	virtual	SPSAPITYPE::COUNT		GetFtpTransByte( void ) = 0;

public:
	virtual	SPSAPITYPE::ERRVALUE	SetFtpPort( SPSAPITYPE::PORT ) = 0;
	virtual	SPSAPITYPE::PORT		GetFtpPort( void ) = 0;

	virtual	SPSAPITYPE::ERRVALUE	SetFtpTransMode( SPSAPITYPE::MODE ) = 0;
	virtual	SPSAPITYPE::MODE		GetFtpTransMode( void ) = 0;

};

/*----------------------------------------------------------------------------------------
 |  Extern Function to handle IPSAPI
 +--------------------------------------------------------------------------------------*/
extern "C" PSAPIDECLSPEC IPSAPI* GetIPSAPI(	void	);
extern "C" PSAPIDECLSPEC void DeleteIPSAPI(	IPSAPI*	);


#endif


