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
#include "nuklear.h"
#define G 9.81
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

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
    voxels->transforms[i] = m44_identity(1.0f);

    voxels->transforms[i] = translate(&voxels->transforms[i], position->x,
                                      position->y, position->z);
  }
}

// TODO(Jovan): Use arena
GLuint generateColorTexture(int width, int height, int r, int g, int b) {
    GLuint texture;
    GLubyte *data = (GLubyte *)malloc(width * height * 3);

    for (int i = 0; i < width * height * 3; i += 3) {
        data[i] = r;
        data[i + 1] = g;
        data[i + 2] = b;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    free(data);
    return texture;
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
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  Arena cubesArena = sfArenaCreate(MEGABYTE, 100);
  Arena voxelsArena = sfArenaCreate(MEGABYTE, 100);

  Voxels *voxels[MAX_VOXELS];
  unsigned voxelsCount = 0;

  unsigned starVao, starVbo, starVertexCount, starInstanceCount;
  sfInitStarBuffers(&starVao, &starVbo, &starVertexCount, &starInstanceCount);

  unsigned floorInstanceCount = 4096;

  /* Voxels *floors = sfVoxelsArenaAlloc(&voxelsArena, floorInstanceCount); */
  /* sfVoxelInitFloorInstances(floors); */
  /* voxels[voxelsCount++] = floors; */

  unsigned physCubeCount = 10;
  Cubes *physCubes = sfCubesArenaAlloc(&cubesArena, physCubeCount);

  float cubeAccelerationMag = 16.0f;
  float cubeSpeed = 10.0f;

  for (int i = 0; i < physCubes->count; ++i) {
    physCubes->positions[i] = (v3){2.0f * i, 20.0f, 0.0f};
    physCubes->speeds[i] = i + 1.0f;
  }

  Voxels *cubeVoxels = sfVoxelsArenaAlloc(&voxelsArena, physCubes->count);
  voxels[voxelsCount++] = cubeVoxels;

  v3 eye = {0.0f, 0.0f, 4.0f};
  v3 center = {0.0f, 0.0f, 0.0f};
  v3 up = {0.0f, 1.0f, 0.0f};

  int starProgram = createShaderProgram("shaders/basic.vs", "shaders/basic.fs");
  int voxelProgram =
      createShaderProgram("shaders/voxels.vs", "shaders/voxels.fs");
  int debugProgram = createShaderProgram("shaders/voxels.vs", "shaders/debug.fs");

  Camera camera = {0};
  sfInitCamera(&camera);
  Player player;
  sfPlayerInit(&player);

  float dt = 0.0f;
  double time = 0.0f;

  unsigned containerTexture = loadImageAsTexture("res/container.jpg");
  unsigned redDebugTexture = generateColorTexture(64, 64, 255, 0, 0);
  unsigned greenDebugTexture = generateColorTexture(64, 64, 0, 255, 0);
  unsigned blueDebugTexture = generateColorTexture(64, 64, 0, 0, 255);
  char windowTitle[128];

  float walkingFov = 45.0f;
  float flyingFov = 60.0f;
  float fov = walkingFov;
  float fovAnimDuration = 0.2f;
  char fovAnimDirection = 0;
  char fovAnimTimeStart = time;
  float fovAnimTime = 0;

  v3 X[4] = {
    {1.0f, 2.0f, 5.0f}, {5.0f, 2.0f, 5.0f}, {5.0f, 5.0f, 5.0f}, {1.0f, 5.0f, 5.0f}
  };

  v3 Y[4] = {
    {2.0f, 3.0f, 5.0f}, {7.0f, 3.0f, 5.0f}, {7.0f, 8.0f, 5.0f}, {2.0f, 8.0f, 5.0f}
  };

  v3 minkowskiDiff[16] = {0};
  unsigned minkowskiIdx = 0;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      minkowskiDiff[minkowskiIdx++]= v3_sub(X[i], Y[j]);
      /* printf("____\n"); */
      /* printf("v[%d]\n\t  (%lf, %lf, %lf)\n\t- (%lf, %lf, %lf)", minkowskiIdx, X[i].x, X[i].y, X[i].z, Y[j].x, Y[j].y, Y[j].z); */
      /* printf("\n\t= (%lf, %lf, %lf) | diff\n", diff.x, diff.y, diff.z); */
      /* printf("\n\t= (%lf, %lf, %lf) | minkowskiDiff\n", minkowskiDiff[minkowskiIdx].x, minkowskiDiff[minkowskiIdx].y, minkowskiDiff[minkowskiIdx].z); */
    }
  }

  Voxels* minkowskiVoxels = sfVoxelsArenaAlloc(&voxelsArena, 16);
  for(int i = 0; i < 16; ++i) {
    const v3 *position = &minkowskiDiff[i];
    m44 transform = m44_identity(1.0f);
    transform = translate_v3(&transform, position);
    transform = scale(&transform, 0.2, 0.2, 0.2);
    minkowskiVoxels->transforms[i] = transform;
    minkowskiVoxels->textures[i] = redDebugTexture;

  }

  Keyboard *keyboard = input->keyboard;
  while (!glfwWindowShouldClose(window)) {
    float startTime = glfwGetTime();
    sfInputClearControllers(input);
    glfwPollEvents();
    sfUpdate(input, &camera, &player, dt);

    for (int i = 0; i < physCubes->count; ++i) {
      // v3 *acceleration = &cubes.accelerations[i];
      // v3_zero(acceleration);
      // acceleration->y += -G * dt;

      // v3 *velocity = &cubes.velocities[i];
      // *velocity =
      //     v3_add(*velocity, v3_scale(*acceleration, cubeAccelerationMag *
      //     dt));

      // v3 *position = &cubes.positions[i];
      // *position = v3_add(*position, v3_scale(*velocity, cubes.speeds[i] *
      // dt));

      // if (position->y < 1.5f) {
      //   position->y = 1.5f;
      //   v3_zero(velocity);
      //   v3_zero(acceleration);
      // }
      float angle = fmod(time * (i + 1.0f), 2 * PI);

      physCubes->positions[i].y = SIN(angle) * 5.0f;
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
    // printf("anim direction: %d, animtime: %lf, fov: %lf\n", fovAnimDirection,
    //        fovAnimTime, fov);

    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(starProgram);

    setUniformFloat(starProgram, "time", time);
    v3 center = v3_add(camera.position, camera.forward);
    m44 view = lookAt(camera.position, center, camera.up);
    m44 projection =
        perspective(fov, windowWidth / (float)windowHeight, 0.1f, 1000.0f);

    setUniformM44(starProgram, "projection", &projection);
    setUniformM44(starProgram, "view", &view);

    glBindVertexArray(starVao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, starVertexCount, starInstanceCount);

    glUseProgram(voxelProgram);
    setUniformM44(voxelProgram, "projection", &projection);
    setUniformM44(voxelProgram, "view", &view);
    glBindTexture(GL_TEXTURE_2D, containerTexture);

    /* sfVoxelsFromCubes(cubeVoxels, physCubes); */

    // Render voxel arena
    glBindTexture(GL_TEXTURE_2D, blueDebugTexture);
    sfRenderVoxels(minkowskiVoxels);
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

    // igNewFrame();
    // igBegin("FPS Display", NULL, 0);
    // char buffer[50];
    // snprintf(buffer, sizeof(buffer), "FPS: %.2f", fps);
    // igText(buffer);
    // igEnd();
    // igEndFrame();
    //

    glfwSetWindowTitle(window, windowTitle);
  }

  sfArenaFree(&voxelsArena);
  sfArenaFree(&cubesArena);

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
