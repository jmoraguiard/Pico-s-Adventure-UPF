#ifndef _SOUND_SECOND_CLASS_H_
#define _SOUND_SECOND_CLASS_H_

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <string>
#include <map>

///////////////////////////////////////////////////////////////////////////////
// Class name: SoundClass
///////////////////////////////////////////////////////////////////////////////
class SoundSecondClass
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
		SoundSecondClass();
		SoundSecondClass(const SoundSecondClass&);
		~SoundSecondClass();

		bool setup(HWND);
		void destroy();

		// Pico players
		bool loadFile(std::string name, std::string fileName);
		bool playFile(std::string name, bool loop);
		bool isPlaying(std::string name);

		// Specific methods for multiple sound randomization
		bool playCelebrate();
		bool playHi();
		bool playPointing();
		bool playSad();
		bool playSurprise();

	private:
		bool setupDirectSound(HWND);
		void destroyDirectSound();

		bool loadWaveFile(std::string, IDirectSoundBuffer8**);
		void destroyWaveFile(IDirectSoundBuffer8**);

	private:
		IDirectSound8* m_DirectSound;
		IDirectSoundBuffer* m_primaryBuffer;

		// Pico Buffers
		std::map<std::string, IDirectSoundBuffer8*> sounds_;
};

#endif //_SOUND_SECOND_CLASS_H_