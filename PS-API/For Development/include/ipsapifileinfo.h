#ifndef _IPSAPIFINFO_
#define _IPSAPIFINFO_
/******************************************************************************************
 *  INTERFACE MODULE	PSAPI FILE INFO INTERFACE
 ******************************************************************************************
 *
 *	UNIT:			Psapi File Info Interface
 *
 *	FILE NAME:		ipsapifileinfo.h
 *
 *	DESCRIPTION:	PSAPI File Info Interface
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
 *	PSN         04/22/11	Created
 *
 ******************************************************************************************
 *  COPYRIGHT:		Panasonic System Networks Co., Ltd. 2011
 *****************************************************************************************/

/*----------------------------------------------------------------------------------------
 |  PSAPI FILE INFO INTERFACE Class
 +--------------------------------------------------------------------------------------*/
class PSAPIDECLSPEC IFileInfo
{
public:
	virtual ~IFileInfo() {};

public:
	virtual SPSAPITYPE::ERRVALUE	GetAVParamInfo(	SFILE_INFO*		) = 0;
	
	virtual SPSAPITYPE::ERRVALUE	GetEventListCount(	SPSAPIFILEINFO::INDEXCOUNT&	) = 0;
	virtual SPSAPITYPE::ERRVALUE	GetEventListItem(	SPSAPIFILEINFO::INDEX,
														SEVENT_INFO*				) = 0;
};

/*----------------------------------------------------------------------------------------
 |  Extern Function to handle IFileInfo 
 +--------------------------------------------------------------------------------------*/
extern "C" PSAPIDECLSPEC IFileInfo* GetIFileInfo(	void	);
extern "C" PSAPIDECLSPEC void DeleteIFileInfo(	IFileInfo*	);



#endif


