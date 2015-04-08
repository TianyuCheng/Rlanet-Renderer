#version 330

uniform sampler2D uDecalmap;

in vec2 vUV;

out vec4 frag_color;

void main()
{
    frag_color = texture(uDecalmap, vUV);
}
