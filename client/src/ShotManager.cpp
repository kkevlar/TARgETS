

#include "ShotManager.h"
#include "message.h"
#include "webclient.h"

#define MAX_SHOTS_PER_PLAYER 32

/* Oh no! A global mutex :(.... Idk how to make this a member of the class but
 * who cares we will only ever need one shot manager */
std::mutex shotMutex;
static uint8_t tmpWriteBuf[256];

ShotManager::ShotManager(int players)
{
    shots = std::vector<Shot>();
    nextShotIndex = 0;
    lastShotTime = 0;
    initialized = true;

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

Shot ShotManager::getMyShotAtIndex(int index, int myPlayerId)
{
    Shot ret;

    shotMutex.lock();

    index = selfIndexCalc(index, myPlayerId);
    if (index >= 0) ret = shots.at(index);

    shotMutex.unlock();

    return ret;
}

void ShotManager::setMyShotAtIndex(Shot shot, int index, int myPlayerId)
{
    shotMutex.lock();

    index = selfIndexCalc(index, myPlayerId);

    if (index >= 0) shots.data()[index] = shot;

    shotMutex.unlock();
}

void ShotManager::shootAndSendToServer(glm::vec3 targetPos,
                                       int myPlayerId,
                                       float currentTime)
{
    int index;

    if (currentTime - lastShotTime < 0.25f)
        return;
    else
        lastShotTime = currentTime;

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

    ball.obj.source.scale = glm::vec3(4, 4, 4);
    ball.obj.source.pos = glm::vec3(0, -8, 0);
    ball.obj.source.rot = glm::vec3(0, 0, 0);
    ball.obj.target = ball.obj.source;

    ball.obj.target.pos = targetPos;
    ball.obj.target.scale = glm::vec3(0.1, 0.1, 0.1);
    ball.obj.phase = 0;
    ball.obj.show = 1;
    ball.obj.resetInterp();

    int offset = 0;
    offset += assignBytesFromNum(tmpWriteBuf + offset, index, 3);
    offset += assignBytesFromVec3(tmpWriteBuf + offset, ball.obj.source.pos);
    offset += assignBytesFromVec3(tmpWriteBuf + offset, ball.obj.target.pos);
    clientMsgWrite(MSG_NEW_SHOT_FROM_CLIENT, tmpWriteBuf, offset);

    setMyShotAtIndex(ball, nextShotIndex, myPlayerId);
}

void ShotManager::advanceShots(float frametime)
{
    shotMutex.lock();
    for (int i = 0; i < shots.size(); i++)
    {
        Shot& ball = shots.data()[i];

        if (!ball.obj.show) continue;

        ball.obj.interp += 3 * frametime;
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
        shape.draw(prog);
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
