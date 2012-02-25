//////////////////////////////////////////////////////////////
// File Name	:	"CLevel.h"
//
// Author Name	:	JC Ricks
//
// Purpose		:	To Parse level files and provide to Game
//					Also contains Tile structure for tile usage					
/////////////////////////////////////////////////////////////
#ifndef _CLEVEL_H_
#define _CLEVEL_H_

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include <vector>
#include <string>
using namespace std;

#include "..\SGD Wrappers\CSGD_TextureManager.h"
#include "..\SGD Wrappers\CSGD_Direct3D.h"
#include "..\IBaseInterface.h"
#include "../messaging/CSGD_EventSystem.h"
#include "..\models\CWorld.h"

////////////////////////////////////////
//		   FORWARD DECLARATIONS
////////////////////////////////////////
struct TTile;

class CLevel : public IListener
{
public:
	/********** Construct / Deconstruct / OP Overloads ************/
		CLevel();
		~CLevel();

	/********** Public Utility Functions ************/
		void Update();
		void Render();
	
		void AddRef();
		void Release();

		void LoadLevel(string szFile);
		void HandleEvent(CEvent* pEvent);

	/********** Public Accessors ************/
		int GetMapWidth() {return m_nMapWidth;}
		int GetMapHeight() {return m_nMapHeight;}
		int GetTileWidth() {return m_nTileWidth;}
		int GetTileHeight() {return m_nTileHeight;}
		int GetPlayerSpawnX() {return m_nPlayerSpawnX;}
		int GetPlayerSpawnY() {return m_nPlayerSpawnY;}
		float GetOffsetX() {return m_fCameraX;}
		float GetOffsetY() {return m_fCameraY;}

	/********** Public Mutators  ************/	
		void SetMapWidth(int nMapWidth) {m_nMapWidth = nMapWidth;}
		void SetMapHeight(int nMapHeight) {m_nMapHeight = nMapHeight;}
		void SetTileWidth(int nTileWidth) {m_nTileWidth = nTileWidth;}
		void SetTileHeight(int nTileHeight) {m_nTileHeight = nTileHeight;}
		void SetPlayerSpawnX(int nX) {m_nPlayerSpawnX = nX;}
		void SetPlayerSpawnY(int nY) {m_nPlayerSpawnY = nY;}
		

private:
	/********** Private Members ************/
		// Camera viewport
		// So Level class can handle everything pertaining to level display
		float m_fCameraX;
		float m_fCameraY;

		// Ref count for Object Manager
		unsigned int m_uiRefCount;

		// Map size (in pixels based on rows/columns and tile size)
		int m_nMapWidth;
		int m_nMapHeight;

		// Tile size
		int m_nTileWidth;
		int m_nTileHeight;

		// Player Spawn in world space
		int m_nPlayerSpawnX;
		int m_nPlayerSpawnY;

		// Strings holding tileset names (used for tileset loading)
		vector<string> m_vTileSet;

		// Tileset Image IDs
		vector<int> m_vTileSetIDs;		

		//***** Tile Layer vectors, can hold 3 layers need not be fully filled ******************
		// First vector in each is X value (running across top of screen)
		// Second Vector contained in each is Y value (running down side of screen holding tiles)
		// EG:  xxxxxxxxxxxxxxxxxxx
		//		ytttttttttttttttttt
		//		ytttttttttttttttttt
		//		ytttttttttttttttttt
		//		ytttttttttttttttttt
		//		ytttttttttttttttttt
		//****************************************************************************************
		vector<vector<TTile*>*> m_vLayer1;
		vector<vector<TTile*>*> m_vLayer2;
		vector<vector<TTile*>*> m_vLayer3;

		// Sound IDs
			// BGM 
			int m_nBGMID;

	/********** Private Accessors ************/

	/********** Private Mutators ************/

	/********** Private Utility Functions ************/	
		void CleanUp();
			// Level Load stuff used to clean up load level function
		void LoadTiles(string szFile);
		void LoadCollisions(string szFile);

			// Layer drawing stuff to clean up render function
		void DrawTileLayers();	
};


// Tile struct to hold tile info
struct TTile
{
	// World position
	int nPosX;
	int nPosY;
	// Index into picture for tile
	int nIndX;
	int nIndY;
	// Image number to use
	int nImage;
	// If the tile is actually used
	bool bOccupied;

	TTile(int posX = 0,int posY = 0,int nX = 0,int nY = 0,int nPic = 0)
	{	
		nPosX = posX;
		nPosY = posY;
		nIndX = nX;
		nIndY = nY;
		nImage = nPic;
	}
};

#endif