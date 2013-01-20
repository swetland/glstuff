uniform mat4 uMVP;
uniform mat4 uMV;
uniform vec4 uLight;

attribute vec4 aVertex;
attribute vec4 aNormal;
attribute vec2 aTexCoord;

varying vec2 vTexCoord;
varying float vDiffuse;

void main() {
	vec3 mvVertex = vec3(uMV * aVertex);

	// this is cheating, as you should use the inverse transpose of the MV
	// matrix, but if there's no nonuniform scaling going on, it works 
	vec3 mvNormal = vec3(uMV * vec4(aNormal.xyz, 0));

	vec3 lightVector = normalize(vec3(uLight) - mvVertex);
	float lightDistance = length(vec3(uLight) - mvVertex);

	float diffuse = max(dot(mvNormal, lightVector), 0.0);

	vTexCoord = aTexCoord;
	vDiffuse = diffuse;

	gl_Position = uMVP * aVertex;
}

