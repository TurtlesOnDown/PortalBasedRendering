#include "RenderStructure.h"

void Renderer::draw(Camera cam) {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  this->current_shader.Use();

  
  GLint projLoc = glGetUniformLocation(this->current_shader.Program, "projection");
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(this->Perspective));

  glm::mat4 view = cam.GetViewMatrix();
  GLint viewLoc = glGetUniformLocation(this->current_shader.Program, "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

  the_World[0].Draw(this->current_shader);

}
