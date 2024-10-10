#include "shader.h"

char *readShaderSource(const char *filepath) {
  FILE *file = fopen(filepath, "r");
  if (file == NULL) {
    printf("Failed to open file: %s\n", filepath);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buffer = (char *)malloc(length + 1);
  if (buffer == NULL) {
    printf("Failed to allocate memory for shader source\n");
    fclose(file);
    return NULL;
  }

  fread(buffer, 1, length, file);
  buffer[length] = '\0';

  fclose(file);
  return buffer;
}

GLuint compileShader(const char *source, GLenum shaderType) {
  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  return shader;
}

int checkShaderCompilation(GLuint shader, const char *shaderTypeStr) {
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar info_log[512];
    glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
    printf("ERROR::SHADER::%s::COMPILATION_FAILED\n%s\n", shaderTypeStr,
           info_log);
    return 0;
  }
  return 1;
}

int checkProgramLinking(GLuint program) {
  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    GLchar info_log[512];
    glGetProgramInfoLog(program, sizeof(info_log), NULL, info_log);
    printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", info_log);
    return 0;
  }
  return 1;
}

GLuint createShaderProgram(const char *vertexPath, const char *fragmentPath) {
  char *vertexSource = readShaderSource(vertexPath);
  char *fragmentSource = readShaderSource(fragmentPath);

  if (!vertexSource || !fragmentSource) {
    free(vertexSource);
    free(fragmentSource);
    return 0;
  }

  GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
  GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

  free(vertexSource);
  free(fragmentSource);

  if (!checkShaderCompilation(vertexShader, "VERTEX") ||
      !checkShaderCompilation(fragmentShader, "FRAGMENT")) {
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return 0;
  }

  GLuint program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  if (!checkProgramLinking(program)) {
    glDeleteProgram(program);
    return 0;
  }

  return program;
}

void setUniformM44(int program, const char *uniform, const m44 *m) {
  glUniformMatrix4fv(glGetUniformLocation(program, uniform), 1, GL_FALSE,
                     (float *)m);
}

void setUniformFloat(int program, const char *uniform, float f) {
  glUniform1f(glGetUniformLocation(program, uniform), f);
}

void setUniformF4(int program, const char *uniform, float r, float g, float b,
                  float a) {
  glUniform4f(glGetUniformLocation(program, uniform), r, g, b, a);
}
