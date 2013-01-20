
uniform sampler2D uTexture;
varying vec2 vTexCoord;
varying float vDiffuse;

void main() {
	//vec4 c = vec4(1.0, 0.0, 0.0, 1.0);
	vec4 c = texture2D(uTexture, vTexCoord);
	gl_FragColor = c * 0.25 + c * vDiffuse;
}
