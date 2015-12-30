#include "ofApp.h"
#include <deque>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

#include "../deps/WindowsAudioListener/AudioListener.h"
#include "SampleQueue.h"
ofApp::ofApp() : m_queue(m_bitDepth, 2), m_listener(16, WAVE_FORMAT_PCM, 4, 0)
{
}

void ofApp::setup()
{
	//ofSoundStreamSetup(1, 0); // mono output
	//ofSoundStreamSetup(2, 0, 44100, m_bufferSize, 4);

	//AudioListener(int BitsPerSample, int FormatTag, int BlockAlign, int XSize);
	//0x8889000a
	//	AudioListener listener(bitDepth, WAVE_FORMAT_PCM, 4, 0);
	m_listenerThread = std::thread(&AudioListener::RecordAudioStream, m_listener, &m_queue);

	//make an interface for audio sink -- done, 

	//make a thread for audio listener, run it's function.
	
	//could we just make samplequeue implement the sink interface?
	//then every time we get data from audio listener, we lock the queue and add the data there.
	//sound good?

	//get data from audio listener

	//std::thread t1(&A::foo, &a, 100);
}

//--------------------------------------------------------------
void ofApp::update(){
	static unsigned int start = ofGetElapsedTimeMillis();
	static unsigned int previousEnd = 0;

	unsigned int sinceStart = ofGetElapsedTimeMillis() - start;
	unsigned int samplesShouldBeUsed =  sinceStart * 10 * 441 * 2; //441 samples per 10th of a second, in two channels

	unsigned int sinceLast = ofGetElapsedTimeMillis() - previousEnd;

	int sampleCount = samplesShouldBeUsed - m_totalSamplesUsed;
	m_totalSamplesUsed += sampleCount;

	std::vector<float> data;
	m_queue.Dequeue(data, sampleCount);

	if (data.size() == 0)
		return;

	std::cout << data.size() << " since last " << sinceLast << std::endl;
	auto mags = getFrequencyMagnitudes(data.data());
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
		float length = 3000;
		auto targetArr = m_waves[m_waves.size() - 1];
		for(float i = 1; i < targetArr.size() / 2; i++)
		{
			float height = m_waves[m_waves.size() - 1][i];
			visualizerLine.addVertex(
				//(float)50 + length * i / ((float)m_bufferSize / 2),
				50.0f + length * i / ((float)m_bufferSize),
				300 - height * 4,
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

	for (unsigned int i = 0; i < List.size(); i++)
	{
		List[i] = 10.f * log10f(List[i] / max);
	}
}

std::vector<float>* ofApp::getFrequencyMagnitudes(float* samples) 
{
	const int fftBufferSize = 256; //windows is being a dick and doesn't let it be 2 * m_bufferSize, even though it should be constant;
	//const int fftBufferSize = 2 * m_bufferSize;

	//apply hanning to samples
	float fftBuffer[fftBufferSize];
	for (int i = 0; i < fftBufferSize; i++) {
		float multiplier = 0.5 * (1 - cos(2 * M_PI * i / (fftBufferSize- 1)));
		fftBuffer[i] = multiplier * samples[i];
	}

	//fft it.
	float fftOutput[fftBufferSize];
    m_fftObject.do_fft(fftOutput, fftBuffer);     // x (real) --FFT---> f (complex)

	//compute magnitude of resultant vectors
	std::vector<float>* mags = new std::vector<float>();
	for(int i = 0; i < fftBufferSize / 2; i++)
	{
		float realPart = pow(fftOutput[i], 2);
		float imagPart = pow(fftOutput[fftBufferSize / 2 + i], 2);

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
