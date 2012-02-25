////////////////////////////////////////////////////////
//  File			:	"CSGD_TextureManager.cpp"
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

#include "CSGD_TextureManager.h"
#include <assert.h>		// code in assertions gets compiled out in Release mode

//	For macros
#include "SGD_Util.h" // for SAFE_RELEASE and DXERROR

#pragma warning (disable : 4996)

CSGD_TextureManager CSGD_TextureManager::m_Instance;

///////////////////////////////////////////////////////////////////
//	Function:	"CSGD_TextureManager(Constructor)"
///////////////////////////////////////////////////////////////////
CSGD_TextureManager::CSGD_TextureManager(void)
{
	m_lpDevice = NULL;
	m_lpSprite = NULL;
}

///////////////////////////////////////////////////////////////////
//	Function:	"CSGD_TextureManager(Destructor)"
///////////////////////////////////////////////////////////////////
CSGD_TextureManager::~CSGD_TextureManager(void)
{
}

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
CSGD_TextureManager *CSGD_TextureManager::GetInstance(void)
{
	return &m_Instance;
}

///////////////////////////////////////////////////////////////////
//	Function:	"InitTextureManager"
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
bool CSGD_TextureManager::InitTextureManager(LPDIRECT3DDEVICE9 lpDevice, LPD3DXSPRITE lpSprite)
{
	m_lpDevice = lpDevice;
	m_lpDevice->AddRef();

	m_lpSprite = lpSprite;
	m_lpSprite->AddRef();

	return (m_lpDevice && m_lpSprite) ? true : false;
}

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
void CSGD_TextureManager::ShutdownTextureManager(void)
{
	for (unsigned int i = 0; i < m_Textures.size(); i++)
	{
		// Remove ref.
		m_Textures[i].ref = 0;

		// Release the texture if it's not being used.
		SAFE_RELEASE(m_Textures[i].texture);
		m_Textures[i].filename[0] = '\0';
	}

	// Clear the list of all loaded textures.
	m_Textures.clear();

	//	Release our references to the sprite interface and d3d device
	SAFE_RELEASE(m_lpSprite);
	SAFE_RELEASE(m_lpDevice);
}

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
int CSGD_TextureManager::LoadTexture(const char* szFilename, DWORD dwColorkey)
{
	//	Make sure the filename is valid.
	if (!szFilename)	return -1;

	// Make sure the texture isn't already loaded.
	for (unsigned int i = 0; i < m_Textures.size(); i++)
	{
		// compare strings without caring about upper or lowercase.
		if (stricmp(szFilename, m_Textures[i].filename) == 0)	// 0 means they are equal.
		{
			m_Textures[i].ref++; // add a reference to this texture.
			return i;	// return the index.
		}
	}

	// Look for an open spot.
	int nID = -1;
	for (unsigned int i = 0; i < m_Textures.size(); i++)
	{
		if (m_Textures[i].ref == 0)
		{
			nID = i;
			break;
		}
	}

	// if we didn't find an open spot, load it in a new one
	if (nID == -1)
	{
		// A temp texture object.
		TEXTURE loaded;

		// Copy the filename of the loaded texture.
		strcpy(loaded.filename, szFilename);

		// Load the texture from the given file.
		HRESULT hr = 0;
		if (FAILED(hr = D3DXCreateTextureFromFileEx(m_lpDevice, szFilename, 0, 0, D3DX_DEFAULT, 0,
			D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
			D3DX_DEFAULT, dwColorkey, 0, 0, &loaded.texture)))
		{
			// Failed.
			char szBuffer[256] = {0};
			sprintf(szBuffer, "Failed to Create Texture - %s", szFilename); 
			MessageBox(0, szBuffer, "TextureManager Error", MB_OK);
			return -1;
		}

		// AddRef.
		loaded.ref = 1;

		// Get surface description (to find Width/Height of the texture)
		D3DSURFACE_DESC d3dSurfDesc;
		ZeroMemory(&d3dSurfDesc, sizeof(d3dSurfDesc));

		loaded.texture->GetLevelDesc(0, &d3dSurfDesc);

		// Remember the Width and Height
		loaded.Width	= d3dSurfDesc.Width;
		loaded.Height	= d3dSurfDesc.Height;

		// Put the texture into the list.
		m_Textures.push_back(loaded);

		// Return the nID of the texture.
		return (int)m_Textures.size() - 1;
	}
	// we found an open spot
	else
	{
		// Make sure the texture has been released.
		SAFE_RELEASE(m_Textures[nID].texture);

		// Copy the filename of the loaded texture.
		strcpy(m_Textures[nID].filename, szFilename);

		// Load the texture from the given file.
		HRESULT hr = 0;
		if (FAILED(hr = D3DXCreateTextureFromFileEx(m_lpDevice, szFilename, 0, 0, D3DX_DEFAULT, 0,
			D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
			D3DX_DEFAULT, dwColorkey, 0, 0, &m_Textures[nID].texture)))
		{
			// Failed.
			char szBuffer[256] = {0};
			sprintf(szBuffer, "Failed to Create Texture - %s", szFilename); 
			MessageBox(0, szBuffer, "TextureManager Error", MB_OK);
			return -1;
		}

		// Get surface description (to find Width/Height of the texture)
		D3DSURFACE_DESC d3dSurfDesc;
		ZeroMemory(&d3dSurfDesc, sizeof(d3dSurfDesc));

		m_Textures[nID].texture->GetLevelDesc(0, &d3dSurfDesc);

		// Remember the Width and Height
		m_Textures[nID].Width	= d3dSurfDesc.Width;
		m_Textures[nID].Height	= d3dSurfDesc.Height;

		// AddRef
		m_Textures[nID].ref = 1;

		// Return the nID of the texture.
		return nID;
	}
}

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
void CSGD_TextureManager::UnloadTexture(int nID)
{
	// Make sure the nID is in range.
	assert(nID > -1 && nID < (int)m_Textures.size() && "nID is out of range");
	
	// Remove ref.
	m_Textures[nID].ref--;

	// Release the texture if it's not being used.
	if (m_Textures[nID].ref <= 0)
	{
		// Do a lazy delete and leave this spot empty
		SAFE_RELEASE(m_Textures[nID].texture);
		m_Textures[nID].filename[0] = '\0';
		m_Textures[nID].ref = 0;
	}
}

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
int CSGD_TextureManager::GetTextureWidth(int nID)
{
	// Make sure the nID is in range.
	assert(nID > -1 && nID < (int)m_Textures.size() && "nID is out of range");

	return m_Textures[nID].Width;
}

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
int CSGD_TextureManager::GetTextureHeight(int nID)
{
	// Make sure the nID is in range.
	assert(nID > -1 && nID < (int)m_Textures.size() && "nID is out of range");

	return m_Textures[nID].Height;
}

///////////////////////////////////////////////////////////////////
//	Function:	"DrawTexture"
//
//	Last Modified:		8/22/2009
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
bool CSGD_TextureManager::Draw(int nID, int nX, int nY, float fScaleX, float fScaleY,
	RECT* pSection, float fRotCenterX, float fRotCenterY, float fRotation, DWORD dwColor)
{
	// Make sure the nID is in range.
	assert(nID > -1 && nID < (int)m_Textures.size() && "nID is out of range");

	// Make sure that the texture is valid
	assert(m_Textures[nID].texture != NULL && "Attempting to draw released texture id");

	// Make sure the sprite was created and we have a valid texture.
	if (!m_lpSprite)
		return false;

	D3DXMATRIX scale;
	D3DXMATRIX rotation;
	D3DXMATRIX translate;
	D3DXMATRIX combined;

	// Initialize the Combined matrix.
	D3DXMatrixIdentity(&combined);

	// Scale the sprite.
	D3DXMatrixScaling(&scale, fScaleX, fScaleY, 1.0f);
	combined *= scale;

	// Rotate the sprite.
	D3DXMatrixTranslation(&translate, -fRotCenterX * fScaleX, -fRotCenterY * fScaleY, 0.0f);
	combined *= translate;
	D3DXMatrixRotationZ(&rotation, fRotation);
	combined *= rotation;
	D3DXMatrixTranslation(&translate, fRotCenterX * fScaleX, fRotCenterY * fScaleY, 0.0f);
	combined *= translate;

	// Translate the sprite
	D3DXMatrixTranslation(&translate, (float)nX, (float)nY, 0.0f);
	combined *= translate;

	// Apply the transform.
	m_lpSprite->SetTransform(&combined);

	// Draw the sprite.
	if (FAILED(m_lpSprite->Draw(m_Textures[nID].texture, pSection, NULL, NULL, dwColor)))
		DXERROR("Failed to draw the texture.");

	// Move the world back to identity.
	D3DXMatrixIdentity(&combined);
	m_lpSprite->SetTransform(&combined);

	// success.
	return true;
}

///////////////////////////////////////////////////////////////////
//	Function:	"DrawTexture"
//
//	Last Modified:		8/22/2009
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

bool CSGD_TextureManager::DrawF(int nID, float nX, float nY, float fScaleX, float fScaleY,
	RECT* pSection, float fRotCenterX, float fRotCenterY, float fRotation, DWORD dwColor)
{
	// Make sure the nID is in range.
	assert(nID > -1 && nID < (int)m_Textures.size() && "nID is out of range");

	// Make sure that the texture is valid
	assert(m_Textures[nID].texture != NULL && "Attempting to draw released texture id");

	// Make sure the sprite was created and we have a valid texture.
	if (!m_lpSprite)
		return false;

	D3DXMATRIX scale;
	D3DXMATRIX rotation;
	D3DXMATRIX translate;
	D3DXMATRIX combined;

	// Initialize the Combined matrix.
	D3DXMatrixIdentity(&combined);

	// Scale the sprite.
	D3DXMatrixScaling(&scale, fScaleX, fScaleY, 1.0f);
	combined *= scale;

	// Rotate the sprite.
	D3DXMatrixTranslation(&translate, -fRotCenterX * fScaleX, -fRotCenterY * fScaleY, 0.0f);
	combined *= translate;
	D3DXMatrixRotationZ(&rotation, fRotation);
	combined *= rotation;
	D3DXMatrixTranslation(&translate, fRotCenterX * fScaleX, fRotCenterY * fScaleY, 0.0f);
	combined *= translate;

	// Translate the sprite
	D3DXMatrixTranslation(&translate, (float)nX, (float)nY, 0.0f);
	combined *= translate;

	// Apply the transform.
	m_lpSprite->SetTransform(&combined);

	// Draw the sprite.
	if (FAILED(m_lpSprite->Draw(m_Textures[nID].texture, pSection, NULL, NULL, dwColor)))
		DXERROR("Failed to draw the texture.");

	// Move the world back to identity.
	D3DXMatrixIdentity(&combined);
	m_lpSprite->SetTransform(&combined);

	// success.
	return true;
}

#pragma warning (default : 4996)