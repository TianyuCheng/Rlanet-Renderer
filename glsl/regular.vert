#version 330

in vec3 aVertex;

// transformation
uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat4 uNMatrix;
uniform mat4 uTransform;

const float PI = 3.1415926;

void main()
{
    // perform the vertex transformation
    gl_Position = uPMatrix * uMVMatrix * uTransform * vec4(aVertex, 1);
}

