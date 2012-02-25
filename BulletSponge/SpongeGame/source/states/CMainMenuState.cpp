///////////////////////////////////////////////////////
// File Name	:	"CMainMenuState.cpp"
//
// Author Name	:	JC Ricks
//
// Purpose		:	To contain menu data and rendering
//////////////////////////////////////////////////////

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "CMainMenuState.h"
#include "COptionsState.h"
#include "CHighScoreState.h"
#include "CCreditsState.h"
#include "CInstructionsState.h"
#include "../CSGD_ObjectManager.h"
#include "../factories/CSGD_ObjectFactory.h"
#include "../models/CEnemy.h"

////////////////////////////////////////
//				MISC
////////////////////////////////////////
// Static instantiation / definition
CMainMenuState* CMainMenuState::sm_pInstance = NULL;

///////////////////////////////////////////////
//  CONSTRUCTOR / DECONSTRUCT / OP OVERLOADS
///////////////////////////////////////////////
CMainMenuState::CMainMenuState()
{
	m_nCursorID = -1;
	m_pFont = NULL;
	m_fPulse = 0.9f;
	m_bGrow = false;
	m_bInitOnce = false;
	m_fSpawnTime = 0.0f;
}

CMainMenuState::~CMainMenuState()
{
	// Cleaning up font
	SAFE_DELETE(m_pFont);
}

//		PUBLIC UTILITY FUNCTIONS
////////////////////////////////////////

void CMainMenuState::Enter(void)
{
	// Only initializes certain values once
	
	{
		// Clean up GamePlaystate (crap fix for now)
		//CGamePlayState::DeleteInstance();	

		m_nCursorID = CSGD_TextureManager::GetInstance()->LoadTexture("resource/graphics/JCR_mainarrow.png");
		m_nFontID = CSGD_TextureManager::GetInstance()->LoadTexture("resource/graphics/JCR_menutext.png");
		m_nSelection = 0;
		m_pFont = new CBitmapFont(33,32,8,m_nFontID,'A');
				
		// Object Factory register
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->RegisterClassType<CBase>("CBase");		
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->RegisterClassType<CEnemy>("CEnemy");	

		m_nBGMID = XAUDIO->MusicLoadSong("resource/sound/JCR_enemydrawsnear.xwm");

		// Setting Message System proc
		MS->InitMessageSystem(CMainMenuState::MessageProc);		

		m_vMenuOptions.push_back("PLAY");
		m_vMenuOptions.push_back("OPTIONS");
		m_vMenuOptions.push_back("HOW TO PLAY");
		m_vMenuOptions.push_back("HIGH SCORES");
		m_vMenuOptions.push_back("CREDITS");
		m_vMenuOptions.push_back("EXIT");		

		m_bInitOnce = true;
	}
}

bool CMainMenuState::Input(void)
{	
	// Menu option selection w/ wrapping
	if(CSGD_DirectInput::GetInstance()->KeyPressed(DIK_UP))
	{
		--m_nSelection;	

		if(m_nSelection < 0)
			m_nSelection = m_vMenuOptions.size() - 1;
	}

	if(CSGD_DirectInput::GetInstance()->KeyPressed(DIK_DOWN))
	{
		++m_nSelection;	

		if(m_nSelection >  (int)m_vMenuOptions.size() - 1)
			m_nSelection = 0;
	}

	if(CSGD_DirectInput::GetInstance()->KeyPressed(DIK_RETURN))
	{
		if(m_nSelection == 0)
		{					
			CGame::GetInstance()->ChangeState(CGamePlayState::GetInstance());
		}
		else if(m_nSelection == 1)
		{
			GAME->ChangeState(COptionsState::GetInstance());
		}
		else if(m_nSelection == 2)
		{
			GAME->ChangeState(CInstructionsState::GetInstance());
		}
		else if(m_nSelection == 3)
		{
			GAME->ChangeState(CHighScoreState::GetInstance());
		}
		else if(m_nSelection == 4)
		{
			GAME->ChangeState(CCreditsState::GetInstance());
		}
		else if(m_nSelection == m_vMenuOptions.size() - 1)
			return false;		
	}


	return true;
}

void CMainMenuState::Update(void)
{
	OM->UpdateObjects(GAME->GetTimer().GetDeltaTime());

	if(m_fSpawnTime > 2.0f)
	{
		MS->SendMsg(new CCreateEnemyMessage());
		m_fSpawnTime = 0.0f;
	}

	if(!XAUDIO->MusicIsSongPlaying(m_nBGMID))
		XAUDIO->MusicPlaySong(m_nBGMID);

	m_fSpawnTime += GAME->GetTimer().GetDeltaTime();

	MS->ProcessMessages();
}

void CMainMenuState::Render(void)
{
	CSGD_Direct3D::GetInstance()->Clear(128,128,128);
	CSGD_Direct3D::GetInstance()->DeviceBegin();
	CSGD_Direct3D::GetInstance()->SpriteBegin();	

	OM->RenderObjects();

	CSGD_Direct3D::GetInstance()->GetSprite()->Flush();	// Draw everything now that is queued up

	// Writing title out
	m_pFont->Print("BULLET SPONGE",float(CGame::GetInstance()->GetWindowWidth() >> 1) - (m_pFont->GetCharWidth() * 6),50,1,1.1f);
	
	for(unsigned int i = 0; i < m_vMenuOptions.size(); ++i)
	{
		// Get some menu item pulse up in hurrrrr
		
		if(m_fPulse <= 0.5f)
			m_bGrow = true;
		else if(m_fPulse >= 0.9f)
			m_bGrow = false;

		if(m_bGrow)
			m_fPulse += 0.1f * GAME->GetTimer().GetDeltaTime();
		else if(!m_bGrow)
			m_fPulse -= 0.1f * GAME->GetTimer().GetDeltaTime();
		

		// Offset stuff to make everything line up nicely
		float nPosX = float(390 - ((m_vMenuOptions[i].size() >> 1) * m_pFont->GetCharWidth()));
		float nPosY = float(300 + (m_pFont->GetCharHeight() + 20) * i);				

		if(i == m_nSelection)
		{
			m_pFont->Print(m_vMenuOptions[i],nPosX,nPosY,1,m_fPulse);
			CSGD_TextureManager::GetInstance()->Draw(m_nCursorID,(int)nPosX - 42,(int)nPosY);
		}
		else
			m_pFont->Print(m_vMenuOptions[i],nPosX,nPosY,1,0.9f);
	}	

	CSGD_Direct3D::GetInstance()->SpriteEnd();
	CSGD_Direct3D::GetInstance()->DeviceEnd();
	CSGD_Direct3D::GetInstance()->Present();
}

void CMainMenuState::Exit(void)
{		
	if(m_bInitOnce)
	{
		if(m_nCursorID > -1)
			CSGD_TextureManager::GetInstance()->UnloadTexture(m_nCursorID);

		if(m_nFontID > -1)
			CSGD_TextureManager::GetInstance()->UnloadTexture(m_nFontID);

		m_nSelection = 0;

		m_vMenuOptions.clear();

		SAFE_DELETE(m_pFont);

		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->UnregisterClassType("CBase");	
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->UnregisterClassType("CEnemy");	

		if(XAUDIO->MusicIsSongPlaying(m_nBGMID))
			XAUDIO->MusicStopSong(m_nBGMID);

		XAUDIO->MusicUnloadSong(m_nBGMID);

		OM->RemoveAllObjects();
		MS->ClearMessages();

		m_nCursorID = -1;
		m_pFont = NULL;
		m_fPulse = 0.9f;
		m_bGrow = false;
		m_bInitOnce = false;	
		m_fSpawnTime = 0.0f;
	}
}

CMainMenuState* CMainMenuState::GetInstance()
{
	if(sm_pInstance == NULL)
		sm_pInstance = new CMainMenuState();

	return sm_pInstance;
}

void CMainMenuState::DeleteInstance()
{
	if(sm_pInstance)
	{
		sm_pInstance->Exit();

		delete sm_pInstance;
		sm_pInstance = NULL;
	}
}

void CMainMenuState::MessageProc(CBaseMessage* pMsg)
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