#pragma once

#include "ofMain.h"
#include "ofxAutoReloadedShader.h"
#include "ofxRemoteUIServer.h"
#include "ofxCubeMap.h"
#include "ofxFboBlur.h"

class testApp : public ofBaseApp{

	enum showSH{SHOW_ALL, SHOW_N, SHOW_L, SHOW_E, SHOW_R, SHOW_SPECULAR, SHOW_EYE_SPECULAR, SHOW_NORMAL_NOISE, NUM_SHOWS};

	ofEasyCam	cam;
	ofxAutoReloadedShader*   shader;
	ofMaterial	mMatMainMaterial;

	ofxFboBlur gpuBlur;
	
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
	bool doShader;
	bool drawAxes;

	float lightSpeed;
	float lightDist;
	float timeSpeed;
	showSH showInShader;

	float diffuseGain, diffusePow;
	float specularGain, specularClamp, specularPow;
	float eyeSpecularGain, eyeSpecularClamp, eyeSpecularPow;

	//quick temp GLSL varrs
	float normalNoiseX;
	float normalNoiseY;
	float normalNoiseZ;
	float normalNoiseGain;
	float normalMix;

	float lightH;

	ofxCubeMap myCubeMap;
	ofShader cubeMapShader;

	ofImage tex;
	ofImage normalTex;
	ofImage normalNoiseTex;
	ofImage normalLavaTex;
	ofImage maskLavaTex;
	
	public:
		void setup();
		void update();
		void draw();

	void exit();

		void keyPressed  (int key);
		
};
