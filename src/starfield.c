#include "starfield.h"

void sfMovePlayer(Player *player, const v3 *direction) {
  player->velocity =
      v3_add(player->velocity, v3_scale(*direction, player->movementSpeed));
}

void sfUpdatePlayer(Player *player, float dt) {
  player->position = v3_add(player->position, v3_scale(player->velocity, dt));
}

void sfUpdate(Input *input, Camera *camera, Player *player, float dt) {

  v3_zero(&player->velocity);

  if (input->lookUp) {
    camera->pitch += 50 * dt;
  }

  if (input->lookDown) {
    camera->pitch -= 50 * dt;
  }

  if (input->lookLeft) {
    camera->yaw += 50 * dt;
  }

  if (input->lookRight) {
    camera->yaw -= 50 * dt;
  }

  if (input->moveLeft) {
    v3 direction = v3_neg(&camera->right);
    sfMovePlayer(player, &direction);
  }

  if (input->moveRight) {
    sfMovePlayer(player, &camera->right);
  }

  if (input->moveForward) {
    sfMovePlayer(player, &camera->forward);
  }

  if (input->moveBackward) {
    v3 direction = v3_neg(&camera->forward);
    sfMovePlayer(player, &direction);
  }

  if (input->moveUp) {
    sfMovePlayer(player, &camera->WORLD_UP);
  }

  if (input->moveDown) {
    v3 direction = v3_neg(&camera->WORLD_UP);
    sfMovePlayer(player, &direction);
  }

  sfUpdatePlayer(player, dt);
  camera->position = player->position;
  sfUpdateCameraVectors(camera);
}
