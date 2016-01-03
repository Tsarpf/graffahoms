#include "SampleQueue.h"
#include <algorithm>
#include <iostream>

SampleQueue::SampleQueue(int BitsPerSample, int nChannels, int ChunkSize)
{
	if (BitsPerSample > 32)
	{
		throw "too many bits";
	}
	else if (BitsPerSample == 32)
	{
		m_maxSampleVal = (1 << 31) - 1;
	}
	else
	{
		m_maxSampleVal = (1 << BitsPerSample) - 1;
	}
	m_bitDepth = BitsPerSample;
	m_nChannels = nChannels;
	m_chunkSize = ChunkSize;
}

SampleQueue::~SampleQueue()
{
}

void SampleQueue::Dequeue(std::vector<AudioChunk*>* outChunks, int Count)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_queue.size() == 0)
		return;

	int count = std::min((int)m_queue.size(), Count);
	for (int i = 0; i < count; i++)
	{
		outChunks->push_back(m_queue.front());
		m_queue.pop();
	}
}

int SampleQueue::CopyData(const BYTE* Data, const int NumFramesAvailable)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (NumFramesAvailable % 448 != 0 || NumFramesAvailable == 0 || Data == NULL)
	{
		std::cout << "sample count wasn't 448, was: " << NumFramesAvailable << std::endl;
		return 1;
	}

	short* dataStart = (short*)Data;
	const int divisor = sizeof(long) / sizeof(short);
	for (short* data = (short*)Data; data < dataStart + NumFramesAvailable * 2; data+=128)
	{
		AudioChunk* chunk = new AudioChunk();

		long long* copier = new long long[128 / divisor];

		for (int i = 0; i < (128 / divisor) ; i++)
		{
			copier[i] = ((long long*)data)[i];
		}

		chunk->chunk = (short*)copier;
			
		chunk->size = 128;

		m_queue.push(chunk);
	}

	return 0;
}
