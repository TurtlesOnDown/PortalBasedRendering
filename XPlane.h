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
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Camera.h"


class Sector;
class XPlane;
typedef vector<Sector> World;

struct XVertex {
  // Position
  glm::vec3 Position;
  // Normal
  //glm::vec3 Normal;
  // TexCoords
  glm::vec2 TexCoords;
};

#include "ScreenPlane.h"

class XPlane {
  friend std::ostream& operator<<(std::ostream& out, const XPlane& f);

public:
  XPlane(const glm::mat4 tf, GLuint tex, XPlane* lk, Sector* prnt) :
    transform(tf), texture(tex), link(lk), parent(prnt) {
    XVertex v0 = {glm::vec3(tf[0] + tf[1]), {1.0f,1.0f } };
    XVertex v1 = {glm::vec3(tf[1] - tf[0]), { .0f,1.0f } };
    XVertex v2 = {glm::vec3(-tf[0] - tf[1]), { .0f, .0f } };
    XVertex v3 = {glm::vec3(tf[0] - tf[1]), { .0f,1.0f } };
    vector<XVertex> vs { v0, v1, v2, v3 };
	verts = vs;
    setUp();
  }

  XPlane(const vector<XVertex>& vs, glm::vec3 up, GLuint tex, XPlane* lk, Sector* prnt);
  //get the texture
  GLuint getTex() { return texture; }
  //set the texture if it is valid TODO
  void setTex(GLuint tx) { if (true) texture = tx; }
  //get the parent sector
  Sector* getParent() { return parent; }
  //get the plane corresponding to the other side of the portal
  XPlane* getLink() { return link; }
  vector<XVertex> getVerts() { return verts; }
  //modify the portals in some way
  void setLinkRaw(XPlane* newlink) { link = newlink; }
  void setLinkTwoWayRaw(XPlane* newlink) { link = newlink; newlink->link = this; }
  //get the transform
  glm::mat4 getTransform() { return transform; }
  //fix the plane to a specific transform in world space
  void setTransform(const glm::mat4& m) { transform = m; }

  void Draw(Shader shader, Camera cam, ScreenPlane& screenplane, glm::mat4 proj, int depth, const glm::mat4& modelMatrix);
  void DrawOntoScreen(Shader shader, Camera& cam);
private:

  glm::mat4 transform;
  XPlane* link;
  Sector* parent;
  GLuint texture;
  vector<XVertex> verts;
  GLuint indices[6] = { 0,1,3,1,2,3 };


  GLuint VAO, VBO, EBO;

  void setUp();

  glm::mat4 flip(glm::mat4&& toflip) {
    return glm::mat4(-toflip[0], toflip[1], -toflip[2], toflip[3]);
  };
};


class Sector {
public:
  //constructor takes in a vector of pointers to XPlanes
  Sector(const vector<XPlane>& fs = {}) : faces(fs) {}
  //destructor...? TODO

  void Draw(Shader s, Camera cam, ScreenPlane& screenplane, glm::mat4 proj, int depth, const glm::mat4& tr = glm::mat4(1)) {
    for (auto fs : faces) {
      glm::vec3 norm(fs.getTransform()[2]);
      glm::vec3 posn(fs.getTransform()[3]);
      float cosangle = glm::dot(cam.Front, norm) / (glm::length(cam.Front) * glm::length(norm));
      if (cosangle < 0.707) {
        float gtz = glm::dot(cam.Position - posn, norm);
        if (fs.getParent() != this || gtz >= 0) {
          fs.Draw(s, cam, screenplane, proj, depth, tr);
        }
      }
    }
  }

  vector<XPlane> getFaces() { return faces; }

  void Move(const glm::mat4 moveby) {
    for (auto face : faces) {
      face.setTransform(moveby * face.getTransform());
      vector<XVertex> vs = face.getVerts();
      for (auto v : vs) {
        v.Position = glm::vec3(moveby * glm::vec4(v.Position, 1));
      }
    }
  }

private:
  vector<XPlane> faces;
};

vector<XVertex> makeQuad(const vector<glm::vec3>& pts);

