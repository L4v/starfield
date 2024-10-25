#include "camera.h"

void sfInitCamera(Camera *camera) {
  camera->position = (v3){0.0f, 0.0f, 4.0f};
  camera->up = (v3){0.0f, 1.0f, 0.0f};

  camera->forward.x = -SIN(camera->yaw * RAD) * COS(camera->pitch * RAD);
  camera->forward.y = SIN(camera->pitch * RAD);
  camera->forward.z = -COS(camera->yaw * RAD) * COS(camera->pitch * RAD);
  camera->forward = v3_norm(camera->forward);
  camera->right = v3_norm(v3_cross(camera->forward, camera->up));
  camera->WORLD_UP = (v3){0.0f, 1.0f, 0.0f};
  camera->pitch = 0.0f;
}

void sfCameraRotatePitch(Camera *camera, float pitch, float dt) {
  camera->pitch += pitch * dt;
}

void sfCameraRotateYaw(Camera *camera, float yaw, float dt) {
  camera->yaw += yaw * dt;
}

void sfUpdateCameraVectors(Camera *camera) {
  float yaw = camera->yaw * RAD;
  float pitch = camera->pitch * RAD;

  if (camera->pitch > 90.0f) {
    camera->pitch = 90.0f;
  }

  if (camera->pitch < -90.0f) {
    camera->pitch = -90.0f;
  }

  camera->pitch = clampf(camera->pitch, -90.0f, 90.0f);

  camera->forward.x = -SIN(yaw) * COS(pitch);
  camera->forward.y = SIN(pitch);
  camera->forward.z = -COS(yaw) * COS(pitch);
  camera->forward = v3_norm(camera->forward);
  camera->right.x = COS(yaw);
  camera->right.y = 0.0f;
  camera->right.z = -SIN(yaw);
  camera->right = v3_norm(camera->right);
  camera->up = v3_norm(v3_cross(camera->right, camera->forward));
}
