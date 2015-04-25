#version 330

// Used in the vertex transformation
uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat4 uNMatrix;
uniform mat4 uTransform;

uniform sampler2D uRenderTexture;

uniform float uGrid;

in vec2 vHeightUV;
in vec2 vDecalTexCoord;
in vec4 vColor;

in vec3 vPos;
in vec3 vView;
in vec3 vNormal;
in float linearZ;

out vec4 frag_color;

void main()
{
    // I am going to fake a light here
    // Uniform for light will be implemented later
    vec3 lightPos = vec3(0.0, 5000.0, 0.0);
    vec3 lightAmbient = vec3(0.1, 0.1, 0.1);
    vec3 lightDiffuse = vec3(0.8, 0.8, 0.8);

    vec3 n = normalize(vNormal);
    vec3 v = normalize(-vView);
    vec3 l = normalize(lightPos - vView);

    vec4 clipReflection = uPMatrix * uMVMatrix * vec4(vPos.x, 0.0, vPos.z, 1.0);
    vec2 deviceReflection = clipReflection.st / clipReflection.q;
    vec2 decalReflection = vec2(0.5, 0.5) + 0.5 * deviceReflection;
    vec3 decal = texture(uRenderTexture, decalReflection).rgb;

    vec3 ambient = lightAmbient;
    vec3 diffuse = lightDiffuse * clamp(0.0, 1.0, max(0.0, dot(n, l)));

    vec3 color = (ambient + diffuse) * decal * 0.6;
    frag_color = vec4(color, 1.0);
    gl_FragDepth = linearZ;
}
