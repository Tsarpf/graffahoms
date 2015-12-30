#include "SampleQueue.h"
#include <algorithm>
#include <iostream>

SampleQueue::SampleQueue(int BitsPerSample, int nChannels)
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
}

SampleQueue::~SampleQueue()
{
}

void SampleQueue::Dequeue(std::vector<float>& outData, size_t Count)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	int count = std::min(Count, m_queue.size());
	for (int i = 0; i < count; i++)
	{
		int iSample = m_queue.front();
		m_queue.pop();

		//move from f.ex -2^15 -> 2^15 to -1 -> 1
		float fSample = (float)iSample / (float)m_maxSampleVal;

		outData.push_back(fSample);
	}
	//std::cout << "Asked for count: " << Count << " giving out: " << count << std::endl;
}
void SampleQueue::Enqueue(std::vector<int>& Samples)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	for (int i = 0; i < Samples.size(); i++)
	{
		m_queue.push(Samples[i]);
	}
}


int SampleQueue::CopyData(const BYTE* Data, const int NumFramesAvailable)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	//std::vector<int> data;

	int byteCount = NumFramesAvailable * (m_bitDepth / 8) * m_nChannels;
	int step = m_bitDepth / 2;
	for (int i = 0; i < NumFramesAvailable * m_nChannels; i+=step)
	{
		short sample = Data[i];
		//data.push_back(sample);
		m_queue.push(sample);
	}

	//Enqueue(data);

	return 0;
}
