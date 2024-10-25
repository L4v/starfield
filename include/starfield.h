#include "camera.h"

typedef struct {
  unsigned char isDown;
  unsigned char isDoubleTap;
  double lastPressTime;
  double lastDoubleTapTime;
} Key;

typedef struct {
  union {
    Key keys[11];
    struct {
      Key moveLeft;
      Key moveRight;
      Key moveForward;
      Key moveBackward;
      Key moveUp;
      Key moveDown;

      Key lookLeft;
      Key lookRight;
      Key lookUp;
      Key lookDown;

      Key toggleFly;
    };
  };
} Input;

typedef struct {
  v3 position;
  float movementSpeed;
  v3 velocity;
  unsigned char isFlying;
} Player;

void sfInitPlayer(Player *player);

void sfMovePlayer(Player *player, const v3 *direction);
void sfUpdatePlayer(Player *player, float dt);

void sfUpdate(Input *input, Camera *camera, Player *player, float dt);
