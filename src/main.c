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
// #define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
// #include <cimgui.h>

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

  State *state = (State *)glfwGetWindowUserPointer(window);
  Input *input = state->input;
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
  State *state = (State *)glfwGetWindowUserPointer(window);
  Input *input = state->input;
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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, data);

  free(data);
  return texture;
}

int main() {
  // TODO(Jovan): Better management
  Arena stateArena = sfArenaCreate(MEGABYTE, 1);
  Arena cubesArena = sfArenaCreate(MEGABYTE, 100);
  Arena voxelsArena = sfArenaCreate(MEGABYTE, 100);

  if (!glfwInit()) {
    fprintf(stderr, "Failed to init glfw\n");
    return -1;
  }

  int windowWidth = 1680;
  int windowHeight = 1050;
  GLFWwindow *window = initGlfwWindow(windowWidth, windowHeight);
  State *state = sfStateInit(&stateArena);

  glfwSetWindowUserPointer(window, state);

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  unsigned containerTexture = loadImageAsTexture("res/container.jpg");
  unsigned redDebugTexture = generateColorTexture(64, 64, 255, 0, 0);
  unsigned greenDebugTexture = generateColorTexture(64, 64, 0, 255, 0);
  unsigned blueDebugTexture = generateColorTexture(64, 64, 0, 0, 255);

  // TODO(Jovan): Arena
  Voxels *voxels[MAX_VOXELS];
  unsigned voxelsCount = 0;

  Voxels *testVoxels = sfVoxelsArenaAlloc(&voxelsArena, 1);
  voxels[voxelsCount++] = testVoxels;

  testVoxels->texture = containerTexture;
  testVoxels->transforms[0] = m44_identity(1.0f);

  unsigned starVao, starVbo, starVertexCount, starInstanceCount;
  sfStarInitBuffers(&starVao, &starVbo, &starVertexCount, &starInstanceCount);

  int starProgram = createShaderProgram("shaders/basic.vs", "shaders/basic.fs");
  int voxelProgram =
      createShaderProgram("shaders/voxels.vs", "shaders/voxels.fs");
  int debugProgram =
      createShaderProgram("shaders/voxels.vs", "shaders/debug.fs");

  char windowTitle[128];

  while (!glfwWindowShouldClose(window)) {
    float startTime = glfwGetTime();
    sfInputClearControllers(state->input);
    glfwPollEvents();
    sfUpdate(state);

    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(starProgram);

    setUniformFloat(starProgram, "time", state->time);
    Camera *camera = state->camera;
    v3 center = v3_add(camera->position, camera->forward);
    m44 view = lookAt(camera->position, center, camera->up);
    m44 projection = perspective(camera->fov, windowWidth / (float)windowHeight,
                                 0.1f, 1000.0f);

    setUniformM44(starProgram, "projection", &projection);
    setUniformM44(starProgram, "view", &view);

    // glBindVertexArray(starVao);
    // glDrawArraysInstanced(GL_TRIANGLES, 0, starVertexCount,
    // starInstanceCount);

    glUseProgram(voxelProgram);
    setUniformM44(voxelProgram, "projection", &projection);
    setUniformM44(voxelProgram, "view", &view);

    // Render voxel arena
    for (int i = 0; i < voxelsCount; ++i) {
      sfRenderVoxels(voxels[i]);
    }

    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glfwSwapBuffers(window);

    state->dt = glfwGetTime() - startTime;
    state->time += state->dt;

    float fps = 1.0f;
    if (state->dt > 0) {
      fps = 1.0f / state->dt;
    }
    snprintf(windowTitle, 128, "FPS: %0.1f", fps);

    glfwSetWindowTitle(window, windowTitle);
  }

  sfArenaFree(&voxelsArena);
  sfArenaFree(&cubesArena);
  sfArenaFree(&stateArena);

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
