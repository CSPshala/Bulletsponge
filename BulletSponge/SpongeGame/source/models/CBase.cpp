///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CBase.cpp"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	To contain all related data and
//					functionality for our game objects.
///////////////////////////////////////////////////////////////////////////

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "../Macros.h"
#include "CBase.h"
#include "../SGD Wrappers/CSGD_TextureManager.h"	//	Include where used
#include "CLevel.h"

///////////////////////////////////////////////
//  CONSTRUCTOR / DECONSTRUCT / OP OVERLOADS
///////////////////////////////////////////////

CBase::CBase(void)
{
	m_uiRefCount = 1;	//	 start with a ref to yourself
	SetPosX(0);
	SetPosY(0);
	SetVelX(0);
	SetVelY(0);
	SetHeight(0);
	SetWidth(0);
	SetIsActive(true);
	SetImageID(-1);
	SetOffsetX(0);
	SetOffsetY(0);
	SetMovementSpeedX(100.0f);
	SetMovementSpeedY(100.0f);

	m_fGravity = -10;

	
}

CBase::~CBase(void)
{
	
}

////////////////////////////////////////
//		PUBLIC UTILITY FUNCTIONS
////////////////////////////////////////

void CBase::Update(float fElapsedTime)
{
	//	pos += vel		<--	frame based
	//	pos += vel * dt	<--	time based
	SetOldPosX(GetPosX());
	SetOldPosY(GetPosY());

	SetPosX(GetPosX()+GetVelX() * fElapsedTime);
	SetPosY(GetPosY()+GetVelY() * fElapsedTime);
}

void CBase::Render(void)
{
	// Drawing at screen position
	CSGD_TextureManager::GetInstance()->DrawF(GetImageID(),GetPosX() - GetOffsetX(),GetPosY() - GetOffsetY());
}

void CBase::Release(void)
{
	m_uiRefCount--;

	if(m_uiRefCount == 0)
		delete this;
}

RECT CBase::GetCollisionRect()
{
	RECT rTemp;
	rTemp.left = (int)GetPosX();
	rTemp.top = (int)GetPosY();
	rTemp.right = (int)(GetPosX() + GetWidth());
	rTemp.bottom = (int)(GetPosY() + GetHeight());

	return rTemp;
}

bool CBase::CheckCollision(IBaseInterface* pBase)
{
	RECT rTemp;
	if(IntersectRect(&rTemp,&GetCollisionRect(),&pBase->GetCollisionRect()))
	{
		return true;
	}
	else
		return false;
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