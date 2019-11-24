

#include "ShotManager.h"

#define MAX_SHOTS_PER_PLAYER 64

ShotManager::ShotManager(int players)
{
    shots = std::vector<Shot>();
    nextShotIndex = 0;

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
    index = selfIndexCalc(index, myPlayerId);
    if (index < 0)
        return Shot();
    else
        return shots.at(index);
}

void ShotManager::setMyShotAtIndex(Shot shot, int index, int myPlayerId)
{
    index = selfIndexCalc(index, myPlayerId);

    if (index >= 0) shots.data()[index] = shot;
}

void ShotManager::shootAndSendToServer(glm::vec3 targetPos, int myPlayerId)
{
    int index;
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

    setMyShotAtIndex(ball, nextShotIndex, myPlayerId);
}

void ShotManager::advanceShots(float frametime)
{
    for (int i = 0; i < shots.size(); i++)
    {
        Shot& ball = shots.data()[i];

        if (!ball.obj.show) return;

        ball.obj.interp += 3 * frametime;
        ball.obj.interpBetween();

        if (ball.obj.interp > 0.99)
        {
            ball.obj.show = 0;
        }
    }
}

void ShotManager::drawShots(std::shared_ptr<Program> prog,
                            Shape shape,
                            ColorList& color_list)
{
    for (int i = 0; i < shots.size(); i++)
    {
        Shot& ball = shots.data()[i];

        if (!ball.obj.show) return;

        glm::vec3 clr = color_list.get_color(i / MAX_SHOTS_PER_PLAYER);
        glUniform3f(prog->getUniform("bonuscolor"), clr.x, clr.y, clr.z);
        ball.obj.sendModelMatrix(prog, glm::mat4(1));
        shape.draw(prog);
    }
}

void ShotManager::fillCollisionHandlerWithMyShots(CollisionHandler& collision)
{
    collision.prepTableWithShots(shots, 0, shots.size(), COLLISION_RADIUS);
}
