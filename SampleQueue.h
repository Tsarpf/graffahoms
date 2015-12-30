#pragma once

#include <vector>
#include <queue>
#include <mutex>
class SampleQueue
{
	std::queue<int> m_queue;
	int m_bitsPerSample;
	int m_maxSampleVal;
	std::mutex m_mutex;
public:
	void Dequeue(std::vector<float>& outData, size_t count);
	void Queue(std::vector<int> Samples);
	SampleQueue(int);
	~SampleQueue();
};

