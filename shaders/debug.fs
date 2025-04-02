#version 330


in VS_OUT {
  vec4 color;
  vec2 texCoord;
} fs_in;



uniform vec4 color;
out vec4 fragColor;

void main() {
  fragColor = color;

}
