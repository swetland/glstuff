uniform sampler2D uTexture0;
uniform sampler2D uTexture1;

varying vec4 vTexCoord; // u, v, cx, cy

//varying float vChar;

// width and height of the "character buffer" 
const float cbw = 32.0;
const float cbh = 32.0;

// cell count (h & v) for character map
const float cc = 16.0;

void main() {
	vec2 base, offset;
	float ch;

	// look up characer in cbw x cbh character buffer texture
	ch = texture2D(uTexture1, vec2(vTexCoord.z / cbw, vTexCoord.w / cbh)).a
		* 255.0 + 0.001953125;
	//ch = vChar;

	// base texcoord of ch (0..255) in cc x cc character map texture
	base = vec2(fract(ch / cc), -floor(ch / cc) / cc); 

	// scale offset texcoord
	offset = vTexCoord.xy / cc;

	gl_FragColor = texture2D(uTexture0, base + offset * vec2(1.0,-1.0));
}

