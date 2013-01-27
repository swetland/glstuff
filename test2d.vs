uniform mat4 uMVP;
uniform sampler2D uTexture1;
attribute vec4 aVertex;
attribute vec4 aTexCoord;

varying vec4 vTexCoord;
//varying float vChar;

const float cbw = 32.0;
const float cbh = 32.0;

void main() {
	gl_Position = uMVP * vec4(aVertex.xy,0.0,1.0);
	vTexCoord = aTexCoord;

	//vChar = texture2D(uTexture1, vec2(aTexCoord.z / cbw, aTexCoord.w / cbh)).a 
	//	* 255.0 + 0.001953125;

}

