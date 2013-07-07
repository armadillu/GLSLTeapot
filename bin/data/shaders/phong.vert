#extension GL_EXT_gpu_shader4 : require

varying vec3 normal, lightDir, eyeVec;

uniform sampler2D tex;
uniform float normalNoiseX;
uniform float normalNoiseY;
uniform float normalNoiseZ;
uniform float time;


void main( void ){
	
	//noise in tex coords
	gl_TexCoord[0] = gl_MultiTexCoord0 ;
	
	normal = gl_NormalMatrix * gl_Normal;
	normal = normalize(gl_ModelViewMatrix * vec4(gl_Normal,0.0)).xyz;
	//normal += 0.05 * noise3( 9.2 * gl_Vertex); //make normals a bit dirty
	
	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	lightDir = vec3(gl_LightSource[0].position.xyz - vVertex);
	eyeVec = -vVertex;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex ;
}