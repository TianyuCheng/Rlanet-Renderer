#version 330

in vec2 vUV;

uniform sampler2D uDecalmap;
uniform sampler2D uAlphamap;

out vec4 frag_color;

void main()
{
    vec3 color = texture(uDecalmap, vUV).rgb;
    float alpha = texture(uAlphamap, vUV).r;
    frag_color = vec4(color, alpha);
}
