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

float randf() { return (float)rand() / RAND_MAX; }

void glfwErrorCallback(int error, const char *description) {
  fprintf(stderr, "GLFW Error: %s\n", description);
}

void processKey(unsigned char *key, unsigned char isDown) { *key = isDown; }

void glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action,
                     int mods) {

  Input *input = (Input *)glfwGetWindowUserPointer(window);
  unsigned char isDown = action == GLFW_PRESS;

  if (action != GLFW_REPEAT) {
    switch (key) {
    case GLFW_KEY_ESCAPE:
      if (isDown) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
      }
      break;
    case GLFW_KEY_UP:
      processKey(&input->lookUp, isDown);
      break;
    case GLFW_KEY_DOWN:
      processKey(&input->lookDown, isDown);
      break;
    case GLFW_KEY_LEFT:
      processKey(&input->lookLeft, isDown);
      break;
    case GLFW_KEY_RIGHT:
      processKey(&input->lookRight, isDown);
      break;
    case GLFW_KEY_W:
      processKey(&input->moveForward, isDown);
      break;
    case GLFW_KEY_S:
      processKey(&input->moveBackward, isDown);
      break;
    case GLFW_KEY_A:
      processKey(&input->moveLeft, isDown);
      break;
    case GLFW_KEY_D:
      processKey(&input->moveRight, isDown);
      break;
    case GLFW_KEY_SPACE:
      processKey(&input->moveUp, isDown);
      break;
    case GLFW_KEY_LEFT_SHIFT:
      processKey(&input->moveDown, isDown);
      break;
    }
  }
}

GLFWwindow *initGlfwWindow(int width, int height) {
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

int main() {
  if (!glfwInit()) {
    fprintf(stderr, "Failed to init glfw\n");
    return -1;
  }

  int windowWidth = 1680;
  int windowHeight = 1050;
  glfwSetErrorCallback(glfwErrorCallback);
  GLFWwindow *window = initGlfwWindow(windowWidth, windowHeight);
  glfwSwapInterval(1);

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  unsigned starVao, starVbo, starVertexCount, starInstanceCount;
  sfInitStarBuffers(&starVao, &starVbo, &starVertexCount, &starInstanceCount);

  unsigned voxelVao, voxelVbo, voxelInstanceCount;
  sfInitVoxelBuffers(&voxelVao, &voxelVbo, &voxelInstanceCount);

  v3 eye = {0.0f, 0.0f, 4.0f};
  v3 center = {0.0f, 0.0f, 0.0f};
  v3 up = {0.0f, 1.0f, 0.0f};

  int starProgram = createShaderProgram("shaders/basic.vs", "shaders/basic.fs");
  int voxelProgram =
      createShaderProgram("shaders/voxels.vs", "shaders/voxel.fs");

  Input input = {0};
  Camera camera = {0};
  sfInitCamera(&camera);
  Player player = {0};
  player.movementSpeed = 10.0f;

  glfwSetWindowUserPointer(window, &input);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  float dt = 0.0f;
  double time = 0.0f;

  unsigned containerTexture = loadImageAsTexture("res/container.jpg");

  while (!glfwWindowShouldClose(window)) {
    float startTime = glfwGetTime();
    glfwPollEvents();

    sfUpdate(&input, &camera, &player, dt);

    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(starProgram);

    setUniformFloat(starProgram, "time", time);
    v3 center = v3_add(camera.position, camera.forward);
    m44 view = lookAt(camera.position, center, camera.up);
    m44 projection =
        perspective(45.0f, windowWidth / (float)windowHeight, 0.1f, 1000.0f);

    setUniformM44(starProgram, "projection", &projection);
    setUniformM44(starProgram, "view", &view);

    glBindVertexArray(starVao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, starVertexCount, starInstanceCount);

    glUseProgram(voxelProgram);
    setUniformM44(voxelProgram, "projection", &projection);
    setUniformM44(voxelProgram, "view", &view);
    glBindTexture(GL_TEXTURE_2D, containerTexture);
    sfRenderVoxels(voxelVao, voxelInstanceCount);

    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glfwSwapBuffers(window);

    dt = glfwGetTime() - startTime;
    time += dt;
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
