#ifndef _IAPPCB_
#define _IAPPCB_
/******************************************************************************************
 *  INTERFACE MODULE	PS API APPLICATION CALL BACK INTERFACE
 ******************************************************************************************
 *
 *	UNIT:			PS API Application Call Back Interface
 *
 *	FILE NAME:		iappcallback.h
 *
 *	DESCRIPTION:	PS API Application Call Back Interface
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
 *	S.Ramaswamy 10/30/08	Created
 *	S.Ramaswamy 08/24/09	Added OnSearchExCB() and OnFtpStatusCB()
 *	S.Ramaswamy 10/21/09	Changed all IAppCallBack functions from pure virtual to virtual
 *	
 *
 ******************************************************************************************
 *  COPYRIGHT:		Panasonic System Networks Co., Ltd. 2008 - 2009
 *****************************************************************************************/

/*----------------------------------------------------------------------------------------
 |  PS API Device Application Call Back Interface Class
 +--------------------------------------------------------------------------------------*/

class IAppCallBack
{
public:
	virtual ~IAppCallBack()	{};

public:
	virtual void	OnAlmStatusCB(  SPSAPITYPE::STATUS	){};

	virtual void	OnRecCB(		SPSAPITYPE::STATE	){};

	virtual void	OnSearchCB(		void		){};

	virtual void	OnSearchExCB(		void	){};

	virtual void	OnPortStatusCB( SPSAPITYPE::STATUS	){};

	virtual void	OnOpStatusCB(   SPSAPITYPE::STATUS	){};

	virtual void	OnPlayStatusCB( SPSAPITYPE::STATUS	){};

	virtual void	OnFtpStatusCB(  SPSAPITYPE::STATUS	){};
	
};

#endif