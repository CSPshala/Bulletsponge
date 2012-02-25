///////////////////////////////////////////////////////
// File Name	:	"CMainMenuState.h"
//
// Author Name	:	JC Ricks
//
// Purpose		:	To contain menu data and rendering
//////////////////////////////////////////////////////
#ifndef _CMAINMENUSTATE_H_
#define _CMAINMENUSTATE_H_

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include <vector>
#include <string>
using std::string;
using std::vector;

#include "IGameState.h"
#include "..\CGame.h"
#include "CGamePlayState.h"
#include "../models/CBitmapFont.h"
#include "..\SGD Wrappers\CSGD_Direct3D.h"
#include "..\SGD Wrappers\CSGD_DirectInput.h"
#include "..\SGD Wrappers\CSGD_TextureManager.h"
#include "..\SGD Wrappers\CSGD_XAudio2.h"
#include "../messaging/CSGD_MessageSystem.h"



class CMainMenuState : public IGameState
{
public:
	/********** Public Utility Functions ************/
		void Enter(void); // Enters the Game State
		bool Input(void); // Input
		void Update(void); // Update
		void Render(void); // Draw
		void Exit(void); // Leaves the Game State
		static CMainMenuState* GetInstance();	// Returns this instance
		static void DeleteInstance(); // Deletes the instance

		static void MessageProc(CBaseMessage* pMsg); // Processes msgs
		
	/********** Public Accessors ************/


	/********** Public Mutators  ************/	


private:
	/********** Construct / Deconstruct / OP Overloads ************/
		// Proper singleton
		CMainMenuState(const CMainMenuState&);
		//		Assignment op
		CMainMenuState& operator=(const CMainMenuState&);
		//		Destructor
		~CMainMenuState();
		//		Constructor
		CMainMenuState();

	/********** Private Members ************/
		//		State pointer
		static CMainMenuState* sm_pInstance;
		// Image IDs
		int m_nCursorID;
		int m_nFontID;
		// BGM
		int m_nBGMID;
		// Current selection
		int m_nSelection;
		// Vector of menu options for ease of change
		vector<string> m_vMenuOptions;
		// Font Object
		CBitmapFont* m_pFont;
		// Pulsation
		float m_fPulse;
		bool m_bGrow;
		// Checking on exit if the state as even entered yet
		bool m_bInitOnce;
		// Enemy spawn timer
		float m_fSpawnTime;

	/********** Private Accessors ************/


	/********** Private Mutators ************/


	/********** Private Utility Functions ************/

};

#endif