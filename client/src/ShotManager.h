#pragma once
#include <stdint.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <mutex>
#include <vector>
#include "Collision.h"
#include "Cube.h"
#include "GLSL.h"
#include "Shape.h"
#include "Shot.h"

class ShotManager
{
   private:
    float lastShotTime;
    std::vector<Shot> shots;
    int nextShotIndex;
    int selfIndexCalc(int index, int myPlayerId);
    int serializeShot(uint8_t* buf, Shot shot, int index);
    bool initialized;
    void unitTest();

   public:
    ShotManager(int max_player_count);
    void shootAndSendToServer(glm::vec3 targetPos,
                              int myPlayerId,
                              float currentTime);
    void advanceShots(float frametime);
    void drawShots(std::shared_ptr<Program> prog,
                   Shape shape,
                   ColorList& color_list);
    void fillCollisionHandlerWithMyShots(CollisionHandler& collision,
                                         int myPlayerId);
    Shot getMyShotAtIndex(int index, int myPlayerId);
    void setMyShotAtIndex(Shot shot, int index, int myPlayerId);
    Shot getGlobalShotAtIndex(int index);
        void
        setGlobalShotAtIndex(Shot shot, int index);
    bool isInitialized();
        void unSerializeShot(uint8_t* buf, int length);
};
