#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>
#include "GLSL.h"
#include "program.h"

class ColorList
{
   private:
    std::vector<glm::vec3> color_list;

   public:
    glm::vec3 get_color(int i);
    ColorList();
};

class MatrixIngridients
{
   public:
    glm::vec3 pos, rot, scale, anchor;
    MatrixIngridients();
    glm::mat4 calc_no_scale();
    glm::mat4 calc_scale(glm::mat4 noscale);
    void self_interpolate_between(MatrixIngridients& a,
                                  MatrixIngridients& b,
                                  float z);
};

class CylCoords
{
   public:
    float angle;
    float height;
    bool show;
    MatrixIngridients result;

    void calc_result();
};

class InterpObject
{
   public:
    MatrixIngridients target;
    MatrixIngridients source;
    MatrixIngridients postInterp;
    int parent;
    float phase;
    float interp;
    bool show;    
    bool dosin;
    glm::vec3 bonuscolor;
    InterpObject();
    InterpObject(int x, int y, int z, glm::vec3 centeroffset);
    InterpObject(int x, int y, int z, glm::vec3 centeroffset, int cubesSize);
    void sendModelMatrix(std::shared_ptr<Program> prog, glm::mat4 parentM);
    
    
    void interpBetween();
    void resetInterp();
    

   protected:
    glm::mat4 cached_no_scale;
    void init(int x, int y, int z, glm::vec3 centeroffset);
};

class Target : public InterpObject
{
   public:
    bool hit;
    void beShot(int myIndex, uint8_t player_id, ColorList* colors);
    void drawTarget(std::shared_ptr<Program> prog,
                     std::vector<Target>& elements,
                     glm::mat4 parentM);
    void sendModelMatrix(std::shared_ptr<Program> prog,
                         std::vector<Target>& elements,
                         glm::mat4 parentM);
};

class TargetManager
{
   public:
    std::vector<Target> elements;

    int snowman_index;
    int pickaxe_index;

    TargetManager();

    void draw(std::shared_ptr<Program> prog);
};
