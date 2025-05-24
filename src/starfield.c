#include "starfield.h"

void sfMovePlayer(Player *player, const v3 *direction) {
  player->velocity =
      v3_add(player->velocity, v3_scale(*direction, player->movementSpeed));
}

void sfPlayerUpdate(Player *player, float dt) {
  player->position = v3_add(player->position, v3_scale(player->velocity, dt));
  // if (!player->isFlying && player->position.y < 1.0f) {
  //   player->position.y = 1.0f;
  // }
}

void sfUpdate(State *state) {
  Player *player = state->player;
  Input *input = state->input;
  Camera *camera = state->camera;
  float dt = state->dt;

  // TODO(Jovan): Test out just in case. Due to `glfwPollEvents` order
  Keyboard *keyboard = input->keyboard;

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
    sfMovePlayer(player, &direction);
  }

  if (keyboard->moveRight.isDown) {
    sfMovePlayer(player, &camera->right);
  }

  if (keyboard->moveForward.isDown) {
    sfMovePlayer(player, &camera->forward);
  }

  if (keyboard->moveBackward.isDown) {
    v3 direction = v3_neg(&camera->forward);
    sfMovePlayer(player, &direction);
  }

  if (keyboard->moveUp.isDown) {
    sfMovePlayer(player, &camera->WORLD_UP);
  }

  if (keyboard->moveDown.isDown) {
    v3 direction = v3_neg(&camera->WORLD_UP);
    sfMovePlayer(player, &direction);
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

  sfPlayerUpdate(player, dt);

  float walkingFov = 45.0f;
  float flyingFov = 60.0f;
  float fovAnimDuration = 0.2f;
  char fovAnimDirection = 0;
  char fovAnimTimeStart = state->time;
  float fovAnimTime = 0;

  if (keyboard->toggleFly.isDoubleTap) {
    fovAnimDirection = player->isFlying ? 1 : -1;
    fovAnimTimeStart = state->time;
  }

  if (fovAnimDirection) {
    fovAnimTime += dt;
    if (fovAnimTime >= fovAnimDuration) {
      fovAnimDirection = 0;
      fovAnimTime = 0;
    }

    float t = fovAnimTime / fovAnimDuration;
    if (fovAnimDirection > 0) {
      camera->fov = lerp(walkingFov, flyingFov, t);
    } else if (fovAnimDirection < 0) {
      camera->fov = lerp(flyingFov, walkingFov, t);
    }
  }

  camera->position = player->position;
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

Input *sfInputInit(Arena *arena) {
  Input *input = (Input *)sfArenaAlloc(arena, sizeof(Input));
  input->keyboard = sfKeyboardInit(arena);
  input->mouse = sfMouseInit(arena);

  return input;
}

void sfInputClearControllers(Input *input) {
  sfKeyboardClearIsDown(input->keyboard);
  sfMouseClearDeltas(input->mouse);
}

Player *sfPlayerInit(Arena *arena) {
  Player *player = (Player *)sfArenaAlloc(arena, sizeof(Player));
  player->movementSpeed = 6.0f;
  player->position = (v3){0.0f, -0.5f, 10.0f};
  player->isFlying = 0;
  return player;
}

State *sfStateInit(Arena *arena) {
  State *state = (State *)sfArenaAlloc(arena, sizeof(State));
  state->input = sfInputInit(arena);
  state->player = sfPlayerInit(arena);
  state->camera = sfCameraInit(arena);
  state->dt = 0.0f;
  state->time = 0.0;
  return state;
}
