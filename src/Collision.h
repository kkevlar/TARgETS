#pragma once
#include <stdint.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>
#include "Cube.h"
#include "GLSL.h"
#include "Shoot.h"

#define COLLISION_RADIUS ((float) 0.2f)

typedef struct CollisionBucket
{
    bool valid;
    glm::vec3 shotLocation;
} CollisionBucket;

class CollisionHandler
{
   public:
    CollisionHandler();

   private:
    std::vector<CollisionBucket> collisionTable;
    int putInTable(glm::vec3 shotPos, uint32_t key);
    int prepTableWithShots(std::vector<Shot>& shotList,
                           float radius,
                           int beginIndex,
                           int endIndex);
    bool testCollision(glm::vec3 position, float radius);
    void invalidateTable();
    bool unitTest();
};

