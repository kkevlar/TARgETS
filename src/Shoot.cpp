

#include "Shoot.h"

#define MAX_SHOTS_PER_PLAYER 64

ShotManager::ShotManager(int players, uint8_t my_player_id)
{
    shots = std::vector<Shot>();
    for (int i = 0; i < players; i++)
    {
        for (int j = 0; j < MAX_SHOTS_PER_PLAYER; j++)
        {
            Shot s;
            s.playerId = i;
            if (i == my_player_id) s.mine = true;
            s.cube = InterpObject();
            s.cube.show = 0;
            s.cube.dosin = 0;
            shots.push_back(s);
        }
    }
}
