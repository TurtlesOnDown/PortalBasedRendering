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
};

class Plane {
public:
  /*  Mesh Data  */
  vector<Vertex> vertices;
  GLuint indices[6] = { 0,1,3,1,2,3 };
  Texture texture;

  Transformation transform;

  /*  Functions  */
  // Constructor
  Plane(vector<Vertex> vertices, Texture texture, Transformation trans);

  // Render the mesh
  void Draw(Shader shader);

private:
  /*  Render data  */
  //vertex array object, vertex buffer object, something buffer object?
  GLuint VAO, VBO, EBO;

  /*  Functions    */
  // Initializes all the buffer objects/arrays
  void setupMesh();
};


