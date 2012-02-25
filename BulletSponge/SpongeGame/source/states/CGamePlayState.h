///////////////////////////////////////////////////////
// File Name	:	"CGamePlayState.h"
//
// Author Name	:	JC Ricks
//
// Purpose		:	To control the playstate
//////////////////////////////////////////////////////
#ifndef _CGAMEPLAYSTATE_H_
#define _CGAMEPLAYSTATE_H_

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "IGameState.h"
#include "../models/CLevel.h"
#include "..\CGame.h"
#include "..\SGD Wrappers\CSGD_Direct3D.h"
#include "..\SGD Wrappers\CSGD_DirectInput.h"
#include "..\SGD Wrappers\CSGD_TextureManager.h"
#include "..\SGD Wrappers\CSGD_XAudio2.h"
#include "..\CSGD_ObjectManager.h"
#include <queue>
using std::queue;

///////////////////////////////////////
//		FWD DECLARATIONS
///////////////////////////////////////
class CBitmapFont;

enum Difficulty {EASY=0,NORMAL,HARD};

class CGamePlayState : public IGameState, public IListener
{
public:
	

	/********** Public Utility Functions ************/
		void Enter(void); // Enters the Game State
		bool Input(void); // Input
		void Update(void); // Update
		void Render(void); // Draw
		void Exit(void); // Leaves the Game State
		static CGamePlayState* GetInstance(); // Returns State instance
		static void DeleteInstance(); // Deletes the instance
		void HandleEvent(CEvent* pEvent) {};
		static void MessageProc(CBaseMessage* pMsg); // Processes msgs

	/********** Public Accessors ************/
		bool GetRenderBoxes() {return m_bRenderBoxes;}
		//(read only) access the level variables		
		const CLevel* const GetLevel() {return m_pCurLevel;}
		bool GetIsPaused() {return m_bIsPaused;}
		bool GetIsDead() {return m_bDead;}
		Difficulty GetDifficulty() {return m_eDifficulty;}
		int GetEnemyCount() {return m_nEnemyCount;}

	/********** Public Mutators  ************/	
		void SetIsPaused(bool bPause) {m_bIsPaused = bPause;}
		void SetIsDead(bool bDead) {m_bDead = bDead;}	
		void SetDifficulty(Difficulty eDiff) {m_eDifficulty = eDiff;}
		void SetEnemyCount(int nCount) {m_nEnemyCount = nCount;}

private:
	/********** Construct / Deconstruct / OP Overloads ************/
		// Proper singleton
		CGamePlayState(const CGamePlayState&);
		//		Assignment op
		CGamePlayState& operator=(const CGamePlayState&);
		//		Destructor
		~CGamePlayState();
		//		Constructor
		CGamePlayState();
		//		State pointer

	/********** Private Members ************/
		static CGamePlayState* sm_pInstance;
		// Current level
		CLevel* m_pCurLevel;	
		// Pause menu font
		CBitmapFont* m_pFont;
		CBitmapFont* m_pNumbers;
		// Font ID
		int m_nFontID;
		int m_nNumbersID;
		// Lives Icon
		int m_nLivesID;

		// Collision boxes rendering
		bool m_bRenderBoxes;
			
		// Sound IDs	
		int m_nWinID;
		int m_nLoseID;
		int m_nLostLifeID;

		/************************PLAYER VARAIBLES**********************/
				// Player HP Rect
				RECT m_rHP;
				// Game bools
				bool m_bDead;				
				// Player Lives
				int m_nLives;
				// Target enemies to kill
				int m_nTarget;
				// Enemies laid waste to
				int m_nEnemiesKilled;
				// Saved difficulty				
				Difficulty m_eDifficulty;
				// Has won bool
				bool m_bHasWon;
				// Player score
				int m_nScore;
				// Cheat strings
				queue<char> m_qInputBuffer;
				vector<char> m_vGodCheat;
				vector<char> m_vShottyCheat;
				vector<char> m_vLivesCheat;
				string m_szGodCheat;
				string m_szShottyCheat;
				string m_szLivesCheat;
				// Timer
				float m_fLossTimer;
				float m_fWinTimer;
				

		// Pause menu variables
		bool m_bIsPaused;
			// Current selection
			int m_nSelection;	
		// Pause menu cursor
		int m_nCursorID;
		// Crosshair ImageID
		int m_nCrosshair;
		// Bullet ImageIDs
		int m_nEnemyID;
		
		// Enemy Counter
		int m_nEnemyCount;
		// Enemy spawn timer
		float m_fEnemyTimer;
		// Checking for Init
		bool m_bInitOnce;
		
	
	/********** Private Accessors ************/
		float GetEnemyTimer() {return m_fEnemyTimer;}
		int GetScore() {return m_nScore;}
	/********** Private Mutators ************/
		void SetEnemyTimer(float fTime) {m_fEnemyTimer = fTime;}
		void SetScore(int nScore) {m_nScore = nScore;}
	/********** Private Utility Functions ************/		
		void SetRenderBoxes(bool bRenderBoxes) {m_bRenderBoxes = bRenderBoxes;}
		void GenerateEnemies();
};

#endif