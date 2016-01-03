#pragma once

#include "ofMain.h"
#include "../deps/ffft/FFTRealFixLen.h"
#include "SampleQueue.h"
#include "../deps/WindowsAudioListener/AudioListener.h"

class ofApp : public ofBaseApp
{
	unsigned int m_sampleCounter = 0;

	const int m_bitDepth = 16;

	std::thread m_listenerThread;
	AudioListener m_listener;

	ffft::FFTRealFixLen<9> m_fftObject;
	std::deque<std::vector<float>> m_waves;
	SampleQueue m_queue;

	std::vector<float> waveform; // this is the lookup table
	double phase;
	float frequency;

	ofMutex waveformMutex;
	ofPolyline waveLine;
	ofPolyline outLine;

	static const int m_bufferSize = 512;
	float m_fftBuffer[m_bufferSize];

	const unsigned int m_waveTimeLength = 200;
	float m_min = 1 << 31;
	float m_max = 0;

public:
	ofApp();

	void setup();
	void update();
	void draw();

	std::vector<float>* getFrequencyMagnitudes(std::vector<AudioChunk*> chunks);
	void logarithmize(std::vector<float>& List);
	void findMinMax(float& Min, float& Max, const std::vector<float>& List);


	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
};
