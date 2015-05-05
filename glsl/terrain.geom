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

/**
 * uv: the global position
 */
float terrainHeight(vec2 uv) {
    float coarse = texture(uHeightmap, uv).x * 3200.0 - 1600.0;
    return coarse;
}

vec3 computeNormal(vec2 gridPos) {
    float gridSize = 1.0 / uGrid;

    vec2 vx = gridPos - vec2(gridSize, 0.0);
    vec2 vy = gridPos - vec2(0.0, gridSize);

    vec3 px = vec3(uScale * vx + uOffset, 0.0).xzy;
    vec3 py = vec3(uScale * vy + uOffset, 0.0).xzy;
    vec3 p = vec3(uScale * gridPos + uOffset, 0.0).xzy;

    vec2 uvx = px.xz / 16384.0 - vec2(0.5, 0.5);
    vec2 uvy = py.xz / 16384.0 - vec2(0.5, 0.5);
    vec2 uv = p.xz / 16384.0 - vec2(0.5, 0.5);

    px.y = terrainHeight(uvx);
    py.y = terrainHeight(uvy);
    p.y  = terrainHeight(uv);
    return normalize(cross(p - py, p - px));
}

void main(void)
{
    vec3 a = gl_in[0].gl_Position.xyz;
    vec3 b = gl_in[1].gl_Position.xyz;
    vec3 c = gl_in[2].gl_Position.xyz;
    vec3 normal = cross((b - a), (c - a));

    // back face culling with geometry shader
    if (dot(normal, a) >= 0.0 && 
        dot(normal, b) >= 0.0 &&    
        dot(normal, c) >= 0.0) {
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

}
