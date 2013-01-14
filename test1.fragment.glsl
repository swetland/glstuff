uniform sampler2D uSampler;
varying vec2 fUV;

void main() {
	gl_FragColor = texture2D(uSampler, fUV);
}
