///////////////////////////////////////////////////////
// File Name	:	"CGame.cpp"
//
// Author Name	:	JC Ricks
//
// Purpose		:	To Contain all game related code
//////////////////////////////////////////////////////

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "CGame.h"
#include "states\COptionsState.h"
#include "states\CHighScoreState.h"
#include "states\CCreditsState.h"
#include "states\CInstructionsState.h"
#include <ctime>

///////////////////////////////////////////////
//  CONSTRUCTOR / DECONSTRUCT / OP OVERLOADS
///////////////////////////////////////////////
CGame::CGame()
{
	m_pD3D  = NULL;
	m_pDI	= NULL;
	m_pTM	= NULL;
	m_pXA	= NULL;	
	m_pOM	= NULL;
	m_pOF	= NULL;
	m_pMS	= NULL;
	m_pES	= NULL;
}

CGame::~CGame()
{
}

////////////////////////////////////////
//		PUBLIC UTILITY FUNCTIONS
////////////////////////////////////////
CGame* CGame::GetInstance()
{
	// Lazy instantiation
	static CGame instance; // Static allows passing back of address
	return &instance;	
}

void CGame::Init(HWND hWnd, HINSTANCE hInstance, int nScreenWidth,
					int nScreenHeight,bool bIsWindowed)
{
	// Get pointers to singletons:
	m_pD3D  = CSGD_Direct3D::GetInstance();
	m_pDI	= CSGD_DirectInput::GetInstance();
	m_pTM	= CSGD_TextureManager::GetInstance();
	m_pXA	= CSGD_XAudio2::GetInstance();
	m_pOM	= CSGD_ObjectManager::GetInstance();
	m_pOF	= CSGD_ObjectFactory<string,IBaseInterface>::GetInstance();
	m_pMS	= CSGD_MessageSystem::GetInstance();
	m_pES	= CSGD_EventSystem::GetInstance();

	// Hiding cursor when in window
	ShowCursor(false);

	// Init singletons:
	m_pD3D->InitDirect3D(hWnd,nScreenWidth,nScreenHeight,bIsWindowed,false);
	m_pTM->InitTextureManager(m_pD3D->GetDirect3DDevice(),m_pD3D->GetSprite());
	m_pDI->InitDirectInput(hWnd,hInstance, DI_KEYBOARD| DI_MOUSE);
	m_pXA->InitXAudio2();
	
	ChangeState(CMainMenuState::GetInstance());

	// Window parameters
	m_bWindowed = bIsWindowed;
	m_nWindowWidth = nScreenWidth;
	m_nWindowHeight = nScreenHeight;

	// Seeding rand
	srand(unsigned int(time(0)));		
}

bool CGame::Main()
{
	/////////////////////////////////////////////
	//  Calculate elapsed time
	DWORD dwStartTimeStamp = GetTickCount();

	m_fElapsedTime = (float)(dwStartTimeStamp - m_dwPreviousTimeStamp) / 1000.0f;	

	m_dwPreviousTimeStamp = dwStartTimeStamp;
	/////////////////////////////////////////////

	// 3 Things the game does during execution
	// Input
	if (Input() == false)
		return false;
	// Update
	Update();
	// Render
	Render();

	return true;
}

void CGame::Shutdown()
{
	ChangeState(NULL);

	// Cleaning up Gameplaystate	
	CGamePlayState::GetInstance()->Exit();
	CGamePlayState::DeleteInstance();	

	// Ditto for MainMenu	
	CMainMenuState::GetInstance()->Exit();
	CMainMenuState::DeleteInstance();

	// Yep	
	COptionsState::GetInstance()->Exit();
	COptionsState::DeleteInstance();

	// Word
	CHighScoreState::GetInstance()->Exit();
	CHighScoreState::DeleteInstance();

	// Is the Bird
	CCreditsState::GetInstance()->Exit();
	CCreditsState::DeleteInstance();

	// Bawww wid dah bawwww
	CInstructionsState::GetInstance()->Exit();
	CInstructionsState::DeleteInstance();

	if(m_pES)
	{
		m_pES->ShutdownEventSystem();
		m_pES = NULL;
	}
	if(m_pMS)
	{
		m_pMS->ShutdownMessageSystem();
		m_pMS = NULL;
	}	

	// Cleanup on Object Factory
	m_pOF->ShutdownObjectFactory();	

	// Cleaning up object manager
	if(m_pOM)
	{
		m_pOM->RemoveAllObjects();
		m_pOM->DeleteInstance();		
	}

	// Shutdown in the opposite order
	if(m_pXA)
	{
		m_pXA->ShutdownXAudio2();
		m_pXA = NULL;
	}

	if(m_pDI)
	{
		m_pDI->ShutdownDirectInput();
		m_pDI = NULL;
	}

	if(m_pTM)
	{
		m_pTM->ShutdownTextureManager();
		m_pTM = NULL;
	}

	if(m_pD3D)
	{
		m_pD3D->ShutdownDirect3D();
		m_pD3D = NULL;
	}

	
}

////////////////////////////////////////
//		PRIVATE UTILITY FUNCTIONS
////////////////////////////////////////
bool CGame::Input()
{
	m_pDI->ReadDevices(); // called ONCE a frame

	// Fullscreen / Window Shift  (KeyDown used on alt to make it
	// so you don't hafta hit both keys exactly at the same time
	if(m_pDI->KeyPressed(DIK_RETURN) && (m_pDI->KeyDown(DIK_RALT) || m_pDI->KeyDown(DIK_LALT)))
	{
		if(m_bWindowed)
		{
			m_bWindowed = false;
			m_pD3D->ChangeDisplayParam(m_nWindowWidth,m_nWindowHeight,m_bWindowed);			
		}
		else
		{
			m_bWindowed = true;
			m_pD3D->ChangeDisplayParam(m_nWindowWidth,m_nWindowHeight,m_bWindowed);
		}
	}		

	if(!m_pCurState->Input())
		return false;
	
	return true;
}

void CGame::Update()
{
	///////////////////////////////
	// UPDATING GAME TIME
	m_fGameTime += m_fElapsedTime;

	m_pCurState->Update();	// must be called or you will mess stuff up

}

void CGame::Render()
{
	m_pCurState->Render();
}

////////////////////////////////////////
//	    PUBLIC ACCESSORS / MUTATORS
////////////////////////////////////////
void CGame::ChangeState(IGameState* pNewState)
{
	if(m_pCurState)
	{
		m_pCurState->Exit();		
	}

	m_pCurState = pNewState;

	if(m_pCurState)
		m_pCurState->Enter();
}

////////////////////////////////////////
//	    PRIVATE ACCESSORS / MUTATORS
////////////////////////////////////////
