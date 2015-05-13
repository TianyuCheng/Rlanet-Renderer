#version 330

uniform sampler2D uHeightmap;
uniform sampler2D uDecalmap0;
uniform sampler2D uDecalmap1;
uniform sampler2D uDecalmap2;
uniform sampler2D uNoisemap;
uniform sampler2D uWaterCaustics;

uniform vec2 lightPolar;
uniform vec3 lightPos;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;

uniform int  uLevel;                // for debugging purpose, consider deprecated

uniform vec3 uCamera;
uniform float uGrid;
uniform float uTime;

in fData
{
    vec3 pos;
    vec3 view;
    vec3 normal;
    vec2 heightUV;
    vec2 texCoords;
    float linearZ;
} frag;

out vec4 frag_color;

vec3 sampledNoise(vec2 x) {
    return texture(uNoisemap, (x + vec2(0.5, 0.5)/256.0)).rgb;
}

vec3 fbm(vec2 x) {
    vec3 sum = vec3(0.0, 0.0, 0.0);

    float lacunarity = 2.0;
    float persistence = 0.6;
    float frequency = 1.0;
    float amplitude = 1.0;

    sum += sampledNoise(x * frequency) * amplitude; amplitude *= persistence; frequency *= lacunarity;
    sum += sampledNoise(x * frequency) * amplitude; amplitude *= persistence; frequency *= lacunarity;
    sum += sampledNoise(x * frequency) * amplitude; amplitude *= persistence; frequency *= lacunarity;
    return sum;
}

void main()
{
    vec3 n;
    if (distance(frag.pos, uCamera) > 4000.0) 
        n = normalize(frag.normal * texture(uNoisemap, frag.pos.xz / 4096.0).xyz);
    else 
        n = normalize(frag.normal * fbm(frag.pos.xz / 1024.0));
    vec3 v = normalize(-frag.view);
    vec3 l = normalize(lightPos - frag.view);

    float height = texture(uHeightmap, frag.heightUV).x;

    vec3 decal;
    if (height < 0.4) {
        decal = texture(uDecalmap0, frag.texCoords).xyz;
    }
    else if (height < 0.6) {
        float factor2 = clamp((height - 0.4) * 5.0, 0.0, 1.0);
        decal = mix(
                texture(uDecalmap0, frag.texCoords).xyz,
                texture(uDecalmap1, frag.texCoords).xyz,
                clamp(factor2, 0.0, 1.0));
    }
    else if (height < 0.7) 
        decal = mix(
                    texture(uDecalmap1, frag.texCoords).xyz,
                    texture(uDecalmap2, frag.texCoords).xyz,
                    (height - 0.6) * 10.0
                );
    else 
        decal = texture(uDecalmap2, frag.texCoords).xyz;

    vec3 ambient = lightAmbient;
    vec3 diffuse = lightDiffuse * clamp(0.0, 1.0, max(0.0, dot(n, l)));
    float siny = clamp((sin(lightPolar.y) - 0.31) / (1 - 0.31), 0, 1);
    float sinyp = pow(siny, 0.25);
    float sinypc = clamp(sinyp, 0.1, 1.0);
    diffuse = diffuse * sinypc;

    vec3 underWaterEffects = vec3(1.0, 1.0, 1.0);
    if (uCamera.y < 0 && frag.pos.y < 0) 
        underWaterEffects = vec3(0.3, 0.3, 0.3) + 0.7 * texture(uWaterCaustics, frag.pos.xz / 1024.0 + uTime / 16.0).xyz;

    vec3 color = (ambient + diffuse) * decal * underWaterEffects * 1.2;
    frag_color = vec4(color, 1.0);
    gl_FragDepth = frag.linearZ;
}
