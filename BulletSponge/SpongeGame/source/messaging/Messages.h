#ifndef _MESSAGES_H_
#define _MESSAGES_H_

typedef int MSGID;

enum eMsgTypes { MSG_NULL = 0,MSG_CREATE_PLAYER,MSG_PLAYER_HIT,MSG_DESTROY_PLAYER,MSG_CREATE_ENEMY,MSG_CREATE_ROBOT,MSG_DESTROY_ENEMY,MSG_CREATE_BULLET,MSG_DESTROY_BULLET,MSG_DESTROY_ROBOT, MSG_MAX };
class CBullet;
class CPlayer;
class CEnemy;
class CBase;
class CRobot;

class CBaseMessage
{
private:
	MSGID	m_msgID;

public:
	CBaseMessage(MSGID msgID)
	{
		m_msgID = msgID;
	}

	virtual ~CBaseMessage(void) {}

	MSGID GetMsgID(void)	{ return m_msgID; }
};


// CREATION MESSAGES
class CCreatePlayerMessage : public CBaseMessage
{
private:
	
public:
	CCreatePlayerMessage();
	~CCreatePlayerMessage();	
};

class CPlayerHitMessage : public CBaseMessage
{
private:
	CPlayer* m_pPlayer;
public:
	CPlayerHitMessage(CPlayer* pPlayer);
	~CPlayerHitMessage();
	CPlayer* GetPlayer(void) {return m_pPlayer;}
};

class CCreateEnemyMessage : public CBaseMessage
{
private:
	
public:
	CCreateEnemyMessage();
	~CCreateEnemyMessage();	
};

class CCreateRobotMessage : public CBaseMessage
{
private:
	
public:
	CCreateRobotMessage();
	~CCreateRobotMessage();	
};

class CCreateBulletMessage : public CBaseMessage
{
private:
	CBase* m_pOwner;
	float m_fAngle;
	int m_nImageID;
public:
	CCreateBulletMessage(CBase* pOwner,float fAngle,int nImageID);
	~CCreateBulletMessage();
	CBase* GetOwner(void) {return m_pOwner;}
	float GetAngle(void) {return m_fAngle;}
	int GetImage(void) {return m_nImageID;}
};

// DESTRUCTION MESSAGES

class CDestroyPlayerMessage : public CBaseMessage
{
private:
	CPlayer* m_pPlayer;
public:
	CDestroyPlayerMessage(CPlayer* pPlayer);
	~CDestroyPlayerMessage();
	CPlayer* GetPlayer(void) {return m_pPlayer;}
};

class CDestroyEnemyMessage : public CBaseMessage
{
private:
	CEnemy* m_pEnemy;
public:
	CDestroyEnemyMessage(CEnemy* pEnemy);
	~CDestroyEnemyMessage();
	CEnemy* GetEnemy(void) {return m_pEnemy;}
};

class CDestroyBulletMessage : public CBaseMessage
{
private:
	CBullet* m_pBullet;
public:
	CDestroyBulletMessage(CBullet* pBullet);
	~CDestroyBulletMessage();
	CBullet* GetBullet(void) {return m_pBullet;}
};

class CDestroyRobotMessage : public CBaseMessage
{
private:
	CRobot* m_pRobot;
public:
	CDestroyRobotMessage(CRobot* pRobot);
	~CDestroyRobotMessage();
	CRobot* GetRobot(void) {return m_pRobot;}
};


#endif