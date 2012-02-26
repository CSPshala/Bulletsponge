///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CRobot.h"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	To take care of robot behavior, closely related to CEnemy
///////////////////////////////////////////////////////////////////////////
#ifndef _C_ROBOT_H
#define _C_ROBOT_H

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include <vector>
using std::vector;

#include "CBase.h"
#include "CAnimation.h"
#include "CWorld.h"
#include "../messaging/IListener.h"
#include "CEnemy.h"
////////////////////////////////////////
//		   FORWARD DECLARATIONS
////////////////////////////////////////

////////////////////////////////////////
//				MISC
////////////////////////////////////////


class CRobot : public CBase, public IListener
{
public:
	/********** Construct / Deconstruct / OP Overloads ************/
		CRobot();
		~CRobot();

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
		int m_nLongPukeFX;

	/********** Private Accessors ************/		
		
		CWorld* GetPlatform() {return m_pPlatform;}

	/********** Private Mutators ************/
		void SetState(EnemyState eState) {m_eState = eState;}	
		void SetPlatform(CWorld* pPlatform);

	/********** Private Utility Functions ************/
		virtual void Behavior();
		void FlashRedTimer(float fTime);		
};
#endif