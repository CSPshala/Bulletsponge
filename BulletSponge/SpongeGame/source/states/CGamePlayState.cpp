///////////////////////////////////////////////////////
// File Name	:	"CGamePlayState.h"
//
// Author Name	:	JC Ricks
//
// Purpose		:	To control the playstate
//////////////////////////////////////////////////////

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "../Macros.h"
#include "CGamePlayState.h"
#include "CHighScoreState.h"
#include <string.h>
using namespace std;
#include "../models/CLevel.h"
#include "COptionsState.h"
#include "../messaging/CSGD_EventSystem.h"
// OBJECTS
#include "../models/CBase.h"
#include "../models/CLevel.h"
#include "../models/CWorld.h"
#include "../models/CPlayer.h"
#include "../models/CEnemy.h"
#include "../models/CBullet.h"
#include "../models/CRobot.h"

////////////////////////////////////////
//				MISC
////////////////////////////////////////
// Static instantiation / define
CGamePlayState* CGamePlayState::sm_pInstance = NULL;

///////////////////////////////////////////////
//  CONSTRUCTOR / DECONSTRUCT / OP OVERLOADS
///////////////////////////////////////////////
CGamePlayState::CGamePlayState()
{
	// Sounds	

	// Alive
	SetIsDead(false);	

	// Boxes
	m_bRenderBoxes = false;

	// Crosshair
	m_nCrosshair = -1;

	// Pause
	SetIsPaused(false);
	m_nSelection = 0;

	SetDifficulty(HARD);
	SetEnemyCount(0);
	SetEnemyTimer(0.0f);

	// Init
	m_bInitOnce = false;

	// Font
	m_pFont = NULL;
	m_nFontID = -1;
	m_pNumbers = NULL;
	m_nNumbersID = -1;

	// Score
	m_nScore = 0;

	// Player Lives
	m_nLives = 3;
	
}

CGamePlayState::~CGamePlayState()
{

}

////////////////////////////////////////
//		PUBLIC UTILITY FUNCTIONS
////////////////////////////////////////

void CGamePlayState::Enter(void)
{
	if(!m_bInitOnce)
	{
		// Load Cursor
		m_nCursorID = CSGD_TextureManager::GetInstance()->LoadTexture("resource/graphics/JCR_pausearrow.png");

		// Lives Icon
		m_nLivesID = TEXTUREMAN->LoadTexture("resource/graphics/JCR_livesicon.png");

		// Getting font
		m_nFontID = TEXTUREMAN->LoadTexture("resource/graphics/JCR_menutext.png");
		m_pFont = new CBitmapFont(33,32,8,m_nFontID,'A');
		m_nNumbersID = TEXTUREMAN->LoadTexture("resource/graphics/JCR_numberfont.png");
		m_pNumbers = new CBitmapFont(64,64,10,m_nNumbersID,'0');

		// Crosshair
		m_nCrosshair = TEXTUREMAN->LoadTexture("resource/graphics/JCR_crosshair.png");
		DI->MouseSetPosX(0);
		DI->MouseSetPosY(0);
	
		// Object Factory register
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->RegisterClassType<CBase>("CBase");
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->RegisterClassType<CWorld>("CWorld");
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->RegisterClassType<CPlayer>("CPlayer");
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->RegisterClassType<CEnemy>("CEnemy");
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->RegisterClassType<CBullet>("CBullet");
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->RegisterClassType<CRobot>("CRobot");

		// Setting Message System proc
		MS->InitMessageSystem(CGamePlayState::MessageProc);	

		// Registering for Events
		ES->RegisterClient("invincible",this);
		ES->RegisterClient("shotty",this);
		ES->RegisterClient("lives",this);

		m_nWinID = XAUDIO->SFXLoadSound("resource/sound/JCR_win.wav");
		m_nLoseID = XAUDIO->SFXLoadSound("resource/sound/JCR_lose.wav");
		m_nLostLifeID = XAUDIO->SFXLoadSound("resource/sound/JCR_lostlife.wav");

		// Creating level
		m_pCurLevel = new CLevel();
		m_pCurLevel->LoadLevel("resource/levels/JCR_citynight.jct");
		//m_pCurLevel->LoadLevel("resource/levels/JCR_citylarge.jct");
		//m_pCurLevel->LoadLevel("resource/levels/JCR_citytest.jct");

		// Creating player
		MS->SendMsg(new CCreatePlayerMessage());

		// HP Rect
		m_rHP.left = (LONG)((3 * m_pFont->GetCharWidth()) * 0.5f);
		m_rHP.top = GAME->GetWindowHeight() - 40;
		m_rHP.bottom = m_rHP.top + 30;
		m_rHP.right = m_rHP.left + (0 + (300-0) * (100/100));

		// Player Specifics
		m_nLives = 4;
		m_nEnemiesKilled = 0;
		m_bHasWon = false;
		m_fWinTimer = 0.0f;
		m_fLossTimer = 0.0f;

		// Cheats
		m_szGodCheat = "dog";
		m_szShottyCheat = "boomstick";
		m_szLivesCheat = "isuck";

		// GRABBING GAME SETTINGS
		XAUDIO->MusicSetMasterVolume(COptionsState::GetInstance()->GetMusicVolume());
		XAUDIO->SFXSetMasterVolume(COptionsState::GetInstance()->GetSFXVolume());
		m_eDifficulty = (Difficulty)COptionsState::GetInstance()->GetDifficulty();
		m_nTarget = COptionsState::GetInstance()->GetEnemyTarget();

		m_bInitOnce = true;
	}
}

bool CGamePlayState::Input(void)
{	
	if(!GetIsDead())
	{
		if(!GetIsPaused())
		{
			if(DI->KeyPressed(DIK_1))
			{
				MS->SendMsg(new CCreateRobotMessage());				
			}


			if(DI->KeyPressed(DIK_ESCAPE))
			{
				SetIsPaused(true);
			}
			else if(isalpha(DI->CheckBufferedKeysEx()))
			{																
				m_qInputBuffer.push(DI->CheckBufferedKeysEx());

				DI->ClearInput();
			}

			if(DI->KeyPressed(DIK_F1))
				SetRenderBoxes(true);
			if(DI->KeyPressed(DIK_F2))
				SetRenderBoxes(false);
	 
			// Using passed in Winproc stuff to set mouse pos
			DI->MouseSetPosX(CGame::GetInstance()->GetMousePos().x);
			DI->MouseSetPosY(CGame::GetInstance()->GetMousePos().y);

			
		}
		else
		{
			if(DI->KeyPressed(DIK_P))
			{
				SetIsPaused(false);
			}

			if(DI->KeyPressed(DIK_UP))
			{
				--m_nSelection;	

				if(m_nSelection < 0)
					m_nSelection = 1;
			}

			if(DI->KeyPressed(DIK_DOWN))
			{
				++m_nSelection;	

				if(m_nSelection >  1)
					m_nSelection = 0;
			}

			if(CSGD_DirectInput::GetInstance()->KeyPressed(DIK_RETURN))
			{
				if(m_nSelection == 0)
					SetIsPaused(false);
				else if(m_nSelection == 1)
				{					
					CGame::GetInstance()->ChangeState(CMainMenuState::GetInstance());	
				}
			}
		}
	}	

	

	return true;
}

void CGamePlayState::Update(void)
{
	if(!GetIsPaused() && !GetIsDead() && !m_bHasWon)
	{	
		// Level update really only plays BGM
			m_pCurLevel->Update();

		// Making tons of enemies
			GenerateEnemies();

		// Adding a single char to the string from the queue
	    // comparing to cheats
		if(!m_qInputBuffer.empty())
		{
			// Adding to cheat queues
			m_vGodCheat.push_back(m_qInputBuffer.front());
			m_vShottyCheat.push_back(m_qInputBuffer.front());
			m_vLivesCheat.push_back(m_qInputBuffer.front());

			// Clearing out first letter in input
			m_qInputBuffer.pop();			

			if(m_vGodCheat.size() == 3)
			{
				for(unsigned int i = 0; i < m_vGodCheat.size(); ++i)
				{
					// Comparing letters
					if(m_vGodCheat[i] != m_szGodCheat[i])
						break;
					else if(i == 2)
						ES->SendEvent("invincible",NULL);
				}

				// Getting rid of first letter
				m_vGodCheat.erase(m_vGodCheat.begin());
			}

			if(m_vShottyCheat.size() == 9)
			{
				for(unsigned int i = 0; i < m_vShottyCheat.size(); ++i)
				{
					if(m_vShottyCheat[i] != m_szShottyCheat[i])
						break;
					else if(i == 8)
						ES->SendEvent("shotty",NULL);
				}
				
				// Purge
				m_vShottyCheat.erase(m_vShottyCheat.begin());
			}

			if(m_vLivesCheat.size() == 5)
			{
				for(unsigned int i = 0; i < m_vLivesCheat.size(); ++i)
				{
					if(m_vLivesCheat[i] != m_szLivesCheat[i])
						break;
					else if(i == 4)
					{
						// adding 5 lives
						ES->SendEvent("lives",NULL);
						m_nLives += 5;
					}
				}

				// Purge
				m_vLivesCheat.erase(m_vLivesCheat.begin());
			}
				
		}

		// Checking for win
		if(m_nEnemiesKilled >= m_nTarget)
		{
			m_bHasWon = true;
			XAUDIO->SFXPlaySound(m_nWinID);
		}
		
		XAUDIO->Update();
		OM->UpdateObjects(CGame::GetInstance()->GetTimer().GetDeltaTime());
		OM->CheckCollisions();
		ES->ProcessEvents();
		MS->ProcessMessages();
	}
	else if(GetIsDead())
	{
		if(m_fLossTimer > 10.0f)
		{
			CHighScoreState::GetInstance()->SetInputMode(true);
			CHighScoreState::GetInstance()->SetInputScore(m_nScore);
			GAME->ChangeState(CHighScoreState::GetInstance());
		}		

		m_fLossTimer += GAME->GetTimer().GetDeltaTime();
	}
	else if(m_bHasWon)
	{		
		if(m_fWinTimer > 10.0f)
		{
			// Multiplier based on difficulty
			m_nScore = m_nScore * (int)(GetDifficulty() + 1);
			CHighScoreState::GetInstance()->SetInputMode(true);
			CHighScoreState::GetInstance()->SetInputScore(m_nScore);
			GAME->ChangeState(CHighScoreState::GetInstance());
		}			

		m_fWinTimer += GAME->GetTimer().GetDeltaTime();
	}
}

void CGamePlayState::Render(void)
{
	D3D->Clear(135,206,250);
	D3D->DeviceBegin();
	D3D->SpriteBegin();

	m_pCurLevel->Render();
	OM->RenderObjects();
	
		// Drawing crosshair
	TEXTUREMAN->Draw(m_nCrosshair,DI->MouseGetPosX() - (TEXTUREMAN->GetTextureWidth(m_nCrosshair) >> 1),
		DI->MouseGetPosY() - (TEXTUREMAN->GetTextureHeight(m_nCrosshair) >> 1));	

	// Drawing score / Kills / Target kills
	char buffer[128];	
	char lives[10] = {0};
	char kills[256] = {0};
	char target[256] = {0};

	sprintf_s(buffer,_countof(buffer),"%i",GetScore());
	sprintf_s(lives,_countof(lives),"%i",m_nLives);
	sprintf_s(kills,_countof(kills),"%i",m_nEnemiesKilled);
	sprintf_s(target,_countof(target),"%i",m_nTarget);

	m_pFont->Print("SCORE",1.0f,1.0f,1,0.5f);
	m_pNumbers->Print(buffer,float(m_pFont->GetCharWidth() * 6) * 0.5f,1.0f,1,0.3f);

	m_pFont->Print("KILLS",200.0f,1.0f,1,0.5f);
	m_pNumbers->Print(kills,200.0f + float(m_pFont->GetCharWidthA() * 6) * 0.5f,1.0f,1,0.3f);

	m_pFont->Print("GOAL",400.0f,1.0f,1,0.5f);
	m_pNumbers->Print(target,400.0f + float(m_pFont->GetCharWidthA() * 5) * 0.5f,1.0f,1,0.3f);

	// Drawing Life count
	TEXTUREMAN->Draw(m_nLivesID,GAME->GetWindowWidth() - 128,1,0.6f,0.7f);
	m_pNumbers->Print(lives,(float)(GAME->GetWindowWidth() - 64),1.0f,1,0.3f);

	if(GetIsDead())
	{
		m_pFont->Print("YOU ARE DEAD//",20,float(CGame::GetInstance()->GetWindowHeight() >> 1) - 50,1,0.9f);
		m_pFont->Print("GOING TO HIGHSCORES",20,float((CGame::GetInstance()->GetWindowHeight() >> 1) + 25), 1,0.8f);
		m_pFont->Print("TO SEE WHOS BETTER THAN YOU",20,float((CGame::GetInstance()->GetWindowHeight() >> 1) + 50), 1,0.8f);
	}
	else if(GetIsPaused())
	{
		m_pFont->Print("UNPAUSE",(float)(CGame::GetInstance()->GetWindowWidth() >> 1),(float)(CGame::GetInstance()->GetWindowHeight() >> 1),1,0.8f);
		m_pFont->Print("EXIT",(float)(CGame::GetInstance()->GetWindowWidth() >> 1),(float)((CGame::GetInstance()->GetWindowHeight() >> 1) + 20 + m_pFont->GetCharHeight()),1,0.8f);		
		TEXTUREMAN->Draw(m_nCursorID,(CGame::GetInstance()->GetWindowWidth() >> 1) - 42,(CGame::GetInstance()->GetWindowHeight() >> 1) + (20 + m_pFont->GetCharHeight() )* m_nSelection);
	}
	else if(m_bHasWon)
	{
		m_pFont->Print("YOU WIN//",20,float(CGame::GetInstance()->GetWindowHeight() >> 1) - 50,1,0.9f);
		m_pFont->Print("GOOD JOB COMMITING A WARCRIME",20,float((CGame::GetInstance()->GetWindowHeight() >> 1) + 25), 1,0.8f);
		m_pFont->Print("GOING TO HIGHSCORES",20,float((CGame::GetInstance()->GetWindowHeight() >> 1) + 50), 1,0.8f);
	}

	// Drawing HP Bar
	m_pFont->Print("HP ",1.0f,float(GAME->GetWindowHeight() - 30),1,0.7f);
	
	D3D->SpriteEnd();

	D3D->DrawRect(m_rHP,255,0,0);

	D3D->DeviceEnd();
	D3D->Present();
}

void CGamePlayState::Exit(void)
{
	if(m_bInitOnce)
	{
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->UnregisterClassType("CBase");
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->UnregisterClassType("CWorld");
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->UnregisterClassType("CPlayer");
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->UnregisterClassType("CEnemy");
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->UnregisterClassType("CBullet");
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->UnregisterClassType("CRobot");

		TEXTUREMAN->UnloadTexture(m_nCursorID);		
		TEXTUREMAN->UnloadTexture(m_nCrosshair);
		TEXTUREMAN->UnloadTexture(m_nFontID);
		TEXTUREMAN->UnloadTexture(m_nNumbersID);
		TEXTUREMAN->UnloadTexture(m_nLivesID);

		SAFE_DELETE(m_pFont);
		SAFE_DELETE(m_pNumbers);

		ES->UnregisterClient("invincible",this);
		ES->UnregisterClient("shotty",this);
		ES->UnregisterClient("lives",this);

		OM->RemoveAllObjects();	
	
		MS->ClearMessages();	
		ES->ClearEvents();
	
		SAFE_RELEASE(m_pCurLevel);
		m_pCurLevel = NULL;

		DI->MouseSetPosX(0);
		DI->MouseSetPosY(0);

		// Sounds	

		// Alive
		SetIsDead(false);	

		// Boxes
		m_bRenderBoxes = false;

		// Crosshair
		m_nCrosshair = -1;

		// Pause
		SetIsPaused(false);
		m_nSelection = 0;

		SetDifficulty(NORMAL);
		SetEnemyCount(0);
		SetEnemyTimer(0.0f);

		// Init
		m_bInitOnce = false;

		// Score
		m_nScore = 0;

		// Player Lives
		m_nLives = 4;
		m_bHasWon = false;		

		OM->RemoveAllObjects();
	}
}

CGamePlayState* CGamePlayState::GetInstance()
{
	if(sm_pInstance == NULL)
		sm_pInstance = new CGamePlayState();

	return sm_pInstance;
}

void CGamePlayState::DeleteInstance()
{
	if(sm_pInstance)
	{
		sm_pInstance->Exit();

		delete sm_pInstance;
		sm_pInstance = NULL;
	}
}

void CGamePlayState::MessageProc(CBaseMessage* pMsg)
{
	switch(pMsg->GetMsgID())
	{
	case MSG_CREATE_PLAYER:
		{
			CGamePlayState* pGameState = CGamePlayState::GetInstance();

			// Creating player
			CPlayer* tempPlayer = (CPlayer*)CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->CreateObject("CPlayer");
			tempPlayer->SetPosX((float)pGameState->m_pCurLevel->GetPlayerSpawnX());
			tempPlayer->SetPosY((float)pGameState->m_pCurLevel->GetPlayerSpawnY());
			tempPlayer->SetOffsetX(pGameState->m_pCurLevel->GetOffsetX());
			tempPlayer->SetOffsetY(pGameState->m_pCurLevel->GetOffsetY());
			tempPlayer->SetWidth(64);
			tempPlayer->SetHeight(111);
			tempPlayer->SetVelY(tempPlayer->GetMovementSpeedY());
			OM->AddObject(tempPlayer);
			tempPlayer->Release();	
		}
		break;

	case MSG_PLAYER_HIT:
		{
			CGamePlayState* pGameState = CGamePlayState::GetInstance();
			CPlayerHitMessage* pPHM = (CPlayerHitMessage*)pMsg;
			
			pGameState->m_rHP.bottom = pGameState->m_rHP.top + 30;
			pGameState->m_rHP.right = LONG(pGameState->m_rHP.left + (0 + (300 - 0) * ((float)pPHM->GetPlayer()->GetHP() / (float)pPHM->GetPlayer()->GetMaxHP())));
						//                                                ^---initial screen length of HP bar in pixels
		}
		break;

	case MSG_DESTROY_PLAYER:
		{
			CGamePlayState* pGameState = CGamePlayState::GetInstance();
			CDestroyPlayerMessage* pDPM = (CDestroyPlayerMessage*)pMsg;

			if(pGameState->m_nLives > 0)
			{
				pDPM->GetPlayer()->Respawn();
				pGameState->m_rHP.bottom = pGameState->m_rHP.top + 30;
				pGameState->m_rHP.right = LONG(pGameState->m_rHP.left + (0 + (300 - 0) * ((float)pDPM->GetPlayer()->GetHP() / (float)pDPM->GetPlayer()->GetMaxHP())));
				--pGameState->m_nLives;
				XAUDIO->SFXPlaySound(pGameState->m_nLostLifeID);
			}
			else			
			{
				pGameState->SetIsDead(true);	
				XAUDIO->SFXPlaySound(pGameState->m_nLoseID);
			}
		}
		break;

	case MSG_CREATE_ENEMY:
		{
			// Create random enemy in random spot on screen
			CGamePlayState* pGameState = CGamePlayState::GetInstance();

			// Creating player
			CEnemy* tempEnemy = (CEnemy*)CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->CreateObject("CEnemy");
			tempEnemy->SetPosX(rand() % (int)(pGameState->m_pCurLevel->GetOffsetX() + CGame::GetInstance()->GetWindowWidth() - pGameState->m_pCurLevel->GetOffsetX()) 
											+ pGameState->m_pCurLevel->GetOffsetX());
			tempEnemy->SetPosY(pGameState->m_pCurLevel->GetOffsetY() - 64);
			tempEnemy->SetOffsetX(pGameState->m_pCurLevel->GetOffsetX());
			tempEnemy->SetOffsetY(pGameState->m_pCurLevel->GetOffsetY());
			tempEnemy->SetWidth(64);
			tempEnemy->SetHeight(128);
			pGameState->SetEnemyCount(pGameState->GetEnemyCount() + 1);
			
			OM->AddObject(tempEnemy);
			tempEnemy->Release();	
		}
		break;

	case MSG_CREATE_ROBOT:
		{
			// Create random enemy in random spot on screen
			CGamePlayState* pGameState = CGamePlayState::GetInstance();

			// Creating player
			CRobot* tempRobot = (CRobot*)CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->CreateObject("CRobot");
			tempRobot->SetPosX(rand() % (int)(pGameState->m_pCurLevel->GetOffsetX() + CGame::GetInstance()->GetWindowWidth() - pGameState->m_pCurLevel->GetOffsetX()) 
											+ pGameState->m_pCurLevel->GetOffsetX());
			tempRobot->SetPosY(pGameState->m_pCurLevel->GetOffsetY() - 64);
			tempRobot->SetOffsetX(pGameState->m_pCurLevel->GetOffsetX());
			tempRobot->SetOffsetY(pGameState->m_pCurLevel->GetOffsetY());
			tempRobot->SetWidth(64);
			tempRobot->SetHeight(128);
			pGameState->SetEnemyCount(pGameState->GetEnemyCount() + 1);
			
			OM->AddObject(tempRobot);
			tempRobot->Release();	

		}
		break;

	case MSG_DESTROY_ENEMY:
		{				
			CGamePlayState* pGameState = CGamePlayState::GetInstance();

			// Killing enemy
			CDestroyEnemyMessage* pDEM = (CDestroyEnemyMessage*)pMsg;
			pDEM->GetEnemy()->SetIsActive(false);
			pGameState->SetEnemyCount(pGameState->GetEnemyCount() - 1);

			pGameState->SetScore(pGameState->GetScore() + 100);
			++pGameState->m_nEnemiesKilled;
		}
		break;

	case MSG_DESTROY_ROBOT:
		{
			CGamePlayState* pGameState = CGamePlayState::GetInstance();

			// Killing Robot
			CDestroyRobotMessage* pDEM = (CDestroyRobotMessage*)pMsg;
			pDEM->GetRobot()->SetIsActive(false);
			pGameState->SetEnemyCount(pGameState->GetEnemyCount() - 1);

			pGameState->SetScore(pGameState->GetScore() + 200);
			++pGameState->m_nEnemiesKilled;
		}
		break;

	case MSG_CREATE_BULLET:
		{
			CGamePlayState* pGameState = CGamePlayState::GetInstance();
			CCreateBulletMessage* pCBM = (CCreateBulletMessage*)pMsg;
			CBullet* pTempBullet = (CBullet*)CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->CreateObject("CBullet");
			pTempBullet->SetOwner(pCBM->GetOwner());
			pTempBullet->SetImageID(pCBM->GetImage());
			pTempBullet->SetOffsetX(pGameState->m_pCurLevel->GetOffsetX());
			pTempBullet->SetOffsetY(pGameState->m_pCurLevel->GetOffsetY());

			tVector2D tVec;
			tVec.fX = 1.0f;
			tVec.fY = 0.0f;

			tVec = Vector2DRotate(tVec,pCBM->GetAngle());			

			// Bullet power
			if(pCBM->GetOwner()->GetType() == OBJ_ENEMY || pCBM->GetOwner()->GetType() == OBJ_ROBOT)
			{
				tVec = tVec * 300.0f;
				pTempBullet->SetDamage(5);
			}
			else
			{
				tVec = tVec * 600.0f;				
			}

			pTempBullet->SetVelX(tVec.fX);
			pTempBullet->SetVelY(tVec.fY);

			// Setting rotation angle (mainly for screw facing)
			pTempBullet->SetRotation(pCBM->GetAngle());

			// Checking if it's a screw and whether it's being puked
			if(pCBM->GetOwner()->GetType() == OBJ_ROBOT)
			{
				pTempBullet->SetIsScrew(true);

				if(((CRobot*)pCBM->GetOwner())->GetState() == PUKE)
					pTempBullet->SetIsPuke(true);
			}

			OM->AddObject(pTempBullet);
			pTempBullet->Release();
		}
		break;

	case MSG_CREATE_PUKE:
		{


		}
		break;

	case MSG_DESTROY_BULLET:
		{
			// Getting rid of bullet
			CDestroyBulletMessage* pDBM = (CDestroyBulletMessage*)pMsg;
			pDBM->GetBullet()->SetIsActive(false);
		}
		break;
		
	}
}

////////////////////////////////////////
//		PRIVATE UTILITY FUNCTIONS
////////////////////////////////////////
void CGamePlayState::GenerateEnemies()
{
	switch(GetDifficulty())
	{
	case EASY:
		{ 
			if(GetEnemyCount() < 4)
				if(GetEnemyTimer() > 3.0f)				
				{
					MS->SendMsg(new CCreateEnemyMessage());
					SetEnemyTimer(0.0f);
				}		
		}
		break;

	case NORMAL:
		{
			if(GetEnemyCount() < 8)
				if(GetEnemyTimer() > 2.0f)				
				{
					if((rand() % 8) == 3)
					{
						// Making bots based on chance
						MS->SendMsg(new CCreateRobotMessage());
					}
					else
						MS->SendMsg(new CCreateEnemyMessage());

					SetEnemyTimer(0.0f);
				}			
		}
		break;

	case HARD:
		{
			if(GetEnemyCount() < 12)
				if(GetEnemyTimer() > 1.0f)				
				{
					if((rand() % 4) == 3)
					{
						// Making bots based on chance
						MS->SendMsg(new CCreateRobotMessage());
					}
					else
						MS->SendMsg(new CCreateEnemyMessage());


					SetEnemyTimer(0.0f);
				}		
		}
		break;
	}

	// Updating enemy timer
	SetEnemyTimer(GetEnemyTimer() + GAME->GetTimer().GetDeltaTime());
}

////////////////////////////////////////
//	    PUBLIC ACCESSORS / MUTATORS
////////////////////////////////////////

////////////////////////////////////////
//	    PRIVATE ACCESSORS / MUTATORS
////////////////////////////////////////