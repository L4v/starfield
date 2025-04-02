#include "arena.h"
#include "camera.h"

#define sfInit(type, arena) ((type *)sfArenaAlloc((arena), sizeof(type)))

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
} Keyboard;

typedef struct {
  double x;
  double dx;

  double y;
  double dy;
} Mouse;

typedef struct {
  Keyboard *keyboard;
  Mouse *mouse;
} Input;

typedef struct {
  v3 position;
  float movementSpeed;
  v3 velocity;
  unsigned char isFlying;
} Player;

void sfMovePlayer(Player *player, const v3 *direction);
void sfUpdatePlayer(Player *player, float dt);

void sfUpdate(Input *input, Camera *camera, Player *player, float dt);
// TODO(Jovan): Make macro abstraction of simple inits
void sfPlayerInit(Player *player);
Input *sfInputInit(Arena *arena);
void sfInputClearControllers(Input *input);
Keyboard *sfKeyboardInit(Arena *arena);
void sfKeyboardClearIsDown(Keyboard *keyboard);
Mouse *sfMouseInit(Arena *arena);
void sfMouseClearDeltas(Mouse *mouse);
