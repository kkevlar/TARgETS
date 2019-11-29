

#include "Shot.h"

void Shot::animate() {
    obj.source.scale = glm::vec3(4, 4, 4);
    obj.source.rot = glm::vec3(0, 0, 0);

    obj.target.rot = glm::vec3(0, 0, 0);
    obj.target.scale = glm::vec3(0.1, 0.1, 0.1);
    obj.phase = 0;
    obj.show = 1;
    obj.resetInterp();
}
