///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CPlayer.cpp"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	To hold all data and functionality of player character
///////////////////////////////////////////////////////////////////////////

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "../Macros.h"
#include "CPlayer.h"
#include "CWorld.h"
#include "CEnemy.h"
#include "CBullet.h"
#include "../CGame.h"
#include "../SGD Wrappers/CSGD_TextureManager.h"
#include "../SGD Wrappers/CSGD_DirectInput.h"
#include <string>
using std::string;

////////////////////////////////////////
//				MISC
////////////////////////////////////////
#ifdef _DEBUG
//****GLOBAL CONSOLE HANDLE FOR DEBUG ONLY*****//
HANDLE theConsole;
DWORD  consoleChars;
#endif

///////////////////////////////////////////////
//  CONSTRUCTOR / DECONSTRUCT / OP OVERLOADS
///////////////////////////////////////////////
CPlayer::CPlayer()
	: CBase()
{
	// Rest Animation
	CAnimation RestAni(TEXTUREMAN->GetInstance()->LoadTexture("resource/graphics/sgtlollipop/JCR_sgtlollipop_rest.png"),64,128,64,128,1,0,1.0f,false,true);
	m_nAnimations.push_back(RestAni);	

	// Jump Animation
	CAnimation JumpAni(TEXTUREMAN->GetInstance()->LoadTexture("resource/graphics/sgtlollipop/JCR_sgtlollipop_jump.png"),128,128,64,128,2,0,0.08f,false,false);
	m_nAnimations.push_back(JumpAni);

	// Run Animation
	CAnimation RunAni(TEXTUREMAN->GetInstance()->LoadTexture("resource/graphics/sgtlollipop/JCR_sgtlollipop_run.png"),1028,128,64,128,12,0,0.03f,true);
	m_nAnimations.push_back(RunAni);

	// Falling Animation
	CAnimation FallAni(TEXTUREMAN->GetInstance()->LoadTexture("resource/graphics/sgtlollipop/JCR_sgtlollipop_falling.png"),64,256,64,128,3,0,0.05f,true,false);
	m_nAnimations.push_back(FallAni);

	// Crouching Animation
	CAnimation CrouchAni(TEXTUREMAN->GetInstance()->LoadTexture("resource/graphics/sgtlollipop/JCR_sgtlollipop_crouch.png"),64,64,64,64,1,0,1.0f,false,true);
	m_nAnimations.push_back(CrouchAni);

	// Hands
	m_nHands = TEXTUREMAN->LoadTexture("resource/graphics/sgtlollipop/JCR_pistol.png");
	m_fHands = 0.0f;	
	m_nShotty = TEXTUREMAN->LoadTexture("resource/graphics/sgtlollipop/JCR_shotty.png");

	// Bullet Img
	m_nPlayerBulletID = TEXTUREMAN->LoadTexture("resource/graphics/JCR_playerbullet.png");

	// Gunshot
	m_nShotFX = XAUDIO->SFXLoadSound("resource/sound/JCR_10mm.wav");
	m_nShotgunFX = XAUDIO->SFXLoadSound("resource/sound/JCR_shotgun.wav");

	// Voiceovers
	m_nGodOffID = XAUDIO->SFXLoadSound("resource/sound/JCR_godoff.wav");
	m_nGodOnID = XAUDIO->SFXLoadSound("resource/sound/JCR_godon.wav");
	m_nLivesID = XAUDIO->SFXLoadSound("resource/sound/JCR_lives.wav");
	m_nShottyOnID = XAUDIO->SFXLoadSound("resource/sound/JCR_shottyon.wav");
	m_nShottyOffID = XAUDIO->SFXLoadSound("resource/sound/JCR_shottyoff.wav");

	// Registering for events
	ES->RegisterClient("firepistol",this);
	ES->RegisterClient("camerachange",this);
	ES->RegisterClient("playerhit",this);
	ES->RegisterClient("enemyfire",this);
	ES->RegisterClient("invincible",this);
	ES->RegisterClient("shotty",this);
	ES->RegisterClient("lives",this);

	SetType(OBJ_PLAYER);

	SetMovementSpeedX(200.0f);
	SetMovementSpeedY(700.0f);	

	SetState(REST);

	m_pPlatform = NULL;

	// HP
	m_nMaxHP = 100;
	m_nCurHP = 100;

	// Cheats
	m_bGod = false;
	m_bShotty = false;

	m_bRespawning = false;
	m_fRespawnTime = 0.0f;

#ifdef _DEBUG
	// Starting console if in debug
	/*AllocConsole();
	theConsole = GetStdHandle(STD_OUTPUT_HANDLE);*/
#endif
}

CPlayer::~CPlayer()
{	
	SAFE_RELEASE(m_pPlatform);
	// Unregistering for events
	ES->UnregisterClient("camerachange",this);
	ES->UnregisterClient("firepistol",this);
	ES->UnregisterClient("playerhit",this);
	ES->UnregisterClient("enemyfire",this);
	ES->UnregisterClient("invincible",this);
	ES->UnregisterClient("shotty",this);
	ES->UnregisterClient("lives",this);
	
}

////////////////////////////////////////
//		PUBLIC UTILITY FUNCTIONS
////////////////////////////////////////

void CPlayer::Update(float fElapsedTime)
{

	/////////////////////////// INPUT ////////////////////////////////////////
	//	Moves camera space around based on position
		ES->SendEvent("playermoved",this);	

		// Getting input and settings states

	if(GetState() != CROUCH)
	{
		if(DI->KeyDown(DIK_D))
		{
			SetVelX(GetMovementSpeedX());	
			if(GetState() != JUMP && GetState() != FALLING)
				SetState(RUN);
		}
		if(DI->KeyDown(DIK_A))
		{
			SetVelX(-GetMovementSpeedX());	
			if(GetState() != JUMP && GetState() != FALLING)
				SetState(RUN);
		}

		if(DI->KeyUp(DIK_A) && DI->KeyUp(DIK_D) && GetState() != FALLING && GetState() != JUMP)
		{
			SetVelX(0);	
			SetState(REST);
		}

		if(GetState() != JUMP && GetState() != FALLING)
			if(DI->KeyPressed(DIK_SPACE))
			{
				SetVelY(-GetMovementSpeedY());	
				SetState(JUMP);
				m_nAnimations[JUMP].ResetAnimation();
			}
	}

	if(GetState() != JUMP && GetState() != FALLING)
	{
		if(DI->KeyDown(DIK_LCONTROL))		
			SetState(CROUCH);			
		
		if(DI->KeyReleased(DIK_LCONTROL))
			SetState(REST);
	}
		
		
		
		
		static float fFire = 0.0f;
		if(DI->MouseButtonDown(0))
		{
			// Fire timer
			if(fFire <= 0.0f)
			{				
				ES->SendEvent("firepistol");
				fFire = 0.5f;
			}		
		}
		fFire -= fElapsedTime;
	//
	//////////////////////////////////////END OF INPUT//////////////////////////////

	/////////////////////////////////////MISC///////////////////////////////////////
	
		// Hand rotation
		HandAiming();

		// Checking to see if we're off the edge of our platform
		if(GetPlatform())
		{
			if(GetCollisionRect().right < GetPlatform()->GetCollisionRect().left)
			{
				SetState(FALLING);
			}
			else if(GetCollisionRect().left > GetPlatform()->GetCollisionRect().right)
			{
				SetState(FALLING);
			}
		}

		if(GetVelY() > 0.0f)
			SetState(FALLING);
		
	//
	////////////////////////////////////END OF MISC/////////////////////////////////

	/////////////////////////////////////MANUAL UPDATE/////////////////////////////
	//
		SetOldPosX(GetPosX());
		SetOldPosY(GetPosY());

		// Gravity
		if(GetState() == JUMP || GetState() == FALLING)
			SetVelY(GetVelY() + 1000.0f * GAME->GetTimer().GetDeltaTime());

		SetPosX(GetPosX()+(GetVelX() * GAME->GetTimer().GetDeltaTime()));
		SetPosY(GetPosY()+(GetVelY() * GAME->GetTimer().GetDeltaTime()));	
	//
	/////////////////////////////////////END OF UPDATE//////////////////////////////

#ifdef _DEBUG
		/*static float outputtime = 0.2f;

		if(GetVelY() < -800.0f || GetVelY() > 800.0f)
		{
		char buffer[50];
		
		sprintf(buffer,"%f \n",GetVelY());

		WriteConsole(theConsole,buffer,strlen(buffer),&consoleChars,NULL);

		outputtime = 0.5f;
		}
		else
			outputtime -= GAME->GetTimer().GetDeltaTime();
*/
#endif

}

void CPlayer::Render()
{
	
	// Orientation
	static bool bFacingRight = true; //True for right, False for left
	if(GetVelX() > 0.0f)
		bFacingRight = true;
	else if(GetVelX() < 0.0f)
		bFacingRight = false;

	if(!m_bRespawning)
	{
		// Drawing Player (based on facing)
		if(bFacingRight)
			TEXTUREMAN->DrawF(m_nAnimations[GetState()].GetImageID(),GetPosX() - GetOffsetX(),GetPosY() - GetOffsetY(),1.0f,1.0f,&m_nAnimations[GetState()].GetFrame());
		else
			TEXTUREMAN->DrawF(m_nAnimations[GetState()].GetImageID(),(GetPosX() - GetOffsetX()) + GetWidth(),GetPosY() - GetOffsetY(),-1.0f,1.0f,&m_nAnimations[GetState()].GetFrame());
	
		

		// Drawing Hands
		if(!m_bShotty)
		{

			RECT tRect;
			tRect.left = 0;
			tRect.right = 64;
			tRect.top = 0;
			tRect.bottom = 64;

			if(bFacingRight)
				TEXTUREMAN->DrawF(m_nHands,m_tHands.fX - GetOffsetX(),m_tHands.fY - GetOffsetY(),0.5f,0.5f,&tRect,-50.0f,0.0f,m_fHands);
			else
				TEXTUREMAN->DrawF(m_nHands,m_tHands.fX - GetOffsetX(),(m_tHands.fY - GetOffsetY()) + 8.0f,0.5f,-0.5f,&tRect,-50.0f,0.0f,m_fHands);
		}
		else
		{
			RECT tRect;
			tRect.left = 0;
			tRect.right = 128;
			tRect.top = 0;
			tRect.bottom = 64;

			if(bFacingRight)
				TEXTUREMAN->DrawF(m_nShotty,m_tHands.fX - GetOffsetX(),m_tHands.fY - GetOffsetY() + 20,0.5f,0.5f,&tRect,11.0f,50.0f,m_fHands);
			else
				TEXTUREMAN->DrawF(m_nShotty,m_tHands.fX - GetOffsetX(),(m_tHands.fY - GetOffsetY()) + 60.0f,0.5f,-0.5f,&tRect,11.0f,50.0f,m_fHands);
		}
	}
	else
	{
		D3DCOLOR temp = D3DCOLOR_ARGB(int(255 * m_fRespawnTime),255,255,255);
		
			
		// Drawing Player (based on facing)
		if(bFacingRight)
			TEXTUREMAN->DrawF(m_nAnimations[GetState()].GetImageID(),GetPosX() - GetOffsetX(),GetPosY() - GetOffsetY(),1.0f,1.0f,&m_nAnimations[GetState()].GetFrame(),0.0f,0.0f,0.0f,temp);
		else
			TEXTUREMAN->DrawF(m_nAnimations[GetState()].GetImageID(),(GetPosX() - GetOffsetX()) + GetWidth(),GetPosY() - GetOffsetY(),-1.0f,1.0f,&m_nAnimations[GetState()].GetFrame(),0.0f,0.0f,0.0f,temp);
			

		// Drawing Hands
		if(!m_bShotty)
		{
			RECT tRect;
			tRect.left = 0;
			tRect.right = 64;
			tRect.top = 0;
			tRect.bottom = 64;

			if(bFacingRight)
				TEXTUREMAN->DrawF(m_nHands,m_tHands.fX - GetOffsetX(),m_tHands.fY - GetOffsetY(),0.5f,0.5f,&tRect,-50.0f,0.0f,m_fHands,temp);
			else
				TEXTUREMAN->DrawF(m_nHands,m_tHands.fX - GetOffsetX(),(m_tHands.fY - GetOffsetY()) + 8.0f,0.5f,-0.5f,&tRect,-50.0f,0.0f,m_fHands,temp);
		}
		else
		{

			RECT tRect;
			tRect.left = 0;
			tRect.right = 128;
			tRect.top = 0;
			tRect.bottom = 64;

			if(bFacingRight)
				TEXTUREMAN->DrawF(m_nShotty,m_tHands.fX - GetOffsetX(),m_tHands.fY - GetOffsetY() + 20.0f,0.5f,0.5f,&tRect,11.0f,41.0f,m_fHands,temp);
			else
				TEXTUREMAN->DrawF(m_nShotty,m_tHands.fX - GetOffsetX(),(m_tHands.fY - GetOffsetY()) + 60.0f,0.5f,-0.5f,&tRect,11.0f,41.0f,m_fHands,temp);
		}

		if(m_fRespawnTime < 3.0f)
			m_fRespawnTime += GAME->GetTimer().GetDeltaTime();
		else
			m_bRespawning = false;
	}

	if(CGamePlayState::GetInstance()->GetRenderBoxes())
	{
		char buffer[128];
		sprintf_s(buffer,_countof(buffer),"XVel: %f  YVel: %f",GetVelX(),GetVelY());
		D3D->DrawText(buffer,(CGame::GetInstance()->GetWindowWidth() >> 1) + 100,0,255,0,0);
	}

}

void CPlayer::HandleEvent(CEvent* pEvent)
{
	if("firepistol" == pEvent->GetEventID())
	{
		if(!m_bShotty)
		{
			XAUDIO->SFXPlaySound(m_nShotFX);
			MS->SendMsg(new CCreateBulletMessage(this,m_fHands,m_nPlayerBulletID));
		}
		else
		{
			XAUDIO->SFXPlaySound(m_nShotgunFX);
			// Firing a fan of 3 rounds
			MS->SendMsg(new CCreateBulletMessage(this,m_fHands,m_nPlayerBulletID));
			MS->SendMsg(new CCreateBulletMessage(this,m_fHands + 0.17f,m_nPlayerBulletID));
			MS->SendMsg(new CCreateBulletMessage(this,m_fHands - 0.17f,m_nPlayerBulletID));
		}

		return;
	}	
	else if("playerhit" == pEvent->GetEventID())
	{
		if(!m_bRespawning && !m_bGod)
		{
			SetHP(GetHP() - ((CBullet*)pEvent->GetParam())->GetDamage());

			if(GetHP() <= 0)
				MS->SendMsg(new CDestroyPlayerMessage(this));
			else
				MS->SendMsg(new CPlayerHitMessage(this));
		}
		return;
	}
	else if("camerachange" == pEvent->GetEventID())
	{
		CLevel* tLevel = (CLevel*)pEvent->GetParam();

		SetOffsetX(tLevel->GetOffsetX());
		SetOffsetY(tLevel->GetOffsetY());
		return;
	}	
	else if("enemyfire" == pEvent->GetEventID())
	{
		// Getting enemy
		CEnemy* pEnemy = (CEnemy*)pEvent->GetParam();

		// Hand vector
		tVector2D tShootVector;
		tShootVector.fX = 1.0f;
		tShootVector.fY = 0.0f;
	
		// Player position (with randomness to enemy accuracy)
		tVector2D tPlayerpos;
		tPlayerpos.fX = GetPosX() + ((rand() % 21) - 10);
		tPlayerpos.fY = GetPosY() + ((rand() % 21) - 10);

		// Getting vector between enemy position and player position
		tPlayerpos.fX = tPlayerpos.fX - (pEnemy->GetPosX()  + (pEnemy->GetWidth() >> 1));
		tPlayerpos.fY = tPlayerpos.fY - (pEnemy->GetPosY()  + (pEnemy->GetHeight() >> 1));

		float fFireAngle = 0.0f;

		fFireAngle = AngleBetweenVectors(tPlayerpos,tShootVector);

		if(GetPosY() < (pEnemy->GetPosY() + (pEnemy->GetHeight() >>1)))
		{
			fFireAngle = -1 * fFireAngle;	
		}

		MS->SendMsg(new CCreateBulletMessage(pEnemy,fFireAngle,pEnemy->GetBulletID()));

		return;
	}	
	else if("invincible" == pEvent->GetEventID())
	{
		SetHP(GetMaxHP());

		// Toggleing invince
		m_bGod = !m_bGod;

		if(m_bGod)
			XAUDIO->SFXPlaySound(m_nGodOnID);
		else
			XAUDIO->SFXPlaySound(m_nGodOffID);
	}
	else if("shotty" == pEvent->GetEventID())
	{
		m_bShotty = !m_bShotty;

		if(m_bShotty)
			XAUDIO->SFXPlaySound(m_nShottyOnID);
		else
			XAUDIO->SFXPlaySound(m_nShottyOffID);
	}
	else if("lives" == pEvent->GetEventID())
	{
		XAUDIO->SFXPlaySound(m_nLivesID);
	}
}

bool CPlayer::CheckCollision(IBaseInterface* pBase)
{
	RECT rTemp;
	
	if(pBase->GetType() == OBJ_WORLD)
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
					SetState(REST);
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

	

	return false;
}

void CPlayer::Respawn()
{
	m_bRespawning = true;
	m_fRespawnTime = 0.0f;

	SetHP(GetMaxHP());
}

////////////////////////////////////////
//		PRIVATE UTILITY FUNCTIONS
////////////////////////////////////////

void CPlayer::HandAiming()
{
	// Updating hands aiming
	if(!m_bShotty)
	{
		m_tHands.fX = GetPosX() + GetWidth();
		m_tHands.fY = GetPosY() + (GetHeight() >> 1);
	}
	else
	{
		// If using shotty, centering it better
		m_tHands.fX = GetPosX() + (GetWidth() >> 1) - 11;
		m_tHands.fY = GetPosY() + (GetHeight() >> 1) - 40;
	}
	

	// Hand vector
	tVector2D tHandStart;
	tHandStart.fX = 1.0f;
	tHandStart.fY = 0.0f;
	
	// Mouse screen position
	tVector2D tMouse;
	tMouse.fX = (CGame::GetInstance()->GetMousePos().x + GetOffsetX());
	tMouse.fY = (CGame::GetInstance()->GetMousePos().y + GetOffsetY());

	// Getting vector between our position and mouse position
	tMouse.fX = tMouse.fX - (GetPosX() + (GetWidth() >> 1));
	tMouse.fY = tMouse.fY - (GetPosY() + (GetHeight() >> 1));

	m_fHands = AngleBetweenVectors(tMouse,tHandStart);

	if((CGame::GetInstance()->GetMousePos().y + GetOffsetY()) < (GetPosY() + (GetHeight() >>1)))
	{
		m_fHands = -1 * m_fHands;	
	}
}

RECT CPlayer::GetCollisionRect()
{
	if(GetState() == CROUCH)
	{
		RECT rTemp;
		rTemp.left = (int)GetPosX();
		rTemp.top = (int)GetPosY();
		rTemp.right = (int)(GetPosX() + (GetWidth() >> 1) - 5);
		rTemp.bottom = (int)(GetPosY() + (GetHeight() >> 1) - 5);

		return rTemp;
	}
	else
		return CBase::GetCollisionRect();
}

////////////////////////////////////////
//	    PUBLIC ACCESSORS / MUTATORS
////////////////////////////////////////

////////////////////////////////////////
//	    PRIVATE ACCESSORS / MUTATORS
////////////////////////////////////////

void CPlayer::SetState(State eState)
{
	// Saving previous state
	static State lastState = REST;

	lastState = m_eState;

	if(eState == CROUCH && lastState != CROUCH)
	{
		SetPosY(GetPosY() + (GetHeight() >> 1) - 5);
		SetVelX(0);
	}
	if(eState != CROUCH && lastState == CROUCH)
	{
		SetPosY(GetPosY() - (GetHeight() >> 1) + 5);
	}

	m_eState = eState;
}

void CPlayer::SetPlatform(CWorld* pPlatform)
{
	if(pPlatform == GetPlatform())
		return;

	if(GetPlatform() != NULL)
		GetPlatform()->Release();

	m_pPlatform = pPlatform;

	if(GetPlatform() != NULL)
		GetPlatform()->AddRef();
}



