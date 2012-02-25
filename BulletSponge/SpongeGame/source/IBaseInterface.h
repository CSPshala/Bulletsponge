///////////////////////////////////////////////////////
// File Name	:	"IBaseInterface.h"
//
// Author Name	:	JC Ricks
//
// Purpose		:	Interface for Object manager
//////////////////////////////////////////////////////
#ifndef _IBASEINTERFACE_H_
#define _IBASEINTERFACE_H_
#include <Windows.h>

class IBaseInterface
{
public:
	virtual ~IBaseInterface() = 0 {}
	virtual void Update(float fElapsedTime) = 0;
	virtual void Render() = 0;
	
	virtual void AddRef() = 0;
	virtual void Release() = 0;

	virtual int GetType() = 0;

	virtual RECT GetCollisionRect() = 0;
	virtual bool CheckCollision(IBaseInterface* pBase) = 0;
};

#endif