#include "SampleQueue.h"
#include <algorithm>

void SampleQueue::Dequeue(std::vector<float>& outData, size_t Count)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	int count = std::min(Count, m_queue.size());
	for (int i = 0; i < Count; i++)
	{
		int iSample = m_queue.front();
		m_queue.pop();

		//move from f.ex -2^15 -> 2^15 to -1 -> 1
		float fSample = iSample / m_maxSampleVal;

		outData.push_back(fSample);
	}
}
void SampleQueue::Queue(std::vector<int> Samples)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	for (int i = 0; i < Samples.size(); i++)
	{
		m_queue.push(Samples[i]);
	}
}

SampleQueue::SampleQueue(int BitsPerSample)
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
	m_bitsPerSample = BitsPerSample;
}

SampleQueue::~SampleQueue()
{
}
