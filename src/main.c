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

void initCircularOrbit(Cubes *cubes) {
  float G = 1.0f;
  float radius = 10.0f;

  for (int i = 0; i < cubes->count; ++i) {
    float theta = randf() * 2.0f * PI;
    float phi = (randf() - 0.5f) * PI;
    float r = radius * (0.5f + 0.5f * randf());
    float mass = 1.0f;

    float x = r * COS(phi) * COS(theta);
    float y = r * COS(phi) * SIN(theta);
    float z = r * SIN(phi);
    cubes->positions[i] = (v3){x, y, z};
    cubes->masses[i] = mass;
    cubes->sizes[i] = 0.1f;

    v3 radial = v3_norm((v3){x, y, z});
    v3 axis = fabsf(radial.z) < 0.9f ? (v3){0, 0, 1} : (v3){1, 0, 0};
    v3 tangent = v3_norm(v3_cross(axis, radial));

    float v = sqrtf(G * mass / fmaxf(r, 1e-3f));
    cubes->velocities[i] = v3_scale(tangent, v);
  }
}

int main() {
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
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Arena cubesArena = sfArenaCreate(MEGABYTE, 100);
  Arena voxelsArena = sfArenaCreate(MEGABYTE, 100);

  Voxels *voxels[MAX_VOXELS];
  unsigned voxelsCount = 0;

  unsigned starVao, starVbo, starVertexCount, starInstanceCount;
  sfInitStarBuffers(&starVao, &starVbo, &starVertexCount, &starInstanceCount);

  Cubes *physCubes = sfCubesArenaAlloc(&cubesArena, 1000);
  initCircularOrbit(physCubes);

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
  while (!glfwWindowShouldClose(window)) {
    float startTime = glfwGetTime();
    sfInputClearControllers(input);
    glfwPollEvents();
    sfUpdate(input, &camera, &player, dt);

    // Calculate gravitational forces
    // F = G * (m1 * m2) / r^2
    // r = (p1 - p2)^2
    // for (int i = 0; i < physCubes->count; ++i) {
    //   v3 *position1 = &physCubes->positions[i];
    //   v3 *accel1 = &physCubes->accelerations[i];
    //   float mass1 = physCubes->masses[i];
    //   for (int j = i + 1; j < physCubes->count; ++j) {
    //     v3 *position2 = &physCubes->positions[j];
    //     v3 *accel2 = &physCubes->accelerations[j];
    //     float mass2 = physCubes->masses[j];
    //
    //     v3 r = v3_sub(*position2, *position1);
    //     float len = v3_len(r);
    //     v3 f = v3_scale(r, 1 / (fmaxf(len, 1e-2f) * len));
    //
    //     *accel1 = v3_add(*accel1, v3_scale(f, mass2));
    //     *accel2 = v3_sub(*accel2, v3_scale(f, mass1));
    //   }
    // }

    // Integrate Cube accelerations & velocities
    for (int i = 0; i < physCubes->count; ++i) {
      v3 *acceleration = &physCubes->accelerations[i];
      v3 *velocity = &physCubes->velocities[i];
      v3 *position = &physCubes->positions[i];

      *velocity = v3_add(*velocity, v3_scale(*acceleration, dt));

      *position = v3_add(*position, v3_scale(*velocity, dt));
      v3_zero(acceleration);
    }

    Octant initialOctant =
        sfOctantContaining(physCubes->positions, physCubes->count);
    sfOctreeClear(octree, &initialOctant);

    for (int i = 0; i < physCubes->count; ++i) {
      const v3 position = physCubes->positions[i];
      sfOctreeInsert(octree, position, physCubes->masses[i]);
    }

    printf("Octant count: %d\n", octree->count);
    sfOctreePropagate(octree);
    for (int i = 0; i < physCubes->count; ++i) {
      physCubes->accelerations[i] =
          sfOctreeAcceleration(octree, physCubes->positions[i]);
    }

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

    sfVoxelsFromCubes(cubeVoxels, physCubes);

    // Render voxel arena
    for (int i = 0; i < voxelsCount; ++i) {
      sfRenderVoxels(voxels[i]);
    }

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

    glfwSetWindowTitle(window, windowTitle);
  }

  sfArenaFree(&voxelsArena);
  sfArenaFree(&cubesArena);

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
