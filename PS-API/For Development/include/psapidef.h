#ifndef _PSAPIDEF_
#define _PSAPIDEF_
/******************************************************************************************
 *  INTERFACE MODULE	PS API DLL
 ******************************************************************************************
 *
 *	UNIT:			PS API DLL Definitions
 *
 *	FILE NAME:		psapidef.h
 *
 *	DESCRIPTION:	PS API DLL Definitions
 *
 *	INCLUDES:
 */

					#include	<windows.h>

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
 *	S.Ramaswamy 09/24/08	Created
 *	S.Ramaswamy 12/22/08	Added Search Type definition struct and SSEARCHRSLT_INFO struct
 *	S.Ramaswamy 12/22/08	Added PSAPIDECLSPEC defintion from ipsapi.h
 *	S.Ramaswamy 05/29/09	Increased MAXSIZE_SRCHRECKIND to 8
 *	S.Ramaswamy 08/24/09	Added SPSAPITYPE::RATE, SPSAPIVID::STRMNO, SDST_INFO,
 *							STIMEZONE_INFO and SSEARCHRSLT_INFO_EX.
 *	PSN         04/05/10	Added CFILENAME
 *	PSN         04/26/10	Added SPLAYCTRL_CMD value.
 *	PSN         07/15/10	ChgNo.060 - Added SPSAPITYPE::ISDST value. 
 *	PSN         08/31/10	Added VIEWTYPE. 
 *	PSN         09/30/10	Added QUALITY, NUMBER, PROGRESS and SFILELIST. 
 *	PSN         09/30/10	Added MAXSIZE_FILEPWD
 *	PSN         04/22/11	Added SFTPFILELIST_INFO, SFILE_INFO, SEVENT_INFO.
 *                          Added MAXSIZE_INFO, SPSAPITYPE::PERSONID, SPSAPIVID::ZOOMR,
 *                                SPSAPIVID::FOCUSLEN.
 *	PSN         04/22/11	Added SPSAPIFILEINFO, SPSAPIFTPLIST.
 *	PSN         05/25/11	Added SPSAPIVID::RESULT.
 *	PSN         07/14/11	Added SPSAPITYPE::DEGREE.
 *	PSN         07/14/11	Added serial number if to SFILE_INFO.
 *	PSN         09/01/11	Added SPSAPITYPE::TRNSMISSIV, SPSAPITYPE::RANGE.
 *	PSN         02/01/12	Added SUID_INFO and SSID_INFO.
 *	PSN         10/08/12	Added SPSAPITYPE::ANGLE
 *	PSN         01/16/13	Added SPSAPIVID::CAPTURE,SPSAPIVID::INST
 *	PSN         01/07/15	Added SPSAPITYPE::TIMEZONE
 *	PSN         03/22/15	Added SPSAPITYPE::FREQUENCY, GAIN
 *
 ******************************************************************************************
 *  COPYRIGHT:		Panasonic System Networks Co., Ltd. 2008 - 2015
 *****************************************************************************************/

/*----------------------------------------------------------------------------------------
 |  DLL Export Macros
 +--------------------------------------------------------------------------------------*/
#ifdef IPROPSAPILIB_EXPORT
	#define  PSAPIDECLSPEC __declspec(dllexport)
#else
	#define  PSAPIDECLSPEC __declspec(dllimport)
#endif

/*----------------------------------------------------------------------------------------
 |	Maximum count defintions
 +--------------------------------------------------------------------------------------*/
#define MAXSIZE_FRAMETIME		24		/* Frame time Character count					*/
#define MAXSIZE_TIME			20		/* Time Character count							*/
#define MAXSIZE_TIMEDATE		32		/* Time Date Character count					*/
#define MAXSIZE_SRCHRECKIND		8		/* Search Record Kind Character count			*/
#define MAXSIZE_DSTRANGE		10		/* DST Range									*/
#define MAXSIZE_LOGMSG			128		/* Log Message									*/
#define MAXSIZE_LOGPARAM		64		/* Log Parameters								*/
#define MAXSIZE_PROTOCOL		16		/* Protocol										*/
#define MAXSIZE_FILEPATH		256		/* N3R File name								*/
#define MAXSIZE_FILEPWD			16		/* N3R File password							*/
#define MAXSIZE_INFO			64		/* Profuct num, IP, ID, etc						*/
//2020/07/15 v11.8 add
#define MAXSIZE_KEY_IV			16		/* KEY and IV									*/

/*----------------------------------------------------------------------------------------
 |	Constant defintions
 +--------------------------------------------------------------------------------------*/
#define NULLCHAR				'\0'	/* Define Null Character						*/

/*----------------------------------------------------------------------------------------
 |	PSAPI General Type definitions
 +--------------------------------------------------------------------------------------*/
typedef struct SPSAPITYPE
{
	typedef		long	ERRVALUE;
	typedef		long	CHANNEL;
	typedef		long	STATUS;
	typedef		long	COMMAND;
	typedef		long	DATA;
	typedef		long	MODE;
	typedef		long	STATE;
	typedef		long	PORT;
	typedef		char*	IPADDR;
	typedef		char*	PROXY;
	typedef		char*	MODELPTR;
	typedef		char*	TIMEDATE;
	typedef		long	TIME;
	typedef		long	UID;
	typedef		long	ID;
	typedef		char*	UNAME;
	typedef		char*	PSSWD;
	typedef		char*	DESC;
	typedef		long	COUNT;
	typedef		long	VALUE;
	typedef		long	DEVTYPE;
	typedef		bool	BOOL;
	typedef		long	RATE;
	typedef		char*	CHARPTR;
	typedef		const char*	CTIMEDATE;
	typedef		const char*	CDESC;
	typedef		long	COLOR;
	typedef		long	STYLE;
	typedef		long	TYPE;
	typedef		char*	FOLDER;
	typedef		const char*	CFILENAME;
	typedef		long	ISDST;
	typedef		long	VIEWTYPE;
	typedef		long	QUALITY;
	typedef		long	NUMBER;
	typedef		long	PROGRESS;
	typedef		char*	PERSONID;
	typedef		long	DEGREE;
	typedef		long	TRNSMISSIV;
	typedef		long	RANGE;
	typedef		long	ANGLE;
	typedef		long	TIMEZONE;
	typedef		long	FREQUENCY;
	typedef		long	GAIN;
	typedef		long	LEVEL;
	typedef		long	BRIGHTNESS;
	typedef		long	SIZE;
	typedef		long	PRECISION;
	typedef		long	POS;
	typedef		long	RESOLUTION;
	typedef		long	INDEX;
	typedef		char*	INAME;
	typedef		char*	KEY;	//2020/07/15 v11.8
	typedef		char*	IV;		//2020/07/15 v11.8
	typedef		char*	IMGPOS;	//2020/10/25 v11.9
	typedef		long	LINETYPE;//2020/10/25 v11.9
	typedef		long	RADIUS;		//2021/09/01 v12.2
	typedef		long	DIRECTION;	//2021/09/01 v12.2

}SPSAPITYPE;

typedef struct SPSAPITEXT
{
	typedef		char*	FONT;
	typedef		long	ALIGN;
}SPSAPITEXT;

typedef struct SPSAPIVID
{
	typedef		char*	FILENAME;
	typedef		long	SPEED;
	typedef		long	FRAMERATE;
	typedef		HWND	VIDWINDOW;
	typedef		long	RESOLUTION;
	typedef		long	FORMAT;
	typedef		long	IMGSIZE;
	typedef		long	IMGTYPE;
	typedef		long	STRMNO;
	typedef		unsigned char	IMGBUFFER;
	typedef		long	IMGPOS;
	typedef		long	SENSITIVITY;
	typedef		long	ZOOMR;
	typedef		long	FOCUSLEN;
	typedef		long	RESULT;
	typedef		long	CAPTURE;
	typedef		long	INST;
}SPSAPIVID;

typedef struct SPSAPIOP
{
	typedef		long	PAN;
	typedef		long	TILT;
	typedef		long	ZOOM;
	typedef		long	FOCUS;
	typedef		long	IRIS;

}SPSAPIOP;

typedef struct SPSAPIALM
{
	typedef		long	ALMTYPE;
	typedef		long	TERMINAL;

}SPSAPIALM;

typedef struct SPSAPIDEV
{
	typedef		long	EVNTTYPE;

}SPSAPIDEV;

typedef struct SPSAPIPSB
{
	typedef		long	ACCESSTYPE;

}SPSAPIPSB;

typedef struct SPSAPISEARCH
{
	typedef		long	INDEX;
	typedef		long	INDEXCOUNT;

}SPSAPISEARCH;

typedef struct SPSAPIFILEINFO
{
	typedef		long	INDEX;
	typedef		long	INDEXCOUNT;

}SPSAPIFILEINFO;

typedef struct SPSAPIFTPLIST
{
	typedef		long	INDEX;
	typedef		long	INDEXCOUNT;

}SPSAPIFTPLIST;

typedef struct SPSAPIAUDIO
{
	typedef		long	VOLUME;
	typedef		long	MUTE;

}SPSAPIAUDIO;

/*----------------------------------------------------------------------------------------
 |  SMOOTH Structure definition	なめらかディワープ対応
 +--------------------------------------------------------------------------------------*/
typedef struct SPSAPIVIDSMOOTH
{
	typedef		long	XPOS;
	typedef		long	YPOS;
	typedef		long	LZOOM;
	typedef		char*	ZOOM;
	typedef		long	ZOOMSIZE;
	typedef		long	IMAGEWIDTH;
	typedef		long	IMAGEHEIGHT;
	typedef		long	STYLE;
	typedef		long	LSEAMLESSZOOM;
	typedef		char*	SEAMLESSZOOM;
	typedef		long	FISHEYEROTATE;
	typedef		long	MODE;
	typedef		long	RATE;
	typedef		long	INTERVAL;
}SPSAPIVIDSMOOTH;

/*----------------------------------------------------------------------------------------
 |  Play Control Command Type definition
 +--------------------------------------------------------------------------------------*/
typedef struct SPLAYCTRL_CMD
{
	typedef SPSAPITYPE::COMMAND CMD;
	enum{	PLAYBACK_STOP,
			PLAYLIVE_STOP,
			PLAYFILE_STOP,
			PLAY_PAUSE,
			PLAY_FWD,
			PLAY_REW,
			PLAY_NEXTFRAME,
			PLAY_PREVFRAME,
			PLAY_STEPUPFFWD,
			PLAY_STEPUPFREW,
			PLAY_NEXTRECORD,
			PLAY_PREVRECORD,
			PLAY_BYTIME
	};
}SPLAYCTRL_CMD;



/*----------------------------------------------------------------------------------------
 |  Video Frametime Structure definition
 +--------------------------------------------------------------------------------------*/
typedef struct SVIDEO_FRAMETIME 
{
	char	m_cLocaltime[MAXSIZE_FRAMETIME];		/* yyyy mm dd hh mm ss */
	long	m_lTimezone;
	long	m_lTimemode;							/* dst or not */

	//Constructor
	SVIDEO_FRAMETIME()
	{
		memset( &m_cLocaltime[0], NULLCHAR, MAXSIZE_FRAMETIME	);
		m_lTimezone	= 0;
		m_lTimemode	= 0;
	}

}SVIDEO_FRAMETIME;

/*----------------------------------------------------------------------------------------
 |  Video Channel Info Structure definition
 +--------------------------------------------------------------------------------------*/
typedef struct SVIDEOCHNL_INFO
{
	long		m_lChannel;
	char*		m_pcTimeDate;
	long		m_lDuration;
	long		m_lType;

	//Constructor
	SVIDEOCHNL_INFO()
	{
		m_lChannel	= 0;
		m_pcTimeDate	= 0;
		m_lDuration	= 0;
		m_lType		= 0;
	}

}SVIDEOCHNL_INFO;

/*----------------------------------------------------------------------------------------
 |  Time Zone Information
 +--------------------------------------------------------------------------------------*/
typedef struct SDST_INFO
{
	char		m_cInTime[MAXSIZE_TIME];
	char		m_cOutTime[MAXSIZE_TIME];

	//Constructor
	SDST_INFO()
	{
		memset( &m_cInTime[0], NULLCHAR, MAXSIZE_TIME	);
		memset( &m_cOutTime[0], NULLCHAR, MAXSIZE_TIME	);
	}

}SDST_INFO;

typedef struct STIMEZONE_INFO
{
	long		m_lTimeZone;
	long		m_IsDST;
	SDST_INFO	m_stDSTList[MAXSIZE_DSTRANGE];

	//Constructor
	STIMEZONE_INFO()
	{
		m_lTimeZone = 0;
		m_IsDST		= 0;
	}

}STIMEZONE_INFO;


/*----------------------------------------------------------------------------------------
 |  Search Result Info Structure definition
 +--------------------------------------------------------------------------------------*/
typedef struct SSEARCHRSLT_INFO
{
	long	m_lChannel;
	char	m_cStartTD[MAXSIZE_TIMEDATE];
	char	m_cEndTD[MAXSIZE_TIMEDATE];
	char	m_cRecKind[MAXSIZE_SRCHRECKIND];
	long	m_lAudio;

	//Constructor
	SSEARCHRSLT_INFO()
	{
		m_lChannel = 0;
		memset( &m_cStartTD[0], NULLCHAR, MAXSIZE_TIMEDATE	);
		memset( &m_cEndTD[0], NULLCHAR, MAXSIZE_TIMEDATE	);
		memset( &m_cRecKind[0], NULLCHAR, MAXSIZE_SRCHRECKIND	);
		m_lAudio = 0;
	}
		
}SSEARCHRSLT_INFO;

/*----------------------------------------------------------------------------------------
 |  Search Result Info Structure definition
 +--------------------------------------------------------------------------------------*/
typedef struct SSEARCHRSLT_INFO_EX
{
	long	m_lChannel;
	char	m_cStartTD[MAXSIZE_TIMEDATE];
	char	m_cEndTD[MAXSIZE_TIMEDATE];
	long	m_lTimeZone;
	long	m_lTimeMode;
	char	m_cRecKind[MAXSIZE_SRCHRECKIND];
	long	m_lAudio;

	//Constructor
	SSEARCHRSLT_INFO_EX()
	{
		m_lChannel = 0;
		memset( &m_cStartTD[0], NULLCHAR, MAXSIZE_TIMEDATE	);
		memset( &m_cEndTD[0], NULLCHAR, MAXSIZE_TIMEDATE	);
		m_lTimeZone = 0;
		m_lTimeMode = 0;
		memset( &m_cRecKind[0], NULLCHAR, MAXSIZE_SRCHRECKIND	);
		m_lAudio = 0;
	}
		
}SSEARCHRSLT_INFO_EX;

/*----------------------------------------------------------------------------------------
 |  Log Result Info Structure definition
 +--------------------------------------------------------------------------------------*/
typedef struct SLOGRESULT
{
	long	m_lNumber;
	char	m_cTimeDate[MAXSIZE_TIMEDATE];
	long	m_isDST;
	long	m_lTimeZone;
	char	m_cProtocol[MAXSIZE_PROTOCOL];
	char	m_cMsg[MAXSIZE_LOGMSG];

	//Constructor
	SLOGRESULT()
	{
		m_lNumber	= 0;
		memset( &m_cTimeDate[0], NULLCHAR, MAXSIZE_TIMEDATE	);
		m_isDST	= 0;
		m_lTimeZone	= 0;
		memset( &m_cProtocol[0], NULLCHAR, MAXSIZE_PROTOCOL	);
		memset( &m_cMsg[0], NULLCHAR, MAXSIZE_LOGMSG	);
	}

}SLOGRESULT;
	
/*----------------------------------------------------------------------------------------
 |  Convert file list Structure definition
 +--------------------------------------------------------------------------------------*/
typedef struct SFILELIST
{
	char	m_cFileName[MAXSIZE_FILEPATH];
	char	m_cPassword[MAXSIZE_FILEPWD];

	//Constructor
	SFILELIST()
	{
		memset( &m_cFileName[0], NULLCHAR, MAXSIZE_FILEPATH	);
		memset( &m_cPassword[0], NULLCHAR, MAXSIZE_FILEPWD	);
	}

}SFILELIST;

/*----------------------------------------------------------------------------------------
 |  Ftp file list Structure definition
 +--------------------------------------------------------------------------------------*/
typedef struct SFTPFILELIST_INFO
{
	char	m_cFileName[MAXSIZE_FILEPATH];
	long	m_lFileSize;
	char	m_cCreateTD[MAXSIZE_TIMEDATE];
	char	m_cLastWriteTD[MAXSIZE_TIMEDATE];
	long	m_lFileAttributes;
	char	m_cAlarmDate[MAXSIZE_TIMEDATE];
	long	m_lEventType;
	char	m_cID[MAXSIZE_INFO];

	//Constructor
	SFTPFILELIST_INFO()
	{
		memset( &m_cFileName[0], NULLCHAR, MAXSIZE_FILEPATH	);
		m_lFileSize			= 0;
		memset( &m_cCreateTD[0], NULLCHAR, MAXSIZE_TIMEDATE	);
		memset( &m_cLastWriteTD[0], NULLCHAR, MAXSIZE_TIMEDATE	);
		m_lFileAttributes	= 0;
		memset( &m_cAlarmDate[0], NULLCHAR, MAXSIZE_TIMEDATE	);
		m_lEventType		= 0;
		memset( &m_cID[0], NULLCHAR, MAXSIZE_INFO	);
	}

}SFTPFILELIST_INFO;

/*----------------------------------------------------------------------------------------
 |  File info Structure definition
 +--------------------------------------------------------------------------------------*/
typedef struct SFILE_INFO
{
	char	m_cProductNum[MAXSIZE_INFO];
	char	m_cSerialNo[MAXSIZE_INFO];
	char	m_cIPAddress[MAXSIZE_INFO];
	char	m_cID[MAXSIZE_INFO];
	long	m_lVideoFormat;
	long	m_lVideoWidth;
	long	m_lVideoHeight;
	long	m_lAudioFormat;
	long	m_lAudioSampleRate;
	long	m_lAudioBitPerSample;
	long	m_lAudioChannel;
	char	m_cTimeDate[MAXSIZE_TIMEDATE];
	long	m_lDuration;
	long	m_lSecurityV;
	long	m_lSecurityA;
	
	//Constructor
	SFILE_INFO()
	{
		memset( &m_cProductNum[0], NULLCHAR, MAXSIZE_INFO	);
		memset( &m_cSerialNo[0], NULLCHAR, MAXSIZE_INFO	);
		memset( &m_cIPAddress[0], NULLCHAR, MAXSIZE_INFO	);
		memset( &m_cID[0], NULLCHAR, MAXSIZE_INFO	);
		m_lVideoFormat			= 0;
		m_lVideoWidth			= 0;
		m_lVideoHeight			= 0;
		m_lAudioFormat			= 0;
		m_lAudioSampleRate		= 0;
		m_lAudioBitPerSample	= 0;
		m_lAudioChannel			= 0;
		memset( &m_cTimeDate[0], NULLCHAR, MAXSIZE_TIMEDATE	);
		m_lDuration				= 0;
		m_lSecurityV			= 0;
		m_lSecurityA			= 0;
	}

}SFILE_INFO;

/*----------------------------------------------------------------------------------------
 |  File info event list Structure definition
 +--------------------------------------------------------------------------------------*/
typedef struct SEVENT_INFO
{
	char	m_cTimeDate[MAXSIZE_TIMEDATE];
	long	m_lTriggerID;
	long	m_lFiller;
	long	m_lAlarmID;

	//Constructor
	SEVENT_INFO()
	{
		memset( &m_cTimeDate[0], NULLCHAR, MAXSIZE_TIMEDATE	);
		m_lTriggerID	= 0;
		m_lFiller		= 0;
		m_lAlarmID		= 0;
	}

}SEVENT_INFO;

/*----------------------------------------------------------------------------------------
 |  UID info Structure definition
 +--------------------------------------------------------------------------------------*/
typedef struct SUID_INFO
{
	long	m_lMaxCount;
	long	m_lInUseCount;

	//Constructor
	SUID_INFO()
	{
		m_lMaxCount		= 0;
		m_lInUseCount	= 0;
	}

}SUID_INFO;

/*----------------------------------------------------------------------------------------
 | StreamID info Structure definition
 +--------------------------------------------------------------------------------------*/
typedef struct SSID_INFO
{
	long	m_lMode;
	long	m_lMaxCount;
	long	m_lInUseCount;

	//Constructor
	SSID_INFO()
	{
		m_lMode			= 0;
		m_lMaxCount		= 0;
		m_lInUseCount	= 0;
	}

}SSID_INFO;


#endif
