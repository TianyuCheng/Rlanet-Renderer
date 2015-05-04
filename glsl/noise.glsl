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

