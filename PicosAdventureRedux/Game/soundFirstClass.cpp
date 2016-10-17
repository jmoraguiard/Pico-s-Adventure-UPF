#include "soundFirstClass.h"

SoundFirstClass::SoundFirstClass()
{
	m_DirectSound = 0;
	m_primaryBuffer = 0;

	//Pico
	celebratingBuffer_[0] = 0;
	celebratingBuffer_[1] = 0;
	eatingBuffer_[0] = 0;
	eatingBuffer_[1] = 0;
	eatingBuffer_[2] = 0;
	goodbyeBuffer_[0] = 0;	
	goodbyeBuffer_[1] = 0;	
	hiBuffer_[0] = 0;	
	hiBuffer_[1] = 0;	
	pointingBuffer_[0] = 0;
	pointingBuffer_[1] = 0;
	purrBuffer_[0] = 0;
	purrBuffer_[1] = 0;
	surpriseBuffer_[0] = 0;
	surpriseBuffer_[1] = 0;
	transformationBuffer_ = 0;
	birthdayBuffer_ = 0;
	christmasBuffer_ = 0;
	pirateBuffer_ = 0;
	tropicalBuffer_ = 0;

	// Game
	forestBuffer_ = 0;
	fruitFallBuffer_ = 0;
	leavesBuffer_ = 0;

	// Other
	birdEnterBuffer_ = 0;
	birdEatBuffer_ = 0;
	changeLevelBuffer_ = 0;
	selectionBuffer_ = 0;
	spaceshipFallingBuffer_ = 0;
	happySongBuffer_ = 0;
	owlBuffer_ = 0;
}


SoundFirstClass::SoundFirstClass(const SoundFirstClass& other)
{
}


SoundFirstClass::~SoundFirstClass()
{
}


bool SoundFirstClass::setup(HWND hwnd, std::string fileName)
{
	bool result;


	// Initialize direct sound and the primary sound buffer.
	result = setupDirectSound(hwnd);
	if(!result)
	{
		MessageBoxA(NULL, "Could not setup Direct Sound", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Load a wave audio file onto a secondary buffer.
	result = loadWaveFile("./Data/sounds/Feliz_celebracion_1.wav", &celebratingBuffer_[0]);
	if(!result)
	{
		MessageBoxA(NULL, "1", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadWaveFile("./Data/sounds/Feliz_celebracion_2.wav", &celebratingBuffer_[1]);
	if(!result)
	{
		MessageBoxA(NULL, "2", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadWaveFile("./Data/sounds/comer_1_long.wav", &eatingBuffer_[0]);
	if(!result)
	{
		MessageBoxA(NULL, "3", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadWaveFile("./Data/sounds/comer_2_long.wav", &eatingBuffer_[1]);
	if(!result)
	{
		MessageBoxA(NULL, "4", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadWaveFile("./Data/sounds/comer_3_long.wav", &eatingBuffer_[2]);
	if(!result)
	{
		MessageBoxA(NULL, "5", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadWaveFile("./Data/sounds/Adios_1.wav", &goodbyeBuffer_[0]);
	if(!result)
	{
		MessageBoxA(NULL, "5.1", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadWaveFile("./Data/sounds/Adios_2.wav", &goodbyeBuffer_[1]);
	if(!result)
	{
		MessageBoxA(NULL, "5.2", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadWaveFile("./Data/sounds/Hola_1.wav", &hiBuffer_[0]);
	if(!result)
	{
		MessageBoxA(NULL, "6.1", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadWaveFile("./Data/sounds/Hola_2.wav", &hiBuffer_[1]);
	if(!result)
	{
		MessageBoxA(NULL, "6.2", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadWaveFile("./Data/sounds/pointing_1.wav", &pointingBuffer_[0]);
	if(!result)
	{
		MessageBoxA(NULL, "7", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadWaveFile("./Data/sounds/pointing_2.wav", &pointingBuffer_[1]);
	if(!result)
	{
		MessageBoxA(NULL, "8", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadWaveFile("./Data/sounds/ronronear_1.wav", &purrBuffer_[0]);
	if(!result)
	{
		MessageBoxA(NULL, "9", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadWaveFile("./Data/sounds/ronronear_2.wav", &purrBuffer_[1]);
	if(!result)
	{
		MessageBoxA(NULL, "10", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadWaveFile("./Data/sounds/Sorprendido_1.wav", &surpriseBuffer_[0]);
	if(!result)
	{
		MessageBoxA(NULL, "11", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadWaveFile("./Data/sounds/Sorprendido_2.wav", &surpriseBuffer_[1]);
	if(!result)
	{
		MessageBoxA(NULL, "12", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadWaveFile("./Data/sounds/transformation.wav", &transformationBuffer_);
	if(!result)
	{
		MessageBoxA(NULL, "13", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadWaveFile("./Data/sounds/birthday_song.wav", &birthdayBuffer_);
	if(!result)
	{
		MessageBoxA(NULL, "14.1", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadWaveFile("./Data/sounds/christmas_song.wav", &christmasBuffer_);
	if(!result)
	{
		MessageBoxA(NULL, "14.2", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadWaveFile("./Data/sounds/pirate_song.wav", &pirateBuffer_);
	if(!result)
	{
		MessageBoxA(NULL, "15", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadWaveFile("./Data/sounds/tropical_song.wav", &tropicalBuffer_);
	if(!result)
	{
		MessageBoxA(NULL, "16", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// FOREST
	result = loadWaveFile("./Data/sounds/background_forest.wav", &forestBuffer_);
	if(!result)
	{
		MessageBoxA(NULL, "17", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadWaveFile("./Data/sounds/fruit_fall.wav", &fruitFallBuffer_);
	if(!result)
	{
		MessageBoxA(NULL, "18", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadWaveFile("./Data/sounds/leaves.wav", &leavesBuffer_);
	if(!result)
	{
		MessageBoxA(NULL, "19", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// OTHER
	result = loadWaveFile("./Data/sounds/bird_fly.wav", &birdEnterBuffer_);
	if(!result)
	{
		MessageBoxA(NULL, "20", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadWaveFile("./Data/sounds/bird_on_fruit.wav", &birdEatBuffer_);
	if(!result)
	{
		MessageBoxA(NULL, "21", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadWaveFile("./Data/sounds/change_level.wav", &changeLevelBuffer_);
	if(!result)
	{
		MessageBoxA(NULL, "22", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadWaveFile("./Data/sounds/selection.wav", &selectionBuffer_);
	if(!result)
	{
		MessageBoxA(NULL, "23", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadWaveFile("./Data/sounds/spaceship_falls.wav", &spaceshipFallingBuffer_);
	if(!result)
	{
		MessageBoxA(NULL, "23.1", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadWaveFile("./Data/sounds/happy.wav", &happySongBuffer_);
	if(!result)
	{
		MessageBoxA(NULL, "24", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadWaveFile("./Data/sounds/owl.wav", &owlBuffer_);
	if(!result)
	{
		MessageBoxA(NULL, "25", "SoundFirstClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	return true;
}


void SoundFirstClass::destroy()
{
	// Release the secondary buffer.
	destroyWaveFile(&celebratingBuffer_[0]);
	destroyWaveFile(&celebratingBuffer_[1]);
	destroyWaveFile(&eatingBuffer_[0]);
	destroyWaveFile(&eatingBuffer_[1]);
	destroyWaveFile(&eatingBuffer_[2]);
	destroyWaveFile(&goodbyeBuffer_[0]);
	destroyWaveFile(&goodbyeBuffer_[1]);
	destroyWaveFile(&hiBuffer_[0]);
	destroyWaveFile(&hiBuffer_[1]);
	destroyWaveFile(&pointingBuffer_[0]);
	destroyWaveFile(&pointingBuffer_[1]);
	destroyWaveFile(&purrBuffer_[0]);
	destroyWaveFile(&purrBuffer_[1]);
	destroyWaveFile(&surpriseBuffer_[0]);
	destroyWaveFile(&surpriseBuffer_[1]);
	destroyWaveFile(&transformationBuffer_);
	destroyWaveFile(&birthdayBuffer_);
	destroyWaveFile(&christmasBuffer_);
	destroyWaveFile(&pirateBuffer_);
	destroyWaveFile(&tropicalBuffer_);

	// Release game secondary buffers
	destroyWaveFile(&forestBuffer_);
	destroyWaveFile(&fruitFallBuffer_);
	destroyWaveFile(&leavesBuffer_);

	// Other
	destroyWaveFile(&birdEnterBuffer_);
	destroyWaveFile(&birdEatBuffer_);
	destroyWaveFile(&changeLevelBuffer_);
	destroyWaveFile(&selectionBuffer_);
	destroyWaveFile(&spaceshipFallingBuffer_);
	destroyWaveFile(&happySongBuffer_);
	destroyWaveFile(&owlBuffer_);

	// Shutdown the Direct Sound API.
	destroyDirectSound();

	return;
}


bool SoundFirstClass::setupDirectSound(HWND hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;


	// Initialize the direct sound interface pointer for the default sound device.
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level to priority so the format of the primary sound buffer can be modified.
	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the primary buffer description.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Get control of the primary sound buffer on the default sound device.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the format of the primary sound bufffer.
	// In this case it is a .WAV file recorded at 44,100 samples per second in 16-bit stereo (cd audio format).
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the primary buffer to be the wave format specified.
	result = m_primaryBuffer->SetFormat(&waveFormat);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}


void SoundFirstClass::destroyDirectSound()
{
	// Release the primary sound buffer pointer.
	if(m_primaryBuffer)
	{
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	// Release the direct sound interface pointer.
	if(m_DirectSound)
	{
		m_DirectSound->Release();
		m_DirectSound = 0;
	}

	return;
}


bool SoundFirstClass::loadWaveFile(char* filename, IDirectSoundBuffer8** secondaryBuffer)
{
	int error;
	FILE* filePtr;
	unsigned int count;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char* bufferPtr;
	unsigned long bufferSize;


	// Open the wave file in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if(error != 0)
	{
		return false;
	}

	// Read in the wave file header.
	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Check that the chunk ID is the RIFF format.
	if((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') || 
	   (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	// Check that the file format is the WAVE format.
	if((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
	   (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	// Check that the sub chunk ID is the fmt format.
	if((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
	   (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	// Check that the audio format is WAVE_FORMAT_PCM.
	if(waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	// Check that the wave file was recorded in stereo format.
	if(waveFileHeader.numChannels != 2)
	{
		return false;
	}

	// Check that the wave file was recorded at a sample rate of 44.1 KHz.
	if(waveFileHeader.sampleRate != 44100)
	{
		return false;
	}

	// Ensure that the wave file was recorded in 16 bit format.
	if(waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	// Check for the data chunk header.
	if((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
	   (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}

	// Set the wave format of secondary buffer that this wave file will be loaded onto.
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Create a temporary sound buffer with the specific buffer settings.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the temporary buffer.
	tempBuffer->Release();
	tempBuffer = 0;

	// Move to the beginning of the wave data which starts at the end of the data chunk header.
	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	// Create a temporary buffer to hold the wave file data.
	waveData = new unsigned char[waveFileHeader.dataSize];
	if(!waveData)
	{
		return false;
	}

	// Read in the wave file data into the newly created buffer.
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if(count != waveFileHeader.dataSize)
	{
		return false;
	}

	// Close the file once done reading.
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	// Lock the secondary buffer to write wave data into it.
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if(FAILED(result))
	{
		return false;
	}

	// Copy the wave data into the buffer.
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// Unlock the secondary buffer after the data has been written to it.
	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if(FAILED(result))
	{
		return false;
	}
	
	// Release the wave data since it was copied into the secondary buffer.
	delete [] waveData;
	waveData = 0;

	return true;
}


void SoundFirstClass::destroyWaveFile(IDirectSoundBuffer8** secondaryBuffer)
{
	// Release the secondary sound buffer.
	if(*secondaryBuffer)
	{
		(*secondaryBuffer)->Release();
		*secondaryBuffer = 0;
	}

	return;
}

bool SoundFirstClass::playCelebratingFile()
{
	HRESULT result;

	DWORD status[2];
	celebratingBuffer_[0]->GetStatus(&status[0]);
	celebratingBuffer_[1]->GetStatus(&status[1]);
	if(!(status[0] && DSBSTATUS_PLAYING) && !(status[1] && DSBSTATUS_PLAYING))
	{
		int soundToPlay = rand() % 2;

		// Set position at the beginning of the sound buffer.
		result = celebratingBuffer_[soundToPlay]->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = celebratingBuffer_[soundToPlay]->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = celebratingBuffer_[soundToPlay]->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playEatingFile()
{
	HRESULT result;

	DWORD status[3];
	eatingBuffer_[0]->GetStatus(&status[0]);
	eatingBuffer_[1]->GetStatus(&status[1]);
	eatingBuffer_[2]->GetStatus(&status[2]);
	if(!(status[0] && DSBSTATUS_PLAYING) && !(status[1] && DSBSTATUS_PLAYING) && !(status[2] && DSBSTATUS_PLAYING))
	{
		int soundToPlay = rand() % 3;
		// Set position at the beginning of the sound buffer.
		result = eatingBuffer_[soundToPlay]->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = eatingBuffer_[soundToPlay]->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = eatingBuffer_[soundToPlay]->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playGoodbyeFile()
{
	HRESULT result;
	
	DWORD status[2];
	goodbyeBuffer_[0]->GetStatus(&status[0]);
	goodbyeBuffer_[1]->GetStatus(&status[1]);
	if(!(status[0] && DSBSTATUS_PLAYING) && !(status[1] && DSBSTATUS_PLAYING))
	{
		int soundToPlay = rand() % 2;

		// Set position at the beginning of the sound buffer.
		result = goodbyeBuffer_[soundToPlay]->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = goodbyeBuffer_[soundToPlay]->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = goodbyeBuffer_[soundToPlay]->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playHiFile()
{
	HRESULT result;
	
	DWORD status[2];
	hiBuffer_[0]->GetStatus(&status[0]);
	hiBuffer_[1]->GetStatus(&status[1]);
	if(!(status[0] && DSBSTATUS_PLAYING) && !(status[1] && DSBSTATUS_PLAYING))
	{
		int soundToPlay = rand() % 2;

		// Set position at the beginning of the sound buffer.
		result = hiBuffer_[soundToPlay]->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = hiBuffer_[soundToPlay]->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = hiBuffer_[soundToPlay]->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playPointingFile()
{
	HRESULT result;

	DWORD status[2];
	pointingBuffer_[0]->GetStatus(&status[0]);
	pointingBuffer_[1]->GetStatus(&status[1]);
	if(!(status[0] && DSBSTATUS_PLAYING) && !(status[1] && DSBSTATUS_PLAYING))
	{
		int soundToPlay = rand() % 2;

		// Set position at the beginning of the sound buffer.
		result = pointingBuffer_[soundToPlay]->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = pointingBuffer_[soundToPlay]->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = pointingBuffer_[soundToPlay]->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playPurrFile()
{
	HRESULT result;

	DWORD status[2];
	purrBuffer_[0]->GetStatus(&status[0]);
	purrBuffer_[1]->GetStatus(&status[1]);
	if(!(status[0] && DSBSTATUS_PLAYING) && !(status[1] && DSBSTATUS_PLAYING))
	{
		int soundToPlay = rand() % 2;

		// Set position at the beginning of the sound buffer.
		result = purrBuffer_[soundToPlay]->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = purrBuffer_[soundToPlay]->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = purrBuffer_[soundToPlay]->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playSurpriseFile()
{
	HRESULT result;

	DWORD status[2];
	surpriseBuffer_[0]->GetStatus(&status[0]);
	surpriseBuffer_[1]->GetStatus(&status[1]);
	if(!(status[0] && DSBSTATUS_PLAYING) && !(status[1] && DSBSTATUS_PLAYING))
	{
		int soundToPlay = rand() % 2;

		// Set position at the beginning of the sound buffer.
		result = surpriseBuffer_[soundToPlay]->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = surpriseBuffer_[soundToPlay]->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = surpriseBuffer_[soundToPlay]->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playTransformationFile()
{
	HRESULT result;
	
	DWORD status;
	transformationBuffer_->GetStatus(&status);
	if(!(status && DSBSTATUS_PLAYING))
	{
		// Set position at the beginning of the sound buffer.
		result = transformationBuffer_->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = transformationBuffer_->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = transformationBuffer_->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playBirthdayFile()
{
	HRESULT result;
	
	DWORD status;
	birthdayBuffer_->GetStatus(&status);
	if(!(status && DSBSTATUS_PLAYING))
	{
		// Set position at the beginning of the sound buffer.
		result = birthdayBuffer_->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = birthdayBuffer_->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = birthdayBuffer_->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playChristmasFile()
{
	HRESULT result;
	
	DWORD status;
	christmasBuffer_->GetStatus(&status);
	if(!(status && DSBSTATUS_PLAYING))
	{
		// Set position at the beginning of the sound buffer.
		result = christmasBuffer_->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = christmasBuffer_->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = christmasBuffer_->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playPirateFile()
{
	HRESULT result;
	
	DWORD status;
	pirateBuffer_->GetStatus(&status);
	if(!(status && DSBSTATUS_PLAYING))
	{
		// Set position at the beginning of the sound buffer.
		result = pirateBuffer_->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = pirateBuffer_->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = pirateBuffer_->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playTropicalFile()
{
	HRESULT result;
	
	DWORD status;
	tropicalBuffer_->GetStatus(&status);
	if(!(status && DSBSTATUS_PLAYING))
	{
		// Set position at the beginning of the sound buffer.
		result = tropicalBuffer_->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = tropicalBuffer_->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = tropicalBuffer_->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playForest()
{
	HRESULT result;

	DWORD status;
	forestBuffer_->GetStatus(&status);
	if(!(status && DSBSTATUS_PLAYING))
	{
		// Set position at the beginning of the sound buffer.
		result = forestBuffer_->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = forestBuffer_->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = forestBuffer_->Play(0, 0, DSBPLAY_LOOPING);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playFruitFall()
{
	HRESULT result;

	DWORD status;
	fruitFallBuffer_->GetStatus(&status);
	if(!(status && DSBSTATUS_PLAYING))
	{
		// Set position at the beginning of the sound buffer.
		result = fruitFallBuffer_->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = fruitFallBuffer_->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = fruitFallBuffer_->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playLeaves()
{
	HRESULT result;

	DWORD status;
	leavesBuffer_->GetStatus(&status);
	if(!(status && DSBSTATUS_PLAYING))
	{
		// Set position at the beginning of the sound buffer.
		result = leavesBuffer_->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = leavesBuffer_->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = leavesBuffer_->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playBirdEnter()
{
	HRESULT result;

	DWORD status;
	birdEnterBuffer_->GetStatus(&status);
	if(!(status && DSBSTATUS_PLAYING))
	{
		// Set position at the beginning of the sound buffer.
		result = birdEnterBuffer_->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = birdEnterBuffer_->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = birdEnterBuffer_->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playBirdEat()
{
	HRESULT result;

	DWORD status;
	birdEatBuffer_->GetStatus(&status);
	if(!(status && DSBSTATUS_PLAYING))
	{
		// Set position at the beginning of the sound buffer.
		result = birdEatBuffer_->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = birdEatBuffer_->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = birdEatBuffer_->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playChangeLevel()
{
	HRESULT result;

	DWORD status;
	changeLevelBuffer_->GetStatus(&status);
	if(!(status && DSBSTATUS_PLAYING))
	{
		// Set position at the beginning of the sound buffer.
		result = changeLevelBuffer_->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = changeLevelBuffer_->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = changeLevelBuffer_->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playSelection()
{
	HRESULT result;

	DWORD status;
	selectionBuffer_->GetStatus(&status);
	if(!(status && DSBSTATUS_PLAYING))
	{
		// Set position at the beginning of the sound buffer.
		result = selectionBuffer_->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = selectionBuffer_->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = selectionBuffer_->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playSpaceshipFalling()
{
	HRESULT result;

	DWORD status;
	spaceshipFallingBuffer_->GetStatus(&status);
	if(!(status && DSBSTATUS_PLAYING))
	{
		// Set position at the beginning of the sound buffer.
		result = spaceshipFallingBuffer_->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = spaceshipFallingBuffer_->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = spaceshipFallingBuffer_->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playHappySong()
{
	HRESULT result;

	DWORD status;
	happySongBuffer_->GetStatus(&status);
	if(!(status && DSBSTATUS_PLAYING))
	{
		// Set position at the beginning of the sound buffer.
		result = happySongBuffer_->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = happySongBuffer_->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = happySongBuffer_->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundFirstClass::playOwl()
{
	HRESULT result;

	DWORD status;
	owlBuffer_->GetStatus(&status);
	if(!(status && DSBSTATUS_PLAYING))
	{
		// Set position at the beginning of the sound buffer.
		result = owlBuffer_->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = owlBuffer_->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = owlBuffer_->Play(0, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}
