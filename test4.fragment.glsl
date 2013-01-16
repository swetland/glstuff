uniform sampler2D uSampler;
varying vec2 fUV;

void main() {
	float mask = texture2D(uSampler, fUV).a;

	if (mask < 0.5)
		discard;

	gl_FragColor = vec4(1, 1, 0, 1);
}
