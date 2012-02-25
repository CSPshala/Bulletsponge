///////////////////////////////////////////////////////
// File Name	:	"CGame.h"
//
// Author Name	:	JC Ricks
//
// Purpose		:	To provide gamesate interface
//////////////////////////////////////////////////////
#ifndef _IGAMESTATE_H_
#define _IGAMESTATE_H_

class IGameState
{
public:
	virtual ~IGameState() = 0 {} // Virtual Destructor
	virtual void Enter(void) = 0; // Enters the Game State
	virtual bool Input(void) = 0; // Input
	virtual void Update(void) = 0; // Update
	virtual void Render(void) = 0; // Draw
	virtual void Exit(void) = 0; // Leaves the Game State

};

#endif