#include "starfield.h"

void sfInitPlayer(Player *player) {
  player->movementSpeed = 6.0f;
  player->position = (v3){0.0f, 1.0f, 10.0f};
  player->isFlying = 0;
}

void sfMovePlayer(Player *player, const v3 *direction) {
  player->velocity =
      v3_add(player->velocity, v3_scale(*direction, player->movementSpeed));
}

void sfUpdatePlayer(Player *player, float dt) {
  player->position = v3_add(player->position, v3_scale(player->velocity, dt));
  if (!player->isFlying && player->position.y < 1.0f) {
    player->position.y = 1.0f;
  }
}

void sfUpdate(Input *input, Camera *camera, Player *player, float dt) {

  v3_zero(&player->velocity);
  float cameraRotateSpeed = 80;

  if (input->lookUp.isDown) {
    sfCameraRotatePitch(camera, 80, dt);
  }

  if (input->lookDown.isDown) {
    sfCameraRotatePitch(camera, -80, dt);
  }

  if (input->lookLeft.isDown) {
    sfCameraRotateYaw(camera, 80, dt);
  }

  if (input->lookRight.isDown) {
    sfCameraRotateYaw(camera, -80, dt);
  }

  if (input->moveLeft.isDown) {
    v3 direction = v3_neg(&camera->right);
    sfMovePlayer(player, &direction);
  }

  if (input->moveRight.isDown) {
    sfMovePlayer(player, &camera->right);
  }

  if (input->moveForward.isDown) {
    sfMovePlayer(player, &camera->forward);
  }

  if (input->moveBackward.isDown) {
    v3 direction = v3_neg(&camera->forward);
    sfMovePlayer(player, &direction);
  }

  if (input->moveUp.isDown) {
    sfMovePlayer(player, &camera->WORLD_UP);
  }

  if (input->moveDown.isDown) {
    v3 direction = v3_neg(&camera->WORLD_UP);
    sfMovePlayer(player, &direction);
  }

  if (input->toggleFly.isDoubleTap) {
    player->isFlying = !player->isFlying;
  }

  if (!player->isFlying) {
    v3 direction = v3_neg(&camera->WORLD_UP);
    sfMovePlayer(player, &direction);
  }

  sfUpdatePlayer(player, dt);
  camera->position = player->position;
  sfUpdateCameraVectors(camera);
}
