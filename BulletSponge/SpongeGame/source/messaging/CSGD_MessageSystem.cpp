//	CSGD_MessageSystem.cpp

#include "CSGD_MessageSystem.h"

CSGD_MessageSystem* CSGD_MessageSystem::GetInstance(void)
{
	static CSGD_MessageSystem instance;
	return &instance;
}

void CSGD_MessageSystem::InitMessageSystem(MESSAGEPROC pfnMsgProc)
{
	//	Error check to make sure the message proc is valid.
	if (!pfnMsgProc)	return;

	//	Get the Msg Proc
	m_pfnMsgProc = pfnMsgProc;
}

void CSGD_MessageSystem::SendMsg(CBaseMessage* pMsg)
{
	//	Make sure the message exists.
	if (!pMsg)	return;

	//	Send the message to the queue for processing later on.
	m_MsgQueue.push(pMsg);
}

void CSGD_MessageSystem::ProcessMessages(void)
{
	//	Error check to make sure we get a message proc.
	if(!m_pfnMsgProc)	return;

	//	go through the entire queue and process the messsages that are waiting.
	while(!m_MsgQueue.empty())
	{
		m_pfnMsgProc(m_MsgQueue.front());		//	Process the first message.
		delete m_MsgQueue.front();				//	Delete the message memory.
		m_MsgQueue.pop();						//	Go to the next message.
	}
}

void CSGD_MessageSystem::ClearMessages(void)
{
	//	Clear out any messages waititng.
	while(!m_MsgQueue.empty())
	{
		delete m_MsgQueue.front();
		m_MsgQueue.pop();
	}
}

void CSGD_MessageSystem::ShutdownMessageSystem(void)
{
	//	Clear out any messages waiting.
	ClearMessages();

	//	Clear the function pointer
	m_pfnMsgProc = NULL;
}