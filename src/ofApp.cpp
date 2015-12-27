#include "ofApp.h"
#include <deque>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

void ofApp::setup(){
	phase = 0;
	updateWaveform(32);
	//ofSoundStreamSetup(1, 0); // mono output
	ofSoundStreamSetup(2, 0, 44100, m_bufferSize, 4);
}

//--------------------------------------------------------------
void ofApp::update(){
	ofScopedLock waveformLock(waveformMutex);
	updateWaveform(ofMap(ofGetMouseX(), 0, ofGetWidth(), 3, 64, true));
	frequency = ofMap(ofGetMouseY(), 0, ofGetHeight(), 60, 700, true);
}

//--------------------------------------------------------------
void ofApp::draw(){
	if (waves.size() > 0)
	{
		//draw eq things
		ofScopedLock waveformLock(waveformMutex);
		ofPolyline visualizerLine;
		float length = 3000;
		auto targetArr = waves[waves.size() - 1];
		for(float i = 1; i < targetArr.size() / 2; i++)
		//for(float i = 1; i < m_bufferSize; i++)
		{
			float height = waves[waves.size() - 1][i];
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

void ofApp::updateWaveform(int waveformResolution)
{
	waveform.resize(waveformResolution);
	waveLine.clear();

	// "waveformStep" maps a full oscillation of sin() to the size 
	// of the waveform lookup table
	float waveformStep = (M_PI * 2.) / (float) waveform.size();

	for(unsigned int i = 0; i < waveform.size(); i++) {
		waveform[i] = sin(i * waveformStep);
		waveLine.addVertex(ofMap(i, 0, waveform.size() - 1, 0, ofGetWidth()),
				ofMap(waveform[i], -1, 1, 0, ofGetHeight()));
	}
}

void ofApp::audioOut(float * output, int bufferSize, int nChannels)
{
	ofScopedLock waveformLock(waveformMutex);

	float sampleRate = 44100;
	float phaseStep = frequency / sampleRate;

	outLine.clear();

	for(int i = 0; i < bufferSize * nChannels; i += nChannels) {
		phase += phaseStep;
		int waveformIndex = (int)(phase * waveform.size()) % waveform.size();
		output[i] = waveform[waveformIndex];

		outLine.addVertex(ofMap(i, 0, bufferSize - 1, 0, ofGetWidth()),
				ofMap(output[i], -1, 1, 0, ofGetHeight()));
	}
	if(bufferSize != m_bufferSize) {
		std::cout << "buffer size abnormal: " << bufferSize << " vs " << m_bufferSize << std::endl;
		return;
	}


	auto mags = getFrequencyMagnitudes(output);
	logarithmize(*mags);
	if(waves.size() >= m_waveTimeLength) 
	{
		waves.pop_front();
	}
	waves.push_back( *mags );
	delete mags;

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
    fft_object.do_fft(fftOutput, fftBuffer);     // x (real) --FFT---> f (complex)

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
