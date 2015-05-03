#version 330

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

void main(void)
{
    int i;

    for (i = 0; i < 3; i++)
    {
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();
}
