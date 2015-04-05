#version 330

in vec3 aVertex;

// transformation
uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat4 uNMatrix;
uniform mat4 uTransform;

const float PI = 3.1415926;

vec3 hemisphere(float radius, vec2 xz, float y) {
    vec2 uv = xz * 2 * PI - PI;
    float theta = uv.x;
    float phi = uv.y;

    float rho = radius + y;
    return vec3(
            rho * cos(theta) * cos(phi),
            rho * sin(phi),
            rho * sin(theta) * cos(phi)
    );
}

void main()
{
    // perform the vertex transformation
    /* vec3 pos = hemisphere(1000.0, aVertex.xz, 0.0); */
    /* gl_Position = uPMatrix * uMVMatrix * uTransform * vec4(pos, 1.0); */

    gl_Position = uPMatrix * uMVMatrix * uTransform * vec4(aVertex, 1.0);
}

