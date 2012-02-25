///////////////////////////////////////////////////////
// File Name	:	"CGame.h"
//
// Author Name	:	JC Ricks
//
// Purpose		:	To Contain all game related code
//////////////////////////////////////////////////////
#ifndef _CGAME_H_
#define _CGAME_H_

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
// SGD Singletons:
#include "SGD Wrappers\CSGD_Direct3D.h"
#include "SGD Wrappers\CSGD_DirectInput.h"
#include "SGD Wrappers\CSGD_TextureManager.h"
#include "SGD Wrappers\CSGD_XAudio2.h"
#include "CSGD_ObjectManager.h"
// Messaging
#include "messaging\CSGD_MessageSystem.h"
#include "messaging\CSGD_EventSystem.h"
// Factories
#include <string>		// String is included to work with factory
using namespace std;
#include "factories\CSGD_ObjectFactory.h"
// Gamestates
#include "states\IGameState.h"
#include "states\CMainMenuState.h"
// Game Class Models
#include "models\CLevel.h"


class CGame
{
public:
	/********** Public Utility Functions ************/
		// Singleton accessor
		static CGame* GetInstance();
		// 3 things a game does during LIFETIME:
		// Init
		void Init(HWND hWnd, HINSTANCE hInstance, int nScreenWidth, int nScreenHeight,bool bIsWindowed);
		// Execute
		// NOTE: Returns false when game should quit
		bool Main();
		// Clean Up
		void Shutdown();				

	/********** Public Accessors ************/
		int GetWindowHeight() {return m_nWindowHeight;}
		int GetWindowWidth() {return m_nWindowWidth;}
		POINT GetMousePos() {return m_tMousePT;}
		float GetElapsedTime() {return m_fElapsedTime;}

	/********** Public Mutators  ************/	
		// Game State
		void ChangeState(IGameState* pNewState);
		void SetMousePos(POINT tMousePT) {m_tMousePT = tMousePT;}

private:
	/********** Construct / Deconstruct / OP Overloads ************/
		// Proper Singleton
		// Trilogy of Evil:
		//		Copy Constructor
		CGame(const CGame&);
		//		Assignment op
		CGame& operator=(const CGame&);
		//		Destructor
		~CGame();
		//		Constructor
		CGame();

	/********** Private Members ************/
		// Singleton pointers:
		CSGD_Direct3D*			m_pD3D;
		CSGD_DirectInput*		m_pDI;
		CSGD_TextureManager*	m_pTM;
		CSGD_XAudio2*			m_pXA;
		CSGD_ObjectManager*		m_pOM;	
		CSGD_ObjectFactory<string,IBaseInterface>*		m_pOF;
		CSGD_MessageSystem*		m_pMS;
		CSGD_EventSystem*		m_pES;
		// Gamestate Pointer
		IGameState* m_pCurState;
		// Class models
		CLevel* m_pCurLevel;
		// Windowed bool
		bool m_bWindowed;
		// Window height and width
		int m_nWindowHeight;
		int m_nWindowWidth;
		// Crosshair coordinates
		POINT m_tMousePT;
		// For Game timing:
		DWORD					m_dwTimeStamp;
		float					m_fElapsedTime;		// delta time. Time (in seconds) between frames
		float					m_fGameTime;		// how long the game has been playing
		DWORD					m_dwPreviousTimeStamp;	// Time samp from the previous frame	

	/********** Private Accessors ************/


	/********** Private Mutators ************/


	/********** Private Utility Functions ************/
		// Private functions:
		bool Input();	// Returns false if game should quit
		void Update();
		void Render();
};


#endif