#ifndef _IAPPLISTENER_
#define _IAPPLISTENER_
/******************************************************************************************
 *  INTERFACE MODULE	PS API APPLICATION LISTENER INTERFACE
 ******************************************************************************************
 *
 *	UNIT:			PS API Application Listener Interface
 *
 *	FILE NAME:		iapplistener.h
 *
 *	DESCRIPTION:	PS API Application Listener Interface
 *
 *	INCLUDES:
 */


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
 *	S.Ramaswamy 09/25/08	Created
 *	S.Ramaswamy 10/26/09	Changed all IAppListener functions from pure virtual to virtual
 *	PSN         04/05/10	Add OnSaveFilea()
 *	PSN         07/15/10	ChgNo.060 - Added isDst flag to OnRecordStatus().
 *	PSN         09/30/10	Add OnConvertFilea()
 *	PSN         09/01/11	Add OnAudio()
 *	PSN         11/18/11	Add OnDetectAlteration()
 *	PSN         04/22/16	Add OnRecStreamStatus()
 *
 ******************************************************************************************
 *  COPYRIGHT:		Panasonic System Networks Co., Ltd. 2008 - 2011
 *****************************************************************************************/

/*----------------------------------------------------------------------------------------
 |  PS API Application Listener Interface Class
 +--------------------------------------------------------------------------------------*/
class IAppListener
{
public:
	virtual ~IAppListener()	{};

public:
	virtual void OnError( 	SPSAPITYPE::ERRVALUE,
							SPSAPITYPE::CDESC		){};

	virtual void OnDevStatus( 	SPSAPITYPE::CHANNEL,
								SPSAPITYPE::STATUS	){};

	virtual void OnRecStatus( 	SPSAPITYPE::CHANNEL,
								SPSAPITYPE::STATE	){};

	virtual void OnRecordStatus (	SPSAPITYPE::TYPE, 
									SPSAPITYPE::CTIMEDATE,
									SPSAPITYPE::ISDST,
									SPSAPITYPE::CTIMEDATE,
									SPSAPITYPE::ISDST){};

	virtual void OnAlmStatus( 	SPSAPITYPE::CHANNEL,
								SPSAPIALM::ALMTYPE,
								SPSAPITYPE::CTIMEDATE,
								SPSAPITYPE::STATUS		){};

	virtual void OnPortStatus( 	SPSAPITYPE::PORT,
								SPSAPITYPE::STATUS	){};

	virtual void OnPlayStatus( 	SPSAPITYPE::CHANNEL,
								SPSAPITYPE::STATUS	){};

	virtual void OnOpStatus( 	SPSAPITYPE::CHANNEL,
								SPSAPITYPE::STATUS	){};

	virtual void OnImage( 	SPSAPIVID::IMGTYPE,
							SPSAPIVID::IMGBUFFER*,
							SPSAPIVID::IMGSIZE		){};

	virtual void OnAudio( 	SPSAPIVID::IMGTYPE,
							SPSAPIVID::IMGBUFFER*,
							SPSAPIVID::IMGSIZE		){};

	virtual void	OnSaveFile(	SPSAPITYPE::TYPE,
								SPSAPITYPE::CFILENAME	){};
	
	virtual void	OnConvertFile(	SPSAPITYPE::NUMBER,
									SPSAPITYPE::PROGRESS	){};

	virtual void	OnDetectAlteration(	SPSAPIVID::RESULT,
										SPSAPITYPE::CFILENAME	){};
	
	virtual void	OnRecStreamStatus(	SPSAPITYPE::STATUS,
										SPSAPITYPE::CTIMEDATE,
										SPSAPITYPE::TIME,
										SPSAPITYPE::CFILENAME	){};
	
	virtual void	OnMP4DownloadStatus(	SPSAPITYPE::STATUS,
											SPSAPITYPE::CFILENAME	){};

	virtual void	OnFigureAreaID( SPSAPITYPE::ID, SPSAPITYPE::TYPE ){};

	virtual void	OnAudioDetect( SPSAPITYPE::LEVEL ){};
};

#endif

