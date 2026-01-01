#include "arena.h"
#include "math3d.h"
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "shader.h"
#include "starfield.h"
#include "stb_image.h"

void glfwFramebufferSizeCallback(GLFWwindow *window, int width, int height) {
  State *state = (State *)glfwGetWindowUserPointer(window);
  state->fbw = width;
  state->fbh = height;

  glViewport(0, 0, width, height);
}

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
    case GLFW_KEY_L:
      processKeyEvent(&keyboard->debugStep, isDown);
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

typedef struct {
  unsigned id;
  int width;
  int height;
  int channels;
} Texture;

Texture loadImageAsTexture(const char *filename, GLint minFilter,
                           GLint magFilter) {
  stbi_set_flip_vertically_on_load(1);
  Texture texture = {0};
  unsigned char *texData = stbi_load(filename, &texture.width, &texture.height,
                                     &texture.channels, 0);
  if (!texData) {
    fprintf(stderr, "ERROR: Failed to load texture\n");
  }

  GLint format = GL_RGB;
  if (texture.channels > 3) {
    format = GL_RGBA;
  }

  glGenTextures(1, &texture.id);

  glBindTexture(GL_TEXTURE_2D, texture.id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
  glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0,
               format, GL_UNSIGNED_BYTE, texData);

  switch (minFilter) {
  case GL_NEAREST_MIPMAP_NEAREST:
  case GL_NEAREST_MIPMAP_LINEAR:
  case GL_LINEAR_MIPMAP_NEAREST:
  case GL_LINEAR_MIPMAP_LINEAR:
    glGenerateMipmap(GL_TEXTURE_2D);
    break;
  }

  stbi_image_free(texData);
  return texture;
}

int main() {
  if (!glfwInit()) {
    fprintf(stderr, "Failed to init glfw\n");
    return -1;
  }

  int windowWidth = 1680;
  int windowHeight = 1050;
  GLFWwindow *window = initGlfwWindow(windowWidth, windowHeight);

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  Arena stateArena = sfArenaCreate(MEGABYTE, 1);
  State *state = sfStateArenaAlloc(&stateArena);
  Input *input = state->input;
  glfwSetWindowUserPointer(window, state);
  glfwGetCursorPos(window, &input->mouse->x, &input->mouse->y);
  glfwGetFramebufferSize(window, &state->fbw, &state->fbh);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  int spritesheetProgram =
      createShaderProgram("shaders/tiles.vs", "shaders/tiles.fs");
  Texture spritesheetTexture =
      loadImageAsTexture("res/spritesheet.png", GL_NEAREST, GL_NEAREST);

  unsigned quadVao;
  unsigned quadVbos[3];
  unsigned tileSize = 100;
  unsigned tileMapCols = 4;
  unsigned tileMapRows = 4;
  unsigned tileCount = tileMapCols * tileMapRows;

  float quadVertices[6 * 5] = {
      // x      y     z     u     v
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // LD
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // RD
      -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, // LU
      -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, // LU
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // RD
      0.5f,  0.5f,  0.0f, 1.0f, 1.0f  // RU
  };

  Arena tileArena = sfArenaCreate(MEGABYTE, 10);
  m44 *tileTransforms =
      (m44 *)sfArenaAlloc(&tileArena, sizeof(m44) * tileCount);

  int tileIds[] = {
      1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1,

  };

  for (int i = 0; i < tileCount; ++i) {
    unsigned col = i % tileMapCols;
    unsigned row = i / tileMapCols;
    m44 *transform = &tileTransforms[i];
    *transform = m44_identity(1.0f);
    *transform = translate(transform, col * tileSize + tileSize / 2.0f,
                           row * tileSize + tileSize / 2.0f, 1.0f);
    *transform = scale(transform, tileSize, tileSize, 1.0f);
  }

  glGenVertexArrays(1, &quadVao);
  glGenBuffers(3, quadVbos);
  glBindVertexArray(quadVao);
  glBindBuffer(GL_ARRAY_BUFFER, quadVbos[0]);

  glBufferData(GL_ARRAY_BUFFER, 5 * 6 * sizeof(float), quadVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));

  glBindBuffer(GL_ARRAY_BUFFER, quadVbos[1]);
  glBufferData(GL_ARRAY_BUFFER, tileCount * sizeof(int), tileIds,
               GL_DYNAMIC_DRAW);
  glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, sizeof(int), (void *)(0));
  glEnableVertexAttribArray(2);
  glVertexAttribDivisor(2, 1);

  glBindBuffer(GL_ARRAY_BUFFER, quadVbos[2]);
  glBufferData(GL_ARRAY_BUFFER, tileCount * sizeof(m44), tileTransforms,
               GL_DYNAMIC_DRAW);
  for (unsigned i = 0; i < 4; ++i) {
    glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(m44),
                          (void *)(sizeof(v4) * i));
    glEnableVertexAttribArray(3 + i);
    glVertexAttribDivisor(3 + i, 1);
  }

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  Camera camera = {0};
  sfInitCamera(&camera);
  Player player;
  sfPlayerInit(&player);

  float dt = 0.0f;
  float time = 0.0f;
  float fov = 45.0f;
  char windowTitle[128];
  while (!glfwWindowShouldClose(window)) {
    float startTime = glfwGetTime();

    sfInputClearControllers(input);
    glfwPollEvents();

    sfUpdate(input, &camera, &player, dt);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(spritesheetProgram);

    v3 cameraCenter = v3_add(camera.position, camera.forward);
    m44 view = lookAt(v3_make(0.0f, 0.0f, 1.0f), v3_make(0.0f, 0.0f, -1.0f),
                      v3_make(0.0f, 1.0f, 0.0f));
    m44 projection = orthographic(0, windowWidth, 0, windowHeight, 0, 1);

    setUniformM44(spritesheetProgram, "projection", &projection);
    setUniformM44(spritesheetProgram, "view", &view);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, spritesheetTexture.id);
    glUniform1i(glGetUniformLocation(spritesheetProgram, "textureId"), 0);
    glUniform1f(glGetUniformLocation(spritesheetProgram, "tileSize"), 16.0f);
    glUniform2f(glGetUniformLocation(spritesheetProgram, "spriteSheetSize"),
                spritesheetTexture.width, spritesheetTexture.height);

    glBindVertexArray(quadVao);
    glBindBuffer(GL_ARRAY_BUFFER, quadVbos[2]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m44) * tileCount,
                    tileTransforms);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, tileCount);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

  sfArenaFree(&stateArena);
  sfArenaFree(&tileArena);

  return 0;
}
