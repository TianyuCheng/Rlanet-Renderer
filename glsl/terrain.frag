#version 330

uniform sampler2D uHeightmap;
uniform sampler2D uDecalmap0;
uniform sampler2D uDecalmap1;
uniform sampler2D uDecalmap2;
uniform sampler2D uNoisemap;
uniform sampler2D uWaterCaustics;

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

void main()
{
    // I am going to fake a light here
    // Uniform for light will be implemented later
    vec3 lightPos = vec3(0.0, 5000.0, 0.0);
    vec3 lightAmbient = vec3(0.2, 0.2, 0.2);
    vec3 lightDiffuse = vec3(0.8, 0.8, 0.8);

    vec3 n = normalize(frag.normal);
    vec3 v = normalize(-frag.view);
    vec3 l = normalize(lightPos - frag.view);

    float height = texture(uHeightmap, frag.heightUV).x;

    vec3 decal;
    if (height < 0.5) 
        decal = texture(uDecalmap0, frag.texCoords).xyz;
    else if (height < 0.6) 
        decal = mix(
                    texture(uDecalmap0, frag.texCoords).xyz,
                    texture(uDecalmap1, frag.texCoords).xyz,
                    (height - 0.5) * 10.0
                );
    else if (height < 0.8) 
        decal = texture(uDecalmap1, frag.texCoords).xyz;
    else if (height < 0.9) 
        decal = mix(
                    texture(uDecalmap1, frag.texCoords).xyz,
                    texture(uDecalmap2, frag.texCoords).xyz,
                    (height - 0.8) * 10.0
                );
    else 
        decal = texture(uDecalmap2, frag.texCoords).xyz;

    vec3 ambient = lightAmbient;
    vec3 diffuse = lightDiffuse * clamp(0.0, 1.0, max(0.0, dot(n, l)));

    vec3 underWaterEffects = vec3(1.0, 1.0, 1.0);
    if (uCamera.y < 0 && frag.pos.y < 0) 
        underWaterEffects = vec3(0.3, 0.3, 0.3) + 0.7 * texture(uWaterCaustics, frag.pos.xz / 1024.0 + uTime / 16.0).xyz;

    vec3 color = (ambient + diffuse) * decal * underWaterEffects;
    frag_color = vec4(color, 1.0);
    gl_FragDepth = frag.linearZ;
}
