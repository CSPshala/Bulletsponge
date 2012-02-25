//////////////////////////////////////////////////////////////
// File Name	:	"CLevel.cpp"
//
// Author Name	:	JC Ricks
//
// Purpose		:	To Parse level files and provide to Game
//					Also contains Tile structure for tile usage					
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//	Additional macros to clean up access of my 2D vectors
#define LAYER1(x,y) ((*m_vLayer1[x])[y])
#define LAYER2(x,y) ((*m_vLayer2[x])[y])
#define LAYER3(x,y) ((*m_vLayer3[x])[y])
////////////////////////////////////////////////////////////

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "../Macros.h"
#include "CLevel.h"
#include "CPlayer.h"
#include "../CGame.h"
#include "../states/CGamePlayState.h"
#include "../CSGD_ObjectManager.h"
#include "../factories/CSGD_ObjectFactory.h"
#include <fstream>

///////////////////////////////////////////////
//  CONSTRUCTOR / DECONSTRUCT / OP OVERLOADS
///////////////////////////////////////////////

CLevel::CLevel()
{
	m_uiRefCount = 1;

	SetTileWidth(0);
	SetTileHeight(0);
	SetMapWidth(0);
	SetMapHeight(0);
	SetPlayerSpawnX(0);
	SetPlayerSpawnY(0);

	// Music	
	m_nBGMID = XAUDIO->MusicLoadSong("resource/sound/JCR_atma.xwm");

	ES->RegisterClient("playermoved",this);
}

CLevel::~CLevel()
{
	CleanUp();

	ES->UnregisterClient("playermoved", this);
}

////////////////////////////////////////
//		PUBLIC UTILITY FUNCTIONS
////////////////////////////////////////

void CLevel::Update()
{
	if(!XAUDIO->MusicIsSongPlaying(m_nBGMID))
		XAUDIO->MusicPlaySong(m_nBGMID,true);
}

void CLevel::Render()
{	
	DrawTileLayers();

	D3D->GetSprite()->Flush();	// Draw everything now that is queued up

	// Drawing Text
	if(CGamePlayState::GetInstance()->GetRenderBoxes())
	{
		D3D->DrawText("DEMO LEVEL",290,0,255,0,0);	
		D3D->DrawText("COLLISION BOXES ON",0,0,255,0,0);
	}
}

void CLevel::AddRef()
{
	m_uiRefCount++;
}

void CLevel::Release()
{
	m_uiRefCount--;

	if(m_uiRefCount == 0)
		delete this;

	
}

void CLevel::LoadLevel(string szFile)
{
	CleanUp();
	LoadTiles(szFile);

	// Setting camera position
	m_fCameraX = 0;
	m_fCameraY = (float)GetPlayerSpawnY() - 418;
	
	LoadCollisions(szFile);		
}

void CLevel::HandleEvent(CEvent* pEvent)
{
	if("playermoved" == pEvent->GetEventID())
	{
		CPlayer* tPlayer = (CPlayer*)pEvent->GetParam();

		/*this->m_fCameraX = m_fCameraX + tPlayer->GetVelX() * CGame::GetInstance()->GetElapsedTime();
		this->m_fCameraY = m_fCameraY + tPlayer->GetVelY() * CGame::GetInstance()->GetElapsedTime();*/

		this->m_fCameraX = tPlayer->GetPosX() - (CGame::GetInstance()->GetWindowWidth() >> 1);
		this->m_fCameraY = tPlayer->GetPosY() - (CGame::GetInstance()->GetWindowHeight() >> 1) - 100;

		ES->SendEvent("camerachange",this);		
	}
}

////////////////////////////////////////
//		PRIVATE UTILITY FUNCTIONS
////////////////////////////////////////


void CLevel::DrawTileLayers()
{
	//*************
	// TODO!!!!!!
	// DRAW CAMERA VIEWPORT BY USING col*tilesize + camera offset
	//*************

	// RENDERING LAYER 1
	for(unsigned int x = 0; x < m_vLayer1.size(); ++x)
	{
		for(unsigned int y = 0; y < m_vLayer1[x]->size(); ++y)
		{
			if(LAYER1(x,y)->bOccupied)
			{
				// Setting rect for tile position on tileset
				RECT rTemp;
				rTemp.left = LAYER1(x,y)->nIndX * GetTileWidth();
				rTemp.top = LAYER1(x,y)->nIndY * GetTileHeight();
				rTemp.right = rTemp.left + GetTileWidth();
				rTemp.bottom = rTemp.top + GetTileHeight();

				TEXTUREMAN->DrawF(m_vTileSetIDs[LAYER1(x,y)->nImage],(LAYER1(x,y)->nPosX * GetTileWidth()) - m_fCameraX,(LAYER1(x,y)->nPosY * GetTileHeight()) - m_fCameraY,1.0f,1.0f,&rTemp);

			}			
		}		
	}

	// RENDERING LAYER 2 (if it exists)
	if(!m_vLayer2.empty())
		for(unsigned int x = 0; x < m_vLayer2.size(); ++x)
			for(unsigned int y = 0; y < m_vLayer2[x]->size(); ++y)
			{
				if(LAYER2(x,y)->bOccupied)
				{
					// Setting rect for tile position on tileset
					RECT rTemp;
					rTemp.left = LAYER2(x,y)->nIndX * GetTileWidth() ;
					rTemp.top = LAYER2(x,y)->nIndY * GetTileHeight();
					rTemp.right = rTemp.left + GetTileWidth();
					rTemp.bottom = rTemp.top + GetTileHeight();

					TEXTUREMAN->DrawF(m_vTileSetIDs[LAYER2(x,y)->nImage],LAYER2(x,y)->nPosX * GetTileWidth() - m_fCameraX ,LAYER2(x,y)->nPosY * GetTileHeight() - m_fCameraY ,1.0f,1.0f,&rTemp);
				}
			}

	// RENDERING LAYER 3 (if it exists)
	if(!m_vLayer3.empty())
		for(unsigned int x = 0; x < m_vLayer3.size(); ++x)
			for(unsigned int y = 0; y < m_vLayer3[x]->size(); ++y)
			{
				if(LAYER3(x,y)->bOccupied)
				{
					// Setting rect for tile position on tileset
					RECT rTemp;
					rTemp.left = LAYER3(x,y)->nIndX * GetTileWidth() ;
					rTemp.top = LAYER3(x,y)->nIndY * GetTileHeight();
					rTemp.right = rTemp.left + GetTileWidth();
					rTemp.bottom = rTemp.top + GetTileHeight();

					TEXTUREMAN->DrawF(m_vTileSetIDs[LAYER3(x,y)->nImage],LAYER3(x,y)->nPosX * GetTileWidth() - m_fCameraX ,LAYER3(x,y)->nPosY * GetTileHeight() - m_fCameraY,1.0f,1.0f,&rTemp);
				}
			}
	
}

void CLevel::LoadTiles(string szFile)
{
	ifstream ifs(szFile.c_str(),ios_base::in | ios_base::binary);
	if(ifs.is_open())
	{
		// Temporary variables
		int* nMapCol = new int;
		int* nMapRow = new int;
		int* nTileWidth = new int;
		int* nTileHeight = new int;
		int* nTileSetNames = new int;
		int* nPlayerSpawnX = new int;
		int* nPlayerSpawnY = new int;
		bool* bLayer2 = new bool;
		bool* bLayer3 = new bool;

		// Columns / rows
		ifs.read((char*)nMapCol,4);
		ifs.read((char*)nMapRow,4); 

		// Individual tile width/height
		ifs.read((char*)nTileWidth,4);
		ifs.read((char*)nTileHeight,4);

		SetTileWidth(*nTileWidth);
		SetTileHeight(*nTileHeight);

		// Calculating map size in pixels
		SetMapWidth(*nMapCol * GetTileWidth());
		SetMapHeight(*nMapRow * GetTileHeight());

		// Getting player spawn
		ifs.read((char*)nPlayerSpawnX,4);
		ifs.read((char*)nPlayerSpawnY,4);

		SetPlayerSpawnX(*nPlayerSpawnX * GetTileWidth());
		SetPlayerSpawnY(*nPlayerSpawnY * GetTileHeight());

		// Layer 2 / 3 Exist?
		ifs.read((char*)bLayer2,1);
		ifs.read((char*)bLayer3,1);

			// Pushing vectors onto vectors
			for(int i = 0; i < *nMapCol; ++i)
			{
				m_vLayer1.push_back(new vector<TTile*>);

				// Only if they exist
				if(*bLayer2)
					m_vLayer2.push_back(new vector<TTile*>);
				if(*bLayer3)
					m_vLayer3.push_back(new vector<TTile*>);
			}


		// Number of tileset names
		ifs.read((char*)nTileSetNames,4);		

		// This loop is only to skip tileset names using seekg
		for(int i = 0; i < *nTileSetNames; ++i)
		{
			// Temporary ID until texture manager handles it
			m_vTileSetIDs.push_back(int(0));

			// Length of string
			int* nLength = new int;
			ifs.read((char*)nLength,4);
			
			// Temp char array to hold filename
			char* nFileName = new char[*nLength + 1];
			nFileName[*nLength] = '\0';

			// Reading filename then copying to string vector
			ifs.read(nFileName,*nLength);
			
			this->m_vTileSet.push_back(string(nFileName));

			delete nLength;
			delete[] nFileName;
		}

		// Reading in first layer
		for(int x = 0; x < *nMapCol; ++x)
			for(int y = 0; y < *nMapRow; ++y)
			{
				TTile* tempTile = new TTile;

				int* nPosX = new int;
				int* nPosY = new int;
				int* nIndX = new int;
				int* nIndY = new int;
				int* nImage = new int;
				bool* bOccupied = new bool;

				ifs.read((char*)nPosX,4);
				ifs.read((char*)nPosY,4);
				ifs.read((char*)nIndX,4);
				ifs.read((char*)nIndY,4);
				ifs.read((char*)nImage,4);
				ifs.read((char*)bOccupied,1);

				tempTile->nPosX = *nPosX;
				tempTile->nPosY = *nPosY;
				tempTile->nIndX = *nIndX;
				tempTile->nIndY = *nIndY;
				tempTile->nImage = *nImage;
				tempTile->bOccupied = *bOccupied;

				delete nPosX;
				delete nPosY;
				delete nIndX;
				delete nIndY;
				delete nImage;
				delete bOccupied;

				m_vLayer1[x]->push_back(tempTile);
			}

		// Reading in second layer
		if(*bLayer2)
		{
			for(int x = 0; x < *nMapCol; ++x)
				for(int y = 0; y < *nMapRow; ++y)
				{
					TTile* tempTile = new TTile;

					int* nPosX = new int;
					int* nPosY = new int;
					int* nIndX = new int;
					int* nIndY = new int;
					int* nImage = new int;
					bool* bOccupied = new bool;

					ifs.read((char*)nPosX,4);
					ifs.read((char*)nPosY,4);
					ifs.read((char*)nIndX,4);
					ifs.read((char*)nIndY,4);
					ifs.read((char*)nImage,4);
					ifs.read((char*)bOccupied,1);

					tempTile->nPosX = *nPosX;
					tempTile->nPosY = *nPosY;
					tempTile->nIndX = *nIndX;
					tempTile->nIndY = *nIndY;
					tempTile->nImage = *nImage;
					tempTile->bOccupied = *bOccupied;

					delete nPosX;
					delete nPosY;
					delete nIndX;
					delete nIndY;
					delete nImage;
					delete bOccupied;

					m_vLayer2[x]->push_back(tempTile);
				}
		}

		// Reading in third layer
		if(*bLayer3)
		{
			for(int x = 0; x < *nMapCol; ++x)
				for(int y = 0; y < *nMapRow; ++y)
				{
					TTile* tempTile = new TTile;

					int* nPosX = new int;
					int* nPosY = new int;
					int* nIndX = new int;
					int* nIndY = new int;
					int* nImage = new int;
					bool* bOccupied = new bool;

					ifs.read((char*)nPosX,4);
					ifs.read((char*)nPosY,4);
					ifs.read((char*)nIndX,4);
					ifs.read((char*)nIndY,4);
					ifs.read((char*)nImage,4);
					ifs.read((char*)bOccupied,1);

					tempTile->nPosX = *nPosX;
					tempTile->nPosY = *nPosY;
					tempTile->nIndX = *nIndX;
					tempTile->nIndY = *nIndY;
					tempTile->nImage = *nImage;
					tempTile->bOccupied = *bOccupied;

					delete nPosX;
					delete nPosY;
					delete nIndX;
					delete nIndY;
					delete nImage;
					delete bOccupied;

					m_vLayer3[x]->push_back(tempTile);
				}
		}

		// Cleaning up temp memory
		delete nMapCol;
		delete nMapRow;
		delete nTileWidth;
		delete nTileHeight;
		delete nTileSetNames;
		delete nPlayerSpawnX;
		delete nPlayerSpawnY;
		delete bLayer2;
		delete bLayer3;
	}

	ifs.close();


	// Texture Manager loading tilesets
	for(unsigned int i = 0; i < m_vTileSet.size(); ++i)
	{
		// Appending filename to graphics path
		string szPath = "resource/graphics/";
		szPath += m_vTileSet[i];

		// Loading into Texturemanager
		m_vTileSetIDs[i] = CSGD_TextureManager::GetInstance()->LoadTexture(szPath.c_str());
	}
}

void CLevel::LoadCollisions(string szFile)
{
	// Load the collisions file - .JCB
	// MUST BE the same file name as the .JCT file
	unsigned int i = szFile.size(); 	
	szFile[i - 1] = 'b';
	szFile[i - 2] = 'c';
	szFile[i - 3] = 'j';
	
	ifstream ifs(szFile.c_str(),ios_base::in | ios_base::binary);
	if(ifs.is_open())
	{
		for(;;)
		{			
			int* top = new int;
			

			ifs.read((char*)top,4);

			if(9001 == *top)
			{
				delete top;
				break;
			}

			CWorld* pTemp = (CWorld*)CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->CreateObject("CWorld");

			int* left = new int;
			int* right = new int;
			int* bottom = new int;

			ifs.read((char*)left,4);
			ifs.read((char*)right,4);
			ifs.read((char*)bottom,4);

			pTemp->SetPosY((float)*top);
			pTemp->SetPosX((float)*left);
			pTemp->SetHeight(*bottom - *top);
			pTemp->SetWidth(*right - *left);
			pTemp->SetOffsetX(m_fCameraX);
			pTemp->SetOffsetY(m_fCameraY);

			// Cleaning up temp variables
			delete top;
			delete left;
			delete right;
			delete bottom;

			// Adding new rect to collision vector
			//m_vWorldCollision.push_back(pTemp);

			// Adding CWorld object to Object Manager for collision purposes
			OM->AddObject(pTemp);

			pTemp->Release();
		}

	}

	ifs.close();

}

void CLevel::CleanUp()
{
	// Cleaning up tile layers
	if(!m_vLayer1.empty())
	{
		for(unsigned int x = 0; x < m_vLayer1.size(); ++x)
		{
			for(unsigned int y = 0; y < m_vLayer1[x]->size(); ++y)
			{
				delete LAYER1(x,y);
			}

			delete m_vLayer1[x];
		}
	}

	if(!m_vLayer2.empty())
	{
		for(unsigned int x = 0; x < m_vLayer2.size(); ++x)
		{
			for(unsigned int y = 0; y < m_vLayer2[x]->size(); ++y)
			{
				delete LAYER2(x,y);
			}

			delete m_vLayer2[x];
		}
	}

	if(!m_vLayer3.empty())
	{
		for(unsigned int x = 0; x < m_vLayer3.size(); ++x)
		{
			for(unsigned int y = 0; y < m_vLayer3[x]->size(); ++y)
			{
				delete LAYER3(x,y);
			}

			delete m_vLayer3[x];
		}
	}

	if(XAUDIO->MusicIsSongPlaying(m_nBGMID))
		XAUDIO->MusicStopSong(m_nBGMID);
	
}

////////////////////////////////////////
//	    PUBLIC ACCESSORS / MUTATORS
////////////////////////////////////////

////////////////////////////////////////
//	    PRIVATE ACCESSORS / MUTATORS
////////////////////////////////////////

