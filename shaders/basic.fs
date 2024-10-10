#version 330

out vec4 fragColor;
uniform float time;

void main() {
  float randValue = sin(dot(gl_FragCoord.xy, vec2(12.9898, 78.233)) + time) * 43758.5453;
  float r = randValue - floor(randValue); // Random red
  float g = sin(r * 6.28318 + time) * 0.5 + 0.5; // Random green
  float b = sin(r * 6.28318 + time + 1.0) * 0.5 + 0.5; // Random blue
  
  vec3 randomColor = vec3(r, g, b); // Combine into a color
  fragColor = vec4(randomColor, 1.0);
}
