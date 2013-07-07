#extension GL_EXT_gpu_shader4 : require

varying vec3 normal, lightDir, eyeVec;

uniform sampler2D tex;
uniform sampler2D normalTex;
uniform sampler2D normalNoiseTex;
uniform sampler2D normalLavaTex;
uniform sampler2D maskLavaTex;

uniform float normalNoiseX;
uniform float normalNoiseY;
uniform float normalNoiseZ;
uniform float time;
uniform float specularGain;
uniform float specularClamp;
uniform float specularPow;
uniform float eyeSpecularGain;
uniform float eyeSpecularClamp;
uniform float eyeSpecularPow;
uniform float normalNoiseGain;
uniform float normalMix;
uniform float diffuseGain;
uniform float diffusePow;


uniform float showInShader;

float pnoise(vec3 P, vec3 rep);
float cnoise(vec3 P);

void main( void ){

	// TEX /////////////////////////////////////
	vec2 texcoord = gl_TexCoord[0].xy;
    vec4 texPix = texture2D(tex, texcoord);  
    vec4 normalTexPix = texture2D(normalTex, texcoord *  vec2(-1.0, 0.6) + vec2(0.3 , 0.25 ) );  
    float goldNoiseFreq = 1.0;
    vec4 normalNoiseTexPix = texture2D(normalNoiseTex, texcoord *  vec2(goldNoiseFreq,  goldNoiseFreq) + vec2(0.0, 0.0 ));  
    vec4 normalLavaTexPix = texture2D(normalLavaTex, texcoord + vec2(0.0, -time )) ;  
    vec4 maskLavaTexPix =  texture2D(maskLavaTex, texcoord + vec2(0.0, -time )) ;  
    
	vec3 ONE = vec3(1.0,1.0,1.0);
	vec3 UP = normalize(vec3(1.0,1.0,1.0));
	vec3 L = normalize(lightDir);
	vec3 E = normalize(eyeVec);
	vec3 H = normalize (L + E);
	vec4 color2 = vec4(0.0, 0.7, 0.0,0.0);

	vec3 N =  (normal) ;
	//float nn = 0.12 * cnoise( 2500.0 * vec3( N.x, N.y, N.z) );
	//N += vec3( nn, nn, nn );

	// NORMAL_NOISE ///////////////////////////
	float NormalNoise = normalNoiseGain * (cnoise( vec3(normalNoiseX * N.x , normalNoiseY * N.y, normalNoiseZ * N.z )));
	vec3 normalTexSum = ( (normalMix) * normalNoiseTexPix.rgb + (1.0 - normalMix) * normalLavaTexPix.rgb);
	N = normalize(  N  + (2.0 * normalTexSum - ONE) );

	vec3 R = (reflect(-L, N));
	


	// AMBIENT /////////////////////////////////
	vec4 final_color =
		(gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient) 
		+
		(gl_LightSource[0].ambient * gl_FrontMaterial.ambient)
		;
	
	
	float lambertTerm = max( dot(N, L), 0.0) ;
	
	// DIFFUSE /////////////////////////////////
	if(lambertTerm > 0.0){
		float shadows = maskLavaTexPix.r;
		vec4 colorMix = color2 * shadows  +  ( 1.0 - shadows ) *  gl_FrontMaterial.diffuse;
		final_color += diffuseGain * gl_LightSource[0].diffuse * colorMix * pow(lambertTerm,diffusePow );
	}


	// SPECULAR /////////////////////////////////
	float specular = specularGain * clamp( pow( max(dot(R,E), 0.0), gl_FrontMaterial.shininess * specularPow ), 0.0, specularClamp);
	vec4 specularV = gl_LightSource[0].specular * gl_FrontMaterial.specular * specular ;	
	
	// EYE SPECULAR ////////////////////////////
	float eyeSpecular = eyeSpecularGain * clamp( pow( max( dot(L,-N) * dot(E,-N), 0.0), eyeSpecularPow * 100.0), 0.0, eyeSpecularClamp);
	vec4 eyeSpecularV = gl_LightSource[0].specular * gl_FrontMaterial.specular * eyeSpecular;

				
	if (showInShader == 0.0){ // SHOW_ALL //////////////////////////////
	
		final_color += specularV;
		//final_color.rgb += (NormalNoise) ;
		//final_color += eyeSpecularV;
		gl_FragColor = final_color;
		
	}else 
	if (showInShader == 1.0){ // SHOW_N //////////////////////////////
	
		gl_FragColor = vec4(N, 1.0);
		
	}else 
	if (showInShader == 2.0){ // SHOW_L //////////////////////////////
	
		gl_FragColor = vec4(L, 1.0);
		
	}else 
	if (showInShader == 3.0){	 // SHOW_E //////////////////////////////

		gl_FragColor = vec4(E, 1.0);
		
	}else 
	if (showInShader == 4.0){	// SHOW_R //////////////////////////////
	
		gl_FragColor = vec4(R, 1.0);
	
	}else
	if (showInShader == 5.0){ // SHOW_SPECULAR //////////////////////////////
	
		final_color += specularV;
		gl_FragColor = final_color;
	
	}else 
	if (showInShader == 6.0){ // SHOW_EYE_SPECULAR //////////////////////////////
	
		final_color += eyeSpecularV;
		gl_FragColor = final_color;
	
	}else	
	if (showInShader == 7.0){ // SHOW_NORMAL_NOISE //////////////////////////////
		gl_FragColor = vec4( normalTexSum.rgb, 1.0);
	}	
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

vec3 mod289(vec3 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x)
{
  return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec3 fade(vec3 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec3 P)
{
  vec3 Pi0 = floor(P); // Integer part for indexing
  vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  vec3 Pf0 = fract(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.yy, Pi1.yy);
  vec4 iz0 = Pi0.zzzz;
  vec4 iz1 = Pi1.zzzz;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 * (1.0 / 7.0);
  vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
  gx0 = fract(gx0);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 * (1.0 / 7.0);
  vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
  gx1 = fract(gx1);
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, vec4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x); 
  return 2.2 * n_xyz;
}

// Classic Perlin noise, periodic variant
float pnoise(vec3 P, vec3 rep)
{
  vec3 Pi0 = mod(floor(P), rep); // Integer part, modulo period
  vec3 Pi1 = mod(Pi0 + vec3(1.0), rep); // Integer part + 1, mod period
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  vec3 Pf0 = fract(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.yy, Pi1.yy);
  vec4 iz0 = Pi0.zzzz;
  vec4 iz1 = Pi1.zzzz;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 * (1.0 / 7.0);
  vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
  gx0 = fract(gx0);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 * (1.0 / 7.0);
  vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
  gx1 = fract(gx1);
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, vec4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x); 
  return 2.2 * n_xyz;
}