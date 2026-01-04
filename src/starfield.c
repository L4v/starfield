#include "starfield.h"

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

void sfUpdate(State *state, Cubes *cubes) {
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
    float s = cubes->sizes[i] / 2.0f;
    v3 cMin = v3_add(*p, v3_make(-s, -s, -s));
    v3 cMax = v3_add(*p, v3_make(s, s, s));

    v3 invEntry = v3_0();
    v3 invExit = v3_0();

    if (player->velocity.x > 0.0f) {
      invEntry.x = cMin.x - pMax.x;
      invExit.x = cMax.x - pMin.x;
    } else {
      invEntry.x = cMax.x - pMin.x;
      invExit.x = cMin.x - pMax.x;
    }

    if (player->velocity.y > 0.0f) {
      invEntry.y = cMin.y - pMax.y;
      invExit.y = cMax.y - pMin.y;
    } else {
      invEntry.y = cMax.y - pMin.y;
      invExit.y = cMin.y - pMax.y;
    }

    if (player->velocity.z > 0.0f) {
      invEntry.z = cMin.z - pMax.z;
      invExit.z = cMax.z - pMin.z;
    } else {
      invEntry.z = cMax.z - pMin.z;
      invExit.z = cMin.z - pMax.z;
    }

    v3 entry = v3_make(-INFINITY, -INFINITY, -INFINITY);
    v3 exit = v3_make(INFINITY, INFINITY, INFINITY);

    v3 velocity = player->velocity;
    if (velocity.x != 0.0f) {
      entry.x = invEntry.x / velocity.x;
      exit.x = invExit.x / velocity.x;
    }

    if (velocity.y != 0.0f) {
      entry.y = invEntry.y / velocity.y;
      exit.y = invExit.y / velocity.y;
    }

    if (velocity.z != 0.0f) {
      entry.z = invEntry.z / velocity.z;
      exit.z = invExit.z / velocity.z;
    }

    float entryTime = fmaxf(fmaxf(entry.x, entry.y), entry.z);
    float exitTime = fminf(fminf(exit.x, exit.y), exit.z);

    if (entryTime > exitTime || entryTime < 0.0f || entryTime > 1.0f) {
      // no collision
    } else {
      printf("Collision\n");
      cubes->debugCollision[i] = 1;
    }

    // unsigned char xIntersect = pMin.x <= cMax.x && pMax.x > cMin.x;
    // unsigned char yIntersect = pMin.y <= cMax.y && pMax.y > cMin.y;
    // unsigned char zIntersect = pMin.z <= cMax.z && pMax.z > cMin.z;
    // if (xIntersect && yIntersect && zIntersect) {
    //   cubes->debugCollision[i] = 1;
    // }
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
