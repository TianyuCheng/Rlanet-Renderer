#version 330

in vec3 aVertex;
in vec2 aTexCoord;

// Used in the vertex transformation
uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat4 uNMatrix;
uniform mat4 uTransform;

uniform vec3 uOffset;
uniform vec3 uScale;

// Used in the fragment shader
out vec2 vUV;

void main()
{
    vec3 pos = aVertex * uScale + uOffset;
    gl_Position = uPMatrix * uMVMatrix * uTransform * vec4(pos, 1.0);
    vUV = aTexCoord;
}
