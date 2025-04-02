#include "math3d.h"

typedef struct {
  v3 position;
  v3 forward;
  v3 right;
  v3 up;
  v3 WORLD_UP;

  float yaw;
  float pitch;
} Camera;

void sfInitCamera(Camera *camera);
void sfUpdateCameraVectors(Camera *camera);
void sfCameraRotatePitch(Camera *camera, float pitchSpeed, float dt);
void sfCameraRotateYaw(Camera *camera, float yawSpeed, float dt);
