///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CBase.h"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	To contain all related data and
//					functionality for our game objects.
///////////////////////////////////////////////////////////////////////////
#ifndef _CBASE_H_
#define _CBASE_H_

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "../IBaseInterface.h"
#include "../SGD_Math.h"
#include "../messaging/CSGD_EventSystem.h"

////////////////////////////////////////
//		   FORWARD DECLARATIONS
////////////////////////////////////////


////////////////////////////////////////
//				MISC
////////////////////////////////////////
enum ObjType{OBJ_BASE,OBJ_PLAYER,OBJ_ENEMY,OBJ_ROBOT,OBJ_BULLET,OBJ_WORLD,OBJ_MAX};


class CBase : public IBaseInterface
{
public:
	/********** Construct / Deconstruct / OP Overloads ************/
		CBase(void);
		virtual ~CBase(void);

	/********** Public Utility Functions ************/	
		void StepBack() {SetPosX(GetOldPosX()); SetPosY(GetOldPosY());}

		//	3 steps a game does EVERY FRAME:
		//	Input
		//	NOTE: Do Input at the top of update if needed
		//	Update
		virtual void Update(float fElapsedTime);
		//	Draw
		virtual void Render(void);

		void AddRef(void)	{m_uiRefCount++;}
		void Release(void);
	
		virtual RECT GetCollisionRect();
		virtual bool CheckCollision(IBaseInterface* pBase);

	/********** Public Accessors ************/
		float GetPosX(void)	{return m_nPosX;}
		float GetPosY(void)	{return m_nPosY;}
		float GetOldPosX(void)	{return m_nOldPosX;}
		float GetOldPosY(void)	{return m_nOldPosY;}
		float GetVelX(void)	{return m_nVelX;}
		float GetVelY(void)	{return m_nVelY;}
		int GetImageID(void)	{return m_nImageID;}
		bool GetIsActive(void)	{return m_bIsActive;}
		int GetWidth(void)	{return m_nWidth;}
		int GetHeight(void)	{return m_nHeight;}
		int GetType() {return m_nType;}
		float GetOffsetX() {return m_fOffsetX;}
		float GetOffsetY() {return m_fOffsetY;}
		float GetMovementSpeedX() {return m_nMoveX;}
		float GetMovementSpeedY() {return m_nMoveY;}

	/********** Public Mutators  ************/	
		void SetPosX(float nPosX)	{m_nPosX = nPosX;}
		void SetPosY(float nPosY)	{m_nPosY = nPosY;}
		void SetOldPosX(float nPosX)	{m_nOldPosX = nPosX;}
		void SetOldPosY(float nPosY)	{m_nOldPosY = nPosY;}
		void SetVelX(float nVelX)	{m_nVelX = nVelX;}
		void SetVelY(float nVelY)	{m_nVelY = nVelY;}
		void SetImageID(int nImageID)	{m_nImageID = nImageID;}
		void SetIsActive(bool bIsActive)	{m_bIsActive = bIsActive;}
		void SetWidth(int nWidth) {m_nWidth = nWidth;}
		void SetHeight(int nHeight) {m_nHeight = nHeight;}	
		void SetOffsetX(float fOffsetX) {m_fOffsetX = fOffsetX;}
		void SetOffsetY(float fOffsetY) {m_fOffsetY = fOffsetY;}
		void SetMovementSpeedX(float nMoveX) {m_nMoveX = nMoveX;}
		void SetMovementSpeedY(float nMoveY) {m_nMoveY = nMoveY;}

protected:
	// Protected members
		int m_nType;
	// Protected methods
		// Set type is private because it should only be edited by children
		void SetType(int nType) {m_nType = nType;}

private:
	/********** Private Members ************/
		unsigned int m_uiRefCount;	//	how many objs have a ptr to me
		//	position
		float m_nPosX;
		float m_nPosY;
		// old position
		float m_nOldPosX;
		float m_nOldPosY;
		//	velocity
		float m_nVelX;
		float m_nVelY;
	
		// Dimensions
		int m_nWidth;
		int m_nHeight;

		// Offset for camera space
		float m_fOffsetX;
		float m_fOffsetY;

		// Base movement speed
		float m_nMoveX;
		float m_nMoveY;

		float m_fGravity;

		//	Image
		int m_nImageID;

		bool m_bIsActive;	

	/********** Private Accessors ************/

	/********** Private Mutators ************/

	/********** Private Utility Functions ************/

};

#endif