#version 330

in vec3 aVertex;

// Used in the vertex transformation
uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat4 uNMatrix;
uniform mat4 uTransform;

// Used in the fragment shader
out vec2 vUV;

void main()
{
    gl_Position = uPMatrix * uMVMatrix * uTransform * vec4(aVertex, 1.0);
}
