#ifndef _ISRCHRSLT_
#define _ISRCHRSLT_
/******************************************************************************************
 *  INTERFACE MODULE	SEARCH RESULT INTERFACE
 ******************************************************************************************
 *
 *	UNIT:			Search Result Interface
 *
 *	FILE NAME:		isearchresult.h
 *
 *	DESCRIPTION:	Search Result Interface
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
 *	S.Ramaswamy 09/17/08	Created
 *	S.Ramaswamy 12/21/08	Modified SetXXX function to return error value
 *	S.Ramaswamy 12/22/08	Added ISearchResult Class and its global operations
  *	S.Ramaswamy 08/24/09	Added Get/Delete ISearchResultEx.
 *
 ******************************************************************************************
 *  COPYRIGHT:		Panasonic System Networks Co., Ltd. 2008 - 2009
 *****************************************************************************************/

/*----------------------------------------------------------------------------------------
 |  SEARCH RESULT INTERFACE Class
 +--------------------------------------------------------------------------------------*/
class PSAPIDECLSPEC ISearchResult
{
public:
	virtual ~ISearchResult() {};
public:
	virtual SPSAPITYPE::ERRVALUE Clear(	void ) = 0;

	virtual SPSAPITYPE::ERRVALUE GetListCount(	SPSAPISEARCH::INDEXCOUNT&	) = 0;

	virtual SPSAPITYPE::ERRVALUE GetResult(		SPSAPISEARCH::INDEX,
												SSEARCHRSLT_INFO*		) = 0;

	virtual SPSAPITYPE::ERRVALUE GetAndDelete(	SSEARCHRSLT_INFO*		) = 0;
};

/*----------------------------------------------------------------------------------------
 |  Extern Function to handle ISearchResult 
 +--------------------------------------------------------------------------------------*/
extern "C" PSAPIDECLSPEC ISearchResult* GetISearchResult(	void	);
extern "C" PSAPIDECLSPEC void DeleteISearchResult(	ISearchResult*	);


/*----------------------------------------------------------------------------------------
 |  SEARCH RESULT EX INTERFACE Class
 +--------------------------------------------------------------------------------------*/
class PSAPIDECLSPEC ISearchResultEx
{
public:
	virtual ~ISearchResultEx() {};
public:
	virtual SPSAPITYPE::ERRVALUE Clear(	void ) = 0;

	virtual SPSAPITYPE::ERRVALUE GetListCount(	SPSAPISEARCH::INDEXCOUNT&	) = 0;

	virtual SPSAPITYPE::ERRVALUE GetResult(		SPSAPISEARCH::INDEX,
												SSEARCHRSLT_INFO_EX*		) = 0;

	virtual SPSAPITYPE::ERRVALUE GetAndDelete(	SSEARCHRSLT_INFO_EX*		) = 0;
};

/*----------------------------------------------------------------------------------------
 |  Extern Function to handle ISearchResultEx 
 +--------------------------------------------------------------------------------------*/
extern "C" PSAPIDECLSPEC ISearchResultEx* GetISearchResultEx(	void	);
extern "C" PSAPIDECLSPEC void DeleteISearchResultEx(	ISearchResultEx*	);


#endif












