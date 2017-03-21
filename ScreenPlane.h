#pragma once

#include <iostream>
#include <vector>
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "XPlane.h"

class ScreenPlane {
private:
  vector<XVertex> verts{
    { { 1.0f,1.0f,0 },{ 1,1 } },
    { { 1.0f,-1.0f,0 },{ 1,0 } },
    { { -1.0f,-1.0f,0 },{ 0,0 } },
    { { -1.0f,1.0f,0 },{ 0,1 } },
  };
  GLuint indices[6] = { 0,1,3,1,2,3 };
  GLuint texture = 1;

  GLuint VAO, VBO, EBO;

public:
  ScreenPlane();

  void render(Shader shader);

};