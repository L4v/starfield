#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include "math3d.h"
#include "shader.h"
#include "starfield.h"
#include "stars.h"
#include "voxels.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define NK_IMPLEMENTATION
#include "arena.h"
#include "cubes.h"
#include "octree.h"
#include "particles.h"
#include <time.h>

float randf() { return (float)rand() / RAND_MAX; }

void glfwErrorCallback(int error, const char *description) {
  fprintf(stderr, "GLFW Error: %s\n", description);
}

void processKeyEvent(Key *key, unsigned char isDown) {
  float doubleTapCooldown = 1.0f;
  unsigned char isDoubleTap = 0;
  key->isDown = isDown;
  if (key->isDown) {
    double currentTime = glfwGetTime();

    if (currentTime - key->lastPressTime < 0.5f && !key->isDoubleTap &&
        currentTime - key->lastDoubleTapTime > doubleTapCooldown) {
      isDoubleTap = 1;
      key->lastDoubleTapTime = currentTime;
    }

    key->lastPressTime = currentTime;
  }
  key->isDoubleTap = isDoubleTap;
}

void glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action,
                     int mods) {

  Input *input = (Input *)glfwGetWindowUserPointer(window);
  Keyboard *keyboard = input->keyboard;
  unsigned char isDown = action == GLFW_PRESS;

  if (action != GLFW_REPEAT) {
    switch (key) {
    case GLFW_KEY_ESCAPE:
      if (isDown) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
      }
      break;
    case GLFW_KEY_UP:
      processKeyEvent(&keyboard->lookUp, isDown);
      break;
    case GLFW_KEY_DOWN:
      processKeyEvent(&keyboard->lookDown, isDown);
      break;
    case GLFW_KEY_LEFT:
      processKeyEvent(&keyboard->lookLeft, isDown);
      break;
    case GLFW_KEY_RIGHT:
      processKeyEvent(&keyboard->lookRight, isDown);
      break;
    case GLFW_KEY_W:
      processKeyEvent(&keyboard->moveForward, isDown);
      break;
    case GLFW_KEY_S:
      processKeyEvent(&keyboard->moveBackward, isDown);
      break;
    case GLFW_KEY_A:
      processKeyEvent(&keyboard->moveLeft, isDown);
      break;
    case GLFW_KEY_D:
      processKeyEvent(&keyboard->moveRight, isDown);
      break;
    case GLFW_KEY_SPACE:
      processKeyEvent(&keyboard->moveUp, isDown);
      processKeyEvent(&keyboard->toggleFly, isDown);
      break;
    case GLFW_KEY_LEFT_SHIFT:
      processKeyEvent(&keyboard->moveDown, isDown);
      break;
    case GLFW_KEY_L:
      processKeyEvent(&keyboard->debugStep, isDown);
      break;
    }
  }
}

void glfwCursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
  Input *input = (Input *)glfwGetWindowUserPointer(window);
  Mouse *mouse = input->mouse;

  mouse->dx = mouse->x - xpos;
  mouse->x = xpos;

  mouse->dy = mouse->y - ypos;
  mouse->y = ypos;
}

GLFWwindow *initGlfwWindow(int width, int height) {
  glfwSetErrorCallback(glfwErrorCallback);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow *window = glfwCreateWindow(width, height, "Starfield", NULL, NULL);
  if (!window) {
    fprintf(stderr, "Failed to create window\n");
    exit(-1);
  }
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, glfwKeyCallback);
  glfwSetCursorPosCallback(window, glfwCursorPosCallback);
  glfwSwapInterval(1);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPos(window, 0, 0);
  return window;
}

unsigned loadImageAsTexture(const char *filename) {
  stbi_set_flip_vertically_on_load(1);
  int texWidth, texHeight, texChannels;
  unsigned char *texData =
      stbi_load(filename, &texWidth, &texHeight, &texChannels, 0);
  if (!texData) {
    fprintf(stderr, "ERROR: Failed to load texture\n");
  }

  unsigned texture;
  glGenTextures(1, &texture);

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB,
               GL_UNSIGNED_BYTE, texData);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(texData);
  return texture;
}

void sfVoxelsFromCubes(Voxels *voxels, const Cubes *cubes) {
  for (int i = 0; i < cubes->count; ++i) {
    const v3 *position = &cubes->positions[i];
    const float size = cubes->sizes[i];
    voxels->transforms[i] = m44_identity(1.0f);

    voxels->transforms[i] = translate(&voxels->transforms[i], position->x,
                                      position->y, position->z);
    voxels->transforms[i] = scale(&voxels->transforms[i], size, size, size);
  }
}

// TODO(Jovan): Use arena
GLuint generateColorTexture(int width, int height, int r, int g, int b, int a) {
  GLuint texture;
  GLubyte *data = (GLubyte *)malloc(width * height * 4);

  for (int i = 0; i < width * height * 4; i += 4) {
    data[i] = r;
    data[i + 1] = g;
    data[i + 2] = b;
    data[i + 3] = a;

    int pixelIndex = i / 4;
    int x = pixelIndex % width;
    int y = pixelIndex / width;

    if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
      data[i] = 255;
      data[i + 1] = 255;
      data[i + 2] = 255;
      data[i + 3] = 255;
    } else {
      data[i] = r;
      data[i + 1] = g;
      data[i + 2] = b;
      data[i + 3] = a;
    }
  }

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);

  free(data);
  return texture;
}

v3 v3_rand_clamp(float min, float max) {
  return v3_make(randf_clamped(min, max), randf_clamped(min, max),
                 randf_clamped(min, max));
}

void initCircularOrbit(Cubes *cubes, v3 worldDimensions) {

  float radius = v3_len(worldDimensions) / 3.0f;
  float minLen = 1e-6;
  v3 center = v3_0();
  float worldVolume = worldDimensions.x * worldDimensions.y * worldDimensions.z;
  float baseMass =
      (cubes->count > 0) ? (worldVolume / cubes->count) * 0.5f : 1.0f;
  // float baseSpeed = v3_len(worldDimensions) / .5f;
  float baseSpeed = 7.0f;
  v3 orbitAxis = v3_norm(v3_make(0.2f, 1.0f, -0.1f));

  for (int i = 0; i < cubes->count; ++i) {
    v3 position = v3_rand_clamp(-1.0f, 1.0f);
    float lenSq = v3_dot(position, position);
    while (lenSq < minLen) {
      position = v3_rand_clamp(-1.0f, 1.0f);
      float lenSq = v3_dot(position, position);
    }

    position = v3_norm(position);
    position = v3_scale(position, radius);
    position = v3_add(position, center);

    float mass = randf_clamped(baseMass * 0.7f, baseMass * 1.3f);
    v3 initialVelocity = v3_0();
    float speed = randf_clamped(baseSpeed * 0.5f, baseSpeed * 1.5f);
    v3 positionRelativeToCenter = v3_sub(position, center);
    v3 tangent = v3_cross(orbitAxis, positionRelativeToCenter);
    if (v3_len(tangent) > minLen) {
      initialVelocity = v3_norm(tangent);
      initialVelocity = v3_scale(initialVelocity, -speed);
    } else {
      v3 randDirection = v3_rand_clamp(-1.0f, 1.0f);
      lenSq = v3_dot(randDirection, randDirection);
      while (lenSq < minLen) {
        randDirection = v3_rand_clamp(-1.0f, 1.0f);
        lenSq = v3_dot(randDirection, randDirection);
      }
      initialVelocity = v3_norm(randDirection);
      initialVelocity = v3_scale(randDirection, speed * 0.1f);
    }
    cubes->positions[i] = position;
    cubes->velocities[i] = initialVelocity;
    cubes->masses[i] = mass;
    cubes->sizes[i] = 0.1f;
  }
}

void updatePhysics(Octree *octree, v3 *positions, v3 *velocities,
                   v3 *accelerations, float *masses, unsigned bodyCount,
                   float dt) {
  Octant initialOctant = sfOctantContaining(positions, bodyCount);
  sfOctreeClear(octree, &initialOctant);

  for (int i = 0; i < bodyCount; ++i) {
    const v3 position = positions[i];
    sfOctreeInsert(octree, position, masses[i]);
  }

  sfOctreePropagate(octree);
  for (int i = 0; i < bodyCount; ++i) {
    accelerations[i] = sfOctreeAcceleration(octree, positions[i]);
  }

  // Integrate accelerations & velocities
  for (int i = 0; i < bodyCount; ++i) {
    v3 *acceleration = &accelerations[i];
    v3 *velocity = &velocities[i];
    v3 *position = &positions[i];

    *velocity = v3_add(*velocity, v3_scale(*acceleration, dt));

    *position = v3_add(*position, v3_scale(*velocity, dt));
    v3_zero(acceleration);
  }
}

int main() {
  srand(time(NULL));
  Arena inputArena = sfArenaCreate(MEGABYTE, 1);

  if (!glfwInit()) {
    fprintf(stderr, "Failed to init glfw\n");
    return -1;
  }

  int windowWidth = 1680;
  int windowHeight = 1050;
  GLFWwindow *window = initGlfwWindow(windowWidth, windowHeight);

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  Input *input = sfInputInit(&inputArena);
  glfwSetWindowUserPointer(window, input);
  glfwGetCursorPos(window, &input->mouse->x, &input->mouse->y);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Arena cubesArena = sfArenaCreate(MEGABYTE, 100);
  Arena voxelsArena = sfArenaCreate(MEGABYTE, 100);
  Arena particlesArena = sfArenaCreate(MEGABYTE, 100);

  Voxels *voxels[MAX_VOXELS];
  unsigned voxelsCount = 0;

  unsigned starVao, starVbo, starVertexCount, starInstanceCount;
  sfInitStarBuffers(&starVao, &starVbo, &starVertexCount, &starInstanceCount);

  Cubes *physCubes = sfCubesArenaAlloc(&cubesArena, 10000);

  v3 worldDimensions = v3_make(5.0f, 5.0f, 5.0f);
  initCircularOrbit(physCubes, worldDimensions);

  Particles *particles =
      sfParticlesArenaAlloc(&particlesArena, physCubes->count);

  unsigned containerTexture = loadImageAsTexture("res/container.jpg");
  unsigned redDebugTexture = generateColorTexture(64, 64, 255, 0, 0, 64);
  unsigned greenDebugTexture = generateColorTexture(64, 64, 0, 255, 0, 255);
  unsigned blueDebugTexture = generateColorTexture(64, 64, 0, 0, 255, 255);

  Voxels *cubeVoxels = sfVoxelsArenaAlloc(&voxelsArena, physCubes->count);
  cubeVoxels->texture = containerTexture;
  voxels[voxelsCount++] = cubeVoxels;

  int starProgram = createShaderProgram("shaders/basic.vs", "shaders/basic.fs");
  int voxelProgram =
      createShaderProgram("shaders/voxels.vs", "shaders/voxels.fs");
  int debugProgram =
      createShaderProgram("shaders/voxels.vs", "shaders/debug.fs");
  int particlesProgram =
      createShaderProgram("shaders/particles.vs", "shaders/particles.fs");

  Camera camera = {0};
  sfInitCamera(&camera);
  Player player;
  sfPlayerInit(&player);

  float dt = 0.0f;
  double time = 0.0f;
  char windowTitle[128];

  float walkingFov = 45.0f;
  float flyingFov = 60.0f;
  float fov = walkingFov;
  float fovAnimDuration = 0.2f;
  char fovAnimDirection = 0;
  char fovAnimTimeStart = time;
  float fovAnimTime = 0;

  Arena octreeArena = sfArenaCreate(MEGABYTE, 100);
  Octree *octree =
      sfOctreeArenaAlloc(&octreeArena, 1.0f, 1.0f, 8 * physCubes->count - 1);

  Keyboard *keyboard = input->keyboard;

  unsigned char wasDebugStepDown = 0;
  unsigned char shouldUpdatePhysics = 0;
  unsigned char shouldPausePhysics = 0;
  while (!glfwWindowShouldClose(window)) {
    float startTime = glfwGetTime();

    wasDebugStepDown = keyboard->debugStep.isDown;

    sfInputClearControllers(input);
    glfwPollEvents();

    if (keyboard->debugStep.isDown && !wasDebugStepDown) {
      printf("stepping...\n");
      shouldUpdatePhysics = 1;
    } else {
      shouldUpdatePhysics = 0;
    }

    sfUpdate(input, &camera, &player, dt);

    float physicsTime = glfwGetTime();
    // Calculate gravitational forces
    if (shouldUpdatePhysics || !shouldPausePhysics) {
      updatePhysics(octree, physCubes->positions, physCubes->velocities,
                    physCubes->accelerations, physCubes->masses,
                    physCubes->count, dt);
    }
    physicsTime = glfwGetTime() - physicsTime;

    if (keyboard->toggleFly.isDoubleTap) {
      printf("flying: %d\n", (int)player.isFlying);
      fovAnimDirection = player.isFlying ? 1 : -1;
      fovAnimTimeStart = time;
    }

    if (fovAnimDirection) {
      fovAnimTime += dt;
      if (fovAnimTime >= fovAnimDuration) {
        fovAnimDirection = 0;
        fovAnimTime = 0;
      }

      float t = fovAnimTime / fovAnimDuration;
      if (fovAnimDirection > 0) {
        fov = lerp(walkingFov, flyingFov, t);
      } else if (fovAnimDirection < 0) {
        fov = lerp(flyingFov, walkingFov, t);
      }
    }

    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(starProgram);

    setUniformFloat(starProgram, "time", time);
    v3 cameraCenter = v3_add(camera.position, camera.forward);
    m44 view = lookAt(camera.position, cameraCenter, camera.up);
    m44 projection =
        perspective(fov, windowWidth / (float)windowHeight, 0.1f, 1000.0f);

    setUniformM44(starProgram, "projection", &projection);
    setUniformM44(starProgram, "view", &view);

    // glBindVertexArray(starVao);
    // glDrawArraysInstanced(GL_TRIANGLES, 0, starVertexCount,
    // starInstanceCount);

    glUseProgram(voxelProgram);
    setUniformM44(voxelProgram, "projection", &projection);
    setUniformM44(voxelProgram, "view", &view);

    // sfVoxelsFromCubes(cubeVoxels, physCubes);

    // Render voxel arena
    for (int i = 0; i < voxelsCount; ++i) {
      sfRenderVoxels(voxels[i]);
    }

    for (int i = 0; i < particles->count; ++i) {
      particles->positions[i] = physCubes->positions[i];
      particles->velocities[i] = physCubes->velocities[i];
    }
    glUseProgram(particlesProgram);
    setUniformM44(particlesProgram, "projection", &projection);
    setUniformM44(particlesProgram, "view", &view);
    setUniformF3(particlesProgram, "cameraPos", camera.position.x,
                 camera.position.y, camera.position.z);
    setUniformI1(particlesProgram, "totalParticles", particles->count);
    setUniformFloat(particlesProgram, "worldSize", v3_len(worldDimensions));

    sfParticlesRender(particles);

    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glfwSwapBuffers(window);

    dt = glfwGetTime() - startTime;
    time += dt;

    float fps = 1.0f;
    if (dt > 0) {
      fps = 1.0f / dt;
    }
    snprintf(windowTitle, 128, "FPS: %0.1f", fps);
    printf("Frame Time: (dt): %fms | Physics Time: %fms\n", dt * 1000.0f,
           physicsTime * 1000.0f);

    glfwSetWindowTitle(window, windowTitle);
  }

  sfArenaFree(&voxelsArena);
  sfArenaFree(&cubesArena);

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
