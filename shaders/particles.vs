#version 330

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aVelocity;

out VS_OUT {
  float particleSize;
  vec3 velocity;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 cameraPos;
uniform int totalParticles;
uniform float worldSize;

float getParticleSize(){
    float baseSize = 1.0;  
    float distanceFactor = length(cameraPos - aPosition);
    float size = baseSize / distanceFactor;
    
    if(totalParticles < 1000) return max(size * worldSize, 3);
    bool pointSize = false;

    if(pointSize)
        return max(size * worldSize, 1);
    return baseSize;
}

void main() {
  gl_Position = projection * view * vec4(aPosition, 1.0);
  gl_PointSize = getParticleSize();
  vs_out.velocity = aVelocity;
  vs_out.particleSize = gl_PointSize;
}
