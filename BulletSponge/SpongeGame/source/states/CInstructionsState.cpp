///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CInstructionsState.cpp"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	Handle instructions display
///////////////////////////////////////////////////////////////////////////

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "../Macros.h"
#include "CInstructionsState.h"
#include "COptionsState.h"
#include "../CGame.h"
#include "../SGD Wrappers/CSGD_DirectInput.h"
#include "../SGD Wrappers/CSGD_TextureManager.h"
#include "../SGD Wrappers/CSGD_Direct3D.h"
#include "../CSGD_ObjectManager.h"
#include "../factories/CSGD_ObjectFactory.h"
#include "../models/CEnemy.h"
using std::getline;
////////////////////////////////////////
//				MISC
////////////////////////////////////////
// Static instantiation / definition
CInstructionsState* CInstructionsState::sm_pInstance = NULL;
///////////////////////////////////////////////
//  CONSTRUCTOR / DECONSTRUCT / OP OVERLOADS
///////////////////////////////////////////////
CInstructionsState::CInstructionsState()
{	
	m_pFont = NULL;
	m_pNumbers = NULL;
	m_bInitOnce = false;	
	m_fSpawnTime = 0.0f;
}

CInstructionsState::~CInstructionsState()
{
	// Cleaning up font
	SAFE_DELETE(m_pFont);
}

////////////////////////////////////////
//		PUBLIC UTILITY FUNCTIONS
////////////////////////////////////////
void CInstructionsState::Enter(void)
{
	if(!m_bInitOnce)
	{
		m_nFontID = CSGD_TextureManager::GetInstance()->LoadTexture("resource/graphics/JCR_menutext.png");
		m_nNumbersID = TEXTUREMAN->LoadTexture("resource/graphics/JCR_numberfont.png");		
		m_pFont = new CBitmapFont(33,32,8,m_nFontID,'A');	
		m_pNumbers = new CBitmapFont(64,64,10,m_nNumbersID,'0');	

		m_nBackgroundID = TEXTUREMAN->LoadTexture("resource/graphics/JCR_instructions.png");
		
		// Music	
		m_nBGMID = XAUDIO->MusicLoadSong("resource/sound/JCR_guardiacastle.xwm");	

		// GRABBING GAME SETTINGS
		XAUDIO->MusicSetMasterVolume(COptionsState::GetInstance()->GetMusicVolume());
		XAUDIO->SFXSetMasterVolume(COptionsState::GetInstance()->GetSFXVolume());		

		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->RegisterClassType<CBase>("CBase");		
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->RegisterClassType<CEnemy>("CEnemy");	

		// Setting Message System proc
		MS->InitMessageSystem(CInstructionsState::MessageProc);		
		

		m_bInitOnce = true;
	}
}

bool CInstructionsState::Input(void)
{	
	if(DI->KeyPressed(DIK_ESCAPE))
		GAME->ChangeState(CMainMenuState::GetInstance());	

	return true;
}

void CInstructionsState::Update(void)
{
	if(!XAUDIO->MusicIsSongPlaying(m_nBGMID))
		XAUDIO->MusicPlaySong(m_nBGMID,true);	


	OM->UpdateObjects(GAME->GetElapsedTime());

	if(m_fSpawnTime > 2.0f)
	{
		MS->SendMsg(new CCreateEnemyMessage());
		m_fSpawnTime = 0.0f;
	}

	m_fSpawnTime += GAME->GetElapsedTime();

	MS->ProcessMessages();
	
}

void CInstructionsState::Render(void)
{
	CSGD_Direct3D::GetInstance()->Clear(128,128,128);
	CSGD_Direct3D::GetInstance()->DeviceBegin();
	CSGD_Direct3D::GetInstance()->SpriteBegin();	

	OM->RenderObjects();

	CSGD_Direct3D::GetInstance()->GetSprite()->Flush();	// Draw everything now that is queued up

	TEXTUREMAN->Draw(m_nBackgroundID,0,0);

	m_pFont->Print("HOW TO PLAY",float(GAME->GetWindowWidth() >> 1) - 180,1.0f);

	m_pFont->Print("W AND D MOVE YOU LEFT AND RIGHT",8,233,1,0.4f);
	m_pFont->Print("SPACEBAR MAKES YOU JUMP",8,248,1,0.4f);
	m_pFont->Print("LEFT CTRL MAKES YOU CROUCH",8,263,1,0.4f);

	m_pFont->Print("AIM AND SHOOT WITH YOUR MOUSE",380,319,1,0.4f);

	m_pFont->Print("YOUR OBJECTIVE IS TO KILL EVERYTHING",25,360,1,0.4f);
	m_pFont->Print("REACH YOUR KILL TARGET TO WIN\\",24,385,1,0.4f);
	m_pFont->Print("ADJUST DIFFICULTY AND KILL TARGET IN OPTIONS MENU",24,410,1,0.4f);

	m_pFont->Print("CHEATS]",25,float(GAME->GetWindowHeight() - 100),1,0.6f);
	m_pFont->Print("TYPE DOG FOR INVINCIBILITY",25,float(GAME->GetWindowHeight() - 75),1,0.4f);
	m_pFont->Print("TYPE BOOMSTICK FOR SHOTTY",25,float(GAME->GetWindowHeight() - 50),1,0.4f);
	m_pFont->Print("TYPE ISUCK FOR LIVES",25,float(GAME->GetWindowHeight() - 25),1,0.4f);

	CSGD_Direct3D::GetInstance()->SpriteEnd();
	CSGD_Direct3D::GetInstance()->DeviceEnd();
	CSGD_Direct3D::GetInstance()->Present();
}

void CInstructionsState::Exit(void) 
{
	if(m_bInitOnce)
	{
		CSGD_TextureManager::GetInstance()->UnloadTexture(m_nFontID);
		CSGD_TextureManager::GetInstance()->UnloadTexture(m_nNumbersID);

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

		OM->RemoveAllObjects();
		MS->ClearMessages();
	}
}

CInstructionsState* CInstructionsState::GetInstance()
{
	if(sm_pInstance == NULL)
		sm_pInstance = new CInstructionsState();

	return sm_pInstance;
}

void CInstructionsState::DeleteInstance()
{
	if(sm_pInstance)
	{
		sm_pInstance->Exit();

		delete sm_pInstance;
		sm_pInstance = NULL;
	}
}

void CInstructionsState::MessageProc(CBaseMessage* pMsg)
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