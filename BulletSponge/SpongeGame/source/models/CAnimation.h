///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CAnimation.h"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	Hold Animation related data
///////////////////////////////////////////////////////////////////////////
#ifndef _CANIMATION_H_
#define _CANIMATION_H_

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include <Windows.h>

class CAnimation
{
public:
	/********** Construct / Deconstruct / OP Overloads ************/
		CAnimation(int nImageID,int nImageWidth,int nImageHeight, int nFrameWidth,
					int nFrameHeight,int nFrames,int nCurrent,float fTimePerFrame = 1.0f,bool bLooping = true, bool bStatic = false);
		~CAnimation();

	/********** Public Utility Functions ************/
		// Returns the rect of the current frame
		RECT GetFrame();
		void ResetAnimation();

	/********** Public Accessors ************/
		int GetImageID() {return m_nImageID;}


	/********** Public Mutators  ************/
		void SetImageID(int nImageID) {m_nImageID = nImageID;}
		void SetImageWidth(int nImageWidth) {m_nWidth = nImageWidth;}
		void SetImageHeight(int nImageHeight) {m_nHeight = nImageHeight;}
		void SetFrameWidth(int nFrameWidth)  {m_nFrameWidth = nFrameWidth;}
		void SetFrameHeight(int nFrameHeight) {m_nFrameHeight = nFrameHeight;}
		void SetFrameCount(int nFrames) {m_nFrames = nFrames;}
		void SetCurrentFrame(int nCurrent) {m_nCurrentFrame = nCurrent;}		

private:
	/********** Private Members ************/
		// Image ID
		int m_nImageID;
		// Width and height of picture
		int m_nWidth;
		int m_nHeight;
		// Width and height of single frame
		int m_nFrameWidth;
		int m_nFrameHeight;
		// Number of frames
		int m_nFrames;
		// Current frame
		int m_nCurrentFrame;
		// Looping
		bool m_bLooping;
		// Static = single picture or manual
		bool m_bStatic;
		// Timing
		float m_fTimeWaited;
		float m_fTimePerFrame;
		// Is it playing?
		bool m_bIsPlaying;

	/********** Private Accessors ************/
		int GetImageWidth() {return m_nWidth;}
		int GetImageHeight() {return m_nHeight;}
		int GetFrameWidth() {return m_nFrameWidth;}
		int GetFrameHeight() {return m_nFrameHeight;}
		int GetFrameCount() {return m_nFrames;}
		int GetCurrentFrame() {return m_nCurrentFrame;}	
		float GetTimeWaited() {return m_fTimeWaited;}
		float GetTimePerFrame() {return m_fTimePerFrame;}
		bool GetLooping() {return m_bLooping;}	
		bool GetStatic() {return m_bStatic;}
		

	/********** Private Mutators ************/
		void SetLooping(bool bAutomatic) {m_bLooping = bAutomatic;}
		void SetStatic(bool bStatic) {m_bStatic = bStatic;}
		void SetTimeWaited(float fTime) {m_fTimeWaited = fTime;}
		void SetTimePerFrame(float fTime) {m_fTimePerFrame = fTime;}
	
	/********** Private Utility Functions ************/
		
};

#endif