///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CAnimation.cpp"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	Hold Animation related data
///////////////////////////////////////////////////////////////////////////

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "CAnimation.h"
#include "../Macros.h"
#include "../CGame.h"

///////////////////////////////////////////////
//  CONSTRUCTOR / DECONSTRUCT / OP OVERLOADS
///////////////////////////////////////////////
CAnimation::CAnimation(int nImageID,int nImageWidth,int nImageHeight, int nFrameWidth,
				int nFrameHeight,int nFrames,int nCurrent,float fTimePerFrame,bool bLooping,bool bStatic)
{
	SetImageID(nImageID);
	SetImageWidth(nImageWidth);
	SetImageHeight(nImageHeight);
	SetFrameWidth(nFrameWidth);
	SetFrameHeight(nFrameHeight);
	// Because framecount starts at 0
	SetFrameCount(nFrames - 1);
	SetCurrentFrame(nCurrent);
	SetLooping(bLooping);
	SetStatic(bStatic);
	SetTimePerFrame(fTimePerFrame);
	SetTimeWaited(0.0f);
}

CAnimation::~CAnimation()
{
}

////////////////////////////////////////
//		PUBLIC UTILITY FUNCTIONS
////////////////////////////////////////
// Gets the current frame in the animation
// and moves to the next one in order.
RECT CAnimation::GetFrame()
{
	SetTimeWaited(GetTimeWaited() + GAME->GetElapsedTime());

	RECT tRect;
	
	tRect.left = GetCurrentFrame() * GetFrameWidth();
	tRect.top = 0;
	tRect.right = tRect.left + GetFrameWidth();
	tRect.bottom = tRect.top + GetFrameHeight();	
	
				
	if(GetTimeWaited() > GetTimePerFrame() && !GetStatic())
	{	
		SetTimeWaited(0.0f);
		SetCurrentFrame(GetCurrentFrame() + 1);

		if(GetCurrentFrame() > GetFrameCount())
		{
			if(GetLooping())
				ResetAnimation();	
			else
			{
				SetCurrentFrame(GetFrameCount());
				SetStatic(true);
			}
		}
		
	}

	return tRect;
}

////////////////////////////////////////
//		PRIVATE UTILITY FUNCTIONS
////////////////////////////////////////
void CAnimation::ResetAnimation()
{
	SetCurrentFrame(0);
	SetTimeWaited(0.0f);
	SetStatic(false);
}

////////////////////////////////////////
//	    PUBLIC ACCESSORS / MUTATORS
////////////////////////////////////////

////////////////////////////////////////
//	    PRIVATE ACCESSORS / MUTATORS
////////////////////////////////////////
