///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CWorld.h"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	To contain data for world architechture
///////////////////////////////////////////////////////////////////////////
#ifndef _CWORLD_H_
#define _CWORLD_H_

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "CBase.h"
#include "../messaging/IListener.h"

class CWorld : public CBase, public IListener
{
public:
	/********** Construct / Deconstruct / OP Overloads ************/
		CWorld();
		~CWorld();

	/********** Public Utility Functions ************/
		void Render();
		void HandleEvent(CEvent* pEvent);

	/********** Public Accessors ************/


	/********** Public Mutators  ************/	
		
private:
	/********** Private Members ************/


	/********** Private Accessors ************/


	/********** Private Mutators ************/


	/********** Private Utility Functions ************/
	
};

#endif