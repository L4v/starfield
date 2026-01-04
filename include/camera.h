#include "arena.h"
#include "math3d.h"

typedef struct {
  v3 position;
  v3 forward;
  v3 right;
  v3 up;
  v3 WORLD_UP;

  float yaw;
  float pitch;
  float fov;
} Camera;

void sfUpdateCameraVectors(Camera *camera);
void sfCameraRotatePitch(Camera *camera, float pitchSpeed, float dt);
void sfCameraRotateYaw(Camera *camera, float yawSpeed, float dt);
Camera *sfCameraArenaAlloc(Arena *arena);
m44 sfCameraLookAt(const Camera *camera);
