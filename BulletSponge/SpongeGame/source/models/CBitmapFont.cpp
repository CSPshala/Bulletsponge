///////////////////////////////////////////////////////
// File Name	:	"CBitmapFont.cpp"
//
// Author Name	:	JC Ricks
//
// Purpose		:	To hold a font's info and drawing
//////////////////////////////////////////////////////
#include "CBitmapFont.h"

CBitmapFont::CBitmapFont(int nCharWidth,int nCharHeight,int nNumCols,int nImageID,char cFirstChar)
{
	m_nCharWidth = nCharWidth;
	m_nCharHeight = nCharHeight;
	m_nNumCols = nNumCols;
	m_nImageID = nImageID;
	m_cFirstChar = cFirstChar;	
}

void CBitmapFont::Print(string szTextToPrint,float nPosX,float nPosY,int nSpacing,float fScale,DWORD dwColor)
{
		// Remember original X

		float nOriginalX = nPosX;

		for(unsigned int i = 0; i < szTextToPrint.size(); ++i)
		{
			const char ch = szTextToPrint[i];

			if (ch == ' ')
			{
				nPosX += m_nCharWidth * fScale;
				continue;
			}
			else if (ch == '\n')
			{
				nPosX = nOriginalX;
				nPosY += m_nCharHeight * fScale;
				continue;
			}

			int id = (int)(ch - m_cFirstChar);

			RECT rLetter = CellAlgorithm(id);

			TEXTUREMAN->DrawF(m_nImageID,nPosX,nPosY,fScale,fScale,&rLetter,0,0,0,dwColor);

			nPosX += (m_nCharWidth + nSpacing) * fScale;
		}
}

RECT CBitmapFont::CellAlgorithm(int id)
{
	RECT temp;
	temp.left = id % GetNumCols() * GetCharWidth();
	temp.right = temp.left + GetCharWidth();
	temp.top = id / GetNumCols() * GetCharHeight();
	temp.bottom = temp.top + GetCharHeight();

	return temp;
}