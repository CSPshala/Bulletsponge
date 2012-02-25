///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CHighScore.h"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	Handle High Score display and input
///////////////////////////////////////////////////////////////////////////
#ifndef _CHIGHSCORE_H_
#define _CHIGHSCORE_H_

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "../messaging/CSGD_MessageSystem.h"
#include "IGameState.h"
#include <string>
#include <vector>
using std::string;
using std::vector;
////////////////////////////////////////
//		   FORWARD DECLARATIONS
////////////////////////////////////////
class CBitmapFont;
////////////////////////////////////////
//				MISC
////////////////////////////////////////

struct TScore
{
	string szName;
	int nScore;
};

class CHighScoreState : public IGameState
{
public:
	/********** Public Utility Functions ************/
		void Enter(void); // Enters the Game State
		bool Input(void); // Input
		void Update(void); // Update
		void Render(void); // Draw
		void Exit(void); // Leaves the Game State
		static CHighScoreState* GetInstance();	// Returns this instance
		static void DeleteInstance(); // Deletes the instance
		static void MessageProc(CBaseMessage* pMsg); // Processes msgs
	/********** Public Accessors ************/

	/********** Public Mutators  ************/	
		void SetInputMode(bool bInput) {m_bInputMode = bInput;}
		void SetInputScore(int nScore) {m_tScore.nScore = nScore;}		

private:
	/********** Construct / Deconstruct / OP Overloads ************/
		// Proper singleton
		CHighScoreState(const CHighScoreState&);
		//		Assignment op
		CHighScoreState& operator=(const CHighScoreState&);
		//		Destructor
		~CHighScoreState();
		//		Constructor
		CHighScoreState();
	/********** Private Members ************/
		//		State pointer
		static CHighScoreState* sm_pInstance;
		// Image IDs	
		int m_nFontID;
		int m_nNumbersID;
		// Sound IDs
		int m_nBGMID;
		// Font Object
		CBitmapFont* m_pFont;
		CBitmapFont* m_pNumbers;
		// Init once
		bool m_bInitOnce;
		// If it's input time
		bool m_bInputMode;
		// Input string for name
		string m_szInputName;
		// Score struct
		TScore m_tScore;
		// Vector of scores
		vector<TScore*> m_vScoreTable;
		// Enemy spawn timer
		float m_fSpawnTime;
	/********** Private Accessors ************/

	/********** Private Mutators ************/

	/********** Private Utility Functions ************/
		void LoadScores();
		void InputScore();
		void SaveScores();
};

#endif