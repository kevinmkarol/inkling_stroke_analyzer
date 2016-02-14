#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"

#define PORT 2016
#define RENDER_HEIGHT 800
#define RENDER_WIDTH 618

#define MAX_X 1876
#define MAX_Y 1920
#define MAX_PRESSURE 1023


typedef struct {
  float cord_x;
  float cord_y;
  float cord_p;
  float tilt_t;
  float tilt_x;
  float tilt_y;
} inkling_data;


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        void toggleRecording();
        void exit();

		void keyReleased(int key);

    
        void onMessageReceived(ofxOscMessage &msg);
    
        void saveInklingData(inkling_data &data);
        void clearInklingData(inkling_data &data);
    
        //Saving XML data
        ofxXmlSettings _dataToRecord;
        std::string _dataFileName;
        unsigned long long _startMilliseconds;
    
        ofxOscReceiver receiver;
        bool _isRecording;
    
        //Mesh drawing stuff
        ofMesh mesh;
		
};
