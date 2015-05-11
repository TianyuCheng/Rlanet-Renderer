#version 330

in vec2 vTexCoords;
in float linearZ;

uniform sampler2D uDecalmap;
uniform sampler2D uAlphamap;
uniform vec2 lightPolar;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform float solarRad;

out vec4 frag_color;

void main()
{
	float siny = (sin(lightPolar.y) + 0.1) / 1.1;
	vec3 diffuse = siny * lightDiffuse;

	float alpha = texture(uAlphamap, vTexCoords).r;
	if (alpha == 0.0) discard;
	vec3 color = texture(uDecalmap, vTexCoords).rgb;
	color = color * (diffuse + lightAmbient);
	frag_color = vec4(color, alpha);
	gl_FragDepth = linearZ;
}
