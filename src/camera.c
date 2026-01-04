#include "camera.h"
#include "math3d.h"

void sfCameraRotatePitch(Camera *camera, float pitchSpeed, float dt) {
  camera->pitch += pitchSpeed * dt;
}

void sfCameraRotateYaw(Camera *camera, float yawSpeed, float dt) {
  camera->yaw += yawSpeed * dt;
}

void sfUpdateCameraVectors(Camera *camera) {
  float yaw = camera->yaw * RAD;
  float pitch = camera->pitch * RAD;

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

Camera *sfCameraArenaAlloc(Arena *arena) {
  Camera *camera = (Camera *)sfArenaAlloc(arena, sizeof(Camera));
  camera->position = (v3){0.0f, 2.0f, 4.0f};
  camera->up = (v3){0.0f, 1.0f, 0.0f};

  camera->WORLD_UP = (v3){0.0f, 1.0f, 0.0f};
  camera->pitch = 0.0f;
  camera->yaw = 0.0f;
  camera->fov = 45.0f;
  sfUpdateCameraVectors(camera);
  return camera;
}

m44 sfCameraLookAt(const Camera *camera) {
  v3 center = v3_add(camera->position, camera->forward);
  return lookAt(camera->position, center, camera->up);
}
