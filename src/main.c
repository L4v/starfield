#include "arena.h"
#include "cubes.h"
#include "math3d.h"
#include "particles.h"
#include "voxels.h"
#include <OpenGL/gl.h>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include "shader.h"
#include "starfield.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

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
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSetCursorPos(window, 0, 0);
  return window;
}

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
      data[i] = 128;
      data[i + 1] = 128;
      data[i + 2] = 128;
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

void sfRender(const State *state, const Voxels *voxels, unsigned voxelsProgram,
              const Particles *snow, unsigned snowProgram) {

  Camera *camera = state->camera;
  m44 view = sfCameraLookAt(camera);
  m44 projection =
      perspective(camera->fov, state->fbw / (float)state->fbh, 0.1f, 1000.0f);

  glUseProgram(voxelsProgram);
  setUniformM44(voxelsProgram, "projection", &projection);
  setUniformM44(voxelsProgram, "view", &view);
  sfRenderVoxels(voxels);

  glUseProgram(snowProgram);
  setUniformM44(snowProgram, "projection", &projection);
  setUniformM44(snowProgram, "view", &view);
  setUniformV3(snowProgram, "cameraPos", &state->camera->position);
  setUniformI1(snowProgram, "totalParticles", snow->count);
  setUniformF1(snowProgram, "worldSize", 256.0f);
  sfParticlesRender(snow);

  glUseProgram(0);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Doubled to avoid overflow. Original had 256 values
int p[512] = {
    151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233, 7,
    225, 140, 36,  103, 30,  69,  142, 8,   99,  37,  240, 21,  10,  23,  190,
    6,   148, 247, 120, 234, 75,  0,   26,  197, 62,  94,  252, 219, 203, 117,
    35,  11,  32,  57,  177, 33,  88,  237, 149, 56,  87,  174, 20,  125, 136,
    171, 168, 68,  175, 74,  165, 71,  134, 139, 48,  27,  166, 77,  146, 158,
    231, 83,  111, 229, 122, 60,  211, 133, 230, 220, 105, 92,  41,  55,  46,
    245, 40,  244, 102, 143, 54,  65,  25,  63,  161, 1,   216, 80,  73,  209,
    76,  132, 187, 208, 89,  18,  169, 200, 196, 135, 130, 116, 188, 159, 86,
    164, 100, 109, 198, 173, 186, 3,   64,  52,  217, 226, 250, 124, 123, 5,
    202, 38,  147, 118, 126, 255, 82,  85,  212, 207, 206, 59,  227, 47,  16,
    58,  17,  182, 189, 28,  42,  223, 183, 170, 213, 119, 248, 152, 2,   44,
    154, 163, 70,  221, 153, 101, 155, 167, 43,  172, 9,   129, 22,  39,  253,
    19,  98,  108, 110, 79,  113, 224, 232, 178, 185, 112, 104, 218, 246, 97,
    228, 251, 34,  242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,  51,
    145, 235, 249, 14,  239, 107, 49,  192, 214, 31,  181, 199, 106, 157, 184,
    84,  204, 176, 115, 121, 50,  45,  127, 4,   150, 254, 138, 236, 205, 93,
    222, 114, 67,  29,  24,  72,  243, 141, 128, 195, 78,  66,  215, 61,  156,
    180, 151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233,
    7,   225, 140, 36,  103, 30,  69,  142, 8,   99,  37,  240, 21,  10,  23,
    190, 6,   148, 247, 120, 234, 75,  0,   26,  197, 62,  94,  252, 219, 203,
    117, 35,  11,  32,  57,  177, 33,  88,  237, 149, 56,  87,  174, 20,  125,
    136, 171, 168, 68,  175, 74,  165, 71,  134, 139, 48,  27,  166, 77,  146,
    158, 231, 83,  111, 229, 122, 60,  211, 133, 230, 220, 105, 92,  41,  55,
    46,  245, 40,  244, 102, 143, 54,  65,  25,  63,  161, 1,   216, 80,  73,
    209, 76,  132, 187, 208, 89,  18,  169, 200, 196, 135, 130, 116, 188, 159,
    86,  164, 100, 109, 198, 173, 186, 3,   64,  52,  217, 226, 250, 124, 123,
    5,   202, 38,  147, 118, 126, 255, 82,  85,  212, 207, 206, 59,  227, 47,
    16,  58,  17,  182, 189, 28,  42,  223, 183, 170, 213, 119, 248, 152, 2,
    44,  154, 163, 70,  221, 153, 101, 155, 167, 43,  172, 9,   129, 22,  39,
    253, 19,  98,  108, 110, 79,  113, 224, 232, 178, 185, 112, 104, 218, 246,
    97,  228, 251, 34,  242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,
    51,  145, 235, 249, 14,  239, 107, 49,  192, 214, 31,  181, 199, 106, 157,
    184, 84,  204, 176, 115, 121, 50,  45,  127, 4,   150, 254, 138, 236, 205,
    93,  222, 114, 67,  29,  24,  72,  243, 141, 128, 195, 78,  66,  215, 61,
    156, 180,
};

v2 randomGrad(int ix, int iy) {
  const unsigned w = 8 * sizeof(unsigned);
  const unsigned s = w / 2;
  unsigned a = ix;
  unsigned b = iy;
  a *= 3284157443;
  b ^= a << s | a >> (w - s);
  b *= 1911520717;

  a ^= b << s | b >> (w - s);
  a *= 2048419325;
  float r = a * (3.14159265 / ~(~0u >> 1)); // [0, 2PI]

  return (v2){SIN(r), COS(r)};
}

float dotGridGrad(int ix, int iy, float x, float y) {
  v2 grad = randomGrad(ix, iy);
  float dx = x - (float)ix;
  float dy = y - (float)iy;

  return dx * grad.x + dy * grad.y;
}

float interpolate(float x, float y, float t) {
  return (y - x) * (-2.0f * t + 3.0f) * t * t + x;
}

/*
 (x0, y0) --- (x1, y0)

    |            |
    |            |
    |            |

 (x0, y1) --- (x1, y1)
 */

// https://www.youtube.com/watch?v=kCIaHqb60Cw
float perlin(float x, float y) {
  int x0 = (int)x;
  int y0 = (int)y;
  int x1 = x0 + 1;
  int y1 = y0 + 1;

  float sx = x - (float)x0;
  float sy = y - (float)y0;

  // Compute and interpolate top two corners
  float n0 = dotGridGrad(x0, y0, x, y);
  float n1 = dotGridGrad(x1, y0, x, y);
  float ix0 = interpolate(n0, n1, sx);

  n0 = dotGridGrad(x0, y1, x, y);
  n1 = dotGridGrad(x1, y1, x, y);
  float ix1 = interpolate(n0, n1, sx);
  return interpolate(ix0, ix1, sy);
}

int rayRectangleIntersection(v3 rayOrigin, v3 rayDir, v3 targetMin,
                             v3 targetMax, float *tMin, v3 *normal) {
  v3 inv = v3_make(1.0f / rayDir.x, 1.0f / rayDir.y, 1.0f / rayDir.z);
  v3 t0 = v3_mul(v3_sub(targetMin, rayOrigin), inv);
  v3 t1 = v3_mul(v3_sub(targetMax, rayOrigin), inv);

  v3 tNear = v3_min(t0, t1);
  v3 tFar = v3_max(t0, t1);

  *tMin = v3_maxf(tNear);
  float tMax = v3_minf(tFar);

  if (tMax < 0 || *tMin > 1.0f || *tMin > tMax) {
    return 0;
  }

  if (*tMin == tNear.x) {
    *normal = v3_make(-fsignf(rayDir.x), 0.0f, 0.0f);
  } else if (*tMin == tNear.y) {
    *normal = v3_make(0.0f, -fsignf(rayDir.y), 0.0f);
  } else {
    *normal = v3_make(0.0f, 0.0f, -fsignf(rayDir.z));
  }

  return 1;
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

  Arena stateArena = sfArenaCreate(MEGABYTE, 1);
  Arena voxelsArena = sfArenaCreate(MEGABYTE, 100);
  Arena cubesArena = sfArenaCreate(MEGABYTE, 100);
  Arena particlesArena = sfArenaCreate(MEGABYTE, 1);

  State *state = sfStateArenaAlloc(&stateArena);
  Input *input = state->input;

  Cubes *cubes = sfCubesArenaAlloc(&cubesArena, 65536);
  Voxels *voxels = sfVoxelsArenaAlloc(&voxelsArena, cubes->count);
  Particles *snowParticles = sfParticlesArenaAlloc(&particlesArena, 1000);

  voxels->texture = generateColorTexture(64, 64, 16, 64, 16, 255);

  unsigned cols = 256;
  unsigned rows = 256;
  unsigned GRID_SIZE = 400.0f;
  for (int index = 0; index < cubes->count; ++index) {

    float x = index % cols;
    float z = (int)(index / cols);

    float y = 0;
    float freq = 1.0f;
    float amp = 1.0f;

    for (int i = 0; i < 12; ++i) {
      y += perlin(x * freq / GRID_SIZE, z * freq / GRID_SIZE) * amp;

      freq *= 2.0f;
      amp /= 2.0f;
    }

    // Contrast
    y *= 1.2;

    y = clampf(y, -1.0f, 1.0f);
    y = ((y + 1.0f) * 0.5f) * 128.0f;
    y = (int)y;

    cubes->positions[index] = v3_make(x, y, z);
  }

  unsigned spawnBlockIdx = rand() % cubes->count;
  state->player->position = cubes->positions[spawnBlockIdx];

  glfwSetWindowUserPointer(window, state);
  glfwGetCursorPos(window, &input->mouse->x, &input->mouse->y);
  glfwGetFramebufferSize(window, &state->fbw, &state->fbh);

  unsigned voxelsProgram =
      createShaderProgram("shaders/voxels.vs", "shaders/voxels.fs");
  unsigned particlesProgram =
      createShaderProgram("shaders/particles.vs", "shaders/particles.fs");
  unsigned basicProgram =
      createShaderProgram("shaders/basic.vs", "shaders/basic.fs");

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  unsigned lineVao, lineVbo;
  float linePts[6] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f};
  glGenVertexArrays(1, &lineVao);
  glGenBuffers(1, &lineVbo);
  glBindVertexArray(lineVao);
  glBindBuffer(GL_ARRAY_BUFFER, lineVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, linePts, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3,
                        (void *)(0));
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  unsigned triangleVao, triangleVbo;
  float trianglePts[9] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f,
                          0.0f,  0.0f,  0.5f, 0.0f};
  glGenVertexArrays(1, &triangleVao);
  glGenBuffers(1, &triangleVbo);
  glBindVertexArray(triangleVao);
  glBindBuffer(GL_ARRAY_BUFFER, triangleVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, trianglePts, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3,
                        (void *)(0));
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  char windowTitle[128];

  v3 dynamicPos = v3_make(200, 1500, 0);
  v3 dynamicSize = v3_make(100, 200, 0);
  v3 dynamicVel = v3_0();

  v3 rayOrigin = v3_make(200, 1500, 0);
  while (!glfwWindowShouldClose(window)) {
    float startTime = glfwGetTime();

    sfInputClearControllers(input);
    glfwPollEvents();

    for (int i = 0; i < cubes->count; ++i) {
      cubes->debugCollision[i] = 0;
    }

    // sfUpdate(state, cubes, snowParticles);

    for (int i = 0; i < cubes->count; ++i) {
      v3 *p = &cubes->positions[i];
      m44 *transform = &voxels->transforms[i];
      *transform = m44_identity(1.0f);
      *transform = translate_v3(transform, p);
      voxels->colors[i] =
          cubes->debugCollision[i] ? v4_make(1.0f, 0.0f, 0.0f, 0.5f) : v4_0();
    }
    float scaleW = (float)state->fbw / windowWidth;
    float scaleH = (float)state->fbh / windowHeight;
    v3 mousePos = v3_make(input->mouse->x * scaleW,
                          state->fbh - input->mouse->y * scaleH, 0.0f);
    v3 rayTarget = v3_make(mousePos.x, mousePos.y, 0.0f);
    rayOrigin = dynamicPos;

    v3 dynMin = v3_sub(dynamicPos, v3_scale(dynamicSize, 0.5f));
    v3 dynMax = v3_add(dynamicPos, v3_scale(dynamicSize, 0.5f));

    v3 boxPos = v3_make(1700.0f, 1000.0f, 0.0f);
    v3 boxSize = v3_make(500.0f, 500.0f, 0.0f);

    float tHitNear = 0.0f;
    v3 normal = v3_0();
    v3 boxMin = v3_sub(boxPos, v3_scale(boxSize, 0.5f));
    v3 boxMax = v3_add(boxPos, v3_scale(boxSize, 0.5f));

    v3 minkMin = v3_sub(boxMin, v3_scale(dynamicSize, 0.5f));
    v3 minkMax = v3_add(boxMax, v3_scale(dynamicSize, 0.5f));
    v3 rayDir = v3_scale(dynamicVel, state->dt);
    int collision = rayRectangleIntersection(rayOrigin, rayDir, minkMin,
                                             minkMax, &tHitNear, &normal) &&
                    (dynamicVel.x != 0 || dynamicVel.y != 0);
    if (collision) {
      float remainingTime = 1.0f - tHitNear;
      float dot = v3_dot(dynamicVel, normal);
      dynamicVel = v3_sub(dynamicVel, v3_scale(normal, dot));
      dynamicVel = v3_scale(dynamicVel, remainingTime);
    }

    if (!collision) {
      dynamicPos = v3_add(dynamicPos, v3_scale(dynamicVel, state->dt));
    }

    if (input->keyboard->debugStep.isDown && !collision) {
      rayOrigin = v3_make(mousePos.x, mousePos.y, 0.0f);
      v3 d = v3_sub(mousePos, dynamicPos);
      dynamicVel = d;
    }

    if (collision) {
      printf("%f, %f %f\n", normal.x, normal.y, normal.z);
    }

    v3 contactPoint = v3_add(rayOrigin, v3_scale(rayDir, tHitNear));
    contactPoint =
        v3_sub(contactPoint, v3_scale(v3_make(normal.x * dynamicSize.x,
                                              normal.y * dynamicSize.y,
                                              normal.z * dynamicSize.z),
                                      0.5f));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(basicProgram);

    m44 model = m44_identity(1.0f);
    m44 view = m44_identity(1.0f);
    m44 projection =
        orthographic(0.0f, state->fbw, 0.0f, state->fbh, -1.0f, 1.0f);

    setUniformM44(basicProgram, "model", &model);
    setUniformM44(basicProgram, "projection", &projection);
    setUniformM44(basicProgram, "view", &view);
    v4 color = v4_make(1.0f, 1.0f, 1.0f, 1.0f);
    setUniformV4(basicProgram, "color", &color);
    glBindVertexArray(lineVao);
    glBindBuffer(GL_ARRAY_BUFFER, lineVbo);

    // Draw grid
    v3 gridLines[8] = {
        v3_make(boxMin.x, 0.0f, -0.5f), v3_make(boxMin.x, state->fbh, -0.5f),
        v3_make(boxMax.x, 0.0f, -0.5f), v3_make(boxMax.x, state->fbh, -0.5f),
        v3_make(0.0f, boxMin.y, -0.5f), v3_make(state->fbw, boxMin.y, -0.5f),
        v3_make(0.0f, boxMax.y, -0.5f), v3_make(state->fbw, boxMax.y, -0.5f),
    };
    color = v4_make(1.0f, 1.0f, 1.0f, 0.25f);
    setUniformV4(basicProgram, "color", &color);
    for (int i = 0; i < 4; ++i) {
      v3 l[2] = {gridLines[i * 2], gridLines[i * 2 + 1]};
      glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(v3), l);
      glDrawArrays(GL_LINES, 0, 2);
    }

    // Draw rect
    v3 boxLines[8] = {
        boxMin, v3_make(boxMin.x, boxMax.y, 0.5f),
        boxMin, v3_make(boxMax.x, boxMin.y, 0.5f),
        boxMax, v3_make(boxMin.x, boxMax.y, 0.5f),
        boxMax, v3_make(boxMax.x, boxMin.y, 0.5f),
    };
    color = v4_make(1.0f, 1.0f, 1.0f, 1.0f);
    if (collision) {
      color = v4_make(1.0f, 0.0f, 0.0f, 1.0f);
    }
    setUniformV4(basicProgram, "color", &color);
    for (int i = 0; i < 4; ++i) {
      v3 l[2] = {boxLines[i * 2], boxLines[i * 2 + 1]};
      glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(v3), l);
      glDrawArrays(GL_LINES, 0, 2);
    }

    // Draw mink
    v3 minkLines[8] = {
        minkMin, v3_make(minkMin.x, minkMax.y, 0.5f),
        minkMin, v3_make(minkMax.x, minkMin.y, 0.5f),
        minkMax, v3_make(minkMin.x, minkMax.y, 0.5f),
        minkMax, v3_make(minkMax.x, minkMin.y, 0.5f),
    };
    color = v4_make(1.0f, 1.0f, 0.0f, 0.3f);
    setUniformV4(basicProgram, "color", &color);
    for (int i = 0; i < 4; ++i) {
      v3 l[2] = {minkLines[i * 2], minkLines[i * 2 + 1]};
      glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(v3), l);
      glDrawArrays(GL_LINES, 0, 2);
    }

    // Draw ray
    color = v4_make(1.0f, 1.0f, 1.0f, 1.0f);
    setUniformV4(basicProgram, "color", &color);
    v3 linePos[2] = {rayOrigin, rayTarget};
    glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(v3), linePos);
    glDrawArrays(GL_LINES, 0, 2);

    // Draw dynamic rect
    v3 dynamicLines[8] = {
        dynMin, v3_make(dynMin.x, dynMax.y, 0.5f),
        dynMin, v3_make(dynMax.x, dynMin.y, 0.5f),
        dynMax, v3_make(dynMin.x, dynMax.y, 0.5f),
        dynMax, v3_make(dynMax.x, dynMin.y, 0.5f),
    };
    for (int i = 0; i < 4; ++i) {
      v3 l[2] = {dynamicLines[i * 2], dynamicLines[i * 2 + 1]};
      glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(v3), l);
      glDrawArrays(GL_LINES, 0, 2);
    }

    // Triangle
    if (collision) {
      glBindVertexArray(triangleVao);
      model = m44_identity(1.0f);
      model = translate_v3(&model, &contactPoint);
      model = scale(&model, 50, 50, 50);
      setUniformM44(basicProgram, "model", &model);
      glDrawArrays(GL_TRIANGLES, 0, 3);

      model = m44_identity(1.0f);
      setUniformM44(basicProgram, "model", &model);
      color = v4_make(1.0f, 1.0f, 0.0f, 1.0f);
      setUniformV4(basicProgram, "color", &color);
      glBindVertexArray(lineVao);
      v3 l[2] = {contactPoint, v3_add(contactPoint, v3_scale(normal, 100.0f))};
      l[0].z = 0.7;
      l[1].z = 0.7;
      glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(v3), l);
      glDrawArrays(GL_LINES, 0, 2);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glfwSwapBuffers(window);

    state->dt = glfwGetTime() - startTime;
    state->time += state->dt;

    float fps = 1.0f;
    if (state->dt > 0) {
      fps = 1.0f / state->dt;
    }
    snprintf(windowTitle, 128, "FPS: %5.0f", fps);

    glfwSetWindowTitle(window, windowTitle);
  }

  sfArenaFree(&stateArena);
  sfArenaFree(&voxelsArena);
  sfArenaFree(&cubesArena);
  sfArenaFree(&particlesArena);
  return 0;
}
