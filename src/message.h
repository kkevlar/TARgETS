#pragma once

#include <vector>
#include <mutex>
#include <stdint.h>
#include "Cube.h"

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
#define MSG_PID_COLORS ((MessageId) 11) 
#define MSG_BALL_PROGRESS_UPDATE ((MessageId) 12) 
#define MSG_BALL_PROGRESS_LIST ((MessageId) 13) 

typedef struct MessageContext
{
	std::vector<Cube>* boxes;
	std::mutex mutex_boxes;
	std::vector<CylCoords>* cursors;
	std::mutex mutex_cursors;
} MessageContext;

void assignBytesFromNum(uint8_t* buf, int num, int bytes);
void assignBytesFromFloat(uint8_t* buf, float num, int bytes);
uint32_t assignNumFromBytes(uint8_t* buf, int bytes);
float assignFloatFromBytes(uint8_t* buf, int bytes);
void initMessageHandler(MessageContext* context);
void handleMessage(MessageId id, uint8_t* message, int length);


