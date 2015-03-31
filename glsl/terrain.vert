#version 130

attribute vec3 aVertex;

// Used in the vertex transformation
uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat4 uNMatrix;
uniform mat4 uTransform;

uniform vec2 uOffset;
uniform vec2 uScale;
uniform int  uLevel;
uniform float uGrid;

uniform sampler2D heightmap;

// Used in the fragment shader
varying vec2 vDecalTexCoord;
varying vec4 vColor;

varying vec3 vView;
varying vec3 vNormal;

const float PI = 3.1415926;

// Transform a xz-plane to sphere
vec3 sphere(float radius, vec2 xz, float y) {
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

/**
 * vertex: normalized coordinate [0,1]
 */
vec3 computeNormal(vec2 gridPos) {
    float gridSize = 1.0 / uGrid;

    vec2 vx = gridPos - vec2(gridSize, 0.0);
    vec2 vy = gridPos - vec2(0.0, gridSize);

    vec3 px = vec3(uScale * vx + uOffset, 0.0).xzy;
    vec3 py = vec3(uScale * vy + uOffset, 0.0).xzy;
    vec3 p = vec3(uScale * gridPos + uOffset, 0.0).xzy;

    vec2 uvx = fract(px.xz / 8192.0) - vec2(0.5, 0.5);
    vec2 uvy = fract(py.xz / 8192.0) - vec2(0.5, 0.5);
    vec2 uv = fract(p.xz / 8192.0) - vec2(0.5, 0.5);

    px.y = texture2D(heightmap, uvx).x * 1400.0 - 700;
    py.y = texture2D(heightmap, uvy).x * 1400.0 - 700;
    p.y = texture2D(heightmap, uv).x * 1400.0 - 700;
    return normalize(cross(p - py, p - px));
}

/**
 * gridPos: normalized coordinate [0,1]
 * vertex: world space position
 * morphK: morph value
 */
vec2 morphVertex(vec2 gridPos, vec2 vertex, float morphK) {
    vec2 fractPart = fract(gridPos * uGrid * 0.5) * 2.0 / uGrid;
    return vertex - fractPart * uScale * morphK;
}

void main()
{
    vec3 pos = vec3(uScale * aVertex.xz + uOffset, 0.0).xzy;
    vec2 uv = fract(pos.xz / 8192.0) - vec2(0.5, 0.5);
    pos.y = texture2D(heightmap, uv).x * 1400.0 - 700;
    vec2 morphedPos = morphVertex(aVertex.xz, pos.xz, 0.3);
    pos.xz = morphedPos;
    gl_Position = uPMatrix * uMVMatrix * uTransform * vec4(pos, 1.0);

    if (uLevel <= 0)      vColor = vec4(1.0, 0.0, 0.0, 1.0);
    else if (uLevel <= 1) vColor = vec4(1.0, 1.0, 0.0, 1.0);
    else if (uLevel <= 2) vColor = vec4(1.0, 0.0, 1.0, 1.0);
    else if (uLevel <= 3) vColor = vec4(0.0, 0.0, 1.0, 1.0);
    else if (uLevel <= 4) vColor = vec4(0.0, 1.0, 1.0, 1.0);
    else if (uLevel <= 4) vColor = vec4(0.0, 0.3, 1.0, 1.0);
    else if (uLevel <= 5) vColor = vec4(0.0, 0.6, 0.4, 1.0);
    else if (uLevel <= 6) vColor = vec4(0.6, 0.0, 0.5, 1.0);
    else if (uLevel <= 7) vColor = vec4(0.0, 0.4, 0.2, 1.0);
    else if (uLevel <= 8) vColor = vec4(0.9, 1.0, 0.4, 1.0);
    else if (uLevel <= 9) vColor = vec4(0.6, 0.6, 1.0, 1.0);
    else vColor = vec4(1.0, 1.0, 1.0, 1.0);

    vColor = vec4(1.0, 1.0, 1.0, 1.0);
    vDecalTexCoord = fract(pos.xz / 1024.0);

    vView = (uMVMatrix * uTransform * vec4(pos, 1.0)).xyz;
    vNormal = computeNormal(aVertex.xz);
}
