// WARNING: VIBED HARD
#version 330 core
in vec2 TexCoords;       // Normalized coordinates across your NX x NY quad
out vec4 FragColor;

uniform sampler2D tempTexture; // The single-channel temperature texture

// Function to map normalized temperature (0.0 to 1.0) to a Heat Gradient
vec3 getHeatColor(float t) {
    // Clamp just to be safe from floating-point overshoots
    t = clamp(t, 0.0, 1.0);
    
    // Define key gradient milestones
    vec3 black  = vec3(0.0, 0.0, 0.0);
    vec3 red    = vec3(1.0, 0.0, 0.0);
    vec3 yellow = vec3(1.0, 1.0, 0.0);
    vec3 white  = vec3(1.0, 1.0, 1.0);
    
    // Smoothly blend between milestones based on where 't' falls
    if (t < 0.33) {
        return mix(black, red, t / 0.33);
    } else if (t < 0.66) {
        return mix(red, yellow, (t - 0.33) / 0.33);
    } else {
        return mix(yellow, white, (t - 0.66) / 0.34);
    }
}

void main() {
    // 1. Fetch the raw temperature from the texture (.r or .x channel)
    float rawTemp = texture(tempTexture, TexCoords).r;
    
    // 2. Normalize the temperature from [0.0, 100.0] to [0.0, 1.0]
    float normalizedTemp = rawTemp / 100.0;
    
    // 3. Convert to heat color and output
    FragColor = vec4(getHeatColor(normalizedTemp), 1.0);
}
