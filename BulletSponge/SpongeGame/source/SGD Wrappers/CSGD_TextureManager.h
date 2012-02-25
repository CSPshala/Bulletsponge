////////////////////////////////////////////////////////
//  File			:	"CSGD_TextureManager.h"
//
//  Author			:	Jensen Rivera (JR)
//
//  Date Created	:	6/26/2006
//
//  Purpose			:	Wrapper class for Direct3D.
////////////////////////////////////////////////////////

/* 
Disclaimer:
This source code was developed for and is the property of:

(c) Full Sail University Game Development Curriculum 2008-2012 and
(c) Full Sail Real World Education Game Design & Development Curriculum 2000-2008

Full Sail students may not redistribute or make this code public, 
but may use it in their own personal projects.
*/

#pragma once

// The include files for Direct3D9
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
using std::vector;

class CSGD_TextureManager
{
private:
	// All the data we need to contain a texture.
	typedef struct _TEXTURE
	{
		char				filename[_MAX_PATH];	// The filename of this texture.
		int					ref;					// The number of times this texture has be loaded.
		LPDIRECT3DTEXTURE9	texture;				// A pointer to the texture.
		int					Width;					// The width of the texture.
		int					Height;					// The Height of the texture.

		// Default constructor.
		_TEXTURE()
		{
			filename[0] = '\0';
			ref = 0;
			texture = NULL;
			Width = 0;
			Height = 0;
		}
	} TEXTURE, *LPTEXTURE;

private:
	///////////////////////////////////////////////////////////////////
	//	Function:	CSGD_TextureManager(Constructors)
	///////////////////////////////////////////////////////////////////
	CSGD_TextureManager(void);
	CSGD_TextureManager(CSGD_TextureManager &ref);
	CSGD_TextureManager &operator=(CSGD_TextureManager &ref);
	~CSGD_TextureManager(void);

private:
	vector<TEXTURE>				m_Textures;			// A list of all the loaded textures.
	LPDIRECT3DDEVICE9			m_lpDevice;			// A pointer to the Direct3D device.
	LPD3DXSPRITE				m_lpSprite;			// A pointer to the sprite interface.
	static CSGD_TextureManager	m_Instance;			// An instance to this class.

public:

	///////////////////////////////////////////////////////////////////
	//	Function:	"GetInstance"
	//
	//	Last Modified:		6/26/2006
	//
	//	Input:		void
	//
	//	Return:		An instance to this class.
	//
	//	Purpose:	Gets an instance to this class.
	///////////////////////////////////////////////////////////////////
	static CSGD_TextureManager *GetInstance(void);

	///////////////////////////////////////////////////////////////////
	//	Function:	"InitializeTextureManager"
	//
	//	Last Modified:		8/29/2006
	//
	//	Input:		lpDevice	-	A pointer to the Direct3D device.
	//				lpSprite	-	A pointer to the sprite object.
	//
	//	Return:		true, if successful.
	//
	//	Purpose:	Initializes the texture manager.
	///////////////////////////////////////////////////////////////////
	bool InitTextureManager(LPDIRECT3DDEVICE9 lpDevice, LPD3DXSPRITE lpSprite);

	///////////////////////////////////////////////////////////////////
	//	Function:	"ShutdownTextureManager"
	//
	//	Last Modified:		10/29/2008
	//
	//	Input:		void
	//
	//	Return:		void
	//
	//	Purpose:	Unloads all the loaded textures and 
	//				releases references to sprite and d3d devices.
	///////////////////////////////////////////////////////////////////
	void ShutdownTextureManager(void);

	///////////////////////////////////////////////////////////////////
	//	Function:	"LoadTexture"
	//
	//	Last Modified:		10/29/2008
	//
	//	Input:		szFilename	-	The file to load.
	//				dwColorkey	-	The color key to use on the texture (use D3DCOLOR_XRGB() Macro).
	//								NOTE: 0 = no color key (which will use the alpha from the image instead)
	//
	//	Return:		The id to the texture that was loaded or found, -1 if it failed.  
	//	
	//	NOTE:		The function searches to see if the texture was already loaded 
	//				and returns the id if it was.
	//
	//	Purpose:	To load a texture from a file. 
	//
	//  NOTE:		Image dimensions must be a power of 2 (i.e. 256x64).
	//
	//				Supports .bmp, .dds, .dib, .hdr, .jpg, .pfm, .png, 
	//				.ppm, and .tga files. 
	///////////////////////////////////////////////////////////////////
	int LoadTexture(const char* szFilename, DWORD dwColorkey = 0);

	///////////////////////////////////////////////////////////////////
	//	Function:	"UnloadTexture"
	//
	//	Last Modified:		10/29/2008
	//
	//	Input:		nID	-	The id to the texture to release.
	//
	//	Return:		void
	//
	//	Purpose:	Releases a reference to a given texture. When the
	//				reference to the texture is zero, the texture is
	//				released from memory.
	///////////////////////////////////////////////////////////////////
	void UnloadTexture(int nID);

	///////////////////////////////////////////////////////////////////
	//	Function:	"GetTextureWidth"
	//
	//	Last Modified:		9/21/2006
	//
	//	Input:		nID	-	The id to the texture who's width you want.
	//
	//	Return:		The width of the given texture.
	//
	//	Purpose:	Gets the width of a specified texture.
	///////////////////////////////////////////////////////////////////
	int GetTextureWidth(int nID);

	///////////////////////////////////////////////////////////////////
	//	Function:	"GetTextureHeight"
	//
	//	Last Modified:		9/21/2006
	//
	//	Input:		nID	-	The id to the texture who's height you want.
	//
	//	Return:		The height of the given texture.
	//
	//	Purpose:	Gets the height of a specified texture.
	///////////////////////////////////////////////////////////////////
	int GetTextureHeight(int nID);

	///////////////////////////////////////////////////////////////////
	//	Function:	"DrawTexture"
	//
	//	Last Modified:		7/09/2008
	//
	//	Input:		nID			-	The id of the texture to draw.
	//				nX			-	The x position to draw the texture at.
	//				nY			-	The y position to draw the texture at.
	//				fScaleX		-	How much to scale the texture in the x.
	//				fScaleY		-	How much to scale the texture in the y.
	//				pSection	-	The section of the bitmap to draw.
	//				fRotCenterX	-	The x center to apply the rotation from.
	//				fRotCenterY	-	The y center to apply the rotation from.
	//				fRotation	-	How much to rotate the texture.
	//				dwColor		-	The color to apply to the texture (use D3DCOLOR_XRGB() Macro).
	//
	//	Return:		true if successful.
	//
	//	Purpose:	Draws a texture to the screen.
	//
	//	NOTE:	Drawing a section of an image will only work properly if 
	//			that image is a power of 2!
	///////////////////////////////////////////////////////////////////
	bool Draw(int nID, int nX, int nY, float fScaleX = 1.0f, float fScaleY = 1.0f,
		RECT* pSection = NULL, float fRotCenterX = 0.0f, float fRotCenterY = 0.0f, float fRotation = 0.0f, DWORD dwColor = 0xFFFFFFFF);

	bool DrawF(int nID, float nX, float nY, float fScaleX = 1.0f, float fScaleY = 1.0f,
		RECT* pSection = NULL, float fRotCenterX = 0.0f, float fRotCenterY = 0.0f, float fRotation = 0.0f, DWORD dwColor = 0xFFFFFFFF);
};
