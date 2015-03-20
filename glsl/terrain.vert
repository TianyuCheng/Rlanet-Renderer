#version 130

attribute vec3 vertex;

// transformation
uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat4 uNMatrix;

uniform vec4 uScale;
uniform vec4 uOffset;
uniform vec4 uMapPosition;

uniform sampler2D uHeightmap;
uniform float level;

varying vec4 color;

const float PI = 3.1415926;

vec3 sphere(vec2 xz, float y) {
    vec2 uv = xz * 2 * PI;
    float theta = uv.x;
    float phi = uv.y;

    float rho = 5.0 + y;
    return vec3(
            rho * cos(theta) * cos(phi),
            rho * sin(phi),
            rho * sin(theta) * cos(phi)
    );
}

void main()
{
    /* vec3 pos = vertex; */
    /* pos.y = texture2D(uHeightmap, pos.xz).x - 1.0; */
    /* gl_Position = uPMatrix * uMVMatrix * vec4(pos, 1); */

    vec4 pos = uScale * (uOffset + vec4(vertex, 1.0));
    vec4 pos_map = (pos + uMapPosition) * 2.0;
    pos.y = texture2D(uHeightmap, pos_map.xz).x;
    pos.y *= 0.5;
    /* pos.y -= 1.0; */
    /* gl_Position = uPMatrix * uMVMatrix * pos; */

    vec3 spherical = sphere(pos.xz, pos.y);
    gl_Position = uPMatrix * uMVMatrix * vec4(spherical, 1.0);

    color = vec4(level, 1 - level, 0.0, 1.0);
}
