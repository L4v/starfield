#include "math3d.h"
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *readShaderSource(const char *filepath);
GLuint compileShader(const char *source, GLenum shaderType);
int checkShaderCompilation(GLuint shader, const char *shaderTypeStr);
int checkProgramLinking(GLuint program);
GLuint createShaderProgram(const char *vertexPath, const char *fragmentPath);

void setUniformM44(int program, const char *uniform, const m44 *m);
void setUniformFloat(int program, const char *uniform, float f);
void setUniformF4(int program, const char *uniform, float r, float g, float b,
                  float a);
