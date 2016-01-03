#pragma once

#include <vector>
#include <queue>
#include <mutex>
#include "../deps/WindowsAudioListener/IAudioSink.h"

struct AudioChunk
{
	short* chunk;
	int size;
};

class SampleQueue : public IAudioSink
{
	std::queue<AudioChunk*> m_queue;
	int m_bitDepth;
	int m_maxSampleVal;
	int m_nChannels;
	int m_chunkSize;
	std::mutex m_mutex;
public:
	void Dequeue(std::vector<AudioChunk*>* outChunks, int Count);
	int CopyData(const BYTE* Data, const int NumFramesAvailable) override;
	SampleQueue(int BitsPerSample, int nChannels, int ChunkSize);
	~SampleQueue();
};
