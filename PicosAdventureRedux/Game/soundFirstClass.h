#ifndef _SOUND_FIRST_CLASS_H_
#define _SOUND_FIRST_CLASS_H_

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <string>

///////////////////////////////////////////////////////////////////////////////
// Class name: SoundClass
///////////////////////////////////////////////////////////////////////////////
class SoundFirstClass
{
	private:
		struct WaveHeaderType
		{
			char chunkId[4];
			unsigned long chunkSize;
			char format[4];
			char subChunkId[4];
			unsigned long subChunkSize;
			unsigned short audioFormat;
			unsigned short numChannels;
			unsigned long sampleRate;
			unsigned long bytesPerSecond;
			unsigned short blockAlign;
			unsigned short bitsPerSample;
			char dataChunkId[4];
			unsigned long dataSize;
		};

	public:
		SoundFirstClass();
		SoundFirstClass(const SoundFirstClass&);
		~SoundFirstClass();

		bool setup(HWND, std::string fileName);
		void destroy();

		// Pico players
		bool playCelebratingFile();
		bool playEatingFile();
		bool playGoodbyeFile();
		bool playHiFile();
		bool playPointingFile();
		bool playPurrFile();
		bool playSurpriseFile();
		bool playTransformationFile();
		bool playBirthdayFile();
		bool playChristmasFile();
		bool playPirateFile();
		bool playTropicalFile();

		// Game players
		bool playForest();
		bool playFruitFall();
		bool playLeaves();

		// Other
		bool playBirdEnter();
		bool playBirdEat();
		bool playChangeLevel();
		bool playSelection();
		bool playSpaceshipFalling();
		bool playHappySong();
		bool playOwl();

	private:
		bool setupDirectSound(HWND);
		void destroyDirectSound();

		bool loadWaveFile(char*, IDirectSoundBuffer8**);
		void destroyWaveFile(IDirectSoundBuffer8**);

	private:
		IDirectSound8* m_DirectSound;
		IDirectSoundBuffer* m_primaryBuffer;

		// Pico Buffers
		IDirectSoundBuffer8* celebratingBuffer_[2];
		IDirectSoundBuffer8* eatingBuffer_[3];
		IDirectSoundBuffer8* goodbyeBuffer_[2];
		IDirectSoundBuffer8* hiBuffer_[2];
		IDirectSoundBuffer8* pointingBuffer_[2];
		IDirectSoundBuffer8* purrBuffer_[2];
		IDirectSoundBuffer8* surpriseBuffer_[2];
		IDirectSoundBuffer8* transformationBuffer_;
		IDirectSoundBuffer8* birthdayBuffer_;
		IDirectSoundBuffer8* pirateBuffer_;
		IDirectSoundBuffer8* tropicalBuffer_;
		IDirectSoundBuffer8* christmasBuffer_;

		// Game Buffers
		IDirectSoundBuffer8* forestBuffer_;
		IDirectSoundBuffer8* fruitFallBuffer_;
		IDirectSoundBuffer8* leavesBuffer_;

		//Others
		IDirectSoundBuffer8* birdEnterBuffer_;
		IDirectSoundBuffer8* birdEatBuffer_;
		IDirectSoundBuffer8* changeLevelBuffer_;
		IDirectSoundBuffer8* selectionBuffer_;
		IDirectSoundBuffer8* spaceshipFallingBuffer_;
		IDirectSoundBuffer8* happySongBuffer_;
		IDirectSoundBuffer8* owlBuffer_;
};

#endif //_SOUND_FIRST_CLASS_H_