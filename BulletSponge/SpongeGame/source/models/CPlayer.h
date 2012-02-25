///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CPlayer.h"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	To hold all data and functionality of player character
///////////////////////////////////////////////////////////////////////////
#ifndef _CPLAYER_H_
#define _CPLAYER_H

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include <vector>
using std::vector;
#include "../SGD_Math.h"
#include "CBase.h"
#include "CAnimation.h"
#include "../messaging/IListener.h"

////////////////////////////////////////
//		   FORWARD DECLARATIONS
////////////////////////////////////////
class CWorld;


class CPlayer : public CBase, public IListener
{
public:
	/********** Construct / Deconstruct / OP Overloads ************/
		CPlayer();
		~CPlayer();

	/********** Public Utility Functions ************/
		void Render();
		void Update(float fElapsedTime);	
		void HandleEvent(CEvent* pEvent);
		bool CheckCollision(IBaseInterface* pBase);
		void Respawn();

	/********** Public Accessors ************/
		float GetShotAngle() {return m_fHands;}
		tVector2D GetWeaponPos() {return m_tHands;}	
		int GetHP() {return m_nCurHP;}
		int GetMaxHP() {return m_nMaxHP;}
	
	/********** Public Mutators  ************/	
		void SetHP(int nHP) {m_nCurHP = nHP;}
		void SetMaxHP(int nMaxHP) {m_nMaxHP = nMaxHP;}
	
private:
	/********** Private Members ************/
		// State enum
		enum State {REST = 0,JUMP,RUN,FALLING,CROUCH,SHOOT,MAX_STATES};

		// Player HP
		int m_nMaxHP;
		int m_nCurHP;

		// Cheats
		bool m_bGod;
		bool m_bShotty;

		// Are we currently respawning?
		bool m_bRespawning;
		float m_fRespawnTime;

		//Holds animations
		vector<CAnimation> m_nAnimations;

		// Holds weapon/hands
		int m_nHands;
		// Shotty ID
		int m_nShotty;
			// Position vector for hands
		tVector2D m_tHands;
			// Rotation for hands
		float m_fHands;

		// Player bullet ID
		int m_nPlayerBulletID;
		// Bulletshot sound ID
		int m_nShotFX;
		int m_nShotgunFX;

			// Voiceover IDs
			int m_nGodOffID;
			int m_nGodOnID;
			int m_nLivesID;
			int m_nShottyOnID;
			int m_nShottyOffID;

		// State
		State m_eState;	

		// Pointer to world object that
		// is currently being stood on
		CWorld* m_pPlatform;

	/********** Private Accessors ************/	
		CWorld* GetPlatform() {return m_pPlatform;}	

	/********** Private Mutators ************/
		void SetState(State eState);
		void SetPlatform(CWorld* pPlatform);

	/********** Private Utility Functions ************/		
		State GetState() {return m_eState;}
		void HandAiming();
		RECT GetCollisionRect();
};

#endif