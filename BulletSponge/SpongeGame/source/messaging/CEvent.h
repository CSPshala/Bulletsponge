#pragma once

#include <string>
using std::string;

typedef string EVENTID;		//	could be an int or enum, just used strings to change it
							//	up.

class CEvent
{
private:
	EVENTID			m_EventID;	//	The event's id type.
	void*			m_pParam;	//	Store any variable type (but only one)
public:
	CEvent(EVENTID eventID, void* pParam = NULL)
	{
		m_EventID = eventID;
		m_pParam  = pParam;
	}

	~CEvent() {}

	//	Accessors:
	inline EVENTID  GetEventID(void)	{	return m_EventID; }
	inline void*	GetParam(void)		{	return m_pParam; }
};