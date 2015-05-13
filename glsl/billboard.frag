#version 330

in vec2 vTexCoords;
in float linearZ;

uniform sampler2D uDecalmap;
uniform vec2 lightPolar;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform float solarRad;

out vec4 frag_color;

void main()
{
	float siny = clamp((sin(lightPolar.y) - 0.31) / (1 - 0.31), 0, 1);
	vec3 diffuse = siny * lightDiffuse;

	vec4 color = texture(uDecalmap, vTexCoords).rgba;
	if (color.a == 0.0)
		discard;
	color = color * vec4(diffuse + lightAmbient, 1.0);
	frag_color = color;
	gl_FragDepth = linearZ;
}
