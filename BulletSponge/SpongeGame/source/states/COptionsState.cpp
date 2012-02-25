///////////////////////////////////////////////////////////////////////////
//	File Name	:	"COptionsState.cpp"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	Handle the options menu
///////////////////////////////////////////////////////////////////////////

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "COptionsState.h"
#include "../models/CBitmapFont.h"
#include "../models/CEnemy.h"
#include "../CGame.h"
#include "../factories/CSGD_ObjectFactory.h"
#include "../CSGD_ObjectManager.h"
#include "../SGD Wrappers/CSGD_TextureManager.h"
#include "../SGD Wrappers/CSGD_DirectInput.h"
#include "../SGD Wrappers/CSGD_XAudio2.h"

#include <iomanip>
#include <fstream>
#include <string>
using std::string;
using std::fstream;


////////////////////////////////////////
//				MISC
////////////////////////////////////////
// Static instantiation / definition
COptionsState* COptionsState::sm_pInstance = NULL;

///////////////////////////////////////////////
//  CONSTRUCTOR / DECONSTRUCT / OP OVERLOADS
///////////////////////////////////////////////
COptionsState::COptionsState()
{
	m_nCursorID = -1;
	m_pFont = NULL;
	m_pNumbers = NULL;
	m_bInitOnce = false;
	m_fSpawnTime = 0.0f;
}

COptionsState::~COptionsState()
{
	// Cleaning up font
	SAFE_DELETE(m_pFont);
}

////////////////////////////////////////
//		PUBLIC UTILITY FUNCTIONS
////////////////////////////////////////
void COptionsState::Enter(void)
{
	if(!m_bInitOnce)
	{
		m_nCursorID = CSGD_TextureManager::GetInstance()->LoadTexture("resource/graphics/JCR_mainarrow.png");
		m_nFontID = CSGD_TextureManager::GetInstance()->LoadTexture("resource/graphics/JCR_menutext.png");
		m_nNumbersID = TEXTUREMAN->LoadTexture("resource/graphics/JCR_numberfont.png");
		m_nSelection = 0;
		m_pFont = new CBitmapFont(33,32,8,m_nFontID,'A');	
		m_pNumbers = new CBitmapFont(64,64,10,m_nNumbersID,'0');
		m_bInitOnce = true;
		m_fChangeTimer = 0.2f;

		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->RegisterClassType<CBase>("CBase");		
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->RegisterClassType<CEnemy>("CEnemy");	

		// Setting Message System proc
		MS->InitMessageSystem(COptionsState::MessageProc);

		// Gunshot
		m_nShotFX = XAUDIO->SFXLoadSound("resource/sound/JCR_10mm.wav");
		// Music	
		m_nBGMID = XAUDIO->MusicLoadSong("resource/sound/JCR_enemydrawsnear.xwm");

		// Volume Setting load
		LoadSettings();

		m_bInitOnce = true;
	}
}

bool COptionsState::Input(void)
{
	if(DI->KeyPressed(DIK_UP))
	{
		--m_nSelection;	

		if(m_nSelection < 0)
			m_nSelection = 3;

		if(XAUDIO->MusicIsSongPlaying(m_nBGMID))
			XAUDIO->MusicStopSong(m_nBGMID);
		if(XAUDIO->SFXIsSoundPlaying(m_nShotFX))
			XAUDIO->SFXStopSound(m_nShotFX);
	}

	if(DI->KeyPressed(DIK_DOWN))
	{
		++m_nSelection;	

		if(m_nSelection >  3)
			m_nSelection = 0;

		if(XAUDIO->MusicIsSongPlaying(m_nBGMID))
			XAUDIO->MusicStopSong(m_nBGMID);
		if(XAUDIO->SFXIsSoundPlaying(m_nShotFX))
			XAUDIO->SFXStopSound(m_nShotFX);
	}

	if(DI->KeyDown(DIK_LEFT))
	{
		if(m_fChangeTimer >= 0.08f)
		{
			if(m_nSelection == 0 && m_fMusicVolume > 0.01f)
			{
				m_fMusicVolume -= 0.01f;
				if(!XAUDIO->MusicIsSongPlaying(m_nBGMID))
					XAUDIO->MusicPlaySong(m_nBGMID);

				XAUDIO->MusicSetMasterVolume(m_fMusicVolume);
				m_fChangeTimer = 0.0f;
			}
			else if(m_nSelection == 1 && m_fSFXVolume > 0.01f)
			{
				m_fSFXVolume -= 0.01f;

				XAUDIO->SFXPlaySound(m_nShotFX);

				XAUDIO->SFXSetMasterVolume(m_fSFXVolume);
				m_fChangeTimer = 0.0f;
			}
			else if(m_nSelection == 2 && m_nDifficulty > 0)
			{
				--m_nDifficulty;
				m_fChangeTimer = 0.0f;
			}
			else if(m_nSelection == 3 && m_nEnemyTarget > 0)
			{
				// Making it faster to scroll through numbers on enemies
				--m_nEnemyTarget;
				m_fChangeTimer = 0.07f;
			}
			
		}
	}
	
	if(DI->KeyDown(DIK_RIGHT))
	{
		if(m_fChangeTimer >= 0.08f)
		{
			if(m_nSelection == 0 && m_fMusicVolume < 0.99f)
			{
				m_fMusicVolume += 0.01f;

				if(!XAUDIO->MusicIsSongPlaying(m_nBGMID))
					XAUDIO->MusicPlaySong(m_nBGMID);

				XAUDIO->MusicSetMasterVolume(m_fMusicVolume);
				m_fChangeTimer = 0.0f;
			}
			else if(m_nSelection == 1 && m_fSFXVolume < 0.99f)
			{
				m_fSFXVolume += 0.01f;
								
				XAUDIO->SFXPlaySound(m_nShotFX);

				XAUDIO->SFXSetMasterVolume(m_fSFXVolume);
				m_fChangeTimer = 0.0f;
			}
			else if(m_nSelection == 2 && m_nDifficulty < 2)
			{
				++m_nDifficulty;
				m_fChangeTimer = 0.0f;
			}
			else if(m_nSelection == 3 && m_nEnemyTarget < 20000)
			{
				// Making it faster to scroll through numbers on enemies
				++m_nEnemyTarget;
				m_fChangeTimer = 0.07f;
			}			
		}
	}

	// Leaving to main menu
	if(DI->KeyPressed(DIK_ESCAPE))
		GAME->ChangeState(CMainMenuState::GetInstance());

	// Updating timer
	m_fChangeTimer += GAME->GetTimer().GetDeltaTime();

	return true;
}

void COptionsState::Update(void)
{
	OM->UpdateObjects(GAME->GetTimer().GetDeltaTime());

	if(m_fSpawnTime > 2.0f)
	{
		MS->SendMsg(new CCreateEnemyMessage());
		m_fSpawnTime = 0.0f;
	}

	m_fSpawnTime += GAME->GetTimer().GetDeltaTime();

	MS->ProcessMessages();
}

void COptionsState::Render(void)
{
	CSGD_Direct3D::GetInstance()->Clear(128,128,128);
	CSGD_Direct3D::GetInstance()->DeviceBegin();
	CSGD_Direct3D::GetInstance()->SpriteBegin();	

	OM->RenderObjects();

	CSGD_Direct3D::GetInstance()->GetSprite()->Flush();	// Draw everything now that is queued up

	TEXTUREMAN->DrawF(m_nCursorID,((float)GAME->GetWindowWidth() / 4.0f) - 42,(float)(m_pFont->GetCharHeight() * (3 * m_nSelection) + 1) + 30);

	char musicBuffer[128] = {0};
	char sfxBuffer[128] = {0};
	char killsBuffer[256] = {0};
	// Rounding to closest int using floorf and multiplication
	int musicLevel =(int)floorf(m_fMusicVolume * 100 + 0.5f);
	int sfxLevel =(int)floorf(m_fSFXVolume * 100 + 0.5f);
	_itoa_s(musicLevel,musicBuffer,10);
	_itoa_s(sfxLevel,sfxBuffer,10);
	_itoa_s(m_nEnemyTarget,killsBuffer,10);

	// What a mess on this spacing
	m_pFont->Print("MUSIC VOLUME",(float)GAME->GetWindowWidth() / 4.0f,(float)(m_pFont->GetCharHeight()) ,1,1.0f);
	m_pNumbers->Print(musicBuffer,(float)GAME->GetWindowWidth() / 4.0f + 64,(float)( m_pFont->GetCharHeight() * 2),0,1.0f);

	m_pFont->Print("SFX VOLUME",(float)GAME->GetWindowWidth() / 4.0f,(float)( m_pFont->GetCharHeight() * 4),1,1.0f);
	m_pNumbers->Print(sfxBuffer,(float)GAME->GetWindowWidth() / 4.0f + 64,(float)( m_pFont->GetCharHeight() * 5),0,1.0f);

	m_pFont->Print("DIFFICULTY",(float)GAME->GetWindowWidth() / 4.0f,(float)(m_pFont->GetCharHeight() * 7),1,1.0f);

	switch(m_nDifficulty)
	{
	case 0:
		m_pFont->Print("EASY",(float)GAME->GetWindowWidth() / 4.0f + 64,(float)( m_pFont->GetCharHeight() * 8),1,1.0f);
		break;
	case 1:
		m_pFont->Print("NORMAL",(float)GAME->GetWindowWidth() / 4.0f + 64,(float)( m_pFont->GetCharHeight() * 8),1,1.0f);
		break;
	case 2:
		m_pFont->Print("HARD",(float)GAME->GetWindowWidth() / 4.0f + 64,(float)( m_pFont->GetCharHeight() * 8),1,1.0f);
		break;
	};

	m_pFont->Print("TARGET KILLS",(float)GAME->GetWindowWidth() / 4.0f,(float)(m_pFont->GetCharHeight() * 10),1,1.0f);
	m_pNumbers->Print(killsBuffer,(float)GAME->GetWindowWidth() / 4.0f + 64,(float)( m_pFont->GetCharHeight() * 11),0,1.0f);

	CSGD_Direct3D::GetInstance()->SpriteEnd();
	CSGD_Direct3D::GetInstance()->DeviceEnd();
	CSGD_Direct3D::GetInstance()->Present();
}

void COptionsState::Exit(void) 
{
	if(m_bInitOnce)
	{
		if(m_nCursorID > -1)
			CSGD_TextureManager::GetInstance()->UnloadTexture(m_nCursorID);

		if(m_nFontID > -1)
			CSGD_TextureManager::GetInstance()->UnloadTexture(m_nFontID);

		SAFE_DELETE(m_pFont);
		SAFE_DELETE(m_pNumbers);

		m_nSelection = 0;
		m_fChangeTimer = 0.2f;
		m_fSpawnTime = 0.0f;
		m_nCursorID = -1;
		m_nFontID = -1;
		m_pFont = NULL;
		m_pNumbers = NULL;
		m_bInitOnce = false;

		if(XAUDIO->MusicIsSongPlaying(m_nBGMID))
			XAUDIO->MusicStopSong(m_nBGMID);
		if(XAUDIO->SFXIsSoundPlaying(m_nShotFX))
			XAUDIO->SFXStopSound(m_nShotFX);

		XAUDIO->MusicUnloadSong(m_nBGMID);
		XAUDIO->SFXUnloadSound(m_nShotFX);

		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->UnregisterClassType("CBase");	
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->UnregisterClassType("CEnemy");		

		OM->RemoveAllObjects();
		MS->ClearMessages();

		SaveSettings();
	}
}

COptionsState* COptionsState::GetInstance()
{
	if(sm_pInstance == NULL)
		sm_pInstance = new COptionsState();

	if(!sm_pInstance->m_bInitOnce)
		sm_pInstance->LoadSettings();

	return sm_pInstance;
}

void COptionsState::DeleteInstance()
{
	if(sm_pInstance)
	{
		sm_pInstance->Exit();

		delete sm_pInstance;
		sm_pInstance = NULL;
	}
}

void COptionsState::MessageProc(CBaseMessage* pMsg)
{
	switch(pMsg->GetMsgID())
	{	
	case MSG_CREATE_ENEMY:
		{
			// Creating player
			CEnemy* tempEnemy = (CEnemy*)CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->CreateObject("CEnemy");
			tempEnemy->SetPosX((float)(rand() % (int)CGame::GetInstance()->GetWindowWidth()));
			tempEnemy->SetPosY(-128);			
			tempEnemy->SetWidth(64);
			tempEnemy->SetHeight(128);
			tempEnemy->SetMainMenu(true);
			
			OM->AddObject(tempEnemy);
			tempEnemy->Release();	
		}
		break;

	case MSG_DESTROY_ENEMY:
		{			
			// Killing enemy
			CDestroyEnemyMessage* pDEM = (CDestroyEnemyMessage*)pMsg;
			pDEM->GetEnemy()->SetIsActive(false);			
		}
		break;
	}
}

////////////////////////////////////////
//		PRIVATE UTILITY FUNCTIONS
////////////////////////////////////////
void COptionsState::LoadSettings()
{

	fstream fin = fstream("resource/settings.ini",ios_base::in);

	if(fin.is_open())
	{		
		fin >> m_fMusicVolume;
		fin >> m_fSFXVolume;	
		fin >> m_nDifficulty;
		fin >> m_nEnemyTarget;

		fin.close();
	}
	else
	{
		m_fMusicVolume = XAUDIO->MusicGetMasterVolume();
		m_fSFXVolume = XAUDIO->SFXGetMasterVolume();
		m_nDifficulty = 1;
		m_nEnemyTarget = 50;
	}
}

void COptionsState::SaveSettings()
{
	fstream fout = fstream("resource/settings.ini",ios_base::out | ios_base::trunc);
	
	if(fout.is_open())
	{
		fout << std::setprecision(2) << m_fMusicVolume << '\n';
		fout << std::setprecision(2) << m_fSFXVolume << '\n';
		fout << m_nDifficulty << '\n';
		fout << m_nEnemyTarget << '\n';		

		fout.close();
	}
	else
		return;
	
}

////////////////////////////////////////
//	    PUBLIC ACCESSORS / MUTATORS
////////////////////////////////////////

////////////////////////////////////////
//	    PRIVATE ACCESSORS / MUTATORS
////////////////////////////////////////