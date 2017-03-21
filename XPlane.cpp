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

void XPlane::Draw(Shader shader)
{
  //cout << *this << endl;
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, this->texture);
  glUniform1i(glGetUniformLocation(shader.Program, "ourTexture"), 0);

  GLint transformLoc = glGetUniformLocation(shader.Program, "model");
  glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(this->transform));

  // Draw mesh
  glBindVertexArray(this->VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  
}
