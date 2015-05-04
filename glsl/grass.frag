#version 330

in vec2 vTexCoords;
in float linearZ;

uniform sampler2D uDecalmap;
uniform sampler2D uAlphamap;

out vec4 frag_color;

void main()
{
    float alpha = texture(uAlphamap, vTexCoords).r;
    if (alpha == 0.0) discard;
    vec3 color = texture(uDecalmap, vTexCoords).rgb;
    frag_color = vec4(color, alpha);
    gl_FragDepth = linearZ;
}
