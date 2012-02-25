///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CBullet.cpp"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	Handle projectile behavior and data
///////////////////////////////////////////////////////////////////////////

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "CBullet.h"
#include "../SGD Wrappers/CSGD_TextureManager.h"
#include "../messaging/CSGD_MessageSystem.h"
#include "../Macros.h"
#include "../CGame.h"
#include "CLevel.h"

///////////////////////////////////////////////
//  CONSTRUCTOR / DECONSTRUCT / OP OVERLOADS
///////////////////////////////////////////////
CBullet::CBullet()
{
	m_pOwner = NULL;	
	SetWidth(16);
	SetHeight(16);
	SetDamage(5);
	SetType(OBJ_BULLET);
	ES->RegisterClient("enemyhit",this);
	ES->RegisterClient("camerachange",this);
	m_fTimer = 0.0f;
};

CBullet::CBullet(float fRotation,CBase* pOwner)
{
	SetOwner(pOwner);
	SetWidth(16);
	SetHeight(16);
	SetDamage(5);
	SetType(OBJ_BULLET);
	ES->RegisterClient("enemydestroyed",this);
	ES->RegisterClient("camerachange",this);
	ES->RegisterClient("playerhit",this);
	m_fTimer = 0.0f;
}
	
CBullet::~CBullet()
{
	if(m_pOwner != NULL)
	{
		m_pOwner->Release();
	}
	// Unregistering events	
	ES->UnregisterClient("enemyhit",this);
	ES->UnregisterClient("camerachange",this);
	ES->UnregisterClient("playerhit",this);
}

////////////////////////////////////////
//		PUBLIC UTILITY FUNCTIONS
////////////////////////////////////////

void CBullet::Render()
{
	TEXTUREMAN->DrawF(GetImageID(),GetPosX() - GetOffsetX(),GetPosY() - GetOffsetY());
}

void CBullet::Update(float fElapsedTime)
{
	// Killing the bullet if it gets offscreen
	if(m_fTimer > 10.0f)
		MS->SendMsg(new CDestroyBulletMessage(this));

	m_fTimer += GAME->GetTimer().GetDeltaTime();

	CBase::Update(fElapsedTime);
}

void CBullet::HandleEvent(CEvent* pEvent)
{
	if("enemydestroyed" == pEvent->GetEventID())
	{
		if(pEvent->GetParam() == GetOwner())
		{
			if(m_pOwner != NULL)
			{
				m_pOwner->Release();
			}			
		}
	}
	if("camerachange" == pEvent->GetEventID())
	{
		CLevel* tLevel = (CLevel*)pEvent->GetParam();

		SetOffsetX(tLevel->GetOffsetX());
		SetOffsetY(tLevel->GetOffsetY());
	}
}

bool CBullet::CheckCollision(IBaseInterface* pBase)
{
	if(pBase->GetType() != OBJ_WORLD && (GetOwner()->GetType() != pBase->GetType()))
	{
		if(CBase::CheckCollision(pBase))
		{
			switch(pBase->GetType())
			{
			case OBJ_ENEMY:
				{				
					MS->SendMsg(new CDestroyBulletMessage(this));
					ES->SendEvent("enemyhit",pBase);
					return true;
				}
				break;

			case OBJ_PLAYER:
				{
					ES->SendEvent("playerhit",this);
					MS->SendMsg(new CDestroyBulletMessage(this));					
					return true;
				}
				break;
			}
		}		
	}	

return false;
}

////////////////////////////////////////
//		PRIVATE UTILITY FUNCTIONS
////////////////////////////////////////

////////////////////////////////////////
//	    PUBLIC ACCESSORS / MUTATORS
////////////////////////////////////////

void CBullet::SetOwner(CBase* pOwner)
{
	if(m_pOwner != NULL)
	{
		m_pOwner->Release();
	}
	m_pOwner = pOwner;
	if(m_pOwner != NULL)
	{
		SetPosX(m_pOwner->GetPosX() + m_pOwner->GetWidth() / 2);
		SetPosY(m_pOwner->GetPosY() + m_pOwner->GetHeight() / 2);		
		m_pOwner->AddRef();
	}
}

////////////////////////////////////////
//	    PRIVATE ACCESSORS / MUTATORS
////////////////////////////////////////



