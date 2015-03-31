uniform sampler2D uDecalmap;

varying vec2 vDecalTexCoord;
varying vec4 vColor;

varying vec3 vView;
varying vec3 vNormal;

void main()
{
    // gl_FragColor = texture2D(uDecalmap, vDecalTexCoord);
    // gl_FragColor = vColor * texture2D(uDecalmap, vDecalTexCoord);
    // gl_FragColor = vec4(vNormal, 1.0) * texture2D(uDecalmap, vDecalTexCoord);

    // I am going to fake a light here
    // Uniform for light will be implemented later
    vec3 lightPos = vec3(0.0, 5000.0, 0.0);
    vec3 lightAmbient = vec3(0.2, 0.2, 0.2);
    vec3 lightDiffuse = vec3(0.8, 0.8, 0.8);

    vec3 n = normalize(vNormal);
    vec3 v = normalize(-vView);
    vec3 l = normalize(lightPos - vView);

    vec3 decal = texture2D(uDecalmap, vDecalTexCoord).xyz;
    vec3 ambient = lightAmbient;
    vec3 diffuse = lightDiffuse * clamp(0.0, 1.0, max(0.0, dot(n, l)));

    gl_FragColor = vec4((ambient + diffuse) * decal, 1.0);
}
