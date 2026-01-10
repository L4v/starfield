#ifndef STARFIELD_H
#define STARFIELD_H
#include "arena.h"
#include "camera.h"
#include "cubes.h"
#include "particles.h"

#define sfInit(type, arena) ((type *)sfArenaAlloc((arena), sizeof(type)))

typedef struct {
  unsigned char isDown;
  unsigned char isDoubleTap;
  double lastPressTime;
  double lastDoubleTapTime;
} Key;

typedef struct {
  union {
    Key keys[12];
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

      Key debugStep;
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
  float height;
  unsigned char isFlying;
} Player;

typedef struct {
  int fbw;
  int fbh;
  float dt;
  float time;
  Player *player;
  Camera *camera;
  Input *input;
} State;

void sfPlayerMove(Player *player, const v3 *direction);

void sfUpdate(State *state, Cubes *world, Particles *snow);
// TODO(Jovan): Make macro abstraction of simple inits
Player *sfPlayerArenaAlloc(Arena *arena);
Input *sfInputArenaAlloc(Arena *arena);
void sfInputClearControllers(Input *input);
Keyboard *sfKeyboardInit(Arena *arena);
void sfKeyboardClearIsDown(Keyboard *keyboard);
Mouse *sfMouseInit(Arena *arena);
void sfMouseClearDeltas(Mouse *mouse);

State *sfStateArenaAlloc(Arena *arena);
#endif
