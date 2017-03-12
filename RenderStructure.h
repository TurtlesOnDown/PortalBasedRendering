#pragma once
#include "Mesh.h"
#include "Camera.h"

class Renderer {
public:
  Renderer(glm::mat4 Persp, Shader shade):Perspective(Persp),current_shader(shade) {};

  void pushPlane(Plane newPlane) { this->drawableObjects.push_back(newPlane); }
  void dumpObjects() { this->drawableObjects.empty(); };
  void draw(Camera cam);

private:
  vector<Plane> drawableObjects;
  glm::mat4 Perspective;
  Shader current_shader;
};