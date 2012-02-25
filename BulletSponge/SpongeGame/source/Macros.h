#ifndef _MACROS_H_
#define _MACROS_H_
////////////////////////////////////////////////////////////////////////
// File Name	:	"Macros.h"
//
// Author Name	:	JC Ricks
//
// Purpose		:	Macros for overused stuff, mainly singleton access
////////////////////////////////////////////////////////////////////////

#define D3D CSGD_Direct3D::GetInstance()
#define TEXTUREMAN CSGD_TextureManager::GetInstance()
#define XAUDIO CSGD_XAudio2::GetInstance()
#define DI CSGD_DirectInput::GetInstance()
#define OM CSGD_ObjectManager::GetInstance()
#define MS CSGD_MessageSystem::GetInstance()
#define ES CSGD_EventSystem::GetInstance()
#define GAME CGame::GetInstance()

#endif