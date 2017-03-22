#include "ScreenPlane.h"

ScreenPlane::ScreenPlane() {
  vector<XVertex> verts;
  verts.push_back(
		  { { 1.0f,1.0f,0 },{ 1,1 } });
  verts.push_back(
		  { { 1.0f,-1.0f,0 },{ 1,0 } });
  verts.push_back(
		  { { -1.0f,-1.0f,0 },{ 0,0 } });
  verts.push_back(
		  { { -1.0f,1.0f,0 },{ 0,1 } });

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
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(XVertex), &verts[0], GL_STATIC_DRAW);

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

void ScreenPlane::render(Shader shader, Camera& cam, glm::mat4 proj) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, this->texture);
  glUniform1i(glGetUniformLocation(shader.Program, "ourTexture"), 0);

  glm::mat4 persp(1);
  GLint projLoc = glGetUniformLocation(shader.Program, "projection");
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(persp));

  glm::mat4 view(1);
  GLint viewLoc = glGetUniformLocation(shader.Program, "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

  glm::mat4 modelMatrix(2);
  GLint transformLoc = glGetUniformLocation(shader.Program, "model");
  //TODO fix dirty hack (we are already in world space).
  //Will have to turn modelmat into a parameter when we draw multiple sectors.
  glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

  // Draw mesh
  glBindVertexArray(this->VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cam.GetViewMatrix()));
  
}
