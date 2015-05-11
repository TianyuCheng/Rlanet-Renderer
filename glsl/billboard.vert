#version 330

in vec3 aVertex;

// Used in the vertex transformation
uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;

void main()
{
	gl_Position = vec4(aVertex.xzy, 1.0);
}
