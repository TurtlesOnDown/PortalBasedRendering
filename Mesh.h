#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "XPlane.h"


struct Vertex {
  // Position
  glm::vec3 Position;
  // Normal
  //glm::vec3 Normal;
  // TexCoords
  glm::vec2 TexCoords;
};

struct Texture {
  GLuint id;
  //string type;
  //aiString path;
};

struct Transformation {
  glm::vec3 Position;
  glm::vec3 Scale;
  GLfloat xrot;
  GLfloat yrot;
  GLfloat zrot;
};

class Plane {
public:
  /*  Mesh Data  */
  vector<XVertex> quad = {
    { { 10.0f,10.0f,0 },{ 1,1 } },
    { { 10.0f,-10.0f,0 },{ 1,0 } },
    { { -10.0f,-10.0f,0 },{ 0,0 } },
    { { -10.0f,10.0f,0 },{ 0,1 } },
  };
  GLuint indices[6] = { 0,1,3,1,2,3 };
  GLuint texture;

  //Transformation transform;

  /*  Functions  */
  // Constructor
  Plane(vector<Vertex> vertices, GLuint texture, glm::mat4 trans);

  // Render the mesh
  void Draw(Shader shader);

private:
  /*  Render data  */
  GLuint VAO, VBO, EBO;

  glm::mat4 transform;

  /*  Functions    */
  // Initializes all the buffer objects/arrays
  void setupMesh();
};


