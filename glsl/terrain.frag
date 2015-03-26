uniform sampler2D uHeightmap;
uniform sampler2D uDecalmap;

varying vec2 vDecalTexCoord;
varying vec4 vColor;

void main()
{
    /* gl_FragColor = texture2D(uDecalmap, vDecalTexCoord); */
    gl_FragColor = vColor * texture2D(uDecalmap, vDecalTexCoord);
}
