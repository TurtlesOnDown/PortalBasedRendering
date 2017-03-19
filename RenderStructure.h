#pragma once
#include "PortalGeometry.h"
#include "Camera.h"

class Renderer {
public:
  Renderer(glm::mat4 Persp, Shader shade):Perspective(Persp),current_shader(shade) {};

  void pushXPlane(XPlane newXPlane) { this->drawableObjects.push_back(newXPlane); }
  void dumpObjects() { this->drawableObjects.empty(); };
  void draw(Camera cam);

private:
  vector<XPlane> drawableObjects;
  glm::mat4 Perspective;
  Shader current_shader;
};