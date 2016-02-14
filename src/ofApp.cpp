#include "ofApp.h"
#include <stdio.h>
#include <stdlib.h>

//--------------------------------------------------------------
void ofApp::setup(){
  receiver.setup(PORT);
  ofSetFrameRate(60);
  ofSetBackgroundColor(ofFloatColor(0));
  
  mesh.setMode(OF_PRIMITIVE_POINTS);
  mesh.enableColors();
}

//--------------------------------------------------------------
void ofApp::onMessageReceived(ofxOscMessage &msg){
  string addr = msg.getAddress();
  string message = msg.getArgAsString(0);
  printf("%s", message.c_str());

}

//--------------------------------------------------------------
void ofApp::update(){
  inkling_data  data;
  while(receiver.hasWaitingMessages()){
    ofxOscMessage msg;
    receiver.getNextMessage(msg);
    if(_isRecording){
      if(msg.getAddress() == "/cord_xyp"){
        clearInklingData(data);
        data.cord_x = (msg.getArgAsFloat(0)/MAX_X);
        data.cord_y = (-msg.getArgAsFloat(1)/MAX_Y);
        data.cord_p = (msg.getArgAsFloat(2)/MAX_PRESSURE);
      }
      if(msg.getAddress() == "/tilt_txy"){
        data.tilt_t = msg.getArgAsFloat(0);
        data.tilt_x = msg.getArgAsFloat(1);
        data.tilt_y = msg.getArgAsFloat(2);
        if(data.cord_x != 0){
          if(data.cord_p != 0){
            ofVec3f vec(data.cord_x * RENDER_WIDTH, data.cord_y * RENDER_HEIGHT, 0);
            mesh.addVertex(vec);
            mesh.addColor(ofFloatColor(1)* data.cord_p);
          }
          saveInklingData(data);
        }
      }
    }else{
      //discard message
    }
  }
}

//--------------------------------------------------------------
void ofApp::saveInklingData(inkling_data &data){
    time_t t = time(0);
    struct tm * now = localtime(&t);
    
    int elapsedMilliseconds = ofGetElapsedTimeMillis() - _startMilliseconds;
    
    int numTags = _dataToRecord.getNumTags("inklingDataPoint");
    _dataToRecord.addTag("inklingDataPoint");
    _dataToRecord.pushTag("inklingDataPoint", numTags);
    _dataToRecord.addValue("milliseconds", elapsedMilliseconds);
    
    _dataToRecord.addValue("cord_x", data.cord_x);
    _dataToRecord.addValue("cord_y", data.cord_y);
    _dataToRecord.addValue("cord_p", data.cord_p);
    _dataToRecord.addValue("tilt_x", data.tilt_x);
    _dataToRecord.addValue("tilt_y", data.tilt_y);
    _dataToRecord.addValue("tilt_t", data.tilt_t);
    
    _dataToRecord.popTag();
}


//--------------------------------------------------------------
void ofApp::clearInklingData(inkling_data &data){
  data.cord_x = 0;
  data.cord_y = 0;
  data.cord_p = 0;
  data.tilt_y = 0;
  data.tilt_x = 0;
  data.tilt_t = 0;
}

//--------------------------------------------------------------
void ofApp::draw(){
  //ofPushMatrix();
  //ofScale(0.4,0.5);
  mesh.drawVertices();
  //ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
   if(key == 'r'){
     toggleRecording();
   }
}

void ofApp::toggleRecording(){
   _isRecording = !_isRecording;
   if(_isRecording){
        //Setup Output file
        time_t now = time(0);
        tm *ltm = localtime(&now);
        _dataFileName = "inklingData" + to_string(ltm->tm_mday) + to_string(ltm->tm_hour)
                                      + to_string(ltm->tm_min) + to_string(ltm->tm_sec);
        _dataToRecord.loadFile(_dataFileName + ".xml");
       
        _startMilliseconds = ofGetElapsedTimeMillis();
        _dataToRecord.addTag("data"); //create root tag
        _dataToRecord.pushTag("data");
       
   }else{
       _dataToRecord.popTag(); //pop data tag
       _dataToRecord.saveFile(_dataFileName + ".xml");
       
       std::ofstream file(_dataFileName + ".xml");
       while(!file){
         //hold until file is saved
         printf("waiting");
       }
       
       std::string py_cmd = "python " + ofToDataPath("stroke_splitter/stroke_splitter.py", true) + " --name " + ofToDataPath(_dataFileName);
       system(py_cmd.c_str());
   }
}



//--------------------------------------------------------------
void ofApp::exit(){
    if(_isRecording){
      toggleRecording();
    }
    
    // let's close down Leap and kill the controller
}