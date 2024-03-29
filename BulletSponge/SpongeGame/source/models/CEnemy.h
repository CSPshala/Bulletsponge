///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CEnemy.h"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	To hold all data and functionality of enemy characters
///////////////////////////////////////////////////////////////////////////
#ifndef _CENEMY_H_
#define _CENEMY_H_

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include <vector>
using std::vector;

#include "CBase.h"
#include "CAnimation.h"
#include "CWorld.h"
#include "../messaging/IListener.h"

// State enum
enum EnemyState {SPAWNING = 0,REST,PATROL,JUMP,SHOOT,FALLING,PUKE,CROUCH,MAX_STATES};

class CEnemy : public CBase, public IListener
{
public:
	/********** Construct / Deconstruct / OP Overloads ************/
		CEnemy();
		~CEnemy();

	/********** Public Utility Functions ************/
		void Render();
		void Update(float fElapsedTime);
		void HandleEvent(CEvent* pEvent);
		bool CheckCollision(IBaseInterface* pBase);

	/********** Public Accessors ************/
		EnemyState GetState() {return m_eState;}		
		int GetBulletID() {return m_nEnemyBulletID;}
		int GetHP() {return m_nHP;}
	/********** Public Mutators  ************/	
		void SetHP(int nHP) {m_nHP = nHP;}
		void SetMainMenu(bool bMenu) {m_bMainMenu = bMenu;}

private:
	/********** Private Members ************/		
		// HP
		int m_nHP;
		// Hit indicator color
		DWORD m_dwHitColor;
		// Hit color timer
		float m_fHitColorTime;

		//Holds animations
		vector<CAnimation> m_nAnimations;

		// State
		EnemyState m_eState;	
		// Holds previous state
		EnemyState m_eLastState;

		// Timers
			// Behavior change
			float m_fChange;
			float m_fShootTimer;
			float m_fJumpTimer;

		// If spawned in main menu
			bool m_bMainMenu;

		// Main Menu rotation
			float m_fRotation;

		// Pointer to world object that
		// is currently being stood on
		CWorld* m_pPlatform;
		
		// Enemybullet picture ID
		int m_nEnemyBulletID;

		// Bulletshot sound ID
		int m_nShotFX;

	/********** Private Accessors ************/		
		
		CWorld* GetPlatform() {return m_pPlatform;}

	/********** Private Mutators ************/
		void SetState(EnemyState eState) {m_eState = eState;}	
		void SetPlatform(CWorld* pPlatform);

	/********** Private Utility Functions ************/
		virtual void Behavior();
		void FlashRedTimer(float fTime);
		void InitStickFigure();
};

#endif