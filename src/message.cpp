

#include "message.h"
#include <stdio.h>


static void (*handler_table[0x100])(MessageContext*, uint8_t*, int);
static MessageContext* mycontext;

void handleMessage(MessageId id, uint8_t* message, int length)
{
	(*handler_table[id])(mycontext, message, length);
}

void handlerDefault(MessageContext* context, uint8_t* data, int length)
{
	fprintf(stderr, "Ignoring message of %d bytes: this message type is unimplemented\n", length);
}

#define PI_CONST ((float)( 103993.0f / 33102.0f))

static inline float map(float value, float min1, float max1, float min2,
	float max2) {
	return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void handlerAddBox(MessageContext* context, uint8_t* data, int length)
{
	if (length != 6)
		return;

 	uint32_t iindex = (0x00ff & data[0]) | (0xff00 &(data[1] << 8));
	uint32_t iangle = (0x00ff & data[2]) | (0xff00 & (data[3] << 8));
	uint32_t iheight = (0x00ff & data[4]) | (0xff00 &(data[5] << 8));

	float angle = iangle * 0.005f;/*map(iangle, 0, 0x200, -PI_CONST /4, PI_CONST/4);*/
			float height = map(iheight, 0, 400, -5 , 5);
			glm::vec3 pos = glm::vec3(0, height, 0);
			pos.x = 25 * sin(angle);
			pos.z = 25 * cos(angle);

			Cube* cube = &context->boxes->data()[iindex];
			cube->show = 1;
			cube->hit = 0;
			cube->dosin = 1;
			cube->target.pos = pos;
			cube->target.scale = glm::vec3(0.5, 0.5, 0.1);
			cube->target.rot = glm::vec3(0, angle, 0);
			cube->source = cube->target;
			cube->source.pos.z *= 100;
			cube->source.pos.x *= 100;
			cube->source.scale = glm::vec3(0.1, 0.1, 0.1);
			cube->phase = map(rand() % 1000, 0, 1000, -0.3, 0);
			cube->resetInterp();

	
}

void initMessageHandler(MessageContext* context)
{
	mycontext = context;
	int i;

	for (i = 0; i < 0x100; i++)
	{
		handler_table[i] = handlerDefault;
	}

	handler_table[MSG_ADDBOX] = handlerAddBox;
}




