#version 130

attribute vec3 aVertex;

// Used in the vertex transformation
uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat4 uNMatrix;

// Used in the clipmap
uniform vec2 uScale;
uniform vec2 uOffset;
uniform vec2 uMapPos;
uniform vec4 uFineBlockOrig;
uniform sampler2D uHeightmap;

uniform float uHeightScale;
uniform float uLevel;

// Used in the fragment shader
varying vec2 vDecalTexCoord;
varying vec4 vColor;

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

float permute(float x0,vec3 p) { 
  float x1 = mod(x0 * p.y, p.x);
  return floor(  mod( (x1 + p.z) *x0, p.x ));
}
vec2 permute(vec2 x0,vec3 p) { 
  vec2 x1 = mod(x0 * p.y, p.x);
  return floor(  mod( (x1 + p.z) *x0, p.x ));
}
vec3 permute(vec3 x0,vec3 p) { 
  vec3 x1 = mod(x0 * p.y, p.x);
  return floor(  mod( (x1 + p.z) *x0, p.x ));
}
vec4 permute(vec4 x0,vec3 p) { 
  vec4 x1 = mod(x0 * p.y, p.x);
  return floor(  mod( (x1 + p.z) *x0, p.x ));
}

/* uniform vec4 pParam;  */
// Example constant with a 289 element permutation
/* const vec4 pParam = vec4( 17.0*17.0, 34.0, 1.0, 7.0); */
const vec4 pParam = vec4( 17.0*34.0, 68.0, 1.0, 11.0);

float taylorInvSqrt(float r)
{ 
  return ( 0.83666002653408 + 0.7*0.85373472095314 - 0.85373472095314 * r );
}

float simplexNoise2(vec2 v)
{
  const vec2 C = vec2(0.211324865405187134, // (3.0-sqrt(3.0))/6.;
                      0.366025403784438597); // 0.5*(sqrt(3.0)-1.);
  const vec3 D = vec3( 0., 0.5, 2.0) * 3.14159265358979312;
// First corner
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);

// Other corners
  vec2 i1  =  (x0.x > x0.y) ? vec2(1.,0.) : vec2(0.,1.) ;

   //  x0 = x0 - 0. + 0. * C
  vec2 x1 = x0 - i1 + 1. * C.xx ;
  vec2 x2 = x0 - 1. + 2. * C.xx ;

// Permutations
  i = mod(i, pParam.x);
  vec3 p = permute( permute( 
             i.y + vec3(0., i1.y, 1. ), pParam.xyz)
           + i.x + vec3(0., i1.x, 1. ), pParam.xyz);

#ifndef USE_CIRCLE
// ( N points uniformly over a line, mapped onto a diamond.)
  vec3 x = fract(p / pParam.w) ;
  vec3 h = 0.5 - abs(x) ;

  vec3 sx = vec3(lessThan(x,D.xxx)) *2. -1.;
  vec3 sh = vec3(lessThan(h,D.xxx));

  vec3 a0 = x + sx*sh;
  vec2 p0 = vec2(a0.x,h.x);
  vec2 p1 = vec2(a0.y,h.y);
  vec2 p2 = vec2(a0.z,h.z);

#ifdef NORMALISE_GRADIENTS
  p0 *= taylorInvSqrt(dot(p0,p0));
  p1 *= taylorInvSqrt(dot(p1,p1));
  p2 *= taylorInvSqrt(dot(p2,p2));
#endif

  vec3 g = 2.0 * vec3( dot(p0, x0), dot(p1, x1), dot(p2, x2) );
#else 
// N points around a unit circle.
  vec3 phi = D.z * mod(p,pParam.w) /pParam.w ;
  vec4 a0 = sin(phi.xxyy+D.xyxy);
  vec2 a1 = sin(phi.zz  +D.xy);
  vec3 g = vec3( dot(a0.xy, x0), dot(a0.zw, x1), dot(a1.xy, x2) );
#endif
// mix
  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x1,x1), dot(x2,x2)), 0.);
  m = m*m ;
  return 1.66666* 70.*dot(m*m, g);
}

void main()
{
    vec4 pos = vec4(uScale * (uOffset + aVertex.xy), 0.0, 1.0);
    vec2 uv  = (pos.xy + uMapPos - vec2(0.25, 0.25)) * 2.0;
    // float height = texture2D(uHeightmap, uv).x * uHeightScale;
    /* float height = textureLod(uHeightmap, uv, uLevel).x * uHeightScale; */
    float height = simplexNoise2(uv) * uHeightScale;
    pos.z = height;

    gl_Position = uPMatrix * uMVMatrix * pos.xzyw;

    if (uLevel <= 0)      vColor = vec4(0.0, 1.0, 0.0, 1.0);
    else if (uLevel <= 1)      vColor = vec4(1.0, 1.0, 0.0, 1.0);
    else if (uLevel <= 2) vColor = vec4(1.0, 0.0, 1.0, 1.0);
    else if (uLevel <= 3) vColor = vec4(1.0, 0.0, 0.0, 1.0);
    else if (uLevel <= 4) vColor = vec4(0.0, 1.0, 1.0, 1.0);
    /* else vColor = vec4(height / uHeightScale, 1.0 - height / uHeightScale, 0.0, 1.0); */
    else vColor = vec4(1.0, 1.0, 1.0, 1.0);

    vDecalTexCoord = uv;
}
