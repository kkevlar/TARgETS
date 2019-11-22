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
    bool mine;
    int playerId;
    InterpObject cube;
};

class ShotManager
{
   private:
    std::vector<Shot> shots;
   public:
    ShotManager(int max_player_count, uint8_t my_player_id);

};
