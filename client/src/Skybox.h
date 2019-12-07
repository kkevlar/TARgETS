#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>
#include "Cube.h"
#include "GLSL.h"
#include "Program.h"

class Skybox
{
   private:
    GLuint VertexArrayID;
    GLuint PositionBufferId, TexCoordsBufferId;
    GLuint Texture;

   public:
    void init(std::shared_ptr<Program>& skyboxprog);
    void draw(std::shared_ptr<Program>& skyboxprog,
              glm::mat4 P,
              glm::mat4 V);
};
