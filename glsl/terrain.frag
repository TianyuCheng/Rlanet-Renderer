uniform sampler2D uDecalmap;

varying vec2 vDecalTex;
varying vec4 vColor;

void main()
{
    // gl_FragColor = vColor;
    gl_FragColor = texture2D(uDecalmap, vDecalTex);
}
