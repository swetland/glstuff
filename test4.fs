uniform sampler2D uTexture;
varying vec2 vTexCoord;

void main() {
	float mask = texture2D(uTexture, vTexCoord).a;

	if (mask < 0.5)
		discard;

	gl_FragColor = vec4(1, 1, 0, 1);
}
