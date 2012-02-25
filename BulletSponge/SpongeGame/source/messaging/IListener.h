#pragma once

#include "CEvent.h"

class IListener
{
public:
	IListener(void) {}
	virtual ~IListener(void) {}

	//	Blue print function - MUST be defined in the derived class.
	//	Mini-proc function for whenever this object receives and event
	virtual void HandleEvent(CEvent* pEvent) = 0;
};