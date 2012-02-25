///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CInstructionsState.h"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	Handle instructions display
///////////////////////////////////////////////////////////////////////////
#ifndef _CInstructionsState_H_
#define _CInstructionsState_H_

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

class CInstructionsState : public IGameState
{
public:
	/********** Public Utility Functions ************/
		void Enter(void); // Enters the Game State
		bool Input(void); // Input
		void Update(void); // Update
		void Render(void); // Draw
		void Exit(void); // Leaves the Game State
		static CInstructionsState* GetInstance();	// Returns this instance
		static void DeleteInstance(); // Deletes the instance
		static void MessageProc(CBaseMessage* pMsg); // Processes msgs
	/********** Public Accessors ************/

	/********** Public Mutators  ************/			

private:
	/********** Construct / Deconstruct / OP Overloads ************/
		// Proper singleton
		CInstructionsState(const CInstructionsState&);
		//		Assignment op
		CInstructionsState& operator=(const CInstructionsState&);
		//		Destructor
		~CInstructionsState();
		//		Constructor
		CInstructionsState();
	/********** Private Members ************/
		//		State pointer
		static CInstructionsState* sm_pInstance;
		// Image IDs	
		int m_nFontID;
		int m_nNumbersID;
		int m_nBackgroundID;
		// Sound IDs
		int m_nBGMID;
		// Font Object
		CBitmapFont* m_pFont;
		CBitmapFont* m_pNumbers;
		// Init once
		bool m_bInitOnce;		
		// Enemy spawn timer
		float m_fSpawnTime;
		
	/********** Private Accessors ************/

	/********** Private Mutators ************/

	/********** Private Utility Functions ************/
		
};
#endif