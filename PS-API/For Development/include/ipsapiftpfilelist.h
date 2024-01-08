#ifndef _IPSAPIFTPFLIST_
#define _IPSAPIFTPFLIST_
/******************************************************************************************
 *  INTERFACE MODULE	PSAPI FTP FILE LIST INTERFACE
 ******************************************************************************************
 *
 *	UNIT:			Psapi Ftp File List Interface
 *
 *	FILE NAME:		ipsapiftpfilelist.h
 *
 *	DESCRIPTION:	PSAPI Ftp File List Interface
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
 |  FTP FILE LIST INTERFACE Class
 +--------------------------------------------------------------------------------------*/
class PSAPIDECLSPEC IFtpFileList
{
public:
	virtual ~IFtpFileList() {};

public:
	virtual SPSAPITYPE::ERRVALUE	ListClear( void ) = 0;

	virtual SPSAPITYPE::ERRVALUE	GetListCount(	SPSAPIFTPLIST::INDEXCOUNT&	) = 0;
	virtual SPSAPITYPE::ERRVALUE	GetListItem(	SPSAPIFTPLIST::INDEX,
													SFTPFILELIST_INFO*			) = 0;
};

/*----------------------------------------------------------------------------------------
 |  Extern Function to handle IFtpFileList 
 +--------------------------------------------------------------------------------------*/
extern "C" PSAPIDECLSPEC IFtpFileList* GetIFtpFileList(	void	);
extern "C" PSAPIDECLSPEC void DeleteIFtpFileList(	IFtpFileList*	);



#endif


