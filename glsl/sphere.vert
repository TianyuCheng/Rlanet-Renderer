#version 130

attribute vec3 aVertex;

// transformation
uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat4 uNMatrix;

const float PI = 3.1415926;

void main()
{
    // perform the vertex transformation
    gl_Position = uPMatrix * uMVMatrix * vec4(aVertex, 1);
}

