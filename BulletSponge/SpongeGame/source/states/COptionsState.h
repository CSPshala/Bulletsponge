///////////////////////////////////////////////////////////////////////////
//	File Name	:	"COptionsState.h"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	Handle the options menu
///////////////////////////////////////////////////////////////////////////
#ifndef _COPTIONSSTATE_H_
#define _COPTIONSSTATE_H_

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "IGameState.h"
#include "../messaging/CSGD_MessageSystem.h"
////////////////////////////////////////
//		   FORWARD DECLARATIONS
////////////////////////////////////////
class CBitmapFont;
////////////////////////////////////////
//				MISC
////////////////////////////////////////


class COptionsState : public IGameState
{
public:	
	/********** Public Utility Functions ************/
		void Enter(void); // Enters the Game State
		bool Input(void); // Input
		void Update(void); // Update
		void Render(void); // Draw
		void Exit(void); // Leaves the Game State
		static COptionsState* GetInstance();	// Returns this instance
		static void DeleteInstance(); // Deletes the instance
		static void MessageProc(CBaseMessage* pMsg); // Processes msgs

	/********** Public Accessors ************/
		float GetMusicVolume() {return m_fMusicVolume;}
		float GetSFXVolume() {return m_fSFXVolume;}
		int GetEnemyTarget() {return m_nEnemyTarget;}
		int GetDifficulty() {return m_nDifficulty;}

	/********** Public Mutators  ************/	


private:
	/********** Construct / Deconstruct / OP Overloads ************/
		// Proper singleton
		COptionsState(const COptionsState&);
		//		Assignment op
		COptionsState& operator=(const COptionsState&);
		//		Destructor
		~COptionsState();
		//		Constructor
		COptionsState();

	/********** Private Members ************/
		//		State pointer
		static COptionsState* sm_pInstance;
		// Image IDs
		int m_nCursorID;
		int m_nFontID;
		int m_nNumbersID;
		// Sound IDs
		int m_nBGMID;
		int m_nShotFX;
		// Current selection
		int m_nSelection;		
		// Font Object
		CBitmapFont* m_pFont;
		CBitmapFont* m_pNumbers;
		// Init once
		bool m_bInitOnce;
		// Sound Volume Variables
		float m_fMusicVolume;
		float m_fSFXVolume;
		// Timer to smooth out changing of volume
		float m_fChangeTimer;
		// Enemy spawn timer
		float m_fSpawnTime;

		// Game Specific Options
		int m_nEnemyTarget;
		int m_nDifficulty;

	/********** Private Accessors ************/

	/********** Private Mutators ************/

	/********** Private Utility Functions ************/
		void LoadSettings();
		void SaveSettings();

};
#endif