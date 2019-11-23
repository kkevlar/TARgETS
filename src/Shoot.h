#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>
#include "GLSL.h"
#include "Cube.h"
#include <stdint.h>

class Shot
{
   public:
    //bool mine;
    int playerId;
    InterpObject obj;
};

class ShotManager
{
   private:
    std::vector<Shot> shots;
    int selfIndexCalc(int index, int myPlayerId);
   public:
    ShotManager(int max_player_count);
    Shot getMyShotAtIndex(int index, int myPlayerId);
    void setMyShotAtIndex(Shot shot, int index, int myPlayerId);

};
