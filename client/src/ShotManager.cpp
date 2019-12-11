

#include "ShotManager.h"
#include "message.h"
#include "webclient.h"
#include <iostream>

#define MAX_SHOTS_PER_PLAYER 32

/* Oh no! A global mutex :(.... Idk how to make this a member of the class but
 * who cares we will only ever need one shot manager */
std::mutex shotMutex;
static uint8_t tmpWriteBuf[256];

#define NUM_BYTES_INDEX 3
#define NUM_BYTES_SINGLE_V3_FLOAT 5
#define NUM_BYTES_V3 (NUM_BYTES_SINGLE_V3_FLOAT * 3)

int ShotManager::serializeShot(uint8_t* buf, Shot shot, int index)
{
    int offset = 0;
    offset += assignBytesFromNum(buf + offset, index, NUM_BYTES_INDEX);
    offset +=
        assignBytesFromVec3(buf + offset, shot.obj.source.pos, NUM_BYTES_V3);
    offset +=
        assignBytesFromVec3(buf + offset, shot.obj.target.pos, NUM_BYTES_V3);
    return offset;
}

void ShotManager::unSerializeShot(uint8_t* buf,
                                  int length,
                                  int player_id,
                                  bool ignoreSelf)
{
    if (length != NUM_BYTES_INDEX + NUM_BYTES_V3 * 2)
    {
        fprintf(stderr,
                "Cannot unserialize shot with nonstandard buffer length. "
                "Expected %d, actual %d\n",
                NUM_BYTES_INDEX + NUM_BYTES_V3 * 2, length);
    }
    int offset = 0;
    int index = assignNumFromBytes(buf + offset, NUM_BYTES_INDEX);
    offset += NUM_BYTES_INDEX;

	int lowbound = selfIndexCalc(0, player_id);
    int highbound = selfIndexCalc(MAX_SHOTS_PER_PLAYER-1, player_id);

    if (index >= lowbound &&
        index <= highbound)
    {
        return;
    }

    Shot shot = getGlobalShotAtIndex(index);
    glm::vec3 temp = assignVec3FromBytes(buf + offset, NUM_BYTES_V3);
    shot.obj.source.pos = temp;
    offset += NUM_BYTES_V3;
    temp = assignVec3FromBytes(buf + offset, NUM_BYTES_V3);
    shot.obj.target.pos = temp;
    offset += NUM_BYTES_V3;

    shot.animate();

    setGlobalShotAtIndex(shot, index);
}

void ShotManager::unitTest()
{
    for (int i = 0; i < 10000; i++)
    {
        uint8_t buf[64];

        glm::vec3 expect = glm::vec3(i, 22.2, -123.3);
        assignBytesFromVec3(buf, expect, NUM_BYTES_V3);

        glm::vec3 actual = assignVec3FromBytes(buf, NUM_BYTES_V3);

        if (glm::distance(actual, expect) > 0.01)
        {
            abort();
        }
    }
}

ShotManager::ShotManager(int players)
{
    shots = std::vector<Shot>();
    nextShotIndex = 0;
    lastShotTime = 0;
    initialized = true;

    unitTest();

    for (int i = 0; i < players; i++)
    {
        for (int j = 0; j < MAX_SHOTS_PER_PLAYER; j++)
        {
            Shot s;
            // s.playerId = i;
            s.obj = InterpObject();
            s.obj.show = 0;
            s.obj.dosin = 0;
            shots.push_back(s);
        }
    }
}

bool ShotManager::isInitialized() { return initialized; }

int ShotManager::selfIndexCalc(int index, int myPlayerId)
{
    if (index >= MAX_SHOTS_PER_PLAYER)
    {
        fprintf(stderr, "ShotManager client asked for a bad index\n");
        return -1;
    }

    int baseindex = ((int)myPlayerId) * MAX_SHOTS_PER_PLAYER;
    index += baseindex;
    if (index >= shots.size())
    {
        fprintf(stderr,
                "ShotManager has an internal list error (getMyShotAtIndex)\n");
        return -1;
    }

    return index;
}

Shot ShotManager::getGlobalShotAtIndex(int index)
{
    Shot ret;

    shotMutex.lock();

    if (index >= 0 && index < shots.size()) ret = shots.at(index);

    shotMutex.unlock();

    return ret;
}

void ShotManager::setGlobalShotAtIndex(Shot shot, int index)
{
    shotMutex.lock();

    if (index >= 0 && index < shots.size()) shots.data()[index] = shot;

    shotMutex.unlock();
}

Shot ShotManager::getMyShotAtIndex(int index, int myPlayerId)
{
    Shot ret;

    index = selfIndexCalc(index, myPlayerId);
    return getGlobalShotAtIndex(index);
}

void ShotManager::setMyShotAtIndex(Shot shot, int index, int myPlayerId)
{
    index = selfIndexCalc(index, myPlayerId);

    setGlobalShotAtIndex(shot, index);
}

void ShotManager::shootAndSendToServer(glm::vec3 targetPos,
                                       int myPlayerId,
                                       bool cooldown,
                                       float currentTime)
{
    int index;

    if (cooldown)
    {
        if (currentTime - lastShotTime < 0.25f)
            return;
        else
            lastShotTime = currentTime;
    }

    for (int i = 0; i < MAX_SHOTS_PER_PLAYER;
         i++, nextShotIndex++, nextShotIndex %= MAX_SHOTS_PER_PLAYER)
    {
        index = selfIndexCalc(nextShotIndex, myPlayerId);
        if (!shots.at(index).obj.show)
        {
            break;
        }
    }

    Shot ball = getMyShotAtIndex(nextShotIndex, myPlayerId);

    ball.obj.source.pos = glm::vec3(0, -8, 0);
    ball.obj.target.pos = targetPos;
    ball.animate();

    int result = serializeShot(tmpWriteBuf, ball, index);
    clientMsgWrite(MSG_NEW_SHOT_FROM_CLIENT, tmpWriteBuf, result);

    setMyShotAtIndex(ball, nextShotIndex, myPlayerId);
}

void ShotManager::advanceShots(float frametime)
{
    shotMutex.lock();
    for (int i = 0; i < shots.size(); i++)
    {
        Shot& ball = shots.data()[i];

        if (!ball.obj.show) continue;

        ball.obj.interp += 2.25 * frametime;
        ball.obj.interpBetween();

        if (ball.obj.interp > 0.99)
        {
            ball.obj.show = 0;
        }
    }
    shotMutex.unlock();
}

void ShotManager::drawShots(std::shared_ptr<Program> prog,
                            Shape shape,
                            ColorList& color_list)
{
    shotMutex.lock();

    for (int i = 0; i < shots.size(); i++)
    {
        Shot& ball = shots.data()[i];

        if (!ball.obj.show) continue;

        glm::vec3 clr = color_list.get_color(i / MAX_SHOTS_PER_PLAYER);
        glUniform3f(prog->getUniform("bonuscolor"), clr.x, clr.y, clr.z);
        ball.obj.sendModelMatrix(prog, glm::mat4(1));
        shape.draw(prog, 0);
    }

    shotMutex.unlock();
}

void ShotManager::fillCollisionHandlerWithMyShots(CollisionHandler& collision,
                                                  int myPlayerId)
{
    shotMutex.lock();
    collision.prepTableWithShots(
        shots, COLLISION_RADIUS, myPlayerId * MAX_SHOTS_PER_PLAYER,
        myPlayerId * MAX_SHOTS_PER_PLAYER + MAX_SHOTS_PER_PLAYER);
    shotMutex.unlock();
}
