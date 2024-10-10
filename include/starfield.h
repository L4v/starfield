#include "camera.h"

typedef struct {
  unsigned char moveLeft;
  unsigned char moveRight;
  unsigned char moveForward;
  unsigned char moveBackward;
  unsigned char moveUp;
  unsigned char moveDown;

  unsigned char lookLeft;
  unsigned char lookRight;
  unsigned char lookUp;
  unsigned char lookDown;
} Input;

typedef struct {
  v3 position;
  float movementSpeed;
  v3 velocity;
} Player;

void sfMovePlayer(Player *player, const v3 *direction);
void sfUpdatePlayer(Player *player, float dt);

void sfUpdate(Input *input, Camera *camera, Player *player, float dt);
