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
    GLuint IBID_Poses_Start;
    GLuint IBID_Poses_Mid;
    GLuint IBID_Poses_End;
    GLuint IBID_TexCoordsOffset;
    GLuint IBID_Phases;

    GLuint Texture, TextureN;
    int myCubeDim;
    std::vector<BBCube> bbCubes;
    std::vector<BBCube> bbCubesPost;

   public:
    std::shared_ptr<Program> initShader(std::string resourceDirectory);
    void initEverythingElse(std::shared_ptr<Program>& bbprog);
    void draw(std::shared_ptr<Program>& bbprog,
              glm::vec3 campos,
              double frametime,
              glm::mat4 P,
              glm::mat4 V);
    Billboard();
};
