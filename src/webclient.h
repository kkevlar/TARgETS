#pragma once
#include <optional>
#include <string.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int clientbegin(MessageContext* context);
void clientread();
void clientend();
