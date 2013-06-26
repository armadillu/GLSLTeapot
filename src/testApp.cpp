#include "testApp.h"
#include <GLUT/GLUT.h>

//--------------------------------------------------------------
void testApp::setup(){

	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofEnableAlphaBlending();
	
	isShaderDirty = true;

	cam.setDistance(450);
	cam.setFov(30);

	light.enable();
	glEnable(GL_DEPTH_TEST);

	OFX_REMOTEUI_SERVER_SETUP(10000); 	//start server

	OFX_REMOTEUI_SERVER_SHARE_PARAM(shaderVal1,0,2);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(shaderVal2,0,2);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(shaderVal3,0,2);

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(0,255,0,64) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SHARE_PARAM(specularGain,0,2);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(specularClamp,0,1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(specularPow,0,1);

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(255,255,0,64) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SHARE_PARAM(eyeSpecularGain,0,2);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(eyeSpecularClamp,0,1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(eyeSpecularPow,0,1);

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(255,0,0,64) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SHARE_PARAM(timeSpeed, 0, 1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lightSpeed,0,10);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lightH,-100,100);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lightDist,100,600);

	OFX_REMOTEUI_SERVER_SHARE_PARAM(animateLight);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(animateCam);


	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(0,255,255,64) ); // set a bg color for the upcoming params
	vector<string> showL; showL.push_back("SHOW_ALL"); showL.push_back("SHOW_N");
	showL.push_back("SHOW_L"); showL.push_back("SHOW_E"); showL.push_back("SHOW_R");
	showL.push_back("SHOW_SPECULAR");showL.push_back("SHOW_EYE_SPECULAR");
	OFX_REMOTEUI_SERVER_SHARE_ENUM_PARAM(showInShader, SHOW_ALL, NUM_SHOWS-1, showL);

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(255,0,255,64) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(matAmbient);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(matDiffuse);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(matSpecular);
	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_COLOR( ofColor(255,255,255,64) ); // set a bg color for the upcoming params
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(lightAmbient);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(lightDiffuse);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(lightSpecular);

	OFX_REMOTEUI_SERVER_LOAD_FROM_XML();


}

//--------------------------------------------------------------
void testApp::update(){

	OFX_REMOTEUI_SERVER_UPDATE(0.01666);

	if (isShaderDirty){
		
		GLuint err = glGetError();	// we need this to clear out the error buffer.
		
		if (mShdPhong != NULL ) delete mShdPhong;
		mShdPhong = new ofxAutoReloadedShader();
		mShdPhong->setMillisBetweenFileCheck(500);
		mShdPhong->load("shaders/phong");
		err = glGetError();	// we need this to clear out the error buffer.
		ofLogNotice() << "Loaded Shader: " << err;

		isShaderDirty = false;
	}

	mMatMainMaterial.setAmbientColor( matAmbient);
	mMatMainMaterial.setDiffuseColor(matDiffuse);
	mMatMainMaterial.setSpecularColor(matSpecular);
	mMatMainMaterial.setShininess(25.0f);

	light.setAmbientColor(lightAmbient);
	light.setDiffuseColor(lightDiffuse);
	light.setSpecularColor(lightSpecular);



	if(animateLight || ofGetFrameNum() < 2){
		float r = lightDist;
		lightPos = ofVec3f(r * sin(lightSpeed * ofGetElapsedTimef()),
						   lightH + 50,
						   r * cos(lightSpeed * ofGetElapsedTimef()) );
	}
	light.setGlobalPosition(lightPos);

	if(animateCam || ofGetFrameNum() < 2){
		float r = 450;
		ofVec3f camPos = ofVec3f(
								 r * sin(lightSpeed * ofGetElapsedTimef() * 1.2),
								 lightH + 100 + 20 * sin( ofGetElapsedTimef()),
								 r * cos(lightSpeed * ofGetElapsedTimef() * 1.2)
								 );

		cam.setGlobalPosition( camPos );
		cam.setTarget(ofVec3f());
	}

	if (ofGetFrameNum()%15 == 1) ofSetWindowTitle( ofToString( ofGetFrameRate()) );
}

//--------------------------------------------------------------
void testApp::draw(){

	ofBackgroundGradient(ofColor::fromHsb(0, 0, 120), ofColor::fromHsb(0, 0, 0));

	////////////////////////////////////////////////////////////

	//light.enable();
	
	cam.begin();
		ofSetColor(255);
		ofSphere(lightPos.x, lightPos.y, lightPos.z, 3);

		ofEnableLighting();
		glDisable(GL_COLOR_MATERIAL);
		mMatMainMaterial.begin();

		mShdPhong->begin();
		mShdPhong->setUniform1f("shaderVal1", shaderVal1);
		mShdPhong->setUniform1f("shaderVal2", shaderVal2);
		mShdPhong->setUniform1f("shaderVal3", shaderVal3);
		mShdPhong->setUniform1f("time", ofGetElapsedTimef() * timeSpeed);
		mShdPhong->setUniform1f("specularGain", specularGain);
		mShdPhong->setUniform1f("specularClamp", specularClamp);
		mShdPhong->setUniform1f("specularPow", specularPow);
		mShdPhong->setUniform1f("eyeSpecularGain", eyeSpecularGain);
		mShdPhong->setUniform1f("eyeSpecularClamp", eyeSpecularClamp);
		mShdPhong->setUniform1f("eyeSpecularPow", eyeSpecularPow);

		mShdPhong->setUniform1f("showInShader", showInShader);

		glutSolidTeapot(80);

		mShdPhong->end();
	mMatMainMaterial.end();
	ofDisableLighting();

	//cam.end();

	////////////////////////////////////////////////////////////

	//ofSetupScreen();

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}


void testApp::exit(){
	OFX_REMOTEUI_SERVER_CLOSE();
	OFX_REMOTEUI_SERVER_SAVE_TO_XML();
}
