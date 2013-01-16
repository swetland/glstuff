uniform mat4 uMVP;
attribute vec4 vPosition;
attribute vec2 vUV;
varying vec2 fUV;

void main() {
	gl_Position = uMVP * vPosition;
	fUV = vUV;
}

