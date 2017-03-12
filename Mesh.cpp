#include "Mesh.h"

Plane::Plane(vector<Vertex> vertices, Texture texture, Transformation trans)
{
  this->vertices = vertices;
  this->texture = texture;
  this->transform = trans;

  // Now that we have all the required data, set the vertex buffers and its attribute pointers.
  this->setupMesh();
}

void Plane::Draw(Shader shader)
{
  // Bind appropriate textures

  // Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, this->texture.id);
  glUniform1i(glGetUniformLocation(shader.Program, "ourTexture"), 0);

  glm::mat4 transform;
  transform = glm::translate(transform, this->transform.Position);
  transform = glm::scale(transform, this->transform.Scale);

  GLint transformLoc = glGetUniformLocation(shader.Program, "model");
  glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

  // Draw mesh
  glBindVertexArray(this->VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  
}


void Plane::setupMesh()
{
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
  glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->indices), &this->indices[0], GL_STATIC_DRAW);

  // Set the vertex attribute pointers
  // Vertex Positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
  // Vertex Normals
  //glEnableVertexAttribArray(1);
  //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
  // Vertex Texture Coords
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

  glBindVertexArray(0);
}