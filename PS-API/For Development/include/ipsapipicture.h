#ifndef _IPSAPIPIC_
#define _IPSAPIPIC_
/******************************************************************************************
 *  INTERFACE MODULE	PSAPI PICTURE INTERFACE
 ******************************************************************************************
 *
 *	UNIT:			Psapi Picture Interface
 *
 *	FILE NAME:		ipsapipicture.h
 *
 *	DESCRIPTION:	PSAPI Picture Interface
 *
 *	INCLUDES:
 */

					#include	"psapidef.h"


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
 *	S.Ramaswamy 02/11/10	Created
 *
 ******************************************************************************************
 *  COPYRIGHT:		Panasonic System Networks Co., Ltd. 2010
 *****************************************************************************************/

/*----------------------------------------------------------------------------------------
 |  LOG RESULT INTERFACE Class
 +--------------------------------------------------------------------------------------*/
class PSAPIDECLSPEC IPSAPIPicture
{
public:
	virtual ~IPSAPIPicture() {};
public:
	virtual SPSAPITYPE::COUNT		GetBitmapSize( void	)	=	0;
	virtual SPSAPITYPE::ERRVALUE	GetBitmapImage(	SPSAPIVID::IMGBUFFER*, 
													SPSAPIVID::IMGSIZE		)	=	0;
	virtual SPSAPITYPE::ERRVALUE	SaveBitmapImage(	SPSAPIVID::FILENAME	)	=	0;
	virtual SPSAPITYPE::COUNT		GetJpegSize(	void	)	=	0;
	virtual SPSAPITYPE::ERRVALUE	GetJpegImage(	SPSAPIVID::IMGBUFFER*, 
													SPSAPIVID::IMGSIZE		)	=	0;
	virtual SPSAPITYPE::ERRVALUE	SaveJpegImage(	SPSAPIVID::FILENAME	)	=	0;

};

/*----------------------------------------------------------------------------------------
 |  Extern Function to handle ISearchResult 
 +--------------------------------------------------------------------------------------*/
extern "C" PSAPIDECLSPEC IPSAPIPicture* GetIPSAPIPicture(	void	);
extern "C" PSAPIDECLSPEC void DeleteIPSAPIPicture(	IPSAPIPicture*	);



#endif












