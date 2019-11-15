#pragma once
#include <optional>
#include <string.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int clientbegin(MessageContext* context);
void clientread();
void clientMsgWrite(MessageId code, uint8_t* data, int len);
void clientwrite(uint8_t* bytes, int length);
void clientflush();
void clientend();
