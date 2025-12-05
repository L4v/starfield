#version 330


in VS_OUT {
  vec4 color;
  vec2 texCoord;
} fs_in;

out vec4 fragColor;

uniform sampler2D textureId;

void main() {
  fragColor = texture(textureId, fs_in.texCoord);
}
