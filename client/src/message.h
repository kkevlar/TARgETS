#pragma once

#include <vector>
#include <mutex>
#include <stdint.h>
#include "Cube.h"
#include "ShotManager.h"

typedef uint8_t MessageId;

#define MSG_INVALID ((MessageId) 0) 
#define MSG_HEARTBEAT ((MessageId) 1) 
#define MSG_ADDBOX ((MessageId) 2) 
#define MSG_REMBOX ((MessageId) 3) 
#define MSG_CURSOR_LIST ((MessageId) 4) 
#define MSG_CURSOR_UPDATE ((MessageId) 5) 
#define MSG_SET_YOUR_PID ((MessageId) 6) 
#define MSG_ROOM_LIST ((MessageId) 7) 
#define MSG_ROOM_CHOOSE ((MessageId) 8) 
#define MSG_SCORE_LIST ((MessageId) 9) 
#define MSG_CLIENT_QUIT ((MessageId) 10) 
#define MSG_ADDSHOT ((MessageId) 14) 
#define MSG_NEW_SHOT_FROM_CLIENT ((MessageId) 15) 
#define MSG_REALIGN ((MessageId) 55) 

typedef struct MessageContext
{
	std::vector<Target>* boxes;
	std::mutex mutex_boxes;
	std::vector<CylCoords>* cursors;
        ShotManager* shots;
	std::mutex mutex_cursors;
    int16_t player_id;
    ColorList color_list;
} MessageContext;

int assignBytesFromNum(uint8_t* buf, int num, int bytes);
int assignBytesFromFloat(uint8_t* buf, float num, int bytes);
uint32_t assignNumFromBytes(uint8_t* buf, int bytes);
float assignFloatFromBytes(uint8_t* buf, int bytes);
void initMessageHandler(MessageContext* context);
void handleMessage(MessageId id, uint8_t* message, int length);
int assignBytesFromVec3(uint8_t* buf, glm::vec3 vec, int bytes);
glm::vec3 assignVec3FromBytes(uint8_t* buf, int bytes);