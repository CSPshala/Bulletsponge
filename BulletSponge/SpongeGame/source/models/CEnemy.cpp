///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CEnemy.cpp"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	To hold all data and functionality of enemy characters
///////////////////////////////////////////////////////////////////////////

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "../Macros.h"
#include "CEnemy.h"
#include "CBullet.h"

#include "../SGD_Math.h"
#include "../CGame.h"
#include "../SGD Wrappers/CSGD_TextureManager.h"
#include "../SGD Wrappers/CSGD_DirectInput.h"
#include <string>
using std::string;


///////////////////////////////////////////////
//  CONSTRUCTOR / DECONSTRUCT / OP OVERLOADS
///////////////////////////////////////////////
CEnemy::CEnemy()
	: CBase()
{
	// Setting HP
	SetHP(30);
	// Setting hit color
	m_dwHitColor = D3DCOLOR_ARGB(255,255,255,255);
	// Setting hit color timer
	m_fHitColorTime = 0.0f;
	// Main menu?
	m_bMainMenu = false;
	// Rotation
	m_fRotation = float(rand() % 90);

	// Spawn Animation
	CAnimation SpawnAni(TEXTUREMAN->LoadTexture("resource/graphics/angrystickfigure/JCR_AngryStickFigure_rest.png"),64,128,64,128,1,0,false);
	m_nAnimations.push_back(SpawnAni);

	// Rest Animation
	CAnimation RestAni(TEXTUREMAN->LoadTexture("resource/graphics/angrystickfigure/JCR_AngryStickFigure_rest.png"),64,128,64,128,1,0,false);
	m_nAnimations.push_back(RestAni);

	// Patrol Animation
	CAnimation PatrolAni(TEXTUREMAN->LoadTexture("resource/graphics/angrystickfigure/JCR_AngryStickFigure_patrol.png"),256,128,64,128,4,0,0.08f,true,false);
	m_nAnimations.push_back(PatrolAni);

	// Jump Animation
	CAnimation JumpAni(TEXTUREMAN->LoadTexture("resource/graphics/angrystickfigure/JCR_AngryStickFigure_rest.png"),64,128,64,128,1,0,false);
	m_nAnimations.push_back(JumpAni);

	// Shoot Animation
	CAnimation ShootAni(TEXTUREMAN->LoadTexture("resource/graphics/angrystickfigure/JCR_AngryStickFigure_shoot.png"),128,128,64,128,2,0,0.1f,false,false);
	m_nAnimations.push_back(ShootAni);

	// Falling Animation
	CAnimation FallAni(TEXTUREMAN->LoadTexture("resource/graphics/angrystickfigure/JCR_AngryStickFigure_rest.png"),64,128,64,128,1,0,false);
	m_nAnimations.push_back(FallAni);

	// Enemy Bullet
	m_nEnemyBulletID = TEXTUREMAN->LoadTexture("resource/graphics/JCR_enemybullet.png");

	// Gunshot
	m_nShotFX = XAUDIO->SFXLoadSound("resource/sound/JCR_colt45.wav");

	SetType(OBJ_ENEMY);

	SetMovementSpeedX(150.0f);
	SetMovementSpeedY(700.0f);

	SetState(SPAWNING);

	// Setting timers and such
		// Timer for behavior change
		m_fChange = 3.1f;
		// Limits enemy fire
		m_fShootTimer = 1.1f;
		// Limits jumping
		m_fJumpTimer = 5.1f;
		// Holds last state
		m_eLastState = SPAWNING;

	m_pPlatform = NULL;

	// Signing up for events
	ES->RegisterClient("playerhit",this);
	ES->RegisterClient("camerachange",this);
	ES->RegisterClient("enemyhit",this);
	ES->RegisterClient("enemyfire",this);
}

CEnemy::~CEnemy()
{
	// Unregistering events
	ES->UnregisterClient("playerhit",this);
	ES->UnregisterClient("camerachange",this);
	ES->UnregisterClient("enemyhit",this);
	ES->UnregisterClient("enemyfire",this);

	SAFE_RELEASE(m_pPlatform);
}

////////////////////////////////////////
//		PUBLIC UTILITY FUNCTIONS
////////////////////////////////////////
void CEnemy::Update(float fElapsedTime)
{
	if(!m_bMainMenu)
	{
		switch(GetState())
		{
		case JUMP:
			{
				if(m_eLastState != JUMP)
				{
					if(m_fJumpTimer > 5.0f)
					{
						SetVelY(-GetMovementSpeedY());
						m_fJumpTimer = 0.0f;
					}

					
				}
			}
			break;

		case PATROL:
			{
				static int initialXVel = rand() % 2;

				if(GetVelX() == 0.0f)
				{
					switch(initialXVel)
					{
					case 0:
						SetVelX(-GetMovementSpeedX());
						break;
					case 1:
						SetVelX(GetMovementSpeedX());
						break;
					}
				}

				if(GetCollisionRect().left < GetPlatform()->GetCollisionRect().left)
				{
					SetPosX((float)GetPlatform()->GetCollisionRect().left);
					SetVelX(-GetVelX());
				}
				else if(GetCollisionRect().right > GetPlatform()->GetCollisionRect().right)
				{
					SetPosX((float)GetPlatform()->GetCollisionRect().right - GetWidth());
					SetVelX(-GetVelX());
				}
			}
			break;

		case SPAWNING:
			{
				SetVelY(200);
			}
			break;

		case SHOOT:
			{	
				if(m_fShootTimer > 1.0f && GetVelY() == 0.0f)
				{
					m_nAnimations[SHOOT].ResetAnimation();	
					XAUDIO->SFXPlaySound(m_nShotFX);
					ES->SendEvent("enemyfire",this);
					m_fShootTimer = 0.0f;
					SetVelX(0.0f);				
				}

				// Updating fire timer
				m_fShootTimer += GAME->GetTimer().GetDeltaTime();
			}
		}		

		/////////////////////////////////////MANUAL UPDATE/////////////////////////////
		//
			SetOldPosX(GetPosX());
			SetOldPosY(GetPosY());

			// Gravity
			if(GetState() == JUMP || GetState() == FALLING)
				SetVelY(GetVelY() + 1000.0f * GAME->GetTimer().GetDeltaTime());

			SetPosX(GetPosX()+GetVelX() * GAME->GetTimer().GetDeltaTime());
			SetPosY(GetPosY()+GetVelY() * GAME->GetTimer().GetDeltaTime());	
		//
		/////////////////////////////////////END OF UPDATE//////////////////////////////


		// Holding old state
		m_eLastState = GetState();
		// Setting AI state
		Behavior();

		// Updating jump timer 
		m_fJumpTimer += GAME->GetTimer().GetDeltaTime();
	}
	else
	{
		// Gravity
		SetVelY(GetVelY() + 0.01f);

		SetPosX(GetPosX()+GetVelX() * fElapsedTime);
		SetPosY(GetPosY()+GetVelY() * fElapsedTime);	

		m_fRotation += 1.0f *GAME->GetTimer().GetDeltaTime();		
	}

	if(GetPosY() > GAME->GetWindowHeight() + 600)
		MS->SendMsg(new CDestroyEnemyMessage(this));
}

void CEnemy::Render()
{
	if(!m_bMainMenu)
	{
		// Orientation
		static bool bFacingRight = true; //True for right, False for left
		if(GetVelX() < 0.0f)
			bFacingRight = true;
		else if(GetVelX() > 0.0f)
			bFacingRight = false;
	
		// Drawing Player (based on facing)
		if(bFacingRight)
			TEXTUREMAN->DrawF(m_nAnimations[GetState()].GetImageID(),GetPosX() - GetOffsetX(),GetPosY() - GetOffsetY(),1.0f,1.0f,&m_nAnimations[GetState()].GetFrame(),0.0f,0.0f,0.0f,m_dwHitColor);
		else
			TEXTUREMAN->DrawF(m_nAnimations[GetState()].GetImageID(),(GetPosX() - GetOffsetX()) + GetWidth(),GetPosY() - GetOffsetY(),-1.0f,1.0f,&m_nAnimations[GetState()].GetFrame(),0.0f,0.0f,0.0f,m_dwHitColor);
	
		FlashRedTimer(0.08f);
	}
	else
	{
		TEXTUREMAN->DrawF(m_nAnimations[REST].GetImageID(),GetPosX(),GetPosY(),1.0f,1.0f,&m_nAnimations[REST].GetFrame(),16.0f,32.0f,m_fRotation);
	}
}

void CEnemy::HandleEvent(CEvent* pEvent)
{
	if("camerachange" == pEvent->GetEventID())
	{
		CLevel* tLevel = (CLevel*)pEvent->GetParam();

		SetOffsetX(tLevel->GetOffsetX());
		SetOffsetY(tLevel->GetOffsetY());
	}
	else if("enemyhit" == pEvent->GetEventID())
	{
		if(pEvent->GetParam() == this)
			if(GetState() != SPAWNING)
			{
				SetHP(GetHP() - 10);	

				// Setting hit color
				m_dwHitColor = D3DCOLOR_ARGB(255,128,0,0);

				if(GetHP() <= 0)
				{
					MS->SendMsg(new CDestroyEnemyMessage(this));
				}
			}			
	}

}

bool CEnemy::CheckCollision(IBaseInterface* pBase)
{
	RECT rTemp;	

	switch(pBase->GetType())
	{
		case OBJ_PLAYER:
			{
				if(IntersectRect(&rTemp,&GetCollisionRect(),&pBase->GetCollisionRect()))
				{
					SetVelX(-GetVelX());
					ES->SendEvent("playerhit",this);
					return true;
				}
				return false;
			}
			break;

		case OBJ_WORLD:
		{
			if(IntersectRect(&rTemp,&GetCollisionRect(),&pBase->GetCollisionRect()))
			{
				// Checks for top/bottom hit
				// Reverses Y vel if top hit (so can fall)
				// If bottom, sets to 0 
				// Pushes player back by how much they cut into the other rect
			
				if(rTemp.right - rTemp.left > rTemp.bottom - rTemp.top)
				{
					// Bottom hit
					if(GetVelY() > 0)
					{
						SetVelY(0);
						SetPosY(GetPosY() - (rTemp.bottom - rTemp.top));
						SetState(PATROL);
						SetPlatform((CWorld*)pBase);					
					}
					// Top hit
					else
					{
						SetPosY(GetPosY() + (rTemp.bottom - rTemp.top));
						SetVelY(-GetVelY());
						SetState(FALLING);
					}
				
				}
				// Side hits
				// Pushes player back by how much they cut into the other rect
				else
				{
					// Right side hit
					if(GetVelX() > 0)
					{
						SetVelX(0);
						SetPosX(GetPosX() - (rTemp.right - rTemp.left));					
					}
					// Left side hit
					else
					{
						SetVelX(0);
						SetPosX(GetPosX() + (rTemp.right - rTemp.left));
					}
				}				
			
				return true;
			}
			else 
			{	
			
				return false;
			}
		}
		break;		

		default:
			break;
	}

	

	return false;
}

////////////////////////////////////////
//		PRIVATE UTILITY FUNCTIONS
////////////////////////////////////////
void CEnemy::Behavior()
{	
	if(m_fChange > 3.0f && GetState() != SPAWNING  && GetState() != JUMP)
	{
		// Setting a state randomly from PATROL to SHOOT
		SetState(EnemyState((rand() % 3) + 2));			

		m_fChange = 0.0f;
	}

	m_fChange += GAME->GetTimer().GetDeltaTime();
	
	
}
void CEnemy::FlashRedTimer(float fTime)
{
	if(m_dwHitColor != D3DCOLOR_ARGB(255,255,255,255))
	{
		if(m_fHitColorTime > fTime)
		{
			// Setting hit color
			m_dwHitColor = D3DCOLOR_ARGB(255,255,255,255);
			m_fHitColorTime = 0.0f;
		}
		else
			m_fHitColorTime += GAME->GetTimer().GetDeltaTime();		
	}		
}
////////////////////////////////////////
//	    PUBLIC ACCESSORS / MUTATORS
////////////////////////////////////////

////////////////////////////////////////
//	    PRIVATE ACCESSORS / MUTATORS
////////////////////////////////////////
void CEnemy::SetPlatform(CWorld* pPlatform)
{
	if(pPlatform == GetPlatform())
		return;

	if(GetPlatform() != NULL)
		GetPlatform()->Release();

	m_pPlatform = pPlatform;

	if(GetPlatform() != NULL)
		GetPlatform()->AddRef();
}