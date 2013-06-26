#pragma once

#include "ofMain.h"
#include "ofxAutoReloadedShader.h"
#include "ofxRemoteUIServer.h"


class testApp : public ofBaseApp{

	enum showSH{SHOW_ALL, SHOW_N, SHOW_L, SHOW_E, SHOW_R, SHOW_SPECULAR, SHOW_EYE_SPECULAR, NUM_SHOWS};

	ofEasyCam	cam;
	ofxAutoReloadedShader*   mShdPhong;
	ofMaterial	mMatMainMaterial;

	ofVec3f		lightPos;
	ofLight		light;
	
	bool		isShaderDirty;

	ofColor matAmbient;
	ofColor matDiffuse;
	ofColor matSpecular;

	ofColor lightAmbient;
	ofColor lightDiffuse;
	ofColor lightSpecular;

	bool animateLight;
	bool animateCam;

	float lightSpeed;
	float lightDist;
	float timeSpeed;
	showSH showInShader;
	float specularGain, specularClamp, specularPow;
	float eyeSpecularGain, eyeSpecularClamp, eyeSpecularPow;

	//quick temp GLSL varrs
	float shaderVal1;
	float shaderVal2;
	float shaderVal3;

	float lightH;

	public:
		void setup();
		void update();
		void draw();

	void exit();

		void keyPressed  (int key);
		
};
