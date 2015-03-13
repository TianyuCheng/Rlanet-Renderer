#version 130

attribute vec2 parametric;
uniform vec2 torusInfo;

const float PI = 3.1415926;

void main()
{
  // calculate the coefficients
  // c is the radius from the center of tube to center of torus
  // a is the radius of the tube
  float c = 0.5 * (torusInfo.x + torusInfo.y);
  float a = 0.5 * (torusInfo.x - torusInfo.y);

  // use theta, phi as the angle of parametric equation
  // be sure to clamp them
  float theta = clamp(parametric.x, 0.0, 1.0) * 2 * PI;
  float phi = clamp(parametric.y, 0.0, 1.0) * 2 * PI;

  /* // first find out the correct transformation of torus */
  vec3 vertex = vec3(
    (c + a * cos(theta)) * cos(phi),
    (c + a * cos(theta)) * sin(phi),
    (a * sin(theta))
  );

  // calculate the uv coordinate
  // tile 2 times in the inner (smaller) radius and 6 times in the outer (larger) radius
  vec2 uv = vec2(
      clamp(parametric.y, 0.0, 1.0) * 6,
      clamp(parametric.x, 0.0, 1.0) * 2
  );
  gl_TexCoord[0].st = uv;

  // perform the vertex transformation
  gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex, 1);
}

