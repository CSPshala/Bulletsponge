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
	m_fRotation = 0.0f;

	// Screw specifics
	m_bScrew = false;
	m_bPuke = false;
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
	RECT tRect;
	tRect.left = 0;
	tRect.top = 0;

	if(GetIsScrew())
	{
		tRect.right = tRect.left + 32;
		tRect.bottom = tRect.top + 32;
	}
	else
	{
		tRect.right = tRect.left + 16;
		tRect.bottom = tRect.top + 16;
	}


	TEXTUREMAN->DrawF(GetImageID(),GetPosX() - GetOffsetX(),GetPosY() - GetOffsetY(),-1.0f,1.0f,&tRect,1.0f,1.0f,GetRotation());
}

void CBullet::Update(float fElapsedTime)
{
	if(GetIsScrew())
	{
		// Gravity is applied to screws		
		SetVelY(GetVelY() + 100.0f * GAME->GetTimer().GetDeltaTime());

		SetPosX(GetPosX()+(GetVelX() * GAME->GetTimer().GetDeltaTime()));
		SetPosY(GetPosY()+(GetVelY() * GAME->GetTimer().GetDeltaTime()));	

		//if((GetVelY() < 1.0f && GetVelY() > -1.0f) /*|| (GetVelX() < 1.0f && GetVelX() > -1.0f)*/)
		//	MS->SendMsg(new CDestroyBulletMessage(this));
	}

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
					if(GetOwner()->GetType() == OBJ_PLAYER)
					{
					MS->SendMsg(new CDestroyBulletMessage(this));
					ES->SendEvent("enemyhit",pBase);
					return true;
					}
					return false;
				}
				break;

			case OBJ_ROBOT:
				{				
					if(GetOwner()->GetType() == OBJ_PLAYER)
					{
					MS->SendMsg(new CDestroyBulletMessage(this));
					ES->SendEvent("enemyhit",pBase);
					return true;
					}
					return false;
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

	//if(GetIsScrew())
	//{
	//	if(pBase->GetType() == OBJ_WORLD)
	//	{
	//		RECT rTemp;

	//			if(IntersectRect(&rTemp,&GetCollisionRect(),&pBase->GetCollisionRect()))
	//		{
	//			// Checks for top/bottom hit
	//			// Reverses Y vel if top hit (so can fall)
	//			// If bottom, sets to 0 
	//			// Pushes player back by how much they cut into the other rect
	//		
	//			if(rTemp.right - rTemp.left > rTemp.bottom - rTemp.top)
	//			{
	//				// Bottom hit
	//				if(GetVelY() > 0)
	//				{
	//					SetVelY(GetVelY() *0.5f);
	//				}
	//				// Top hit
	//				else
	//				{
	//					SetVelY(GetVelY() * -0.75);
	//				}
	//			
	//			}
	//			// Side hits
	//			// Pushes player back by how much they cut into the other rect
	//			else
	//			{
	//				// Right side hit
	//				if(GetVelX() > 0)
	//				{
	//					SetVelX(GetVelX() * -0.5);			
	//				}
	//				// Left side hit
	//				else
	//				{
	//					SetVelX(GetVelX() * -0.5);
	//				}
	//			}				
	//		
	//			return true;
	//		}
	//		else 
	//		{			
	//			return false;
	//		}

	//	}
	//}

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



