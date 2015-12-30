#pragma once

#include <vector>
#include <queue>
#include <mutex>
#include "../deps/WindowsAudioListener/IAudioSink.h"

class SampleQueue : public IAudioSink
{
	std::queue<int> m_queue;
	int m_bitDepth;
	int m_maxSampleVal;
	std::mutex m_mutex;
public:
	void Dequeue(std::vector<float>& outData, size_t count);
	void Queue(std::vector<int> Samples);
	int CopyData(const BYTE* Data, const int NumFramesAvailable) override;
	SampleQueue(int);
	~SampleQueue();
};

