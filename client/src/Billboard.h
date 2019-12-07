#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>
#include "Cube.h"
#include "GLSL.h"
#include "Program.h"

class BBCube : public InterpObject
{
   public:
    glm::vec2 texOffset;
};

class Billboard
{
   private:
    GLuint VertexArrayID;
    GLuint PositionBufferId, NormBufferId, TexCoordsBufferId, IndexBufferId;
    GLuint Texture;
    int myCubeDim;
    std::vector<BBCube> bbCubes;
    std::vector<BBCube> bbCubesPost;

   public:
    void init(std::shared_ptr<Program>& bbprog);
    void draw(std::shared_ptr<Program>& bbprog,
              glm::vec3 campos,
              double frametime,
              glm::mat4 P,
              glm::mat4 V);
    Billboard();
};
