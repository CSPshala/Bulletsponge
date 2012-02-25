///////////////////////////////////////////////////////////////////////////
//	File Name	:	"CHighScore.cpp"
//	
//	Author Name	:	JC Ricks
//	
//	Purpose		:	Handle High Score display and input
///////////////////////////////////////////////////////////////////////////

////////////////////////////////////////
//				INCLUDES
////////////////////////////////////////
#include "../Macros.h"
#include "CHighScoreState.h"
#include "COptionsState.h"
#include "../CGame.h"
#include "../SGD Wrappers/CSGD_DirectInput.h"
#include "../SGD Wrappers/CSGD_TextureManager.h"
#include "../SGD Wrappers/CSGD_Direct3D.h"
#include "../CSGD_ObjectManager.h"
#include "../factories/CSGD_ObjectFactory.h"
#include "../models/CEnemy.h"
#include <fstream>
using std::fstream;
using std::getline;
////////////////////////////////////////
//				MISC
////////////////////////////////////////
// Static instantiation / definition
CHighScoreState* CHighScoreState::sm_pInstance = NULL;
///////////////////////////////////////////////
//  CONSTRUCTOR / DECONSTRUCT / OP OVERLOADS
///////////////////////////////////////////////
CHighScoreState::CHighScoreState()
{	
	m_pFont = NULL;
	m_pNumbers = NULL;
	m_bInitOnce = false;
	m_bInputMode = false;
	m_fSpawnTime = 0.0f;
}

CHighScoreState::~CHighScoreState()
{
	// Cleaning up font
	SAFE_DELETE(m_pFont);
}

////////////////////////////////////////
//		PUBLIC UTILITY FUNCTIONS
////////////////////////////////////////
void CHighScoreState::Enter(void)
{
	if(!m_bInitOnce)
	{
		m_nFontID = CSGD_TextureManager::GetInstance()->LoadTexture("resource/graphics/JCR_menutext.png");
		m_nNumbersID = TEXTUREMAN->LoadTexture("resource/graphics/JCR_numberfont.png");		
		m_pFont = new CBitmapFont(33,32,8,m_nFontID,'A');	
		m_pNumbers = new CBitmapFont(64,64,10,m_nNumbersID,'0');		
		
		// Music	
		m_nBGMID = XAUDIO->MusicLoadSong("resource/sound/JCR_enhasa.xwm");

		// GRABBING GAME SETTINGS
		XAUDIO->MusicSetMasterVolume(COptionsState::GetInstance()->GetMusicVolume());
		XAUDIO->SFXSetMasterVolume(COptionsState::GetInstance()->GetSFXVolume());

		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->RegisterClassType<CBase>("CBase");		
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->RegisterClassType<CEnemy>("CEnemy");	

		// Setting Message System proc
		MS->InitMessageSystem(CHighScoreState::MessageProc);		

		// Volume Setting load
		LoadScores();

		// If it's input mode, comparing scores to see if
		// anyone qualifies
		if(m_bInputMode && m_vScoreTable.size() >= 10)
		{			
			if(m_tScore.nScore <= m_vScoreTable[m_vScoreTable.size() - 1]->nScore)
			{
				m_bInputMode = false;				
			}
		}
		

		m_bInitOnce = true;
	}
}

bool CHighScoreState::Input(void)
{
	if(!m_bInputMode)
	{
		if(DI->KeyPressed(DIK_ESCAPE))
			GAME->ChangeState(CMainMenuState::GetInstance());
	}
	else
	{
		if(DI->CheckBufferedKeysEx())
		{
			if(m_szInputName.size() < 17)			
				if(isalpha(DI->CheckBufferedKeysEx()))							
					m_szInputName += toupper(DI->CheckBufferedKeysEx());

			DI->ClearInput();
		}

		if(DI->KeyPressed(DIK_BACKSPACE))
		{
			if(!m_szInputName.empty())
				m_szInputName.pop_back();
		}

		if(DI->KeyPressed(DIK_RETURN))
		{
			if(!m_szInputName.empty())
				InputScore();
		}
	}

	return true;
}

void CHighScoreState::Update(void)
{
	if(!XAUDIO->MusicIsSongPlaying(m_nBGMID))
		XAUDIO->MusicPlaySong(m_nBGMID,true);

	OM->UpdateObjects(GAME->GetTimer().GetDeltaTime());

	if(m_fSpawnTime > 2.0f)
	{
		MS->SendMsg(new CCreateEnemyMessage());
		m_fSpawnTime = 0.0f;
	}

	m_fSpawnTime += GAME->GetTimer().GetDeltaTime();

	MS->ProcessMessages();
}

void CHighScoreState::Render(void)
{
	CSGD_Direct3D::GetInstance()->Clear(128,128,128);
	CSGD_Direct3D::GetInstance()->DeviceBegin();
	CSGD_Direct3D::GetInstance()->SpriteBegin();
	
	OM->RenderObjects();

	CSGD_Direct3D::GetInstance()->GetSprite()->Flush();	// Draw everything now that is queued up

	m_pFont->Print("HIGH SCORES",float((GAME->GetWindowWidth() >> 1) - 180),20.0f,1);
	D3D->DrawLine(0,70,GAME->GetWindowWidth(),70,0,0,0);
	m_pFont->Print("NAME",50,float(70 + m_pFont->GetCharHeight() - 20));
	m_pFont->Print("SCORE",450,float(70 + m_pFont->GetCharHeight() - 20));

	if(!m_bInputMode)
	{
		for(unsigned int i = 0; i < m_vScoreTable.size(); ++i)
		{
			char buffer[256] = {0};
			sprintf_s(buffer,_countof(buffer),"%i",m_vScoreTable[i]->nScore);

			m_pFont->Print(m_vScoreTable[i]->szName,50,float(140) + i * 30,1,0.7f);
			m_pNumbers->Print(buffer,450,float(140) + i * 30,1,0.4f);
		}
	}
	else
	{
		for(unsigned int i = 0; i < m_vScoreTable.size(); ++i)
		{
			char buffer[256] = {0};
			sprintf_s(buffer,_countof(buffer),"%i",m_vScoreTable[i]->nScore);

			m_pFont->Print(m_vScoreTable[i]->szName,50,float(140) + (i + 1) * 30,1,0.7f);
			m_pNumbers->Print(buffer,450,float(140) + (i + 1) * 30,1,0.4f);
		}

		int nBlinkStart = 50 + int(m_pFont->GetCharWidth() * 0.7f) * m_szInputName.size();
		int nBlinkEnd = nBlinkStart + 20;

		D3D->DrawLine(nBlinkStart,130 + m_pFont->GetCharHeight(),nBlinkEnd,130 + m_pFont->GetCharHeight(),255,255,255);

		m_pFont->Print(m_szInputName,50,140,1,0.7f);
		char tbuffer[256] = {0};
		sprintf_s(tbuffer,_countof(tbuffer),"%i",m_tScore.nScore);

		m_pNumbers->Print(tbuffer,450,140,1,0.4f);

		m_pFont->Print("YOU HAVE A HIGH SCORE\\",100,float(GAME->GetWindowHeight() - m_pFont->GetCharHeight() * 2),1, 0.8f);
		m_pFont->Print("TYPE YOUR NAME AND HIT ENTER",20,float(GAME->GetWindowHeight() - m_pFont->GetCharHeight()),1, 0.8f);
	}

	CSGD_Direct3D::GetInstance()->SpriteEnd();
	CSGD_Direct3D::GetInstance()->DeviceEnd();
	CSGD_Direct3D::GetInstance()->Present();
}

void CHighScoreState::Exit(void) 
{
	if(m_bInitOnce)
	{
		if(m_nFontID > -1)
			CSGD_TextureManager::GetInstance()->UnloadTexture(m_nFontID);

		SAFE_DELETE(m_pFont);
		SAFE_DELETE(m_pNumbers);
				
		m_nFontID = -1;
		m_pFont = NULL;
		m_pNumbers = NULL;
		m_bInitOnce = false;

		if(XAUDIO->MusicIsSongPlaying(m_nBGMID))
			XAUDIO->MusicStopSong(m_nBGMID);		

		XAUDIO->MusicUnloadSong(m_nBGMID);		

		SaveScores();

		for(unsigned int i = 0; i < m_vScoreTable.size(); ++i)
			delete m_vScoreTable[i];

		m_vScoreTable.clear();
		m_szInputName = "";

		m_bInputMode = false;

		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->UnregisterClassType("CBase");	
		CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->UnregisterClassType("CEnemy");		

		OM->RemoveAllObjects();
		MS->ClearMessages();
	}
}

CHighScoreState* CHighScoreState::GetInstance()
{
	if(sm_pInstance == NULL)
		sm_pInstance = new CHighScoreState();

	return sm_pInstance;
}

void CHighScoreState::DeleteInstance()
{
	if(sm_pInstance)
	{
		sm_pInstance->Exit();

		delete sm_pInstance;
		sm_pInstance = NULL;
	}
}

void CHighScoreState::MessageProc(CBaseMessage* pMsg)
{
	switch(pMsg->GetMsgID())
	{	
	case MSG_CREATE_ENEMY:
		{
			// Creating player
			CEnemy* tempEnemy = (CEnemy*)CSGD_ObjectFactory<string,IBaseInterface>::GetInstance()->CreateObject("CEnemy");
			tempEnemy->SetPosX((float)(rand() % (int)CGame::GetInstance()->GetWindowWidth()));
			tempEnemy->SetPosY(-128);			
			tempEnemy->SetWidth(64);
			tempEnemy->SetHeight(128);
			tempEnemy->SetMainMenu(true);
			
			OM->AddObject(tempEnemy);
			tempEnemy->Release();	
		}
		break;

	case MSG_DESTROY_ENEMY:
		{			
			// Killing enemy
			CDestroyEnemyMessage* pDEM = (CDestroyEnemyMessage*)pMsg;
			pDEM->GetEnemy()->SetIsActive(false);			
		}
		break;
	}
}
////////////////////////////////////////
//		PRIVATE UTILITY FUNCTIONS
////////////////////////////////////////
void CHighScoreState::LoadScores()
{	
	fstream fin = fstream("resource/scores.scr",ios_base::in);

	if(fin.is_open())
	{	
		string name;

		while(!fin.eof())
		{
			fin >> name;

			if(fin.eof())
				break;

			TScore* tempScore = new TScore();
			tempScore->szName = name;
			fin >> tempScore->nScore;
			m_vScoreTable.push_back(tempScore);
		}		
		fin.close();
	}
	
}

void CHighScoreState::InputScore()
{
	vector<TScore*>::iterator iter = m_vScoreTable.begin();	
	m_tScore.szName = m_szInputName;
	TScore* tScore = new TScore();
	tScore->nScore = m_tScore.nScore;
	tScore->szName = m_szInputName;

	bool bFound = false;

	for(;iter != m_vScoreTable.end();++iter)
	{
		if(m_tScore.nScore > (*iter)->nScore)
		{
			m_vScoreTable.insert(iter,tScore);
			bFound = true;
			break;
		}
	}

	if(!bFound && m_vScoreTable.size() < 10)
		m_vScoreTable.push_back(tScore);
	else if(!bFound)
		delete tScore;

	m_bInputMode = false;
}

void CHighScoreState::SaveScores()
{	
	fstream fout = fstream("resource/scores.scr",ios_base::out | ios_base::trunc);
	
	if(fout.is_open())
	{
		for(unsigned int i = 0; i < m_vScoreTable.size(); ++i)
		{
			if(i == 10)
				break;

			fout << m_vScoreTable[i]->szName << '\n';
			fout << m_vScoreTable[i]->nScore << '\n';			
		}

		fout.close();
	}
	else
		return;
}
////////////////////////////////////////
//	    PUBLIC ACCESSORS / MUTATORS
////////////////////////////////////////

////////////////////////////////////////
//	    PRIVATE ACCESSORS / MUTATORS
////////////////////////////////////////