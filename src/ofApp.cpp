#include "ofApp.h"
#include <deque>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

#include "../deps/WindowsAudioListener/AudioListener.h"
#include "SampleQueue.h"
ofApp::ofApp() : m_queue(m_bitDepth, 2, 128), m_listener(16, WAVE_FORMAT_PCM, 4, 0)
{
}

void ofApp::setup()
{
	m_listenerThread = std::thread(&AudioListener::RecordAudioStream, m_listener, &m_queue);
}

//--------------------------------------------------------------
void FreeChunks(std::vector<AudioChunk> chunks)
{
	for (int i = 0; i < chunks.size(); i++)
	{
		delete [] chunks[i].chunk;
	}
}
void ofApp::update(){
	static unsigned int start = ofGetElapsedTimeMillis();
	static unsigned int previousEnd = 0;

	static unsigned int currentTime = 0;
	//unsigned int sampleCount = sinceLast * 44100 / 1000 * 2;
	unsigned int lastTime = currentTime;
	currentTime = ofGetElapsedTimeMillis();
	unsigned int elapsedTime = currentTime - lastTime;
	m_sampleCounter += elapsedTime * 44100.f / 1000.f * 2.f;

	if (m_sampleCounter < m_bufferSize)
	{
		return;
	}

	m_sampleCounter -= m_bufferSize;

	int count = m_bufferSize / 128;
	auto chunks = new std::vector<AudioChunk*>();
	m_queue.Dequeue(chunks, count);

	if (chunks->size() < count)
	{
		std::cout << "not enough chunks " << chunks->size() << std::endl;
		//FreeChunks(chunks);
		return;
	}

	auto mags = getFrequencyMagnitudes(*chunks);

	//FreeChunks(chunks);

	logarithmize(*mags);
	if(m_waves.size() >= m_waveTimeLength) 
	{
		m_waves.pop_front();
	}
	m_waves.push_back( *mags );
	delete mags;
	previousEnd = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
void ofApp::draw(){
	if (m_waves.size() > 0)
	{
		//draw eq things
		ofPolyline visualizerLine;
		float length = 1000;
		auto targetArr = m_waves[m_waves.size() - 1];
		//for(float i = 1; i < targetArr.size() / 2; i++)
		for(float i = 1; i < targetArr.size(); i++)
		{
			float height = abs(m_waves[m_waves.size() - 1][i]);
			visualizerLine.addVertex(
				//(float)50 + length * i / ((float)m_bufferSize / 2),
				0.0f + length * i / ((float)targetArr.size()),
				500 - height * 16,
				10
			);
		}
		ofBackground(ofColor::black);
		ofSetLineWidth(2);
		ofSetColor(ofColor::red);
		visualizerLine.draw();
	}
}

void ofApp::findMinMax(float& Min, float& Max, const std::vector<float>& List)
{
	for (unsigned int i = 0; i < List.size(); i++)
	{
		if (List[i] > m_max)
		{
			m_max = List[i];
		}
		if (List[i] < m_min && List[i] != 0)
		{
			m_min = List[i];
		}
	}
	Min = m_min;
	Max = m_max;
}

void ofApp::logarithmize(std::vector<float>& List)
{
	float min, max;
	findMinMax(min, max, List);


	auto newList = std::vector<float>();
	newList.push_back(List[1]);
	float multiplier = 1.5f;
	for (unsigned int i = 2; i < List.size(); i *= multiplier)
	{
		float sum = 0;
		float count = 0;
		for (int c = i; c < i * multiplier && c < List.size(); c++)
		{
			sum += List[c];
			count++;
		}
		float result = sum / count;
		//result *= pow(i,2); //way too steep
		result *= log2f(i);
		newList.push_back(result);
	}
	List.clear();
	for (unsigned int i = 0; i < newList.size(); i++)
	{
		List.push_back(newList[i]);
	}
}

std::vector<float>* ofApp::getFrequencyMagnitudes(std::vector<AudioChunk*> chunks) 
{
	
	for (int j = 0; j < chunks.size(); j++)
	{
		for (int i = 0; i < chunks[j]->size; i++)
		{
			float multiplier = 0.5 * (1 - cos(2 * M_PI * i / (m_bufferSize- 1)));
			short sample = chunks[j]->chunk[i];
			float fSample = (float)sample / (float)(pow(2, 16) - 1);
			int index = i + chunks[j]->size * j;
			m_fftBuffer[index] = fSample * multiplier;
		}
	}

	//fft it.
	float fftOutput[m_bufferSize];
    m_fftObject.do_fft(fftOutput, m_fftBuffer);     // x (real) --FFT---> f (complex)

	//compute magnitude of resultant vectors
	std::vector<float>* mags = new std::vector<float>();
	for(int i = 0; i < m_bufferSize / 4; i++)
	{
		float realPart = pow(fftOutput[i], 2);
		float imagPart = pow(fftOutput[m_bufferSize / 2 + i], 2);

		float mag = sqrt(realPart) + sqrt(imagPart);
		mags->push_back(mag);
	}

	return mags;
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
