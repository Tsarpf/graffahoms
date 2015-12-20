#include "ofApp.h"
#include "../deps/ffft/FFTRealFixLen.h"

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

ffft::FFTRealFixLen<9> fft_object;


//--------------------------------------------------------------
void ofApp::setup(){
	phase = 0;
	updateWaveform(32);
	//ofSoundStreamSetup(1, 0); // mono output
	ofSoundStreamSetup(2, 0, 44100, 256, 4);

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
	ofBackground(ofColor::black);
	ofSetLineWidth(5);
	ofSetColor(ofColor::lightGreen);
	outLine.draw();
	ofSetColor(ofColor::cyan);
	waveLine.draw();

	ofTranslate(ofGetWidth()/2, ofGetHeight()/2, 100);
	ofRotate(ofGetElapsedTimef() * 20.0, 1, 1, 0);
	glPointSize(10.f);
	vbo.drawElements( GL_TRIANGLES, 60);
}

void ofApp::updateWaveform(int waveformResolution)
{
	waveform.resize(waveformResolution);
	waveLine.clear();

	// "waveformStep" maps a full oscillation of sin() to the size 
	// of the waveform lookup table
	float waveformStep = (M_PI * 2.) / (float) waveform.size();

	for(int i = 0; i < waveform.size(); i++) {
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
	//Todo: mirror buffer
	//fft it.
	//compute magnitude of resultant vectors
	//draw it!
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
