#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>
#include "GLSL.h"
#include "Program.h"

class Billboard
{
   private:
    GLuint VertexArrayID;
    GLuint PositionBufferId, NormBufferId, TexCoordsBufferId, IndexBufferId;
    GLuint Texture;

   public:
    void init(std::shared_ptr<Program>& bbprog);
    void draw(std::shared_ptr<Program>& bbprog, glm::mat4 P, glm::mat4 V);
};
