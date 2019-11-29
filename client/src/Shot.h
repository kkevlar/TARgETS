#pragma once
#include <stdint.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>
#include "Cube.h"
#include "GLSL.h"

class Shot
{
   public:
    InterpObject obj;
    void animate();
};
