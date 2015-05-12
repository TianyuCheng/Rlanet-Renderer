#version 330

in vec3 aVertex;

// transformation
uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat4 uNMatrix;
uniform mat4 uTransform;

uniform vec3 uCenter;

out vec2 vUV;
out vec2 vPos;
out vec3 vWorld;

const float PI = 3.1415926;

vec3 hemisphere(float radius, vec2 xz, float y) {
    float theta = xz.x * 2.0 * PI;
    float phi = xz.y * 0.5 * PI;
    vPos = vec2(theta, phi);

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
    vec3 pos = hemisphere(5000.0, aVertex.xz, 0.0);
    vWorld = pos;

    pos.xz += uCenter.xz;
    pos.y -= 1600.0;
    vec4 shared_vert = uTransform * vec4(pos, 1.0);
    gl_Position = uPMatrix * uMVMatrix * shared_vert;
    vUV = aVertex.xz;
}

