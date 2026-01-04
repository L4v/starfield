#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in mat4 aInstanceModel;
layout (location = 6) in vec4 aInstanceColor;

out VS_OUT {
  vec4 color;
  vec2 texCoord;
} vs_out;

uniform mat4 projection;
uniform mat4 view;

void main() {
  gl_Position = projection * view * aInstanceModel * vec4(aPos, 1.0);
  vs_out.texCoord = aTexCoord;
  vs_out.color = aInstanceColor;
}
