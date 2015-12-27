#include "ofApp.h"
#include <deque>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

const ofIndexType Faces[] = {
	2, 1, 0,
	3, 2, 0,
	4, 3, 0,
	5, 4, 0,
	1, 5, 0,
	11, 6,  7,
	11, 7,  8,
	11, 8,  9,
	11, 9,  10,
	11, 10, 6,
	1, 2, 6,
	2, 3, 7,
	3, 4, 8,
	4, 5, 9,
	5, 1, 10,
	2,  7, 6,
	3,  8, 7,
	4,  9, 8,
	5, 10, 9,
	1, 6, 10 };
const float Verts[] = {
	0.000f,  0.000f,  1.000f,
	0.894f,  0.000f,  0.447f,
	0.276f,  0.851f,  0.447f,
	-0.724f,  0.526f,  0.447f,
	-0.724f, -0.526f,  0.447f,
	0.276f, -0.851f,  0.447f,
	0.724f,  0.526f, -0.447f,
	-0.276f,  0.851f, -0.447f,
	-0.894f,  0.000f, -0.447f,
	-0.276f, -0.851f, -0.447f,
	0.724f, -0.526f, -0.447f,
	0.000f,  0.000f, -1.000f };
ofVec3f v[12];
ofVec3f n[12];
ofFloatColor c[12];
ofVbo vbo;


//--------------------------------------------------------------
void ofApp::setup(){
	phase = 0;
	updateWaveform(32);
	//ofSoundStreamSetup(1, 0); // mono output
	ofSoundStreamSetup(2, 0, 44100, m_bufferSize, 4);

	int i, j = 0;
	for ( i = 0; i < 12; i++ )
	{

		c[i].r = ofRandom(1.0);
		c[i].g = ofRandom(1.0);
		c[i].b = ofRandom(1.0);

		v[i][0] = Verts[j] * 100.f;
		j++;
		v[i][1] = Verts[j] * 100.f;
		j++;
		v[i][2] = Verts[j] * 100.f;
		j++;

	}

	vbo.setVertexData( &v[0], 12, GL_STATIC_DRAW );
	vbo.setColorData( &c[0], 12, GL_STATIC_DRAW );
	vbo.setIndexData( &Faces[0], 60, GL_STATIC_DRAW );

	glEnable(GL_DEPTH_TEST);
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
		float length = 500;
		auto targetArr = waves[waves.size() - 1];
		for(float i = 1; i < targetArr.size() / 2; i++)
		//for(float i = 1; i < m_bufferSize; i++)
		{
			float height = waves[waves.size() - 1][i];
			visualizerLine.addVertex(
				//(float)50 + length * i / ((float)m_bufferSize / 2),
				50.0f + length * i / ((float)m_bufferSize),
				500 - height * 4,
				10
			);
		}
		ofBackground(ofColor::black);
		ofSetLineWidth(2);
		ofSetColor(ofColor::red);
		visualizerLine.draw();
	}
	//draw synth things
	/*
	ofSetLineWidth(5);
	ofSetColor(ofColor::lightGreen);
	outLine.draw();
	ofSetColor(ofColor::cyan);
	waveLine.draw();

	//Draw rotating shape
	ofTranslate(ofGetWidth()/2, ofGetHeight()/2, 100);
	ofRotate(ofGetElapsedTimef() * 20.0, 1, 1, 0);
	glPointSize(10.f);
	vbo.drawElements( GL_TRIANGLES, 60);
	*/
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
	if(waves.size() >= m_waveTimeLength) 
	{
		waves.pop_front();
	}
	waves.push_back( *mags );
	delete mags;
}

std::vector<float>* ofApp::getFrequencyMagnitudes(float* samples) 
{
	const int fftBufferSize = 256; //windows is being a dick and doesn't let it be 2 * m_bufferSize, even though it should be constant;
	//const int fftBufferSize = 2 * m_bufferSize;

	//apply hanning to samples
	float fftBuffer[fftBufferSize];
	//hanning(fftBuffer, samples, fftBufferSize);
	for (int i = 0; i < fftBufferSize; i++) {
		float multiplier = 0.5 * (1 - cos(2 * M_PI * i / (fftBufferSize- 1)));
		fftBuffer[i] = multiplier * samples[i];
	}

	//mirror buffer
	/*
	float fftBuffer[fftBufferSize];
	for(int i = 0; i < fftBufferSize; i++)
	{
		if(i < fftBufferSize / 2)
		{
			fftBuffer[i] = samples[i];
		}
		else
		{
			fftBuffer[i] = samples[fftBufferSize - i - 1];
		}
	}
	*/
    //fft_object.do_fft (f, x);     // x (real) --FFT---> f (complex)
	//fft it.
	float fftOutput[fftBufferSize];
    fft_object.do_fft(fftOutput, fftBuffer);     // x (real) --FFT---> f (complex)
	//fft_object.do_fft(fftOutput, samples);     // x (real) --FFT---> f (complex)

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
