#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

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
            EmitVertex();
        }
        EndPrimitive();
    }

}
