///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CWorld.cpp"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	To contain functionality for world architechture
///////////////////////////////////////////////////////////////////////////

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "CWorld.h"
#include "../states/CGamePlayState.h"
#include "../Macros.h"
#include "../SGD Wrappers/CSGD_Direct3D.h"

///////////////////////////////////////////////
//  CONSTRUCTOR / DECONSTRUCT / OP OVERLOADS
///////////////////////////////////////////////
CWorld::CWorld()
{
	SetType(OBJ_WORLD);
	ES->RegisterClient("camerachange",this);
}

CWorld::~CWorld()
{
	// Unregistering for events
	ES->UnregisterClient("camerachange",this);
}

////////////////////////////////////////
//		PUBLIC UTILITY FUNCTIONS
////////////////////////////////////////
void CWorld::Render()
{
	if(CGamePlayState::GetInstance()->GetRenderBoxes())
	{		
		RECT tRect = GetCollisionRect();
		tRect.left = tRect.left - (LONG)GetOffsetX();
		tRect.right = tRect.left + GetWidth();
		tRect.top = tRect.top - (LONG)GetOffsetY();
		tRect.bottom = tRect.top + GetHeight();

		D3D->DrawRect(tRect,255,0,0);		
	}
}

void CWorld::HandleEvent(CEvent* pEvent)
{
	if("camerachange" == pEvent->GetEventID())
	{
		CLevel* tLevel = (CLevel*)pEvent->GetParam();

		SetOffsetX(tLevel->GetOffsetX());
		SetOffsetY(tLevel->GetOffsetY());
	}
}

////////////////////////////////////////
//		PRIVATE UTILITY FUNCTIONS
////////////////////////////////////////

////////////////////////////////////////
//	    PUBLIC ACCESSORS / MUTATORS
////////////////////////////////////////

////////////////////////////////////////
//	    PRIVATE ACCESSORS / MUTATORS
////////////////////////////////////////