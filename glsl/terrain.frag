#version 330

uniform sampler2D uHeightmap;
uniform sampler2D uDecalmap0;
uniform sampler2D uDecalmap1;
uniform sampler2D uDecalmap2;

uniform float uGrid;

in vec2 vHeightUV;
in vec2 vDecalTexCoord;
in vec4 vColor;

in vec3 vView;
in vec3 vNormal;

out vec4 frag_color;

void main()
{
    // frag_color = texture(uDecalmap, vDecalTexCoord);
    // frag_color = vColor * texture(uDecalmap, vDecalTexCoord);
    // frag_color = vec4(vNormal, 1.0) * texture(uDecalmap, vDecalTexCoord);

    // I am going to fake a light here
    // Uniform for light will be implemented later
    vec3 lightPos = vec3(0.0, 5000.0, 0.0);
    vec3 lightAmbient = vec3(0.2, 0.2, 0.2);
    vec3 lightDiffuse = vec3(0.8, 0.8, 0.8);

    vec3 n = normalize(vNormal);
    vec3 v = normalize(-vView);
    vec3 l = normalize(lightPos - vView);

    float height = texture(uHeightmap, vHeightUV).x;

    vec3 decal;
    if (height < 0.4) 
        decal = texture(uDecalmap0, vDecalTexCoord).xyz;
    else if (height < 0.5) 
        decal = mix(
                    texture(uDecalmap0, vDecalTexCoord).xyz,
                    texture(uDecalmap1, vDecalTexCoord).xyz,
                    (height - 0.4) * 10.0
                );
    else if (height < 0.7) 
        decal = texture(uDecalmap1, vDecalTexCoord).xyz;
    else if (height < 0.8) 
        decal = mix(
                    texture(uDecalmap1, vDecalTexCoord).xyz,
                    texture(uDecalmap2, vDecalTexCoord).xyz,
                    (height - 0.7) * 10.0
                );
    else 
        decal = texture(uDecalmap2, vDecalTexCoord).xyz;

    vec3 ambient = lightAmbient;
    vec3 diffuse = lightDiffuse * clamp(0.0, 1.0, max(0.0, dot(n, l)));

    frag_color = vec4((ambient + diffuse) * decal, 1.0);
}
