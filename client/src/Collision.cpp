

#include "Collision.h"

#define TABLE_SIZE 1559
#define BONUS_ADD_BOUND_NEG ((int)-1)
#define BONUS_ADD_BOUND_POS ((int)1)

int32_t float_to_rough_int(float f, float precision)
{
    float result = f * (1.0f / precision);
    return ((int32_t)result);
}

uint32_t float_to_hashable_int(float f, float precision, int32_t offset)
{
    int32_t num;

    num = float_to_rough_int(f, precision);
    num += offset;

    uint32_t result;

    result = (uint32_t)num;

    return result;
}

uint32_t hash_combine(uint32_t a, uint32_t b)
{
    a ^= b + 0x9e3779b9 + (a << 6) + (a >> 2);
    return a;
}

uint32_t v3hash(glm::vec3 v,
                float precision,
                int32_t xoffset,
                int32_t yoffset,
                int32_t zoffset)
{
    uint32_t xhash = float_to_hashable_int(v.x, precision, xoffset);
    uint32_t yhash = float_to_hashable_int(v.y, precision, yoffset);
    uint32_t zhash = float_to_hashable_int(v.z, precision, zoffset);

    uint32_t yzcomb = hash_combine(yhash, zhash);

    return hash_combine(xhash, yzcomb);
}

int CollisionHandler::putInTable(glm::vec3 shotPos, uint32_t key)
{
    int j;
    for (j = 0; j < collisionTable.size(); j++)
    {
        key += j;
        key %= collisionTable.size();

        if (!collisionTable.data()[key].valid)
        {
            break;
        }
    }

    if (j == collisionTable.size())
    {
        fprintf(stderr,
                "Collision put shot in collisionTable failed open addressing. "
                "Table too full?\n");
    }

    collisionTable.data()[key].valid = 1;
    collisionTable.data()[key].shotLocation = shotPos;

    // printf("%d\n", j);

    return j;
}

int CollisionHandler::prepTableWithShots(std::vector<Shot>& shotList,
                                         float radius,
                                         int beginIndex,
                                         int endIndex)
{
    int hashCollisions = 0;

    invalidateTable();

    for (int i = beginIndex; i < endIndex; i++)
    {
        if (!(shotList.data()[i].obj.show))
        {
            continue;
        }
      
		glm::vec3 shotPos = shotList.data()[i].obj.postInterp.pos;

        for (int x = BONUS_ADD_BOUND_NEG; x <= BONUS_ADD_BOUND_POS; x++)
        {
            for (int y = BONUS_ADD_BOUND_NEG; y <= BONUS_ADD_BOUND_POS; y++)
            {
                for (int z = BONUS_ADD_BOUND_NEG; z <= BONUS_ADD_BOUND_POS; z++)
                {
                    uint32_t hash = v3hash(shotPos, radius, x, y, z);
                    hashCollisions += putInTable(shotPos, hash);
                }
            }
        }
    }

    return hashCollisions;
}

bool CollisionHandler::testCollision(glm::vec3 position, float radius)
{
    uint32_t key = v3hash(position, radius, 0, 0, 0);
    bool result = false;
    int j;

    for (j = 0; j < collisionTable.size(); j++)
    {
        key += j;
        key %= collisionTable.size();

        if (collisionTable.data()[key].valid && glm::distance(
                collisionTable.data()[key].shotLocation, position) <
            radius)
        {
            result = true;
            break;
        }

        if (!collisionTable.data()[key].valid)
        {
            result = false;
            break;
        }
    }

    return result;
}

void CollisionHandler::invalidateTable()
{
    for (int i = 0; i < collisionTable.size(); i++)
    {
        collisionTable.data()[i].valid = 0;
    }
}

CollisionHandler::CollisionHandler()
{
    CollisionBucket bucket;
    bucket.valid = 0;
    collisionTable = std::vector<CollisionBucket>(TABLE_SIZE, bucket);
    unitTest();
    unitTest();
    unitTest();
}

bool CollisionHandler::unitTest()
{
    float collideDist = 0.12;

    std::vector<Shot> shots = std::vector<Shot>();
    for (int x = 0; x < 5; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            Shot s = Shot();
            s.obj.postInterp.pos = glm::vec3(x, y, 0);
            s.obj.show = 1;
            shots.push_back(s);
        }
    }

    std::vector<glm::vec3> testPoses = std::vector<glm::vec3>();
    std::vector<bool> testExpect = std::vector<bool>();

    testPoses.push_back(glm::vec3(0, 0, 0));
    testExpect.push_back(true);
    testPoses.push_back(glm::vec3(2, 0, 0));
    testExpect.push_back(true);
    testPoses.push_back(glm::vec3(4, 0, 0));
    testExpect.push_back(true);
    testPoses.push_back(glm::vec3(0.1, 0, 0));
    testExpect.push_back(true);
    testPoses.push_back(glm::vec3(2.1, 0, 0));
    testExpect.push_back(true);
    testPoses.push_back(glm::vec3(4.1, 0, 0));
    testExpect.push_back(true);
    testPoses.push_back(glm::vec3(-0.1, 0, 0));
    testExpect.push_back(true);
    testPoses.push_back(glm::vec3(1.9, 0, 0));
    testExpect.push_back(true);
    testPoses.push_back(glm::vec3(3.9, 0, 0));
    testExpect.push_back(true);
    testPoses.push_back(glm::vec3(0, -0.1, 0));
    testExpect.push_back(true);
    testPoses.push_back(glm::vec3(2, 0.1, 0));
    testExpect.push_back(true);
    testPoses.push_back(glm::vec3(4, 0, -0.1));
    testExpect.push_back(true);
    testPoses.push_back(glm::vec3(-0.15, 0, 0));
    testExpect.push_back(false);
    testPoses.push_back(glm::vec3(2.15, 0, 0));
    testExpect.push_back(false);
    testPoses.push_back(glm::vec3(4.15, 0, 0));
    testExpect.push_back(false);
    testPoses.push_back(glm::vec3(0, 0, 0.15));
    testExpect.push_back(false);
    testPoses.push_back(glm::vec3(2, 0, 0.15));
    testExpect.push_back(false);
    testPoses.push_back(glm::vec3(4, 0, -0.15));
    testExpect.push_back(false);

    int c = prepTableWithShots(shots, collideDist, 0, shots.size());
    printf("Collisions: %d, shots: %d\n", c, shots.size());

    for (int i = 0; i < testPoses.size(); i++)
    {
        bool actual;
        actual = testCollision(testPoses.data()[i], collideDist);
        if (actual != testExpect.at(i))
        {
            printf("Test failed!");
            abort();
        }
    }
}
