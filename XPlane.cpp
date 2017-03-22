#pragma once
#include "XPlane.h"

std::ostream& operator<<(std::ostream& out, const XPlane& f) {
  out << "XPlane:" << "\n";
  out << "   transform =" << "\n";
  out << "   [" << f.transform[0][0] << ", " << f.transform[0][1] << ", " << f.transform[0][2] << ", " << f.transform[0][3] << "]" << "\n";
  out << "   [" << f.transform[1][0] << ", " << f.transform[1][1] << ", " << f.transform[1][2] << ", " << f.transform[1][3] << "]" << "\n";
  out << "   [" << f.transform[2][0] << ", " << f.transform[2][1] << ", " << f.transform[2][2] << ", " << f.transform[2][3] << "]" << "\n";
  out << "   [" << f.transform[3][0] << ", " << f.transform[3][1] << ", " << f.transform[3][2] << ", " << f.transform[3][3] << "]" << "\n";
  out << "   link = " << f.link << "\n";
  out << "   parent = " << f.parent << "\n";
  out << "   texture = " << f.texture << endl;
  return out;
};


XPlane::XPlane(const vector<XVertex>& vs, glm::vec3 up, GLuint tex, XPlane* lk, Sector* prnt) :
  verts(vs), texture(tex), link(lk), parent(prnt) {

  //position = center of input polygon
  glm::vec3 p;
  for (auto v = vs.begin(); v != vs.end(); ++v) {
    p += v->Position;
  }
  p = p * (1.0f / vs.size());
  //p = vs[0].Position;
  
  //forward = normal of input polygon, right = forward x up
  glm::vec3 forward = glm::cross(vs[1].Position - vs[0].Position,
    vs[2].Position - vs[0].Position); //assumes at least 3 verts
  glm::vec3 right = glm::cross(forward, up);

  //transform = glm::mat4(glm::vec4(right, 0), glm::vec4(up, 0), glm::vec4(forward, 0), glm::vec4(p, 1));
  //transform = glm::mat4(glm::vec4(1, 0, 0, 0), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec4(p, 1));
  transform = glm::mat4(glm::normalize(glm::vec4(right, 0)), glm::normalize(glm::vec4(up, 0)), glm::normalize(glm::vec4(forward, 0)), glm::vec4(p,1));
  setUp();
}

void XPlane::setUp() {

  // Create buffers/arrays
  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &this->VBO);
  glGenBuffers(1, &this->EBO);

  glBindVertexArray(this->VAO);
  // Load data into vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
  // A great thing about structs is that their memory layout is sequential for all its items.
  // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
  // again translates to 3/2 floats which translates to a byte array.
  glBufferData(GL_ARRAY_BUFFER, this->verts.size() * sizeof(XVertex), &this->verts[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->indices), &this->indices[0], GL_STATIC_DRAW);

  // Set the vertex attribute pointers
  // Vertex Positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(XVertex), (GLvoid*)0);
  // Vertex Normals
  //glEnableVertexAttribArray(1);
  //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
  // Vertex Texture Coords
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(XVertex), (GLvoid*)offsetof(XVertex, TexCoords));

  glBindVertexArray(0);

}

void XPlane::Draw(Shader shader, Camera cam, ScreenPlane& screenplane, glm::mat4 proj, int depth)
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  
  glm::mat4 modelMatrix(1); 
  //cout << *this << endl;
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, this->texture);
  glUniform1i(glGetUniformLocation(shader.Program, "ourTexture"), 0);

  glStencilFunc(GL_GEQUAL, depth, 0xFFFF);
  if (link) {
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
  } else {
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  }
  
  GLint transformLoc = glGetUniformLocation(shader.Program, "model");
  //TODO fix dirty hack (we are already in world space).
  //Will have to turn modelmat into a parameter when we draw multiple sectors.
  glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

  // Draw mesh
  glBindVertexArray(this->VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  
  if (link) {
    //draw geometry behind portal

    //reset portal stencil
    glEnable(GL_DEPTH_TEST);
    glStencilFunc(GL_EQUAL, 1, 0xFFFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glColorMask(false, false, false, false);
    screenplane.render(shader, cam, proj);
    //DrawOntoScreen(shader, cam);
    glColorMask(true, true, true, true);
    glEnable(GL_DEPTH_TEST);
  }  
}

//TODO enfoce pts to have length 4
vector<XVertex> makeQuad(const vector<glm::vec3>& pts) {
  vector<XVertex> vs = {};
  vector<glm::vec2> uvs = { { 1.0f, 1.0f },{ 0.0f, 1.0f },{ 0.0f, 0.0f },{ 1.0f, 0.0f } };
  for (int i = 0; i < 4; ++i) {
    vs.push_back({ pts[i], uvs[i] });
  }

  return vs;
}


void XPlane::DrawOntoScreen(Shader shader, Camera& cam) {
  cout << "no" << endl;
}


void Camera::updateCurrentSector() {
  vector<XPlane> planes = currentSector->getFaces();
  for (auto plane : planes) {
    if (glm::dot(Position - glm::vec3(plane.getTransform()[3]) , glm::vec3(plane.getTransform()[2])) > 0) {
      XPlane* link = plane.getLink();
      if (link != nullptr) {
        cout << *link << endl;
        currentSector = link->getParent();
        //glm::mat4 CoB = flip(link->getTransform()) * glm::inverse(plane.getTransform());
        //Front = glm::vec3(CoB * glm::vec4(Front, 0));
        //Up = glm::vec3(CoB * glm::vec4(Up, 0));
        //Right = glm::vec3(CoB * glm::vec4(Right, 0));
        //Position = glm::vec3(CoB * glm::vec4(Position, 1));
      }
      else {
        //this is where one would implement collision
      }
    }
  }
}