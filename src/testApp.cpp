#include "testApp.h"
#include <GLUT/GLUT.h>

//--------------------------------------------------------------
void testApp::setup(){

	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofEnableAlphaBlending();

	ofDisableArbTex();
	tex.loadImage("tex.png");
	normalTex.loadImage("gatoNormals.png");
	normalTex.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);
	normalNoiseTex.loadImage("normalNoise.png");
	normalNoiseTex.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);
	normalLavaTex.loadImage("lavaNormal.png");
	normalLavaTex.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);
	maskLavaTex.loadImage("lavaMask.png");
	maskLavaTex.getTextureReference().setTextureWrap(GL_REPEAT, GL_REPEAT);


	isShaderDirty = true;

	cam.setDistance(700);
	cam.setFov(30);

	light.enable();
	glEnable(GL_DEPTH_TEST);

	OFX_REMOTEUI_SERVER_SETUP(10000); 	//start server

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("SHADER");
	OFX_REMOTEUI_SERVER_SHARE_PARAM(doShader);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(diffuseGain,0,2);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(diffusePow,0,2.0);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(specularGain,0,2);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(specularClamp,0,1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(specularPow,0,2);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(eyeSpecularGain,0,2);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(eyeSpecularClamp,0,1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(eyeSpecularPow,0,2);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(normalNoiseGain, 0, 2);
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(normalMix,0,1);
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(normalNoiseX,-10,10);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(normalNoiseY,-5,5);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(normalNoiseZ,0,10);



	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	vector<string> showL; showL.push_back("SHOW_ALL"); showL.push_back("SHOW_N");
	showL.push_back("SHOW_L"); showL.push_back("SHOW_E"); showL.push_back("SHOW_R");
	showL.push_back("SHOW_SPECULAR");showL.push_back("SHOW_EYE_SPECULAR");
	showL.push_back("SHOW_NORMAL_NOISE");
	OFX_REMOTEUI_SERVER_SHARE_ENUM_PARAM(showInShader, SHOW_ALL, NUM_SHOWS-1, showL);

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("LIGHT");
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(timeSpeed, 0, 1);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lightSpeed,0,10);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lightH,-100,100);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(lightDist,100,600);

	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(animateLight);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(animateCam);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(drawAxes);



	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("COLORS");
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(matAmbient);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(matDiffuse);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(matSpecular);
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(lightAmbient);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(lightDiffuse);
	OFX_REMOTEUI_SERVER_SHARE_COLOR_PARAM(lightSpecular);

	OFX_REMOTEUI_SERVER_SET_UPCOMING_PARAM_GROUP("BLUR");
	OFX_REMOTEUI_SERVER_SET_NEW_COLOR();
	OFX_REMOTEUI_SERVER_SHARE_PARAM(gpuBlur.blurPasses, 0, 4);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(gpuBlur.blurOffset, 0.0, 10);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(gpuBlur.blurOverlayGain, 0, 255);
	OFX_REMOTEUI_SERVER_SHARE_PARAM(gpuBlur.numBlurOverlays, 0, 7);


	OFX_REMOTEUI_SERVER_LOAD_FROM_XML();

	//cubemap
	myCubeMap.loadImages("cubeMap/cube_posx.png",
						 "cubeMap/cube_negx.png",
						 "cubeMap/cube_posy.png",
						 "cubeMap/cube_negy.png",
						 "cubeMap/cube_posz.png",
						 "cubeMap/cube_negz.png"
						  );
	cubeMapShader.load("shaders/CubeMap");

	// BLUR /////////////////////////////////////////////
	
	ofFbo::Settings s;
	s.width = ofGetWidth();
	s.height = ofGetHeight();
	s.internalformat = GL_RGBA;
	s.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
	s.maxFilter = GL_LINEAR; GL_NEAREST;
	s.numSamples = 8;
	s.numColorbuffers = 3;
	s.useDepth = true;
	s.useStencil = false;

	gpuBlur.setup(s);

}

//--------------------------------------------------------------
void testApp::update(){

	OFX_REMOTEUI_SERVER_UPDATE(0.01666);

	if (isShaderDirty){
		
		GLuint err = glGetError();	// we need this to clear out the error buffer.
		
		if (shader != NULL ) delete shader;
		shader = new ofxAutoReloadedShader();
		shader->setMillisBetweenFileCheck(500);
		shader->load("shaders/phong");
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
		lightPos = ofVec3f(r * (sin(lightSpeed * ofGetElapsedTimef())),
						   lightH + 50,
						   r * (cos(lightSpeed * ofGetElapsedTimef()) ) );
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


	////////////////////////////////////////////////////////////

	//light.enable();

//	gpuBlur.beginDrawScene();
	ofClear(0, 0, 0, 0);

	ofBackgroundGradient(ofColor::fromHsb(0, 0, 120), ofColor::fromHsb(0, 0, 0));
	
	cam.begin();
		ofSetColor(255);
		ofSphere(lightPos.x, lightPos.y, lightPos.z, 3);

//	myCubeMap.bind();
//	cubeMapShader.begin();
//	cubeMapShader.setUniform1i("EnvMap", 0);
//	myCubeMap.drawSkybox( 120 );
//	cubeMapShader.end();
//    myCubeMap.unbind();


	glEnable(GL_DEPTH_TEST);
		ofEnableLighting();
		glDisable(GL_COLOR_MATERIAL);
		mMatMainMaterial.begin();



		if(doShader){
			shader->begin();
			shader->setUniformTexture("tex", tex, tex.getTextureReference().getTextureData().textureID);
			shader->setUniformTexture("normalTex", normalTex, normalTex.getTextureReference().getTextureData().textureID);
			shader->setUniformTexture("normalNoiseTex", normalNoiseTex, normalNoiseTex.getTextureReference().getTextureData().textureID);
			shader->setUniformTexture("normalLavaTex", normalLavaTex, normalLavaTex.getTextureReference().getTextureData().textureID);
			shader->setUniformTexture("maskLavaTex", maskLavaTex, maskLavaTex.getTextureReference().getTextureData().textureID);

			shader->setUniform1f("normalNoiseX", normalNoiseX);
			shader->setUniform1f("normalNoiseY", normalNoiseY);
			shader->setUniform1f("normalNoiseZ", normalNoiseZ);
			shader->setUniform1f("normalNoiseGain", normalNoiseGain);
			shader->setUniform1f("normalMix", normalMix);
			shader->setUniform1f("diffuseGain", diffuseGain);
			shader->setUniform1f("diffusePow", diffusePow);


			shader->setUniform1f("time", ofGetElapsedTimef() * timeSpeed);
			shader->setUniform1f("specularGain", specularGain);
			shader->setUniform1f("specularClamp", specularClamp);
			shader->setUniform1f("specularPow", specularPow);
			shader->setUniform1f("eyeSpecularGain", eyeSpecularGain);
			shader->setUniform1f("eyeSpecularClamp", eyeSpecularClamp);
			shader->setUniform1f("eyeSpecularPow", eyeSpecularPow);
			shader->setUniform1f("showInShader", showInShader);
		}

		glutSolidTeapot(80);


		if(doShader){
			shader->end();
		}
		mMatMainMaterial.end();
		ofDisableLighting();

		ofSetColor(255);
		if(drawAxes)ofDrawAxis(100);

//	gpuBlur.endDrawScene();

	//cam.end();

	glDisable(GL_DEPTH_TEST);


//	ofSetColor(255);
//	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//
//	gpuBlur.performBlur();
//
//	ofClear(0, 0, 0, 0);
//	ofBackgroundGradient(ofColor::fromHsb(0, 0, 120), ofColor::fromHsb(0, 0, 0));
//
//	//draw clean scene
//	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//	gpuBlur.drawSceneFBO();
//	ofEnableBlendMode(OF_BLENDMODE_ADD);
//
//	ofSetColor(255);
//	//overlay the blur
//	gpuBlur.drawBlurFbo();


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
