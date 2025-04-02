#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in mat4 aInstanceModel;
layout (location = 6) in vec4 aColor;

out VS_OUT {
  vec4 color;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 color;

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  vs_out.color = color;
}
