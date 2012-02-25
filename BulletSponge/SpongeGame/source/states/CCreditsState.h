///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CCreditsState.h"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	Handle credit display
///////////////////////////////////////////////////////////////////////////
#ifndef _CCREDITSSTATE_H_
#define _CCREDITSSTATE_H_

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "IGameState.h"
#include "../messaging/CSGD_MessageSystem.h"
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

class CCreditsState : public IGameState
{
public:
	/********** Public Utility Functions ************/
		void Enter(void); // Enters the Game State
		bool Input(void); // Input
		void Update(void); // Update
		void Render(void); // Draw
		void Exit(void); // Leaves the Game State
		static CCreditsState* GetInstance();	// Returns this instance
		static void DeleteInstance(); // Deletes the instance
		static void MessageProc(CBaseMessage* pMsg); // Processes msgs
	/********** Public Accessors ************/

	/********** Public Mutators  ************/			

private:
	/********** Construct / Deconstruct / OP Overloads ************/
		// Proper singleton
		CCreditsState(const CCreditsState&);
		//		Assignment op
		CCreditsState& operator=(const CCreditsState&);
		//		Destructor
		~CCreditsState();
		//		Constructor
		CCreditsState();
	/********** Private Members ************/
		//		State pointer
		static CCreditsState* sm_pInstance;
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
		// Credits scrolling timer
		float m_fScrollTimer;
		// Vector of stuff to scroll
		vector<string> m_vCredits;
		// Enemy spawn timer
		float m_fSpawnTime;
		
	/********** Private Accessors ************/

	/********** Private Mutators ************/

	/********** Private Utility Functions ************/
		
};
#endif