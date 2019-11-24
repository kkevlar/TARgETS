#pragma once
#include <stdint.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>
#include "Cube.h"
#include "GLSL.h"
#include "Shoot.h"

#define COLLISION_RADIUS ((float) 1.0f)

typedef struct CollisionBucket
{
    bool valid;
    glm::vec3 shotLocation;
} CollisionBucket;

class CollisionHandler
{
   public:
    CollisionHandler();
    int prepTableWithShots(std::vector<Shot>& shotList,
                           float radius,
                           int beginIndex,
                           int endIndex);
    bool testCollision(glm::vec3 position, float radius);

   private:
    std::vector<CollisionBucket> collisionTable;
    int putInTable(glm::vec3 shotPos, uint32_t key);
    void invalidateTable();
    bool unitTest();
};

