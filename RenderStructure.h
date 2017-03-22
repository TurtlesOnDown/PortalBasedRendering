#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "XPlane.h"
#include "ScreenPlane.h"

class Renderer {
public:
  Renderer(glm::mat4 Persp, Shader shade, World w):Perspective(Persp),current_shader(shade),the_World(w) {};

  //void pushPlane(XPlane newPlane) { this->drawableObjects.push_back(newPlane); }
  void dumpObjects() { this->the_World.empty(); };
  void draw(Camera cam);

  ScreenPlane testScreenPlane;
  glm::mat4 Perspective;

 private:
  World the_World;
  Shader current_shader;

};
