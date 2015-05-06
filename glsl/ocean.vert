#version 330

in vec3 aVertex;

// Used in the vertex transformation
uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat4 uNMatrix;
uniform mat4 uTransform;

uniform sampler2D uVertexDisplacement;

uniform vec3 uCamera;
uniform vec2 uRange;                // (range, morphArea)

uniform vec2 uOffset;
uniform vec2 uScale;
uniform float uGrid;
uniform float uTime;

// Used in the fragment shader
out vData
{
    vec3 pos;
    vec3 view;
    vec3 normal;
    vec2 heightUV;
    vec2 texCoords;
    float linearZ;
} vertex;

const float PI = 3.1415926;

float displacement(vec2 gridPos) {
    vec2 uv = gridPos * uScale / vec2(250.0, 250.0);
    return texture(uVertexDisplacement, uv).r;
}

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

    amplitude = 40.0; speed = 5.0; waveLength = 200.0; direction = vec2(1.0, 1.0);
    sum += wave(pos, amplitude, speed, waveLength, direction);
    normal += computeNormal(pos, amplitude, speed, waveLength, direction);

    amplitude = 0.5; speed = 0.005; waveLength = 0.003; direction = vec2(0.5, 0.3);
    sum += wave(pos, amplitude, speed, waveLength, direction);
    normal += computeNormal(pos, amplitude, speed, waveLength, direction);

    amplitude = 0.2; speed = 0.001; waveLength = 0.008; direction = vec2(0.2, -0.3);
    sum += wave(pos, amplitude, speed, waveLength, direction);
    normal += computeNormal(pos, amplitude, speed, waveLength, direction);

    amplitude = 0.6; speed = 0.003; waveLength = 0.005; direction = vec2(0.3, 0.7);
    sum += wave(pos, amplitude, speed, waveLength, direction);
    normal += computeNormal(pos, amplitude, speed, waveLength, direction);

    vertex.normal = normal;
    return sum;
    // return sum + displacement(aVertex.xz) * 10.0;
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

float computeMorphK(vec2 gridPos, vec3 vertex) {
    vec2 uv = vertex.xz / 16384.0 - vec2(0.5, 0.5);
    vec3 camera = uCamera;
    float dist = distance(camera, vertex);
    float morphStart = uRange.y * 2.0;
    float morphEnd = uRange.x * 2.0 * sqrt(2.0);
    float morphLerpK = clamp((dist - morphStart) / (morphEnd-morphStart), 0.0, 1.0);

    // Hack for detecting edge
    vec2 scaledGridPos = gridPos * uGrid;
    float x = scaledGridPos.x;
    float y = scaledGridPos.y;
    if (x == uGrid || y == uGrid || x == 0 || y == 0) {
        if (morphLerpK >= 0.5) morphLerpK = 1.0;
        else morphLerpK = 0.0;
    }

    return morphLerpK;
}

void main()
{
    float radius = 2048.0;

    vec3 pos = vec3(uScale * aVertex.xz + uOffset, 0.0).xzy;
    float morphK = computeMorphK(aVertex.xz, pos);
    vec3 morphedPos = vec3(morphVertex(aVertex.xz, pos.xz, morphK), 0.0).xzy;
    vec2 uv = morphedPos.xz / 16384.0 - vec2(0.5, 0.5);
    morphedPos.y = waves(uv);
    vec4 noproj = uMVMatrix * uTransform * vec4(morphedPos, 1.0);
    /* vec4 noproj = uMVMatrix * uTransform * vec4(wrap(radius, pos), 1.0); */
    gl_Position = uPMatrix * noproj;

    vertex.pos = pos;
    vertex.linearZ = (-noproj.z-1.0)/(10000.0-1.0);
    vertex.texCoords = pos.xz / 512.0;
    vertex.view = (uMVMatrix * vec4(morphedPos, 1.0)).xyz;
    vertex.heightUV = uv;
}
