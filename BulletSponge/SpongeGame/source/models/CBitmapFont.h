///////////////////////////////////////////////////////
// File Name	:	"CBitmapFont.h"
//
// Author Name	:	JC Ricks
//
// Purpose		:	To hold a font's info and drawing
//////////////////////////////////////////////////////
#ifndef _CBITMAPFONT_H_
#define _CBITMAPFONT_H_

#include "../Macros.h"
#include <Windows.h>
#include <string>
using std::string;

#include "../SGD Wrappers/CSGD_TextureManager.h"

class CBitmapFont
{
private:
	
	// Image variables
	int m_nCharWidth;
	int m_nCharHeight;
	int m_nNumCols;
	
	// Image ID
	int m_nImageID;	

	// First char on sheet
	char m_cFirstChar; 

	// Cell algorithm
	RECT CellAlgorithm(int id);
		

public:
	CBitmapFont(int nCharWidth,int nCharHeight,int nNumCols,int nImageID,char cFirstChar);
	void Print(string szTextToPrint,float nPosX,float nPosY,int nSpacing = 1,float fScale = 1.0f, DWORD dwColor = D3DCOLOR_ARGB(255,255,255,255));

	// Accessors
	int GetCharWidth() {return m_nCharWidth;}
	int GetCharHeight() {return m_nCharHeight;}
	int GetNumCols() {return m_nNumCols;}
	int GetImageID() {return m_nImageID;}
	char GetFirstChar() {return m_cFirstChar;}	
	
};


#endif