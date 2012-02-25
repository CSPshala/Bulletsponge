///////////////////////////////////////////////////////////////////////////
//	File			:	"CSGD_XAudio2.h"
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
///////////////////////////////////////////////////////////////////////////

/* 
Disclaimer:
This source code was developed for and is the property of:

(c) Full Sail University Game Development Curriculum 2008-2012 and
(c) Full Sail Real World Education Game Design & Development Curriculum 2000-2008

Full Sail students may not redistribute or make this code public, 
but may use it in their own personal projects.
*/

#pragma once

#include <XAudio2.h>
#include <MMSystem.h>
#pragma comment( lib, "Winmm.lib" )

#include <map>
using std::multimap;
using std::pair;

#include <vector>
using std::vector;

#include <queue>
using std::queue;

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

enum CustomAudioTypes{ AUDIO_NULL, AUDIO_SFX, AUDIO_MUSIC };

// The storage structure for audio information.
struct SoundInfo
{
	// For additional file support to be added at a later date.
	int m_nUnitType;

	// If the unit is in use.
	bool m_bInUse;

	float m_fVolume;
	float m_fFrequencyRatio;
	float m_fPan;

	// The filename of the unit.
	char filename[_MAX_PATH];

	// The configuration for the wave.
	WAVEFORMATEX wfx;

	// The storage for the audio data.
	XAUDIO2_BUFFER buffer;

	// The storage for the audio data in xwm format.
	XAUDIO2_BUFFER_WMA bufferwma;

	SoundInfo()
	{
		Init();
	}

	void Init()
	{
		m_nUnitType = AUDIO_NULL;
		filename[0] = '\0';
		m_bInUse = false;
		m_fVolume = 1.0f;
		m_fFrequencyRatio = 1.0f;
		m_fPan = 0.0f;

		ZeroMemory(&wfx, sizeof(wfx));
		ZeroMemory(&buffer, sizeof(buffer));
		ZeroMemory(&bufferwma, sizeof(bufferwma));
	}

	// Configures the audio unit from a wav or .xwm file.
	HRESULT LoadSoundInfo( LPCTSTR strFileName );

	void UnloadSoundInfo(void)
	{
		//	Clean up allocated memory
		if( buffer.pAudioData )
		{
			delete [] buffer.pAudioData;
			buffer.pAudioData = NULL;
		}

		if ( bufferwma.pDecodedPacketCumulativeBytes )
		{
			delete [] bufferwma.pDecodedPacketCumulativeBytes;
			bufferwma.pDecodedPacketCumulativeBytes = NULL;
		}

		Init();
	}
};

// A container for managing voices.
struct VoiceInfo
{
	// The XAudio2 voice pointer.
	IXAudio2SourceVoice*	m_pVoice;

	// The ID of the sound the voice is playing.
	int currentSoundID;

	// If the voice should loop or not.
	bool	m_bIsLooping;
	bool	m_bIsMusic;

	// I actually don't remember what this is.
	int m_nUnitType;

	//	Which pool does m_pVoice belong to?
	int	m_nVoicePoolID;
	int m_nVoicePtrID;

	// The backup buffers for the voice, to ensure it can continue to play.
	XAUDIO2_BUFFER buffer;
	XAUDIO2_BUFFER_WMA bufferwma;

	VoiceInfo()
	{
		m_bIsLooping = false;
		m_bIsMusic = false;
		m_nUnitType = AUDIO_NULL;
		m_pVoice = NULL;
		currentSoundID = -1;
		m_nVoicePoolID = -1;
		m_nVoicePtrID = -1;
	}
};

class CVoicePointerPool;

//	This class can be attached to a voice when it is created
//	to report on specific events (i.e. When a Buffer has ended or if there was an error)
class VoiceCallback : public IXAudio2VoiceCallback
{
public:
	CVoicePointerPool*		m_pVoicePool;
	int						m_nVoiceIndex;

	VoiceCallback(CVoicePointerPool* pVoicePool, int nVoiceIndex)
	{
		m_pVoicePool = pVoicePool;
		m_nVoiceIndex = nVoiceIndex;
	}

	//Called when the voice has just finished playing a contiguous audio stream.
	void _stdcall OnStreamEnd();

	//Unused methods are stubs
	void _stdcall OnVoiceProcessingPassEnd();
	
	// This function has to have the function body in the header OR IT WON'T COMPILE!
	void _stdcall OnVoiceProcessingPassStart(UINT32 SamplesRequired) {}

	void _stdcall OnBufferEnd(void * pBufferContext);  

	void _stdcall OnBufferStart(void * pBufferContext);
	void _stdcall OnLoopEnd(void * pBufferContext);
	void _stdcall OnVoiceError(void * pBufferContext, HRESULT Error);
};

/*
	To avoid title thread interruptions from a blocking DestroyVoice call, 
	the application can destroy voices on a separate non-critical thread, 
	[or the application can use voice pooling strategies to reuse voices 
	rather than destroying them.] 
	
	Note that voices can only be reused with 
	audio that has the [same data format] and the [same number of channels] 
	the voice was created with. 
	
	A voice can play audio data with different 
	sample rates than that of the voice by calling IXAudio2SourceVoice::SetFrequencyRatio 
	with an appropriate ratio parameter.
	*/
class CVoicePointerPool
{
private:
	//	What channel this voice was playing on
	vector<int>							m_vVoiceChannel;
	vector<IXAudio2SourceVoice*>		m_vVoicePool;
	//	DEPRECATED:	Left in to show how a VoiceCallback can be attached to a voice.
	vector<VoiceCallback*>				m_vVoiceCallbacks;

	queue<int>							m_qVoiceUnusedSlots;
	WAVEFORMATEX						m_wfx;

	IXAudio2*							m_pXAudio2;
	IXAudio2SubmixVoice*				m_pSubmixVoice;

	int CreateVoice();

public:
	CVoicePointerPool(IXAudio2* pXAudio2, 
					  IXAudio2SubmixVoice* pSubmixVoice, 
					  WAVEFORMATEX wfx);
	~CVoicePointerPool(void);

	//	Clean up all voices in this pool
	void DestroyVoices(void);

	//	Returns the id of an unused voice
	//	NOTE:	Will create a voice if none can be found.
	int FindUnusedVoice();

	//	Returns the requested voice ptr. 
	//	nVoiceChannel is the index into m_vVoices that this ptr will play on.
	//	NOTE:	Calls Start() on the returned voice ptr.
	IXAudio2SourceVoice* GetVoice(int nID, int nVoiceChannel);

	//	Recycles this voice for future use.
	//	NOTE:	Calls Stop() on the returned voice ptr.
	void ReturnVoice(int nID);

	//	Returns the voice index this voice ptr is currently playing on
	int GetVoiceChannel(int nID);
};

class CSGD_XAudio2
{
	//	Pointer to the XAudio2 interface
	IXAudio2*				m_pXAudio2;
	//	The final output voice
	IXAudio2MasteringVoice* m_pMasterVoice;
	
	//	Submix for SFX to change all sfx volumes at once
	IXAudio2SubmixVoice*	m_pSFXSubmixVoice;
	//	Submix for Music to change all song volumes at once
	IXAudio2SubmixVoice*	m_pMusicSubmixVoice;

	//	Speaker output configuration (SPEAKER_STEREO, SPEAKER_5POINT1, etc.)
	DWORD					m_dwChannelMask;
	//	Master voice channels
	int						m_nMasterOutputChannels;
	//	How many sounds are currently playing
	int						m_nActiveVoiceCount;

	// The loaded library of SFX
	vector<SoundInfo>		m_vSFXLibrary;
	queue<int>				m_qSFXLibraryOpenSlots;

	// The loaded library of Music
	vector<SoundInfo>		m_vMusicLibrary;
	queue<int>				m_qMusicLibraryOpenSlots;

	// The collection of voices currently playing.
	vector<VoiceInfo>		m_vVoices;
	queue<int>				m_qVoicesOpenSlots;

	//	Collection of different format voices
	multimap<int, CVoicePointerPool*>	m_vVoicePools;

	// Proper singleton:
	CSGD_XAudio2();
	CSGD_XAudio2(const CSGD_XAudio2&);
	CSGD_XAudio2& operator=(const CSGD_XAudio2&);
	~CSGD_XAudio2();

	///////////////////////////////////////////////////////////////////
	//	Function:	"Play"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID				-	The id of the sound or music to play
	//				soundTemplate	-	The SoundInfo of the sound or music
	//				bIsLooping		-	If it should loop or not
	//
	//	Return:		void
	//
	//	Purpose:	To play a sound on a voice channel.
	//
	//	NOTE:		BOTH SFXPlaySound and MusicPlaySong ultimately call this.
	//				Tries to find an open voice channel to play on (or makes one)
	//				then chooses a voice from a voice pool that can play this
	//				sound (or makes one) and then ultimately plays it.
	//
	//	IMPORTANT:	This is THE WORKHORSE of the wrapper.
	///////////////////////////////////////////////////////////////////
	void Play(int nID, SoundInfo soundTemplate, bool bIsLooping);

	///////////////////////////////////////////////////////////////////
	//	Function:	"CalcPanOutputMatrix"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		pan				-	The desired pan (left - -1.0 to 1.0 - right)
	//				channels		-	The number of input channels for this sound
	//				pLevelMatrix	-	Pointer to an array of floats
	//
	//	Return:		pLevelMatrix	-	The array filled out with the proper sound levels
	//									for the output channels.
	//
	//	Purpose:	To determine the proper sound levels for output channels
	//				according to the pan value.
	//
	//	NOTE:		UNDER CONSTRUCTION.  Works for mono (ALL output speaker configs)
	//				and stereo sounds (only for stereo speaker and 5.1 sound output configs).
	///////////////////////////////////////////////////////////////////
	void CalcPanOutputMatrix(float pan, WORD inputChannels, float *pLevelMatrix);

	///////////////////////////////////////////////////////////////////
	//	Function:	"GetOpenVoiceChannel"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		void
	//
	//	Return:		The index of an unused voice, -1 if none found.
	//
	//	Purpose:	To get an open voice channel.
	///////////////////////////////////////////////////////////////////
	int GetOpenVoiceChannel(void);

	///////////////////////////////////////////////////////////////////
	//	Function:	"RecycleVoiceNow"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nIndex	-	The voice index to be recycled.
	//
	//	Return:		void
	//
	//	Purpose:	To declare a voice ready to be reused.
	//
	//	NOTE:		Wipes the voice info clean and
	//				effectively stops the voice by returning
	//				the voice pointer to its voice pool.
	///////////////////////////////////////////////////////////////////
	void RecycleVoiceNow(int nIndex);

public:

	// Singleton accessor
	static CSGD_XAudio2* GetInstance( void );

	//	Returns the total number of voices available
	int	NumVoices(void)		{ return (int)m_vVoices.size(); }
	//	Returns the number of voices currently playing audio this frame
	int ActiveVoiceCount()	{ return m_nActiveVoiceCount; }

	///////////////////////////////////////////////////////////////////
	//	Function:	"InitXAudio2"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		void
	//
	//	Return:		true, if successful.
	//
	//	Purpose:	Initializes the XAudio2 manager.
	///////////////////////////////////////////////////////////////////
	bool InitXAudio2(void);

	///////////////////////////////////////////////////////////////////
	//	Function:	"ShutdownXAudio2"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		void
	//
	//	Return:		void
	//
	//	Purpose:	Unloads all the loaded sounds and music and 
	//				shutsdown XAudio2.
	///////////////////////////////////////////////////////////////////
	void ShutdownXAudio2(void);

	///////////////////////////////////////////////////////////////////
	//	Function:	"Update"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		void
	//
	//	Return:		void
	//
	//	Purpose:	Replays or recycles voices playing sounds or music.
	//				
	//	NOTE:	MUST be called ONCE a frame.
	///////////////////////////////////////////////////////////////////
	void Update(void);

	///////////////////////////////////////////////////////////////////
	//	Function:	"SilenceAll"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		void
	//
	//	Return:		void
	//
	//	Purpose:	Stops all playing sounds and music.
	///////////////////////////////////////////////////////////////////
	void SilenceAll(void);

	//////////////////////////////////////////////////////////
	//	SFX
	//////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////
	//	Function:	"SFXLoadSound"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		szFileName	-	The .wav file to load.
	//
	//	Return:		The id of the sound that was loaded or found, -1 if it failed.  
	//
	//	Purpose:	To load a sound into memory. 
	//	
	//	NOTE:		The function searches to see if the sound was already loaded 
	//				and returns the id if it was.
	//				
	//  IMPORTANT:	Supports .wav files ONLY.
	///////////////////////////////////////////////////////////////////
	int SFXLoadSound( const char* szFileName );

	///////////////////////////////////////////////////////////////////
	//	Function:	"SFXUnloadSound"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID	-	The id of the sound to unload.
	//
	//	Return:		void
	//
	//	Purpose:	To unload a sound from memory.
	///////////////////////////////////////////////////////////////////
	void SFXUnloadSound( int nID );

	///////////////////////////////////////////////////////////////////
	//	Function:	"SFXPlaySound"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID			-	The id of the sound to play.
	//				bIsLooping	-	If the sound should loop or not.
	//
	//	Return:		void
	//
	//	Purpose:	To play a sound.
	//
	//	NOTE:		Looped sounds will play until SFXStopSound() 
	//				is called on them.
	///////////////////////////////////////////////////////////////////
	void SFXPlaySound( int nID, bool bIsLooping = false );

	///////////////////////////////////////////////////////////////////
	//	Function:	"SFXStopSound"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID	-	The id of the sound to stop.
	//
	//	Return:		void
	//
	//	Purpose:	To stop a playing sound.
	//
	//	NOTE:		Stops ALL instances of the currently playing sound.
	///////////////////////////////////////////////////////////////////
	void SFXStopSound(int nID);

	///////////////////////////////////////////////////////////////////
	//	Function:	"SFXIsSoundPlaying"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID	-	The id of the sound to check.
	//
	//	Return:		true, if it found an instance of the sound playing.
	//
	//	Purpose:	To check if a sound is playing.
	//
	//	NOTE:		Will return true if ANY instance of the sound is playing.
	///////////////////////////////////////////////////////////////////
	bool SFXIsSoundPlaying(int nID);
	
	///////////////////////////////////////////////////////////////////
	//	Function:	"SFXGetMasterVolume"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		void
	//
	//	Return:		The Master SFX volume [Range: silence - 0.0f to 1.0f - loudest]
	//
	//	Purpose:	To get the overall volume of sounds.
	///////////////////////////////////////////////////////////////////
	float SFXGetMasterVolume(void);

	///////////////////////////////////////////////////////////////////
	//	Function:	"SFXGetSoundVolume"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID	-	The id of the sound to get the volume for.
	//
	//	Return:		The volume of the specific sound [Range: silence - 0.0f to 1.0f - loudest]
	//
	//	Purpose:	To get the volume of a specific sound.
	//
	//	NOTE:		The volume is relative to the Master Volume.
	//				(e.g. MasterVol = 0.5f and ThisVol = 0.5f. 
	//					  ActualOutputVol = MasterVol * ThisVol. 0.25f = 0.5f * 0.5f)
	///////////////////////////////////////////////////////////////////
	float SFXGetSoundVolume(int nID);

	///////////////////////////////////////////////////////////////////
	//	Function:	"SFXGetSoundPan"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID	-	The id of the sound to get the Pan for.
	//
	//	Return:		The pan of the specific sound [Range: left - -1.0f to 1.0f - right]
	//
	//	Purpose:	To get the pan of a specific sound.
	//
	//	NOTE:		left is -1.0f, right = 1.0f, 0.0f = center
	///////////////////////////////////////////////////////////////////
	float SFXGetSoundPan(int nID);

	///////////////////////////////////////////////////////////////////
	//	Function:	"SFXGetSoundFrequencyRatio"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID	-	The id of the sound to get the FrequencyRatio for.
	//
	//	Return:		The frequency ratio of the specific sound 
	//				[Range: 0.1f to 3.0f]
	//
	//	Purpose:	To get the frequency ratio of a specific sound.
	//
	//	NOTE:		A freqRatio > 1.0f will play faster while a freqRatio < 1.0f
	//				will play slower.
	//				
	//				A freqRatio of 0.5f lowers the pitch an octave while
	//				a freqRatio of 2.0f raises the pitch an octave.
	///////////////////////////////////////////////////////////////////
	float SFXGetSoundFrequencyRatio(int nID);

	///////////////////////////////////////////////////////////////////
	//	Function:	"SFXSetMasterVolume"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		fVolume	-	The volume of all sfx [Range: silence - 0.0f to 1.0f - loudest]
	//
	//	Return:		void
	//
	//	Purpose:	To set the overall volume of sounds.
	///////////////////////////////////////////////////////////////////
	void SFXSetMasterVolume(float fVolume);

	///////////////////////////////////////////////////////////////////
	//	Function:	"SFXSetSoundVolume"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID		-	The id of the sound to set the volume for.
	//				fVolume	-	The volume of the specific sound 
	//							[Range: silence - 0.0f to 1.0f - loudest]
	//
	//	Purpose:	To set the volume of a specific sound.
	//
	//	NOTE:		The volume is relative to the Master Volume.
	//				(e.g. MasterVol = 0.5f and ThisVol = 0.5f. 
	//					  ActualOutputVol = MasterVol * ThisVol. 0.25f = 0.5f * 0.5f)
	///////////////////////////////////////////////////////////////////
	void SFXSetSoundVolume(int nID, float fVolume);

	///////////////////////////////////////////////////////////////////
	//	Function:	"SFXSetSoundPan"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID	-	The id of the sound to set the Pan for.
	//				fPan -	The pan of the specific sound [Range: left - -1.0f to 1.0f - right]
	//
	//	Purpose:	To set the pan of a specific sound.
	//
	//	NOTE:		left is -1.0f, right = 1.0f, 0.0f = center
	//
	//	IMPORTANT:	Doesn't set the pan for currently playing instances,
	//				Only NEW ones.  Set BEFORE you play a new instance.
	///////////////////////////////////////////////////////////////////
	void SFXSetSoundPan(int nID, float fPan);

	///////////////////////////////////////////////////////////////////
	//	Function:	"SFXSetSoundFrequencyRatio"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID			-	The id of the sound to set the FrequencyRatio for.
	//				fFreqRatio	-	The frequency ratio of the specific sound 
	//								[Range: 0.1f to 3.0f]
	//
	//	Purpose:	To set the frequency ratio of a specific sound.
	//
	//	NOTE:		A freqRatio > 1.0f will play faster while a freqRatio < 1.0f
	//				will play slower.
	//				
	//				A freqRatio of 0.5f lowers the pitch an octave while
	//				a freqRatio of 2.0f raises the pitch an octave.
	//
	//	IMPORTANT:	Doesn't set the frequency for currently playing instances,
	//				Only NEW ones.  Set BEFORE you play a new instance.
	///////////////////////////////////////////////////////////////////
	void SFXSetSoundFrequencyRatio(int nID, float fFreqRatio);

	//////////////////////////////////////////////////////////
	//	Music
	//////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////
	//	Function:	"MusicLoadSong"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		szFileName	-	The .xwm file to load.
	//
	//	Return:		The id of the music that was loaded or found, -1 if it failed.  
	//
	//	Purpose:	To load a song into memory. 
	//	
	//	NOTE:		The function searches to see if the music was already loaded 
	//				and returns the id if it was.
	//				
	//  IMPORTANT:	Supports .xwm files ONLY.
	///////////////////////////////////////////////////////////////////
	int MusicLoadSong( const char* szFileName );

	///////////////////////////////////////////////////////////////////
	//	Function:	"MusicUnloadSong"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID	-	The id of the music to unload.
	//
	//	Return:		void
	//
	//	Purpose:	To unload a song from memory.
	///////////////////////////////////////////////////////////////////
	void MusicUnloadSong( int nID );

	///////////////////////////////////////////////////////////////////
	//	Function:	"MusicPlaySong"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID			-	The id of the music to play.
	//				bIsLooping	-	If the song should loop or not.
	//
	//	Return:		void
	//
	//	Purpose:	To play a song.
	//
	//	NOTE:		Looped songs will play until MusicStopSong() 
	//				is called on them.
	///////////////////////////////////////////////////////////////////
	void MusicPlaySong( int nID, bool bIsLooping = false );

	///////////////////////////////////////////////////////////////////
	//	Function:	"MusicStopSong"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID	-	The id of the music to stop.
	//
	//	Return:		void
	//
	//	Purpose:	To stop a playing song.
	//
	//	NOTE:		Stops ALL instances of the currently playing song.
	///////////////////////////////////////////////////////////////////
	void MusicStopSong(int nID);

	///////////////////////////////////////////////////////////////////
	//	Function:	"MusicIsSongPlaying"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID	-	The id of the music to check.
	//
	//	Return:		true, if it found an instance of the song playing.
	//
	//	Purpose:	To check if a song is playing.
	//
	//	NOTE:		Will return true if ANY instance of the song is playing.
	///////////////////////////////////////////////////////////////////
	bool MusicIsSongPlaying(int nID);
	
	///////////////////////////////////////////////////////////////
	//	Function:	Accessors
	//
	//	Purpose	:	To get the specified type.
	///////////////////////////////////////////////////////////////
	float MusicGetMasterVolume(void);

	///////////////////////////////////////////////////////////////////
	//	Function:	"MusicGetSongVolume"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID	-	The id of the music to get the volume for.
	//
	//	Return:		The volume of the specific song [Range: silence - 0.0f to 1.0f - loudest]
	//
	//	Purpose:	To get the volume of a specific song.
	//
	//	NOTE:		The volume is relative to the Master Volume.
	//				(e.g. MasterVol = 0.5f and ThisVol = 0.5f. 
	//					  ActualOutputVol = MasterVol * ThisVol. 0.25f = 0.5f * 0.5f)
	///////////////////////////////////////////////////////////////////
	float MusicGetSongVolume(int nID);
	
	///////////////////////////////////////////////////////////////////
	//	Function:	"MusicGetSongPan"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID	-	The id of the music to get the Pan for.
	//
	//	Return:		The pan of the specific song [Range: left - -1.0f to 1.0f - right]
	//
	//	Purpose:	To get the pan of a specific song.
	//
	//	NOTE:		left is -1.0f, right = 1.0f, 0.0f = center
	///////////////////////////////////////////////////////////////////
	float MusicGetSongPan(int nID);

	///////////////////////////////////////////////////////////////////
	//	Function:	"MusicGetSongFrequencyRatio"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID	-	The id of the music to get the FrequencyRatio for.
	//
	//	Return:		The frequency ratio of the specific song 
	//				[Range: 0.1f to 3.0f]
	//
	//	Purpose:	To get the frequency ratio of a specific song.
	//
	//	NOTE:		A freqRatio > 1.0f will play faster while a freqRatio < 1.0f
	//				will play slower.
	//				
	//				A freqRatio of 0.5f lowers the pitch an octave while
	//				a freqRatio of 2.0f raises the pitch an octave.
	///////////////////////////////////////////////////////////////////
	float MusicGetSongFrequencyRatio(int nID);

	///////////////////////////////////////////////////////////////////
	//	Function:	"MusicSetMasterVolume"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		fVolume	-	The volume of all music [Range: silence - 0.0f to 1.0f - loudest]
	//
	//	Return:		void
	//
	//	Purpose:	To set the overall volume of songs.
	///////////////////////////////////////////////////////////////////
	void MusicSetMasterVolume(float fVolume);

	///////////////////////////////////////////////////////////////////
	//	Function:	"MusicSetSongVolume"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID		-	The id of the music to set the volume for.
	//				fVolume	-	The volume of the specific song 
	//							[Range: silence - 0.0f to 1.0f - loudest]
	//
	//	Purpose:	To set the volume of a specific song.
	//
	//	NOTE:		The volume is relative to the Master Volume.
	//				(e.g. MasterVol = 0.5f and ThisVol = 0.5f. 
	//					  ActualOutputVol = MasterVol * ThisVol. 0.25f = 0.5f * 0.5f)
	//
	//	IMPORTANT:	Sets the volume for all currently playing instances 
	//				of this song.
	///////////////////////////////////////////////////////////////////
	void MusicSetSongVolume(int nID, float fVolume);

	///////////////////////////////////////////////////////////////////
	//	Function:	"MusicSetSongPan"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID	-	The id to the music to set the Pan for.
	//				fPan -	The pan of the specific song [Range: left - -1.0f to 1.0f - right]
	//
	//	Purpose:	To set the pan of a specific song.
	//
	//	NOTE:		left is -1.0f, right = 1.0f, 0.0f = center
	//
	//	IMPORTANT:	Sets the pan for all currently playing instances 
	//				of this song.
	///////////////////////////////////////////////////////////////////
	void MusicSetSongPan(int nID, float fPan);

	///////////////////////////////////////////////////////////////////
	//	Function:	"MusicSetSongFrequencyRatio"
	//
	//	Last Modified:		12/29/2011
	//
	//	Input:		nID			-	The id of the music to set the FrequencyRatio for.
	//				fFreqRatio	-	The frequency ratio of the specific song 
	//								[Range: 0.1f to 3.0f]
	//
	//	Purpose:	To set the frequency ratio of a specific song.
	//
	//	NOTE:		A freqRatio > 1.0f will play faster while a freqRatio < 1.0f
	//				will play slower.
	//				
	//				A freqRatio of 0.5f lowers the pitch an octave while
	//				a freqRatio of 2.0f raises the pitch an octave.
	//
	//	IMPORTANT:	Sets the frequency ratio for all currently playing instances 
	//				of this song.
	///////////////////////////////////////////////////////////////////
	void MusicSetSongFrequencyRatio(int nID, float fFreqRatio);
};