#version 330

uniform sampler2D uDecalmap;

in vec2 vUV;
in vec3 vView;

out vec4 frag_color;

vec3 applySkyline( in vec3  rgb,      // original color of the pixel
               in float dist, // camera to point dist
               in vec3  rayOri,   // camera position
               in vec3  rayDir )  // camera to point vector
{
    float a = 0.02;
    float b = 0.01;
    float c = 0.05;
    float skylineAmount = c*exp(-rayOri.y*b)*(1.0-exp(-dist*rayDir.y*b))/rayDir.y;
    vec3  skylineColor  = vec3(0.5,0.6,0.7);
    return mix( rgb, skylineColor, skylineAmount );
}


void main()
{
    vec3 color = texture(uDecalmap, vUV).rgb;
    color = applySkyline(color, length(vView), vec3(0, 0, 0), normalize(vView));
    frag_color = vec4(color, 1.0);
}
