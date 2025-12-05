#version 330


in VS_OUT {
  float particleSize;
  vec3 velocity;
} fs_in;

out vec4 fragColor;
uniform float worldSize;

vec4 getParticleColor(){
    // Define a maximum velocity value
    float maxVelocity = worldSize*1.5;

    // Compute the magnitude of the particle's velocity
    float velocityMagnitude = length(fs_in.velocity);

    // Compute a normalized velocity value between 0 and 1
    float normalizedVelocity = clamp(velocityMagnitude / maxVelocity, 0.0, 1.0);

    // Define three colors for the gradient (e.g. red, orange, yellow)
    vec3 colorLow = vec3(0.0, 0.0, 1.0); // blue (slowest)
    vec3 colorMid = vec3(1.0, 0.5, 0.0); // orange (middle)
    vec3 colorHigh = vec3(1.0, 1.0, 0.0); // yellow (fastest)

    // Interpolate between the three colors based on the normalized velocity value
    float smoothNormalizedVelocity1 = smoothstep(0.0, 0.5, normalizedVelocity);
    float smoothNormalizedVelocity2 = smoothstep(0.5, 1.0, normalizedVelocity);

    vec3 color = mix(colorLow, colorMid, smoothNormalizedVelocity1);
    color = mix(color, colorHigh, smoothNormalizedVelocity2);

    return vec4(color, 1); // pass the velocity to the fragment shader
}

void main() {
   fragColor = getParticleColor();

    vec2 pos = 2.0 * gl_PointCoord.xy - 1.0f;
    if(dot(pos,pos) > 0.25 && fs_in.particleSize > 3.0){
        discard;
    }

}
