

#include "Shoot.h"

#define MAX_SHOTS_PER_PLAYER 64

ShotManager::ShotManager(int players)
{
    shots = std::vector<Shot>();
    for (int i = 0; i < players; i++)
    {
        for (int j = 0; j < MAX_SHOTS_PER_PLAYER; j++)
        {
            Shot s;
            s.playerId = i;
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
