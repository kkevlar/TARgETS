#pragma once
#include <stdint.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>
#include "Cube.h"
#include "GLSL.h"
#include "Shape.h"
#include "Collision.h"
#include "Shot.h"

class ShotManager
{
   private:
    float lastShotTime;
    std::vector<Shot> shots;
    int nextShotIndex;
    int selfIndexCalc(int index, int myPlayerId);

   public:
    ShotManager(int max_player_count);
    void shootAndSendToServer(glm::vec3 targetPos, int myPlayerId, float currentTime);
    void advanceShots(float frametime);
    void drawShots(std::shared_ptr<Program> prog,
                   Shape shape,
                   ColorList& color_list);
    void fillCollisionHandlerWithMyShots(CollisionHandler& collision, int myPlayerId);
    Shot getMyShotAtIndex(int index, int myPlayerId);
    void setMyShotAtIndex(Shot shot, int index, int myPlayerId);
};
