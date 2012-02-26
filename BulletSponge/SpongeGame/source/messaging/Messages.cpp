#include "Messages.h"
#include "../models/CPlayer.h"
#include "../models/CEnemy.h"
#include "../models/CBullet.h"
#include "../models/CRobot.h"

CCreatePlayerMessage::CCreatePlayerMessage() : CBaseMessage(MSG_CREATE_PLAYER)
{

}
CCreatePlayerMessage::~CCreatePlayerMessage()
{

}

CPlayerHitMessage::CPlayerHitMessage(CPlayer* pPlayer) : CBaseMessage(MSG_PLAYER_HIT)
{
	m_pPlayer = pPlayer;
	m_pPlayer->AddRef();
}
CPlayerHitMessage::~CPlayerHitMessage()
{
	m_pPlayer->Release();
}

CCreateEnemyMessage::CCreateEnemyMessage() : CBaseMessage(MSG_CREATE_ENEMY)
{

}
CCreateEnemyMessage::~CCreateEnemyMessage()
{

}

CCreateRobotMessage::CCreateRobotMessage() : CBaseMessage(MSG_CREATE_ROBOT)
{

}
CCreateRobotMessage::~CCreateRobotMessage()
{

}

CCreateBulletMessage::CCreateBulletMessage(CBase* pOwner,float fAngle,int nImageID) : CBaseMessage(MSG_CREATE_BULLET)
{
	m_pOwner = pOwner;
	m_pOwner->AddRef();
	m_fAngle = fAngle;
	m_nImageID = nImageID;
}

CCreateBulletMessage::~CCreateBulletMessage()
{
	m_pOwner->Release();
}

CDestroyPlayerMessage::CDestroyPlayerMessage(CPlayer* pPlayer) : CBaseMessage(MSG_DESTROY_PLAYER)
{
	m_pPlayer = pPlayer;
	m_pPlayer->AddRef();
}
CDestroyPlayerMessage::~CDestroyPlayerMessage()
{
	m_pPlayer->Release();
}

CDestroyEnemyMessage::CDestroyEnemyMessage(CEnemy* pEnemy) : CBaseMessage(MSG_DESTROY_ENEMY)
{
	m_pEnemy = pEnemy;
	m_pEnemy->AddRef();
}
CDestroyEnemyMessage::~CDestroyEnemyMessage()
{
	m_pEnemy->Release();
}

CDestroyBulletMessage::CDestroyBulletMessage(CBullet* pBullet) : CBaseMessage(MSG_DESTROY_BULLET)
{
	m_pBullet = pBullet;
	m_pBullet->AddRef();
}

CDestroyBulletMessage::~CDestroyBulletMessage()
{
	m_pBullet->Release();
}

CDestroyRobotMessage::CDestroyRobotMessage(CRobot* pRobot) : CBaseMessage(MSG_DESTROY_ROBOT)
{
	m_pRobot = pRobot;
	m_pRobot->AddRef();
}

CDestroyRobotMessage::~CDestroyRobotMessage()
{
	m_pRobot->Release();
}