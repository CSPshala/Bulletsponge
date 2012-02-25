///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CCreditsState.cpp"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	Handle credit display
///////////////////////////////////////////////////////////////////////////

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "../Macros.h"
#include "CCreditsState.h"
#include "COptionsState.h"
#include "../CGame.h"
#include "../SGD Wrappers/CSGD_DirectInput.h"
#include "../SGD Wrappers/CSGD_TextureManager.h"
#include "../SGD Wrappers/CSGD_Direct3D.h"
#include "../CSGD_ObjectManager.h"
#include "../factories/CSGD_ObjectFactory.h"
#include "../models/CEnemy.h"
#include <fstream>
using std::fstream;
using std::getline;
////////////////////////////////////////
//				MISC
////////////////////////////////////////
// Static instantiation / definition
CCreditsState* CCreditsState::sm_pInstance = NULL;
///////////////////////////////////////////////
//  CONSTRUCTOR / DECONSTRUCT / OP OVERLOADS
///////////////////////////////////////////////
CCreditsState::CCreditsState()
{	
	m_pFont = NULL;
	m_pNumbers = NULL;
	m_bInitOnce = false;	
	m_fSpawnTime = 0.0f;
	m_fScrollTimer = 0.0f;
}

CCreditsState::~CCreditsState()
{
	// Cleaning up font
	SAFE_DELETE(m_pFont);
}

////////////////////////////////////////
//		PUBLIC UTILITY FUNCTIONS
////////////////////////////////////////
void CCreditsState::Enter(void)
{
	if(!m_bInitOnce)
	{
		m_nFontID = CSGD_TextureManager::GetInstance()->LoadTexture("resource/graphics/JCR_menutext.png");
		m_nNumbersID = TEXTUREMAN->LoadTexture("resource/graphics/JCR_numberfont.png");		
		m_pFont = new CBitmapFont(33,32,8,m_nFontID,'A');	
		m_pNumbers = new CBitmapFont(64,64,10,m_nNumbersID,'0');		
		
		// Music	
		m_nBGMID = XAUDIO->MusicLoadSong("resource/sound/JCR_guardiacastle.xwm");	

		// GRABBING GAME SETTINGS
		XAUDIO->MusicSetMasterVolume(COptionsState::GetInstance()->GetMusicVolume());
		XAUDIO->SFXSetMasterVolume(COptionsState::GetInstance()->GetSFXVolume());

		// Timer
		m_fScrollTimer = 0.0f;

		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->RegisterClassType<CBase>("CBase");		
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->RegisterClassType<CEnemy>("CEnemy");	

		// Setting Message System proc
		MS->InitMessageSystem(CCreditsState::MessageProc);		

		// Credits vector gettin' jammed full o awesome
		m_vCredits.push_back("     BULLETSPONGE");
		m_vCredits.push_back(" ");		
		m_vCredits.push_back("CREATED BY]");
		m_vCredits.push_back("    JC RICKS");
		m_vCredits.push_back(" ");
		m_vCredits.push_back("DRAWN BY]");	
		m_vCredits.push_back("    JC RICKS");
		m_vCredits.push_back(" ");
		m_vCredits.push_back("MUSIC AND SOUND BY]");
		m_vCredits.push_back("    FLASHKIT[COM");
		m_vCredits.push_back("    ONEUP STUDIOS");
		m_vCredits.push_back(" ");
		m_vCredits.push_back("INSTRUCTOR]");
		m_vCredits.push_back("    DAVID BROWN");

		m_bInitOnce = true;
	}
}

bool CCreditsState::Input(void)
{	
	if(DI->KeyPressed(DIK_ESCAPE))
		GAME->ChangeState(CMainMenuState::GetInstance());	

	return true;
}

void CCreditsState::Update(void)
{
	if(!XAUDIO->MusicIsSongPlaying(m_nBGMID))
		XAUDIO->MusicPlaySong(m_nBGMID,true);

	m_fScrollTimer += GAME->GetElapsedTime();


	OM->UpdateObjects(GAME->GetElapsedTime());

	if(m_fSpawnTime > 2.0f)
	{
		MS->SendMsg(new CCreateEnemyMessage());
		m_fSpawnTime = 0.0f;
	}

	m_fSpawnTime += GAME->GetElapsedTime();

	MS->ProcessMessages();

	if(0 > (GAME->GetWindowHeight() - m_fScrollTimer * 10) + (m_vCredits.size() - 1) * 60)
		GAME->ChangeState(CMainMenuState::GetInstance());
}

void CCreditsState::Render(void)
{
	CSGD_Direct3D::GetInstance()->Clear(128,128,128);
	CSGD_Direct3D::GetInstance()->DeviceBegin();
	CSGD_Direct3D::GetInstance()->SpriteBegin();	

	OM->RenderObjects();

	CSGD_Direct3D::GetInstance()->GetSprite()->Flush();	// Draw everything now that is queued up

	for(unsigned int i = 0; i < m_vCredits.size(); ++i)
	{		
		m_pFont->Print(m_vCredits[i],10,(float)(GAME->GetWindowHeight() - m_fScrollTimer * 10) + i * 60,1,1.0f);
	}

	CSGD_Direct3D::GetInstance()->SpriteEnd();
	CSGD_Direct3D::GetInstance()->DeviceEnd();
	CSGD_Direct3D::GetInstance()->Present();
}

void CCreditsState::Exit(void) 
{
	if(m_bInitOnce)
	{
		if(m_nFontID > -1)
			CSGD_TextureManager::GetInstance()->UnloadTexture(m_nFontID);

		SAFE_DELETE(m_pFont);
		SAFE_DELETE(m_pNumbers);
				
		m_nFontID = -1;
		m_pFont = NULL;
		m_pNumbers = NULL;
		m_bInitOnce = false;

		if(XAUDIO->MusicIsSongPlaying(m_nBGMID))
			XAUDIO->MusicStopSong(m_nBGMID);		

		XAUDIO->MusicUnloadSong(m_nBGMID);	

		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->UnregisterClassType("CBase");	
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->UnregisterClassType("CEnemy");	

		m_vCredits.clear();

		OM->RemoveAllObjects();
		MS->ClearMessages();
	}
}

CCreditsState* CCreditsState::GetInstance()
{
	if(sm_pInstance == NULL)
		sm_pInstance = new CCreditsState();

	return sm_pInstance;
}

void CCreditsState::DeleteInstance()
{
	if(sm_pInstance)
	{
		sm_pInstance->Exit();

		delete sm_pInstance;
		sm_pInstance = NULL;
	}
}

void CCreditsState::MessageProc(CBaseMessage* pMsg)
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

////////////////////////////////////////
//	    PUBLIC ACCESSORS / MUTATORS
////////////////////////////////////////

////////////////////////////////////////
//	    PRIVATE ACCESSORS / MUTATORS
////////////////////////////////////////