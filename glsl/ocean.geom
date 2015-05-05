#version 330 core

in vec3 aVertex[3];

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform sampler2D uHeightmap;

uniform vec2 uOffset;
uniform vec2 uScale;
uniform float uGrid;

uniform vec3 uCamera;
uniform vec2 uRange;                // (range, morphArea)

in vData
{
    vec3 pos;
    vec3 view;
    vec3 normal;
    vec2 heightUV;
    vec2 texCoords;
    float linearZ;
} vertices[];

out fData
{
    vec3 pos;
    vec3 view;
    vec3 normal;
    vec2 heightUV;
    vec2 texCoords;
    float linearZ;
} frag;

void main(void)
{
    for (int i = 0; i < gl_in.length(); i++)
    {
        gl_Position = gl_in[i].gl_Position;
        frag.pos = vertices[i].pos;
        frag.view = vertices[i].view;
        frag.normal = vertices[i].normal;
        frag.heightUV = vertices[i].heightUV;
        frag.texCoords = vertices[i].texCoords;
        frag.linearZ = vertices[i].linearZ;
        EmitVertex();
    }
    EndPrimitive();
}
