#pragma once
#ifndef XPLANE
#define XPLANE


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
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Camera.h"

class Sector;
class XPlane;
typedef vector<Sector> World;

constexpr GLuint TEXTURE_COUNT = 1;

struct XVertex {
  // Position
  glm::vec3 Position;
  // Normal
  //glm::vec3 Normal;
  // TexCoords
  glm::vec2 TexCoords;
};

class XPlane {
  friend std::ostream& operator<<(std::ostream& out, const XPlane& f);

public:
  XPlane(const glm::mat4 tf, GLuint tex, XPlane* lk, Sector* prnt) :
    transform(tf), texture(tex), link(lk), parent(prnt) {
    setUp();
  }

  XPlane(const vector<XVertex>& vs, glm::vec3 up, GLuint tex, XPlane* lk, Sector* prnt);
  void Draw(Shader shader, int depth);

  //get the texture
  GLuint getTex() {return texture;}
  //set the texture if it is valid
  void setTex(GLuint tx) {if (tx >= TEXTURE_COUNT) texture = tx;}

  //get the parent sector
  Sector* getParent() {return parent;}
  //get the plane corresponding to the other side of the portal
  XPlane* getLink() {return link;}
  //modify the portals in some way
  void setLinkRaw(XPlane* newlink) {link = newlink;}
  void setLinkTwoWayRaw(XPlane* newlink) {link = newlink; newlink->link = this;}
private:

  glm::mat4 transform;
  XPlane* link;
  Sector* parent;
  GLuint texture;
  vector<XVertex> quad = {
    { { 100.0f,100.0f,0 },{ 1,1 } },
    { { 100.0f,-100.0f,0 },{ 1,0 } },
    { { -100.0f,-100.0f,0 },{ 0,0 } },
    { { -100.0f,100.0f,0 },{ 0,1 } },
  };
  GLuint indices[6] = { 0,1,3,1,2,3 };


  GLuint VAO, VBO, EBO;

  void setUp();
};



class Sector {
public:
  Sector() {}
  //constructor takes in a vector of pointers to XPlanes
  Sector(const vector<XPlane>& fs) : faces(fs) {}
  //destructor...? TODO

  void Draw(Shader s) { for (auto fs : faces) { fs.Draw(s); } };
private:
  vector<XPlane> faces;
};

#endif // !XPLANE
