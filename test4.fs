uniform sampler2D uTexture;
varying vec2 vTexCoord;

void main() {
	float mask = texture2D(uTexture, vTexCoord).a;

	if (mask < 0.471)
		discard;

	float c = smoothstep(mask, 0.47, 0.49);

	gl_FragColor = vec4(c, c, c, 1);
}
