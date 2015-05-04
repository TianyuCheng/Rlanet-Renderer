#version 330

in vec2 vTexCoords;

uniform sampler2D uDecalmap;
uniform sampler2D uAlphamap;

out vec4 frag_color;

void main()
{
    vec3 color = texture(uDecalmap, vTexCoords).rgb;
    float alpha = texture(uAlphamap, vTexCoords).r;
    frag_color = vec4(color, alpha);
}
