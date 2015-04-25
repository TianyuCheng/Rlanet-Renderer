#version 330

in vec3 aVertex;

// Used in the vertex transformation
uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat4 uNMatrix;
uniform mat4 uTransform;

uniform vec2 uOffset;
uniform vec2 uScale;
uniform int  uLevel;
uniform float uGrid;
uniform float uTime;

// Used in the fragment shader
out vec2 vHeightUV;
out vec2 vDecalTexCoord;
out vec4 vColor;

out vec3 vView;
out vec3 vNormal;

out vec3  vPos;
out float vHeight;
out float linearZ;

const float PI = 3.1415926;

float wave(vec2 pos, float amplitude, float speed, float waveLength, vec2 direction) {
    float frequency = 2.0 * PI / waveLength;
    float phase = speed * frequency;
    float k = 3.5;
    return 2.0 * amplitude * pow(0.5 + 0.5 * sin(dot(direction, pos) * frequency + uTime * phase), k);
}

/**
 * gridPos: the global position
 */
vec3 computeNormal(vec2 pos, float amplitude, float speed, float waveLength, vec2 direction) {
    float frequency = 2.0 * PI / waveLength;
    float phase = speed * frequency;
    float k = 3.5;
    float factor = k * amplitude * frequency 
                 * pow(0.5 + 0.5 * sin(dot(direction, pos) * frequency + uTime * phase), k - 1) 
                 * cos(dot(direction, pos) * frequency + uTime * phase);
    float dWdx = direction.x * factor;
    float dWdz = direction.y * factor;
    return vec3(-dWdx, 0.0, -dWdz);
}

float waves(vec2 pos) {
    float sum = 0;
    vec3 normal = vec3(0.0, 1.0, 0.0);
    float amplitude, speed, waveLength;
    vec2 direction;

    amplitude = 20.2; speed = 5.0; waveLength = 100.0; direction = vec2(1.0, 1.0);
    sum += wave(pos, amplitude, speed, waveLength, direction);
    normal += computeNormal(pos, amplitude, speed, waveLength, direction);

    amplitude = 1.5; speed = 0.002; waveLength = 0.005; direction = vec2(0.5, 0.3);
    sum += wave(pos, amplitude, speed, waveLength, direction);
    normal += computeNormal(pos, amplitude, speed, waveLength, direction);

    amplitude = 0.6; speed = 0.003; waveLength = 0.005; direction = vec2(0.3, 0.7);
    sum += wave(pos, amplitude, speed, waveLength, direction);
    normal += computeNormal(pos, amplitude, speed, waveLength, direction);

    vNormal = normal;
    return sum;
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

float computeMorphK(vec3 pos) {
    float dist = length((uPMatrix *uMVMatrix * uTransform * vec4(pos, 1.0)).xyz);
    float morphK = 0.0;
    if (uLevel < 10) {
        float scale = uScale.x;
        float nextScale = scale * 2.0;
        float morphArea = nextScale * 0.85;
        if (dist > morphArea) {
            morphK = clamp((dist - morphArea) / (nextScale * 0.15), 0.0, 1.0);
        }
    }
    return morphK;
}

void main()
{
    float radius = 2048.0;

    vec3 pos = vec3(uScale * aVertex.xz + uOffset, 0.0).xzy;
    float morphK = computeMorphK(pos);
    vec3 morphedPos = vec3(morphVertex(aVertex.xz, pos.xz, morphK), 0.0).xzy;
    vec2 uv = morphedPos.xz / 16384.0 - vec2(0.5, 0.5);
    morphedPos.y = waves(uv);
    vec4 noproj = uMVMatrix * uTransform * vec4(morphedPos, 1.0);
    /* vec4 noproj = uMVMatrix * uTransform * vec4(wrap(radius, pos), 1.0); */
    gl_Position = uPMatrix * noproj;
    linearZ = (-noproj.z-0.01)/(10000.0-0.01);

    /* vColor = vec4((700 + pos.y) / 1400.0, 1.0 - (700 + pos.y) / 1400.0, 0.0, 1.0); */
    vDecalTexCoord = pos.xz / 512.0;

    vView = (uMVMatrix * uTransform * vec4(morphedPos, 1.0)).xyz;
    vHeightUV = uv;
    vPos = pos;
}
