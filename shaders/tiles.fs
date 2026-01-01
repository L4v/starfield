#version 330


in VS_OUT {
  vec2 texCoord;
  flat int tileIndex;
} fs_in;

out vec4 fragColor;

uniform sampler2D textureId;
uniform float tileSize;
uniform vec2 spriteSheetSize;

void main() {

  float w = spriteSheetSize.x;
  float h = spriteSheetSize.y;

  int cols = int(spriteSheetSize.x) / int(tileSize);
  int rows = int(spriteSheetSize.y) / int(tileSize);

  int col = fs_in.tileIndex % cols;
  int row = rows - 1 - fs_in.tileIndex / cols;

  float dx = tileSize / w;
  float dy = tileSize / h;
  vec2 texCoord = fs_in.texCoord;
  vec2 tileOffset = vec2(col * dx, row * dy);
  vec2 uv = tileOffset + texCoord * vec2(dx, dy);

  fragColor = texture(textureId, uv);
}
