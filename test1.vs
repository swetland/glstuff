uniform mat4 uMVP;
attribute vec4 aVertex;
attribute vec2 aTexCoord;
varying vec2 vTexCoord;

void main() {
	gl_Position = uMVP * aVertex;
	vTexCoord = aTexCoord;
}

