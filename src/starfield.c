#include "starfield.h"
#include "math3d.h"
#include <math.h>

Player *sfPlayerArenaAlloc(Arena *arena) {
  Player *player = (Player *)sfArenaAlloc(arena, sizeof(Player));
  player->movementSpeed = 6.0f;
  player->position = (v3){0.0f, 2.5f, 64.0f};
  player->isFlying = 0;
  player->height = 2.0f;

  return player;
}

void sfPlayerMove(Player *player, const v3 *direction) {
  player->velocity =
      v3_add(player->velocity, v3_scale(*direction, player->movementSpeed));
}

void sfUpdate(State *state, Cubes *cubes, Particles *snow) {
  const Input *input = state->input;
  const Keyboard *keyboard = input->keyboard;
  Player *player = state->player;
  Camera *camera = state->camera;
  float dt = state->dt;

  v3_zero(&player->velocity);
  float cameraRotateSpeed = cameraRotateSpeed;

  if (keyboard->lookUp.isDown) {
    sfCameraRotatePitch(camera, cameraRotateSpeed * 10, dt);
  }

  if (keyboard->lookDown.isDown) {
    sfCameraRotatePitch(camera, -cameraRotateSpeed, dt);
  }

  if (keyboard->lookLeft.isDown) {
    sfCameraRotateYaw(camera, cameraRotateSpeed, dt);
  }

  if (keyboard->lookRight.isDown) {
    sfCameraRotateYaw(camera, -cameraRotateSpeed, dt);
  }

  if (keyboard->moveLeft.isDown) {
    v3 direction = v3_neg(&camera->right);
    sfPlayerMove(player, &direction);
  }

  if (keyboard->moveRight.isDown) {
    sfPlayerMove(player, &camera->right);
  }

  if (keyboard->moveForward.isDown) {
    sfPlayerMove(player, &camera->forward);
  }

  if (keyboard->moveBackward.isDown) {
    v3 direction = v3_neg(&camera->forward);
    sfPlayerMove(player, &direction);
  }

  if (keyboard->moveUp.isDown) {
    sfPlayerMove(player, &camera->WORLD_UP);
  }

  if (keyboard->moveDown.isDown) {
    v3 direction = v3_neg(&camera->WORLD_UP);
    sfPlayerMove(player, &direction);
  }

  if (keyboard->toggleFly.isDoubleTap) {
    player->isFlying = !player->isFlying;
  }

  if (!player->isFlying) {
    v3 direction = v3_neg(&camera->WORLD_UP);
    // sfMovePlayer(player, &direction);
  }

  Mouse *mouse = input->mouse;

  if (mouse->dx) {
    sfCameraRotateYaw(camera, mouse->dx * 10.0, dt);
  }

  if (mouse->dy) {
    sfCameraRotatePitch(camera, mouse->dy * 10.0, dt);
  }

  // Snow
  for (int i = 0; i < snow->count; ++i) {
    snow->ttls[i] -= dt;
    float ttl = snow->ttls[i];
    if (ttl <= 0.0f) {
      snow->ttls[i] = randf_clamped(4.0f, 10.0f);
      snow->positions[i] =
          v3_make(randf_clamped(0.0f, 256.0f), randf_clamped(100.0f, 128.0f),
                  randf_clamped(0.0f, 256.0f));
      snow->velocities[i] = v3_make(5.0f, -randf_clamped(0.1f, 10.0f), 0.4f);
      continue;
    }
    snow->positions[i] =
        v3_add(snow->positions[i], v3_scale(snow->velocities[i], dt));
  }

  player->position = v3_add(player->position, v3_scale(player->velocity, dt));
  // if (!player->isFlying && player->position.y < 1.0f) {
  //   player->position.y = 1.0f;
  // }
  camera->position = player->position;
  camera->position.y += player->height;

  v3 pMin = v3_add(player->position, v3_make(-0.5f, 0.0f, -0.5f));
  v3 pMax = v3_add(player->position, v3_make(0.5f, player->height, 0.5f));

  for (int i = 0; i < cubes->count; ++i) {
    v3 *p = &cubes->positions[i];
    float s = cubes->sizes[i] * 0.5f;
    v3 boxMin = v3_add(*p, v3_make(-s, -s, -s));
    v3 boxMax = v3_add(*p, v3_make(s, s, s));
    v3 origin = player->position;
    v3 direction = player->velocity;

    // Sort min and max box bounds
    for (int i = 0; i < 3; ++i) {
      if (boxMin.v[i] > boxMax.v[i]) {
        float t = boxMin.v[i];
        boxMin.v[i] = boxMax.v[i];
        boxMax.v[i] = t;
      }
    }

    v3 t0 = v3_0();
    t0.x =
        direction.x != 0.0f ? (boxMin.x - origin.x) / direction.x : -INFINITY;
    t0.y =
        direction.y != 0.0f ? (boxMin.y - origin.y) / direction.y : -INFINITY;
    float tMin = (t0.x > t0.y) ? t0.x : t0.y;

    v3 t1 = v3_0();
    t1.x = direction.x != 0.0f ? (boxMax.x - origin.x) / direction.x : INFINITY;
    t1.y = direction.y != 0.0f ? (boxMax.y - origin.y) / direction.y : INFINITY;
    float tMax = (t1.x < t1.y) ? t1.x : t1.y;
    if (t0.x > t1.y || t0.y > t1.x) {
      continue; // No collision
    }

    t0.z =
        direction.z != 0.0f ? (boxMin.z - origin.z) / direction.z : -INFINITY;
    t1.z = direction.z != 0.0f ? (boxMax.z - origin.z) / direction.z : INFINITY;
    if (tMin > t1.z || t0.z > tMax) {
      continue; // No collision
    }

    if (t0.z > tMin) {
      tMin = t0.z;
    }

    if (t1.z < tMax) {
      tMax = t1.z;
    }

    printf("Collision\n");
    cubes->debugCollision[i] = 1;
  }

  sfUpdateCameraVectors(camera);
}

Keyboard *sfKeyboardInit(Arena *arena) {
  Keyboard *keyboard = (Keyboard *)sfArenaAlloc(arena, sizeof(Keyboard));
  return keyboard;
}

void sfKeyboardClearIsDown(Keyboard *keyboard) {
  for (int i = 0; i < 11; ++i) {
    keyboard->keys[i].isDoubleTap = 0;
  }
}

Mouse *sfMouseInit(Arena *arena) {
  Mouse *mouse = (Mouse *)sfArenaAlloc(arena, sizeof(Mouse));
  return mouse;
}

void sfMouseClearDeltas(Mouse *mouse) {
  mouse->dx = 0.0;
  mouse->dy = 0.0;
}

Input *sfInputArenaAlloc(Arena *arena) {
  Input *input = (Input *)sfArenaAlloc(arena, sizeof(Input));
  input->keyboard = sfKeyboardInit(arena);
  input->mouse = sfMouseInit(arena);

  return input;
}

void sfInputClearControllers(Input *input) {
  sfKeyboardClearIsDown(input->keyboard);
  sfMouseClearDeltas(input->mouse);
}

State *sfStateArenaAlloc(Arena *arena) {
  State *state = (State *)sfArenaAlloc(arena, sizeof(State));

  state->player = sfPlayerArenaAlloc(arena);
  state->camera = sfCameraArenaAlloc(arena);
  state->input = sfInputArenaAlloc(arena);
  return state;
}
