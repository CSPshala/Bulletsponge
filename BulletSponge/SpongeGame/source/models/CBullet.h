///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CBullet.h"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	Handle projectile behavior and data
///////////////////////////////////////////////////////////////////////////
#ifndef _CBULLET_H_
#define _CBULLET_H_

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "../SGD_Math.h"
#include "CBase.h"
#include "../messaging/CSGD_EventSystem.h"
#include "../messaging/IListener.h"

class CBullet : public CBase, public IListener
{
public:
	/********** Construct / Deconstruct / OP Overloads ************/
		CBullet();
		CBullet(float fRotation,CBase* pOwner);
		~CBullet();

	/********** Public Utility Functions ************/
		void Render();
		void Update(float fElapsedTime);
		void HandleEvent(CEvent* pEvent);
		bool CheckCollision(IBaseInterface* pBase);

	/********** Public Accessors ************/	
		CBase* GetOwner() {return m_pOwner;}
		int GetDamage() {return m_nDamage;}
		bool GetIsScrew() {return m_bScrew;}
		bool GetIsPuke() {return m_bPuke;}
		float GetRotation() {return m_fRotation;}
	/********** Public Mutators  ************/
		void SetOwner(CBase* pOwner);
		void SetDamage(int nDamage) {m_nDamage = nDamage;}
		void SetIsScrew(bool bScrew) {m_bScrew = bScrew;}
		void SetIsPuke(bool bPuke) {m_bPuke = bPuke;}
		void SetRotation(float fRotation) {m_fRotation = fRotation;}

private:
	/********** Private Members ************/
		float m_fRotation;
		CBase* m_pOwner;
		// Time to live
		float m_fTimer;

		// Is it a screw?
		bool m_bScrew;
		// Is it being puked?
		bool m_bPuke;		

		// Bullet dmg (for player right now)
		int m_nDamage;

	/********** Private Accessors ************/

	/********** Private Mutators ************/

	/********** Private Utility Functions ************/
		
};

#endif