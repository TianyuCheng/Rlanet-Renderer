#version 330

in vec3 aVertex;

// Used in the vertex transformation
uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat4 uNMatrix;
uniform mat4 uTransform;

uniform vec2 uOffset;
uniform vec2 uScale;
uniform float uGrid;
uniform int  uLevel;                // for debugging purpose, consider deprecated

uniform vec3 uCamera;
uniform vec2 uRange;                // (range, morphArea)
uniform vec4 uUnderWaterClip;       // plane for under water clipping, xyz is the normal, w is the distance from origin

uniform sampler2D uHeightmap;

// Used in the fragment shader
out vec2 vHeightUV;
out vec2 vDecalTexCoord;
out vec4 vColor;

out vec3 vView;
out vec3 vNormal;

out vec2  vPos;
out float vHeight;
out float linearZ;

const float PI = 3.1415926;
const float SQRT2 = 1.4142;

// Noise functions with fBm
// Source: https://www.shadertoy.com/view/4lB3zz
const int firstOctave = 3;
const int octaves = 8;
const float persistence = 0.9;
const float lacunarity = 2.0;

float noise(int x,int y)
{   
    float fx = float(x);
    float fy = float(y);
    
    return 2.0 * fract(sin(dot(vec2(fx, fy) ,vec2(12.9898,78.233))) * 43758.5453) - 1.0;
}

float smoothNoise(int x,int y)
{
    return noise(x,y)/4.0+(noise(x+1,y)+noise(x-1,y)+noise(x,y+1)+noise(x,y-1))/8.0+(noise(x+1,y+1)+noise(x+1,y-1)+noise(x-1,y+1)+noise(x-1,y-1))/16.0;
}

float COSInterpolation(float x,float y,float n)
{
    float r = n*3.1415926;
    float f = (1.0-cos(r))*0.5;
    return x*(1.0-f)+y*f;
    
}

float InterpolationNoise(vec2 pos)
{
    float x = pos.x;
    float y = pos.y;
    int ix = int(x);
    int iy = int(y);
    float fracx = fract(x);
    float fracy = fract(y);
    
    float v1 = smoothNoise(ix,iy);
    float v2 = smoothNoise(ix+1,iy);
    float v3 = smoothNoise(ix,iy+1);
    float v4 = smoothNoise(ix+1,iy+1);
    
   	float i1 = COSInterpolation(v1,v2,fracx);
    float i2 = COSInterpolation(v3,v4,fracx);
    
    return COSInterpolation(i1,i2,fracy);
    
}

float fbm(vec2 pos)
{
    float sum = 0.0;
    float frequency = pow(lacunarity, float(firstOctave));
    float amplitude = pow(persistence,float(firstOctave));

    for(int i=firstOctave;i<octaves + firstOctave;i++)
    {
        sum += InterpolationNoise(pos * frequency) * amplitude;
        frequency *= lacunarity;
        amplitude *= persistence;
    }
    return sum;
}

// Transform a xz-plane to sphere
vec3 wrap(float radius, vec3 morphedPos) {
    // float s = morphedPos.x;
    // float t = morphedPos.z;
    // // float h = morphedPos.y;
    // float h = 2000;
    //
    // float w = sqrt(s * s + t * t + 1);
    // return vec3(h * s / w, h * t / w, h / w);

    // float rho = morphedPos.y + radius;
    // float theta = PI + morphedPos.x / (2.0 * PI * radius);
    // float phi = PI / 2.0 - morphedPos.z / (PI * radius);
    // float rho = radius; 
    float rho = morphedPos.y + radius;
    float theta = PI + morphedPos.x / (radius);
    float phi = PI / 2.0 - morphedPos.z / (radius);

    /* float theta = morphedPos.x / (radius); */
    /* float phi = morphedPos.z / (radius); */

    return vec3(
            rho * sin(phi) * cos(theta),
            rho * cos(phi),
            rho * sin(phi) * sin(theta)
    );
}

/**
 * uv: the global position
 */
float terrainHeight(vec2 uv) {
    float coarse = texture(uHeightmap, uv).x * 3200.0 - 1600.0;
    /* if (uRange.x > 64.0) */
    /*     return coarse; */
    /* return coarse + fbm(uv); */
    return coarse;
}

/**
 * gridPos: the global position
 */
vec3 computeNormal(vec2 gridPos) {
    float gridSize = 1.0 / uGrid;

    vec2 vx = gridPos - vec2(gridSize, 0.0);
    vec2 vy = gridPos - vec2(0.0, gridSize);

    vec3 px = vec3(uScale * vx + uOffset, 0.0).xzy;
    vec3 py = vec3(uScale * vy + uOffset, 0.0).xzy;
    vec3 p = vec3(uScale * gridPos + uOffset, 0.0).xzy;

    vec2 uvx = px.xz / 16384.0 - vec2(0.5, 0.5);
    vec2 uvy = py.xz / 16384.0 - vec2(0.5, 0.5);
    vec2 uv = p.xz / 16384.0 - vec2(0.5, 0.5);

    px.y = terrainHeight(uvx);
    py.y = terrainHeight(uvy);
    p.y  = terrainHeight(uv);
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

float computeMorphK(vec2 gridPos, vec3 vertex) {
    vec2 uv = vertex.xz / 16384.0 - vec2(0.5, 0.5);
    // vertex.y = terrainHeight(uv);      // approximate
    vec3 camera = uCamera;
    camera.y = 0.0;
    float dist = distance(camera, vertex);
    float morphStart = uRange.y * 2.0;
    float morphEnd = uRange.x * 2.0 * sqrt(2.0);
    // if (dist > morphStart) vColor = vec4(1.0, 0.0, 0.0, 1.0);
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
    vColor = vec4(1.0, 1.0, 1.0, 1.0);
    float radius = 2048.0;

    vec3 pos = vec3(uScale * aVertex.xz + uOffset, 0.0).xzy;
    float morphK = computeMorphK(aVertex.xz, pos);
    // float morphK = 0.0;

    vec3 morphedPos = vec3(morphVertex(aVertex.xz, pos.xz, morphK), 0.0).xzy;
    vec2 uv = morphedPos.xz / 16384.0 - vec2(0.5, 0.5);
    morphedPos.y = terrainHeight(uv);

    vec4 noproj = uMVMatrix * uTransform * vec4(morphedPos, 1.0);
    /* vec4 noproj = uMVMatrix * uTransform * vec4(wrap(radius, morphedPos), 1.0); */
    vec4 proj = uPMatrix * noproj;
    gl_Position = proj;
    linearZ = (-noproj.z-1.0)/(5000.0-1.0);

    // if (uLevel == 0) vColor = vec4(1.0, 0.0, 0.0, 1.0);
    // else if (uLevel == 1) vColor = vec4(1.0, 1.0, 0.0, 1.0);
    // else if (uLevel == 2) vColor = vec4(0.0, 1.0, 1.0, 1.0);
    // else if (uLevel == 3) vColor = vec4(0.0, 0.0, 1.0, 1.0);
    // else if (uLevel == 4) vColor = vec4(1.0, 0.0, 1.0, 1.0);
    // else if (uLevel == 5) vColor = vec4(1.0, 1.0, 0.0, 1.0);
    // else if (uLevel == 6) vColor = vec4(0.0, 1.0, 0.0, 1.0);
    // else if (uLevel == 7) vColor = vec4(0.0, 1.0, 1.0, 1.0);
    // else if (uLevel >= 8) vColor = vec4(1.0, 1.0, 1.0, 1.0);

    vDecalTexCoord = morphedPos.xz / 512.0;

    vView = (uMVMatrix * uTransform * vec4(morphedPos, 1.0)).xyz;
    vNormal = computeNormal(aVertex.xz);
    vHeightUV = uv;
    vPos = morphedPos.xz;
}
