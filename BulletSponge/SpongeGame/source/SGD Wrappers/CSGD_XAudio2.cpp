////////////////////////////////////////////////////////////////
//	File			:	"CSGD_XAudio2.cpp"
//
//	Author			:	David Brown (DB)
//	Based in part on:
//		-The MAudioManager wrapper by Joshua Villareal.
//
//	Creation Date	:	12/26/2011
//
//	Purpose			:	To wrap XAudio2.
//						Allows the loading and playing of 
//						.wav and .xwm files through XAudio2.
//
//	Special thanks to:  Joshua Villareal for providing his MAudioManager.
/////////////////////////////////////////////////////////////////

/* 
Disclaimer:
This source code was developed for and is the property of:

(c) Full Sail University Game Development Curriculum 2008-2012 and
(c) Full Sail Real World Education Game Design & Development Curriculum 2000-2008

Full Sail students may not redistribute or make this code public, 
but may use it in their own personal projects.
*/

#include "CSGD_XAudio2.h"
#include <assert.h>

const UINT32 AUDIO_SAMPLE_RATE = 44100; //XAUDIO2_DEFAULT_SAMPLERATE
const UINT32 AUDIO_ENGINE_OP_SET = XAUDIO2_COMMIT_NOW;//1;
const float MAX_FREQ_RATIO = 3.0f; //XAUDIO2_DEFAULT_FREQ_RATIO

///////////////////////////////////////////////////////////////////////////
CVoicePointerPool::CVoicePointerPool(IXAudio2* pXAudio2,
									 IXAudio2SubmixVoice* pSubmixVoice,
									 WAVEFORMATEX wfx)
{
	m_pXAudio2			= pXAudio2;
	m_pSubmixVoice		= pSubmixVoice;
	m_wfx				= wfx;

	//m_vVoicePool.reserve(8);
}

void CVoicePointerPool::DestroyVoices(void)
{
#ifdef _DEBUG
	char buffer[128];

	sprintf_s(buffer, _countof(buffer), "SGD XAUDIO2: %d - %d sample rate %d bit %d chan voices using SubmixVoice: %p\n", 
		m_vVoicePool.size(), m_wfx.nSamplesPerSec, m_wfx.wBitsPerSample, m_wfx.nChannels, m_pSubmixVoice);
	OutputDebugString(buffer);
#endif

	for(unsigned int i=0; i < m_vVoicePool.size(); i++ )
	{
		m_vVoicePool[ i ]->DestroyVoice();
		//delete m_vVoiceCallbacks[i];
	}
	//	Prolly don't need to do the rest
	m_vVoicePool.clear();
	//	Empty queue
	while(!m_qVoiceUnusedSlots.empty()) 
		m_qVoiceUnusedSlots.pop();
}

CVoicePointerPool::~CVoicePointerPool(void)
{

}

//When first created, source voices are in the stopped state
int CVoicePointerPool::CreateVoice()
{
	//	Create a voice and return it
	XAUDIO2_SEND_DESCRIPTOR send = {0, m_pSubmixVoice};
	XAUDIO2_VOICE_SENDS sendList = {1, &send};

	UINT32 flags = 0;
	/*
		CreateSourceVoice() function of the XAudio2 engine object. 
		This function takes the source voice that will be created, 
		the format of the audio (using the WAVEFORMATEX structure 
		provided by Windows), behavior flags, the maximum 
		frequency ratio, a callback interface function, a send list 
		of source voices for the destination of the audio data 
		(optional), and an audio effect chain. The behavior flags 
		can have one of the following values:

		flags = 
		XAUDIO2_VOICE_NOPITCH for no pitch control

		XAUDIO2_VOICE_NOSRC for no sample rate conversion

		XAUDIO2_VOICE_USEFILTER to enable filter effects on the sound

		XAUDIO2_VOICE_MUSIC to state that the voice is used to play background music
	*/

	VoiceCallback* pVoiceCallback = NULL;//new VoiceCallback(this, m_vVoicePool.size());

	IXAudio2SourceVoice* pSrcVoice;
		
	HRESULT hr;
	hr = m_pXAudio2->CreateSourceVoice( &pSrcVoice, &m_wfx,
		flags, MAX_FREQ_RATIO, pVoiceCallback, &sendList, NULL );

	if ( FAILED(hr) )
	{
		//	If it just added a new voice, trash it
		if (pVoiceCallback) 
			delete pVoiceCallback;

		return -1;
	}
	
	m_vVoicePool.push_back(pSrcVoice);
	m_vVoiceChannel.push_back(-1);

	m_vVoiceCallbacks.push_back(pVoiceCallback);

	return m_vVoicePool.size()-1;
}

int CVoicePointerPool::GetVoiceChannel(int nID)
{
	return m_vVoiceChannel[nID];
}

int CVoicePointerPool::FindUnusedVoice(void)
{
	int nIndex = -1;
	
	if (!m_qVoiceUnusedSlots.empty())
	{
		nIndex = m_qVoiceUnusedSlots.front();
		m_qVoiceUnusedSlots.pop();
	}
	else
		nIndex = CreateVoice();

	return nIndex;
}

IXAudio2SourceVoice* CVoicePointerPool::GetVoice(int nID, int nVoiceChannel)
{
	//	Remember channel it is playing on
	m_vVoiceChannel[nID] = nVoiceChannel;
	m_vVoicePool[nID]->Start(0, AUDIO_ENGINE_OP_SET);

	return m_vVoicePool[nID];
}

void CVoicePointerPool::ReturnVoice(int nID)
{
	m_qVoiceUnusedSlots.push(nID);

	//XAUDIO2_PLAY_TAILS
	m_vVoicePool[nID]->Stop(0, AUDIO_ENGINE_OP_SET);
	m_vVoicePool[nID]->FlushSourceBuffers();
	
	m_vVoiceChannel[nID] = -1;
}
///////////////////////////////////////////////////////////////////////////

//Called when the voice has just finished playing a contiguous audio stream.
void VoiceCallback::OnStreamEnd() 
{ 
	//int nVoiceChannel = m_pVoicePool->GetVoiceChannel(m_nVoiceIndex);

	//OutputDebugString("OnStreamEnd RECYCLE VOICE\n");

	//CSGD_XAudio2::GetInstance()->RecycleVoice(nVoiceChannel);
}

//Unused methods are stubs
void VoiceCallback::OnVoiceProcessingPassEnd() { }
void VoiceCallback::OnBufferEnd(void * pBufferContext)    
{ 
}

void VoiceCallback::OnBufferStart(void * pBufferContext) {    }
void VoiceCallback::OnLoopEnd(void * pBufferContext) 
{   
	int y = 5;
}
void VoiceCallback::OnVoiceError(void * pBufferContext, HRESULT Error) { }


HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
{
    HRESULT hr = S_OK;
    if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) )
        return HRESULT_FROM_WIN32( GetLastError() );

    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;

    while (hr == S_OK)
    {
        DWORD dwRead;
        if( 0 == ReadFile( hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL ) )
            hr = HRESULT_FROM_WIN32( GetLastError() );

        if( 0 == ReadFile( hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL ) )
            hr = HRESULT_FROM_WIN32( GetLastError() );

        switch (dwChunkType)
        {
			case fourccRIFF:
			{
				dwRIFFDataSize = dwChunkDataSize;
				dwChunkDataSize = 4;
				if( 0 == ReadFile( hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL ) )
					hr = HRESULT_FROM_WIN32( GetLastError() );
			}
            break;

			default:
			{
				if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, dwChunkDataSize, NULL, FILE_CURRENT ) )
					return HRESULT_FROM_WIN32( GetLastError() );            
			}
        }

        dwOffset += sizeof(DWORD) * 2;
        
        if (dwChunkType == fourcc)
        {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }

        dwOffset += dwChunkDataSize;
        if (bytesRead >= dwRIFFDataSize) 
			return S_FALSE;
    }

    return S_OK;   
}

HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;
    if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, bufferoffset, NULL, FILE_BEGIN ) )
        return HRESULT_FROM_WIN32( GetLastError() );

    DWORD dwRead;
    if( 0 == ReadFile( hFile, buffer, buffersize, &dwRead, NULL ) )
        hr = HRESULT_FROM_WIN32( GetLastError() );

    return hr;
}

HRESULT SoundInfo::LoadSoundInfo( LPCTSTR strFileName )
{
	// Open the file
	HANDLE hFile = CreateFile(
		strFileName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL );

	if( INVALID_HANDLE_VALUE == hFile )
		return HRESULT_FROM_WIN32( GetLastError() );

	if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) )
		return HRESULT_FROM_WIN32( GetLastError() );

	DWORD dwChunkSize;
	DWORD dwChunkPosition;
	//check the file type, should be fourccWAVE or 'XWMA'
	FindChunk(hFile,fourccRIFF,dwChunkSize, dwChunkPosition );

	DWORD filetype;
	ReadChunkData(hFile,&filetype,sizeof(DWORD),dwChunkPosition);
	if (filetype == fourccWAVE)
	{
		FindChunk(hFile,fourccFMT, dwChunkSize, dwChunkPosition );
		ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition );

		//fill out the audio data buffer with the contents of the fourccDATA chunk
		FindChunk(hFile,fourccDATA,dwChunkSize, dwChunkPosition );
		BYTE * pDataBuffer = new BYTE[dwChunkSize];
		ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

		buffer.AudioBytes = dwChunkSize;  //buffer containing audio data
		buffer.pAudioData = pDataBuffer;  //size of the audio buffer in bytes
		buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
	}
	else if (filetype == fourccXWMA)
	{
		FindChunk(hFile,fourccFMT, dwChunkSize, dwChunkPosition );
		ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition );

		//fill out the audio data buffer with the contents of the fourccDATA chunk
		FindChunk(hFile,fourccDATA,dwChunkSize, dwChunkPosition );
		BYTE * pDataBuffer = new BYTE[dwChunkSize];
		ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

		buffer.AudioBytes = dwChunkSize;  //buffer containing audio data
		buffer.pAudioData = pDataBuffer;  //size of the audio buffer in bytes
		buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

		//fill out the wma data buffer with the contents of the fourccDPDS chunk
		UINT* pDataBufferWma;
		FindChunk(hFile, fourccDPDS, dwChunkSize, dwChunkPosition);
		pDataBufferWma = new UINT32[dwChunkSize/sizeof(UINT)];
		ReadChunkData(hFile, pDataBufferWma, dwChunkSize, dwChunkPosition);

		bufferwma.PacketCount = dwChunkSize/sizeof(UINT);
		bufferwma.pDecodedPacketCumulativeBytes = pDataBufferWma;
	}
	else // Unknown format
		return S_FALSE;

	return S_OK;
};

/////////////////////////////////////////////////////////////////
CSGD_XAudio2::CSGD_XAudio2()
{
#ifndef _XBOX
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif

	m_pXAudio2			= NULL;
	m_pMasterVoice		= NULL;
	
	m_pSFXSubmixVoice	= NULL;
	m_pMusicSubmixVoice	= NULL;

	m_dwChannelMask		= -1;
	m_nMasterOutputChannels			= -1;
}

CSGD_XAudio2::~CSGD_XAudio2()
{
#ifndef _XBOX
	CoUninitialize();
#endif
}

CSGD_XAudio2* CSGD_XAudio2::GetInstance( void )
{
	static CSGD_XAudio2 instance;
	return &instance;
}

//IXAudio2* CSGD_XAudio2::m_pXAudio2 = NULL;

bool CSGD_XAudio2::InitXAudio2()
{
	UINT32 flags = 0;

#if defined(_DEBUG) && defined(_WINDOWS)
	flags |= XAUDIO2_DEBUG_ENGINE; // can only be done on WINDOWS
#endif

	HRESULT hr;

	if (m_pXAudio2 == NULL)
	{
		// Create the XAudio2 Engine.
		hr = XAudio2Create( &m_pXAudio2, flags, XAUDIO2_DEFAULT_PROCESSOR );
		if ( FAILED(hr) )
			return false;

		//if( SUCCEEDED( hr ) ) 
		//{ 
		//	m_pXAudio2->AddRef(); 
		//}
	}
	// Create the Mastering Voice (works with the hardware).
	// XAUDIO2_DEFAULT_SAMPLERATE
	hr = m_pXAudio2->CreateMasteringVoice( &m_pMasterVoice, XAUDIO2_DEFAULT_CHANNELS,
									AUDIO_SAMPLE_RATE, 0, 0, NULL );
	if ( FAILED(hr) )
    {  
		ShutdownXAudio2();
        return false;  
    } 

	// Determine speaker configuration  
    XAUDIO2_DEVICE_DETAILS details;  
    hr = m_pXAudio2->GetDeviceDetails( 0, &details );  
    if( FAILED(hr) )  
    {  
        //wprintf( L"Failed creating getting speaker config: %#X\n", hr );  
		ShutdownXAudio2();
        return false;  
    } 

	m_dwChannelMask = details.OutputFormat.dwChannelMask;  
    m_nMasterOutputChannels = details.OutputFormat.Format.nChannels; 

	hr = m_pXAudio2->CreateSubmixVoice(&m_pSFXSubmixVoice, m_nMasterOutputChannels, AUDIO_SAMPLE_RATE, 0, 0, NULL, NULL);
	if ( FAILED(hr) )
    {  
		ShutdownXAudio2();
        return false;  
    } 

	hr = m_pXAudio2->CreateSubmixVoice(&m_pMusicSubmixVoice, m_nMasterOutputChannels, AUDIO_SAMPLE_RATE, 0, 0, NULL, NULL);
	if ( FAILED(hr) )
    {  
		ShutdownXAudio2();
        return false;  
    } 

	m_vVoices.reserve(32);

	return true;
}

void CSGD_XAudio2::ShutdownXAudio2( void )
{
	SilenceAll();
	//m_pXAudio2->CommitChanges(AUDIO_ENGINE_OP_SET);

	//	Clean up voice pools
	multimap<int, CVoicePointerPool*>::iterator mmIter = m_vVoicePools.begin();

	while(mmIter !=m_vVoicePools.end())
	{
		//	Destroy all voices in this pool
		(*mmIter).second->DestroyVoices();
		delete (*mmIter).second;
		mmIter++;
	}
	m_vVoicePools.clear();

#ifdef _DEBUG
	char buffer[128];

	sprintf_s(buffer, _countof(buffer), "SGD XAUDIO2: Maximum simultaneous active voice channels: %d\n", m_vVoices.size());
	OutputDebugString(buffer);
#endif
	//	Clear all voices
	m_vVoices.clear();
	//	Empty queue
	while(!m_qVoicesOpenSlots.empty()) 
		m_qVoicesOpenSlots.pop();

	//	Unload all SFX from the sound library
	for ( unsigned int i = 0; i < m_vSFXLibrary.size(); ++i )
	{
		if (m_vSFXLibrary[i].m_bInUse)
			m_vSFXLibrary[i].UnloadSoundInfo();
	}
	m_vSFXLibrary.clear();

	//	Empty queue
	while(!m_qSFXLibraryOpenSlots.empty()) 
		m_qSFXLibraryOpenSlots.pop();

	//	Unload all Songs from the Music library
	for ( unsigned int i = 0; i < m_vMusicLibrary.size(); ++i )
	{
		if (m_vMusicLibrary[i].m_bInUse)
			m_vMusicLibrary[i].UnloadSoundInfo();
	}
	m_vMusicLibrary.clear();

	//	Empty queue
	while(!m_qMusicLibraryOpenSlots.empty()) 
		m_qMusicLibraryOpenSlots.pop();

	//	Clean up Music Master voice
	if (m_pMusicSubmixVoice)
	{
		m_pMusicSubmixVoice->DestroyVoice();
		m_pMusicSubmixVoice = NULL;
	}

	//	Clean up SFX Master voice
	if (m_pSFXSubmixVoice)
	{
		m_pSFXSubmixVoice->DestroyVoice();
		m_pSFXSubmixVoice = NULL;
	}

	// Destroy the Mastering Voice
	if (m_pMasterVoice)
	{
		m_pMasterVoice->DestroyVoice();
		m_pMasterVoice = NULL;
	}

	// Release the XAudio2 Engine
	if (m_pXAudio2)
	{
		m_pXAudio2->Release();
		m_pXAudio2 = NULL;
	}
}

void CSGD_XAudio2::Update()
{
	m_nActiveVoiceCount = 0;

	for( unsigned int i = 0; i < m_vVoices.size(); ++i )
	{
		//	If this voice isn't playing audio, skip it
		if (m_vVoices[ i ].m_pVoice == NULL)
			continue;

		m_nActiveVoiceCount++;

		XAUDIO2_VOICE_STATE currentState;
		m_vVoices[ i ].m_pVoice->GetState( &currentState );//, XAUDIO2_VOICE_NOSAMPLESPLAYED );

		// if there are NO buffers playing on this voice
		if( currentState.BuffersQueued < 1 )
		{
			// and it is looping, then play it again
			if( m_vVoices[ i ].m_bIsLooping )
			{
				if( m_vVoices[ i ].m_bIsMusic == false)
					m_vVoices[ i ].m_pVoice->SubmitSourceBuffer( &m_vVoices[ i ].buffer );
				else
					m_vVoices[ i ].m_pVoice->SubmitSourceBuffer( &m_vVoices[ i ].buffer, &m_vVoices[ i ].bufferwma );
			}
			else
			//	otherwise, recycle it for future use
				RecycleVoiceNow(i);
		}
	}

	//char text[64];
	//sprintf_s( text, 64, "%i", m_vVoices.size() );
	//CSGD_Direct3D::GetInstance()->DrawTextA( text, 20, 20 );
	//m_pXAudio2->CommitChanges(AUDIO_ENGINE_OP_SET);//XAUDIO2_COMMIT_ALL);
}

void CSGD_XAudio2::SilenceAll()
{
	//	Recycle all voices (which stops them playing)
	for(unsigned int i=0; i < m_vVoices.size(); i++)
	{
		if (m_vVoices[i].currentSoundID > -1)
		{
			//XAUDIO2_PLAY_TAILS
			m_vVoices[i].m_bIsLooping = false; // kill even if looping
			
			//	Return voice to its pool
			RecycleVoiceNow(i);
		}
	}
}

void CSGD_XAudio2::RecycleVoiceNow(int nIndex)
{
	//	Looping voices must be stopped MANUALLY, so ignore this one
	if (m_vVoices[nIndex].m_bIsLooping == true)
		return;

	int nVoicePoolID = m_vVoices[ nIndex ].m_nVoicePoolID;

	//	Find the voice pool in the voice pool map
	multimap<int, CVoicePointerPool*>::iterator iter = m_vVoicePools.find(nVoicePoolID);

	//	Error check to make sure we found it.
	if (iter == m_vVoicePools.end())
	{
		//	This means we couldn't find it in the map...
		char buffer[128];
		sprintf_s(buffer, sizeof(buffer), "RecycleVoiceNOW::Shouldn't EVER get here: Index %d VoicePool %d\n");
			
		OutputDebugString(buffer);
			
		return;
	}

	//	Return voice ptr to voice pool
	(*iter).second->ReturnVoice( m_vVoices[nIndex].m_nVoicePtrID );

	m_vVoices[ nIndex ].m_pVoice = NULL;
	m_vVoices[ nIndex ].currentSoundID = -1;	// -1 means not playing a sound or music

	//	Return this voice to being available again
	m_qVoicesOpenSlots.push(nIndex);
}

void CSGD_XAudio2::Play(int nID, SoundInfo soundTemplate, bool bIsLooping)
{
	assert(soundTemplate.buffer.AudioBytes != NULL && "sound buffer is NULL");

	//	Did they try to play an empty slot?
	if( !soundTemplate.m_bInUse || soundTemplate.buffer.AudioBytes == NULL)
		return;

	int nIndex = -1;
	
	nIndex = GetOpenVoiceChannel();

	if (nIndex < 0)
	{
		nIndex = m_vVoices.size();

		VoiceInfo newVoice;
		m_vVoices.push_back( newVoice );
	}

	IXAudio2SubmixVoice* pSubmixVoice = NULL;
	int nIsXMA = -1; // neutral (does that mean it's "maybe"?)

	if (soundTemplate.bufferwma.PacketCount == 0)
	{
		nIsXMA = 0; // false
		pSubmixVoice = m_pSFXSubmixVoice;
	}
	else
	{
		nIsXMA = 1; // true
		pSubmixVoice = m_pMusicSubmixVoice;
	}

	// encode type in int for faster checks:
	//	(SamplesPerSec * 10000) + (bits * 100) + (channels * 10) + isXMA = index
	//	22050 * 10000) + (8*100) + (1 * 10) + 0 = 220500810
	int nVoicePoolID = (soundTemplate.wfx.nSamplesPerSec * 10000) + (soundTemplate.wfx.wBitsPerSample*100) + (soundTemplate.wfx.nChannels * 10) + nIsXMA;
	
	//	Find the given key inside of the database (map).
	multimap<int, CVoicePointerPool*>::iterator iter = m_vVoicePools.find(nVoicePoolID);

	CVoicePointerPool* pVoicePtrPool = NULL;

	//	Error check to make sure we found it.
	if (iter == m_vVoicePools.end())
	{
		pVoicePtrPool = new CVoicePointerPool(m_pXAudio2, pSubmixVoice, soundTemplate.wfx);

		std::pair<int, CVoicePointerPool*> voicePoolPair;

		voicePoolPair.first		=	nVoicePoolID;	// key
		voicePoolPair.second	=	pVoicePtrPool;	// value

		m_vVoicePools.insert(voicePoolPair);
	}
	else
	{
		pVoicePtrPool = (*iter).second;
	}

	int nVoicePtrIndex = pVoicePtrPool->FindUnusedVoice();

	m_vVoices[ nIndex ].m_pVoice = pVoicePtrPool->GetVoice(nVoicePtrIndex, nIndex);

	//XAUDIO2_VOICE_STATE currentState;
	//m_vVoices[ nIndex ].m_pVoice->GetState( &currentState );//, XAUDIO2_VOICE_NOSAMPLESPLAYED );
	//// if there are NO buffers playing on this voice
	//if( currentState.BuffersQueued > 0 )
	//{
	//	int y = 8;
	//}

	//	Set all the info on this voice:
	m_vVoices[ nIndex ].m_nVoicePoolID = nVoicePoolID;
	m_vVoices[ nIndex ].m_nVoicePtrID = nVoicePtrIndex;

	m_vVoices[ nIndex ].currentSoundID = nID;
	m_vVoices[ nIndex ].m_nUnitType = soundTemplate.m_nUnitType;
	m_vVoices[ nIndex ].buffer = soundTemplate.buffer;
	m_vVoices[ nIndex ].bufferwma = soundTemplate.bufferwma;
	m_vVoices[ nIndex ].m_bIsLooping = bIsLooping;//XAUDIO2_LOOP_INFINITE
	m_vVoices[ nIndex ].m_bIsMusic = (nIsXMA == 1) ? true : false;

	m_vVoices[ nIndex ].m_pVoice->SetVolume( soundTemplate.m_fVolume, AUDIO_ENGINE_OP_SET);
	m_vVoices[ nIndex ].m_pVoice->SetFrequencyRatio( soundTemplate.m_fFrequencyRatio, AUDIO_ENGINE_OP_SET);
	//	Pan
	float outputMatrix[ 12 ];
	CalcPanOutputMatrix(soundTemplate.m_fPan, soundTemplate.wfx.nChannels, outputMatrix);
	//If the voice sends to a single target voice then specifying NULL will cause 
	//SetOutputMatrix to operate on that target voice.
	m_vVoices[ nIndex ].m_pVoice->SetOutputMatrix( NULL, soundTemplate.wfx.nChannels, m_nMasterOutputChannels, outputMatrix, AUDIO_ENGINE_OP_SET);
	//m_vVoices[ nIndex ].m_pVoice->SetOutputMatrix( pSubmixVoice, soundTemplate.wfx.nChannels, m_nMasterOutputChannels, soundTemplate.outputMatrix, AUDIO_ENGINE_OP_SET);

	//	Put audio in this voice to play
	if (m_vVoices[ nIndex ].m_bIsMusic)//soundTemplate.bufferwma.PacketCount > 0)
		m_vVoices[ nIndex ].m_pVoice->SubmitSourceBuffer( &soundTemplate.buffer, &soundTemplate.bufferwma );
	else
		m_vVoices[ nIndex ].m_pVoice->SubmitSourceBuffer( &soundTemplate.buffer );
}

void CSGD_XAudio2::CalcPanOutputMatrix(float pan, WORD inputChannels, float *pLevelMatrix)
{
	assert( pan >= -1.0f && pan <= 1.0f && "Pan out of range");
    assert( pLevelMatrix != NULL );  
 
	//The minimum size of the output matrix is 
	//the number of channels in the source voice times 
	//the number of channels in the output voice. In this 
	//case an eight element array will handle a mono voice 
	//outputting to any output format up to 7.1 surround sound.
	int nMinimum = inputChannels * m_nMasterOutputChannels;

    assert( m_nMasterOutputChannels <= nMinimum ); 
    // This assumes we use mono sources with up to 8 output channels  
    // (pLevelMatrix is 8 max size)  
    //assert( m_nMasterOutputChannels <= 8 ); 

    // Default to full volume for all channels/destinations  
    for( UINT i=0; i < 12; ++i)
        pLevelMatrix[ i ] = 1.0f;//0.0f;//1.0f; 

	// pan of -1.0 indicates all left speaker, 
	// 1.0 is all right speaker, 0.0 is split between left and right
    float left = ( pan >= 0 ) ? (1.f - pan) : 1.f;  
    float right = ( pan <= 0 ) ? (-pan - 1.f) : 1.f;  
 
	if (inputChannels == 1 && m_nMasterOutputChannels == 2)//nMinimum == 2)
	{
		// Assumes you are panning both front and back left/right channels  
		// If you just wanted front LEFT/RIGHT, then the switch statement  
		// is just MONO vs. everything else.  
		switch( m_dwChannelMask )  
		{  
			case SPEAKER_STEREO:  
			case SPEAKER_2POINT1:  
			case SPEAKER_SURROUND:  
				pLevelMatrix[ 0 ] = left;  
				pLevelMatrix[ 1 ] = right;  
				break;  
 
			case SPEAKER_QUAD:  
				pLevelMatrix[ 0 ] = pLevelMatrix[ 2 ] = left;  
				pLevelMatrix[ 1 ] = pLevelMatrix[ 3 ] = right;  
				break;  
 
			case SPEAKER_4POINT1:  
				pLevelMatrix[ 0 ] = pLevelMatrix[ 3 ] = left;  
				pLevelMatrix[ 1 ] = pLevelMatrix[ 4 ] = right;  
				break;  
 
			case SPEAKER_5POINT1:  
			case SPEAKER_7POINT1:  
			case SPEAKER_5POINT1_SURROUND:  
				pLevelMatrix[ 0 ] = pLevelMatrix[ 4 ] = left;  
				pLevelMatrix[ 1 ] = pLevelMatrix[ 5 ] = right;  
				break;  
 
			case SPEAKER_7POINT1_SURROUND:  
				pLevelMatrix[ 0 ] = pLevelMatrix[ 4 ] = pLevelMatrix[ 6 ] = left;  
				pLevelMatrix[ 1 ] = pLevelMatrix[ 5 ] = pLevelMatrix[ 7 ] = right;  
				break;  
 
			case SPEAKER_MONO:  
			default:  
				// don't do any panning here  
				break;  
		}  
	}
	else if (inputChannels == 2 && m_nMasterOutputChannels == 2)//nMinimum == 4)
	{
		// Stereo input panning to stereo output
		switch( m_dwChannelMask )  
		{  
			case SPEAKER_STEREO:  
			case SPEAKER_2POINT1:  
			case SPEAKER_SURROUND:  
				//	0 and 1 map to Left Output speaker
				pLevelMatrix[ 0 ] = pLevelMatrix[ 1 ] = left;
				//	2 and 3 map to Right Output speaker
				pLevelMatrix[ 2 ] = pLevelMatrix[ 3 ] = right;  
				break;  

			case SPEAKER_QUAD:  
				break;  
 
			case SPEAKER_4POINT1:  
				break;  
 
			case SPEAKER_5POINT1:  
			case SPEAKER_7POINT1:  
			case SPEAKER_5POINT1_SURROUND:  
				break;  
 
			case SPEAKER_7POINT1_SURROUND:  
				break;  
 
			case SPEAKER_MONO:  
			default:  
				// don't do any panning here  
				break;  
		}  
	}
	else if (inputChannels == 2 && m_nMasterOutputChannels == 6)//nMinimum == 12)
	{
		switch( m_dwChannelMask )  
		{  
			/*
				For example, when rendering two-channel stereo input into 5.1 output 
				that is weighted toward the front channels—but is absent from the 
				center and low-frequency channels—the matrix might have the values 
				shown in the following table.
			*/
			case SPEAKER_5POINT1:  
			case SPEAKER_5POINT1_SURROUND:  
			{
				/*
				Left			1.0 [0]	 0.0 [1]
				Right			0.0 [2]	 1.0 [3]
				Front Center	0.0 [4]	 0.0 [5]
				LFE				0.0 [6]	 0.0 [7]
				Rear Left		0.8 [8]	 0.0 [9]
				Rear Right		0.0 [10] 0.8 [11]
				*/
				pLevelMatrix[0] = left; pLevelMatrix[1] = 0.0f;			//	Left
				pLevelMatrix[2] = 0.0f; pLevelMatrix[3] = right;		//	Right
				pLevelMatrix[4] = 0.0f; pLevelMatrix[5] = 0.0f;			//	Front Center
				pLevelMatrix[6] = 0.0f; pLevelMatrix[7] = 0.0f;			//	LFE
				pLevelMatrix[8] = left*0.8f; pLevelMatrix[9] = 0.0f;	//	Rear Left
				pLevelMatrix[10] = 0.0f; pLevelMatrix[11] = right*0.8f;	//	Rear Right
			}
			break;
		}  
	}
}

int CSGD_XAudio2::GetOpenVoiceChannel(void)
{
	int nIndex = -1;

	if (!m_qVoicesOpenSlots.empty())
	{
		nIndex = m_qVoicesOpenSlots.front();
		m_qVoicesOpenSlots.pop();
	}

	return nIndex;
}

//////////////////////////////////////////////////////////
//	SFX
//////////////////////////////////////////////////////////

int CSGD_XAudio2::SFXLoadSound( const char* szFileName )
{
	if (!szFileName)	
		return -1;

	//	Check extention
	int len = strlen(szFileName);
	if (len > 4)
	{
		//	Do some pointer math to get the extention
		const char* pExtention = (szFileName + (len-4));

		if (_stricmp(pExtention, ".xwm") == 0)
		{
			// Trying to load Music as SFX
			char szBuffer[256] = {0};
			sprintf_s(szBuffer, _countof(szBuffer), "Call MusicLoadSong for this file - %s", szFileName); 
			MessageBox(0, szBuffer, "SGD XAudio2 Error", MB_OK);
			return -1;
		}
		else if (_stricmp(pExtention, ".wav") != 0)
		{
			char szBuffer[256] = {0};
			sprintf_s(szBuffer, _countof(szBuffer), "Unsupported file format - %s", pExtention);
			MessageBox(0, szBuffer, "SGD XAudio2 Error", MB_OK);
			return -1;
		}
	}
	else
	{
		char szBuffer[256] = {0};
		sprintf_s(szBuffer, _countof(szBuffer), "Bad filename - %s", szFileName); 
		MessageBox(0, szBuffer, "SGD XAudio2 Error", MB_OK);
		return -1;
	}

	// If they're the same, give the ID of the last one.
	for (unsigned int i = 0; i < m_vSFXLibrary.size(); i++)
	{
		if (_stricmp(szFileName, m_vSFXLibrary[i].filename) == 0)
		{
			return i;
		}
	}

	SoundInfo soundInfo;

	HRESULT hr = soundInfo.LoadSoundInfo( szFileName );

	if (FAILED(hr))
	{
		// Failed.
		char szBuffer[256] = {0};
		sprintf_s(szBuffer, _countof(szBuffer), "Failed to Load - %s", szFileName); 
		MessageBox(0, szBuffer, "SGD XAudio2 Error", MB_OK);
		return -1;
	}

	int nID = -1;

	if (!m_qSFXLibraryOpenSlots.empty())
	{
		nID = m_qSFXLibraryOpenSlots.front();
		m_qSFXLibraryOpenSlots.pop();
	}

	if( nID < 0 )
	{
		nID = m_vSFXLibrary.size();
		m_vSFXLibrary.push_back( soundInfo );
	}
	else
		memcpy(&m_vSFXLibrary[nID], &soundInfo, sizeof(soundInfo));//m_vSFXLibrary[nID] = soundInfo;

	m_vSFXLibrary[nID].m_nUnitType = AUDIO_SFX;
	strcpy_s(m_vSFXLibrary[nID].filename, sizeof(char) * MAX_PATH, szFileName);
	m_vSFXLibrary[nID].m_bInUse = true;

	return nID;
}

void CSGD_XAudio2::SFXUnloadSound( int nID )
{
	assert( nID > -1 && nID < (int)m_vSFXLibrary.size() && "Sound ID out of range" );
	assert( m_vSFXLibrary[nID].m_bInUse && "Sound ID not loaded" );

	//	Wipe it clean
	m_vSFXLibrary[nID].UnloadSoundInfo();

	m_qSFXLibraryOpenSlots.push(nID);
}

void CSGD_XAudio2::SFXPlaySound( int nID, bool bIsLooping )
{	
	assert( nID > -1 && nID < (int)m_vSFXLibrary.size() && "Sound ID out of range" );
	assert( m_vSFXLibrary[nID].m_bInUse && "Sound ID not loaded" );

	SoundInfo soundTemplate = m_vSFXLibrary[nID];

	Play(nID, soundTemplate, bIsLooping);
}

void CSGD_XAudio2::SFXStopSound(int nID)
{
	assert( nID > -1 && nID < (int)m_vSFXLibrary.size() && "Sound ID out of range" );
	assert( m_vSFXLibrary[nID].m_bInUse && "Sound ID not loaded" );

	for(unsigned int i=0; i < m_vVoices.size(); i++)
	{
		// Stop ALL INSTANCES of this sound
		if( m_vVoices[i].m_bIsMusic == false && m_vVoices[i].currentSoundID == nID )
		{
			m_vVoices[i].m_bIsLooping = false;
			RecycleVoiceNow(i);
		}
	}
}

bool CSGD_XAudio2::SFXIsSoundPlaying(int nID)
{
	assert( nID > -1 && nID < (int)m_vSFXLibrary.size() && "Sound ID out of range" );
	assert( m_vSFXLibrary[nID].m_bInUse && "Sound ID not loaded" );

	for( unsigned int i = 0; i < m_vVoices.size(); ++i )
	{
		if( m_vVoices[i].m_bIsMusic == false && m_vVoices[i].currentSoundID == nID )
		{
			return true;
		}
	}

	return false;
}

float CSGD_XAudio2::SFXGetMasterVolume(void)
{
	assert( m_pSFXSubmixVoice != NULL && "Submix voice is NULL!" );

	float fVolume = 0.0f;
	m_pSFXSubmixVoice->GetVolume(&fVolume);

	return fVolume;
}

float CSGD_XAudio2::SFXGetSoundVolume(int nID)
{
	assert( nID > -1 && nID < (int)m_vSFXLibrary.size() && "Sound ID out of range" );
	assert( m_vSFXLibrary[nID].m_bInUse && "Sound ID not loaded" );

	return m_vSFXLibrary[ nID ].m_fVolume;
}

float CSGD_XAudio2::SFXGetSoundPan(int nID)
{
	assert( nID > -1 && nID < (int)m_vSFXLibrary.size() && "Sound ID out of range" );
	assert( m_vSFXLibrary[nID].m_bInUse && "Sound ID not loaded" );

	return m_vSFXLibrary[ nID ].m_fPan;
}

float CSGD_XAudio2::SFXGetSoundFrequencyRatio(int nID)
{
	assert( nID > -1 && nID < (int)m_vSFXLibrary.size() && "Sound ID out of range" );
	assert( m_vSFXLibrary[nID].m_bInUse && "Sound ID not loaded" );

	return m_vSFXLibrary[ nID ].m_fFrequencyRatio;
}

void CSGD_XAudio2::SFXSetMasterVolume(float fVolume)
{
	assert(fVolume >= -XAUDIO2_MAX_VOLUME_LEVEL && fVolume <= XAUDIO2_MAX_VOLUME_LEVEL && "Volume outside valid range");

	m_pSFXSubmixVoice->SetVolume(fVolume, AUDIO_ENGINE_OP_SET);
}

void CSGD_XAudio2::SFXSetSoundVolume(int nID, float fVolume)
{
	assert( nID > -1 && nID < (int)m_vSFXLibrary.size() && "Sound ID out of range" );
	assert( m_vSFXLibrary[nID].m_bInUse && "Sound ID not loaded" );
	assert(fVolume >= -XAUDIO2_MAX_VOLUME_LEVEL && fVolume <= XAUDIO2_MAX_VOLUME_LEVEL && "Volume level out of range");

	m_vSFXLibrary[ nID ].m_fVolume = fVolume;

	//for( unsigned int i = 0; i < m_vVoices.size(); ++i )
	//{
	//	if( m_vVoices[i].m_bIsMusic == false && m_vVoices[i].currentSoundID == nID )
	//	{
	//		m_vVoices[i].m_pVoice->SetVolume(fVolume, AUDIO_ENGINE_OP_SET);
	//	}
	//}
}

void CSGD_XAudio2::SFXSetSoundPan(int nID, float fPan)
{
	assert( nID > -1 && nID < (int)m_vSFXLibrary.size() && "Sound ID out of range" );
	assert( m_vSFXLibrary[nID].m_bInUse && "Sound ID not loaded" );

	m_vSFXLibrary[ nID ].m_fPan = fPan;
}

void CSGD_XAudio2::SFXSetSoundFrequencyRatio(int nID, float fFreqRatio)
{
	assert( nID > -1 && nID < (int)m_vSFXLibrary.size() && "Sound ID out of range" );
	assert( m_vSFXLibrary[nID].m_bInUse && "Sound ID not loaded" );

	//assert(fFreq >= XAUDIO2_MIN_FREQ_RATIO && fFreq <= XAUDIO2_MAX_FREQ_RATIO);//MAX_FREQ);
	assert(fFreqRatio >= 0.1f && fFreqRatio <= MAX_FREQ_RATIO && "Frequency out of range");
	
	m_vSFXLibrary[ nID ].m_fFrequencyRatio = fFreqRatio;

	//for( unsigned int i = 0; i < m_vVoices.size(); ++i )
	//{
	//	if( m_vVoices[i].m_bIsMusic == false && m_vVoices[i].currentSoundID == nID )
	//	{
	//		m_vVoices[i].m_pVoice->SetFrequencyRatio(fFreqRatio, AUDIO_ENGINE_OP_SET);
	//	}
	//}
}

//////////////////////////////////////////////////////////
//	Music
//////////////////////////////////////////////////////////

int CSGD_XAudio2::MusicLoadSong( const char* szFileName )
{
	if (!szFileName)	
		return -1;

		//	Check extention
	int len = strlen(szFileName);
	if (len > 4)
	{
		//	Do some pointer math to get the extention
		const char* pExtention = (szFileName + (len-4));

		if (_stricmp(pExtention, ".wav") == 0)
		{
			// Trying to load SFX as Music
			char szBuffer[256] = {0};
			sprintf_s(szBuffer, _countof(szBuffer), "Call SFXLoadSound for this file - %s", szFileName); 
			MessageBox(0, szBuffer, "SGD XAudio2 Error", MB_OK);
			return -1;
		}
		else if (_stricmp(pExtention, ".xwm") != 0)
		{
			char szBuffer[256] = {0};
			sprintf_s(szBuffer, _countof(szBuffer), "Unsupported file format - %s", pExtention); 
			MessageBox(0, szBuffer, "SGD XAudio2 Error", MB_OK);
			return -1;
		}
	}
	else
	{
		char szBuffer[256] = {0};
		sprintf_s(szBuffer, _countof(szBuffer), "Bad filename - %s", szFileName); 
		MessageBox(0, szBuffer, "SGD XAudio2 Error", MB_OK);
		return -1;
	}

	// If they're the same, give the ID of the last one.
	for (unsigned int i = 0; i < m_vMusicLibrary.size(); i++)
	{
		if (_stricmp(szFileName, m_vMusicLibrary[i].filename) == 0)
		{
			return i;
		}
	}

	SoundInfo soundInfo;

	HRESULT hr = soundInfo.LoadSoundInfo( szFileName );

	if (FAILED(hr))
	{
		// Failed.
		char szBuffer[256] = {0};
		sprintf_s(szBuffer, _countof(szBuffer), "Failed to Load - %s", szFileName); 
		MessageBox(0, szBuffer, "SGD XAudio2 Error", MB_OK);
		return -1;
	}

	int nID = -1;

	if (!m_qMusicLibraryOpenSlots.empty())
	{
		nID = m_qMusicLibraryOpenSlots.front();
		m_qMusicLibraryOpenSlots.pop();
	}

	if( nID < 0 )
	{
		nID = m_vMusicLibrary.size();
		m_vMusicLibrary.push_back( soundInfo );
	}
	else
		memcpy(&m_vMusicLibrary[nID], &soundInfo, sizeof(soundInfo));//m_vMusicLibrary[nID] = soundInfo;

	m_vMusicLibrary[nID].m_nUnitType = AUDIO_MUSIC;
	strcpy_s(m_vMusicLibrary[nID].filename, sizeof(char) * MAX_PATH, szFileName);
	m_vMusicLibrary[nID].m_bInUse = true;

	return nID;
}

void CSGD_XAudio2::MusicUnloadSong( int nID )
{
	assert( nID > -1 && nID < (int)m_vMusicLibrary.size() && "Song ID out of range" );
	assert( m_vMusicLibrary[nID].m_bInUse && "Song ID not loaded" );

	//	Wipe it clean
	m_vMusicLibrary[nID].UnloadSoundInfo();

	m_qMusicLibraryOpenSlots.push(nID);
}

void CSGD_XAudio2::MusicPlaySong( int nID, bool bIsLooping )
{	
	assert( nID > -1 && nID < (int)m_vMusicLibrary.size() && "Song ID out of range" );
	assert( m_vMusicLibrary[nID].m_bInUse && "Song ID not loaded" );

	SoundInfo soundTemplate = m_vMusicLibrary[nID];

	Play(nID, soundTemplate, bIsLooping);
}

void CSGD_XAudio2::MusicStopSong(int nID)
{
	assert( nID > -1 && nID < (int)m_vMusicLibrary.size() && "Song ID out of range" );
	assert( m_vMusicLibrary[nID].m_bInUse && "Song ID not loaded" );

	for(unsigned int i=0; i < m_vVoices.size(); i++)
	{
		// Stop ALL INSTANCES of this sound
		if( m_vVoices[i].m_bIsMusic && m_vVoices[i].currentSoundID == nID )
		{
			m_vVoices[i].m_bIsLooping = false;
			RecycleVoiceNow(i);
		}
	}
}

bool CSGD_XAudio2::MusicIsSongPlaying(int nID)
{
	assert( nID > -1 && nID < (int)m_vMusicLibrary.size() && "Song ID out of range" );
	assert( m_vMusicLibrary[nID].m_bInUse && "Song ID not loaded" );

	for( unsigned int i = 0; i < m_vVoices.size(); ++i )
	{
		if( m_vVoices[i].m_bIsMusic && m_vVoices[i].currentSoundID == nID )
		{
			return true;
		}
	}

	return false;
}

float CSGD_XAudio2::MusicGetMasterVolume(void)
{
	assert( m_pMusicSubmixVoice != NULL && "Submix voice is NULL!" );

	float fVolume = 0.0f;
	m_pMusicSubmixVoice->GetVolume(&fVolume);

	return fVolume;
}

float CSGD_XAudio2::MusicGetSongVolume(int nID)
{
	assert( nID > -1 && nID < (int)m_vMusicLibrary.size() && "Song ID out of range" );
	assert( m_vMusicLibrary[nID].m_bInUse && "Song ID not loaded" );

	return m_vMusicLibrary[ nID ].m_fVolume;
}

float CSGD_XAudio2::MusicGetSongPan(int nID)
{
	assert( nID > -1 && nID < (int)m_vMusicLibrary.size() && "Song ID out of range" );
	assert( m_vMusicLibrary[nID].m_bInUse && "Song ID not loaded" );

	return m_vMusicLibrary[ nID ].m_fPan;
}

float CSGD_XAudio2::MusicGetSongFrequencyRatio(int nID)
{
	assert( nID > -1 && nID < (int)m_vMusicLibrary.size() && "Song ID out of range" );
	assert( m_vMusicLibrary[nID].m_bInUse && "Song ID not loaded" );

	return m_vMusicLibrary[ nID ].m_fFrequencyRatio;
}

void CSGD_XAudio2::MusicSetMasterVolume(float fVolume)
{
	assert(fVolume >= -XAUDIO2_MAX_VOLUME_LEVEL && fVolume <= XAUDIO2_MAX_VOLUME_LEVEL && "Volume outside valid range");

	m_pMusicSubmixVoice->SetVolume(fVolume, AUDIO_ENGINE_OP_SET);
}

void CSGD_XAudio2::MusicSetSongVolume(int nID, float fVolume)
{
	assert( nID > -1 && nID < (int)m_vMusicLibrary.size() && "Song ID out of range" );
	assert( m_vMusicLibrary[nID].m_bInUse && "Song ID not loaded" );
	assert(fVolume >= -XAUDIO2_MAX_VOLUME_LEVEL && fVolume <= XAUDIO2_MAX_VOLUME_LEVEL && "Volume level out of range");

	m_vMusicLibrary[ nID ].m_fVolume = fVolume;

	for( unsigned int i = 0; i < m_vVoices.size(); ++i )
	{
		if( m_vVoices[i].m_bIsMusic == true && m_vVoices[i].currentSoundID == nID )
		{
			m_vVoices[i].m_pVoice->SetVolume(fVolume, AUDIO_ENGINE_OP_SET);
		}
	}
}

// Affects ALL songs with this id
void CSGD_XAudio2::MusicSetSongPan(int nID, float fPan)
{
	assert( nID > -1 && nID < (int)m_vMusicLibrary.size() && "Song ID out of range" );
	assert( m_vMusicLibrary[nID].m_bInUse && "Song ID not loaded" );

	float outputMatrix[ 12 ];
	CalcPanOutputMatrix(fPan, m_vMusicLibrary[ nID ].wfx.nChannels, outputMatrix);

	m_vMusicLibrary[ nID ].m_fPan = fPan;

	for( unsigned int i = 0; i < m_vVoices.size(); ++i )
	{
		if( m_vVoices[i].m_bIsMusic == true && m_vVoices[i].currentSoundID == nID )
		{
			m_vVoices[i].m_pVoice->SetOutputMatrix(NULL, //m_pMusicSubmixVoice, 
				m_vMusicLibrary[ nID ].wfx.nChannels, 
				m_nMasterOutputChannels, 
				outputMatrix, 
				AUDIO_ENGINE_OP_SET);
		}
	}
}

void CSGD_XAudio2::MusicSetSongFrequencyRatio(int nID, float fFreqRatio)
{
	assert( nID > -1 && nID < (int)m_vMusicLibrary.size() && "Song ID out of range" );
	assert( m_vMusicLibrary[nID].m_bInUse && "Song ID not loaded" );

	//assert(fFreq >= XAUDIO2_MIN_FREQ_RATIO && fFreq <= XAUDIO2_MAX_FREQ_RATIO);//MAX_FREQ);
	assert(fFreqRatio >= 0.1f && fFreqRatio <= MAX_FREQ_RATIO && "Frequency out of range");
	
	m_vMusicLibrary[ nID ].m_fFrequencyRatio = fFreqRatio;

	for( unsigned int i = 0; i < m_vVoices.size(); ++i )
	{
		if( m_vVoices[i].m_bIsMusic == true && m_vVoices[i].currentSoundID == nID )
		{
			m_vVoices[i].m_pVoice->SetFrequencyRatio(fFreqRatio, AUDIO_ENGINE_OP_SET);
		}
	}
}