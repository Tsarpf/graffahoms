#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void updateWaveform(int waveformResolution);
		void audioOut(float * output, int bufferSize, int nChannels);

		std::vector<float> waveform; // this is the lookup table
		double phase;
		float frequency;

		ofMutex waveformMutex;
		ofPolyline waveLine;
		ofPolyline outLine;

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

};
