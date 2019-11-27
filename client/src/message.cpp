

#include "message.h"
#include <stdio.h>

static void (*handler_table[0x100])(MessageContext *,
                                    MessageId id,
                                    uint8_t *,
                                    uint8_t);
static MessageContext *mycontext;

void handleMessage(MessageId id, uint8_t *message, int length)
{
    (*handler_table[id])(mycontext, id, message, length);
}

void handlerDefault(MessageContext *context,
                    MessageId id,
                    uint8_t *data,
                    uint8_t length)
{
    fprintf(stderr,
            "Ignoring message (id=%4u) of %4u bytes: this message type is "
            "unimplemented\n",
            id, length);
}

#define PI_CONST ((float)(103993.0f / 33102.0f))

static inline float map(
    float value, float min1, float max1, float min2, float max2)
{
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

#define FLOAT_DEGREES 10000

int assignBytesFromNum(uint8_t *buf, int num, int bytes)
{
    for (int i = 0; i < bytes; i++)
    {
        buf[i] = ((num >> (8 * i)) & 0x00FF);
    }
    return bytes;
}

int assignBytesFromFloat(uint8_t *buf, float num, int bytes)
{
    buf[0] = (!(!(num < 0)));
    assignBytesFromNum(buf + 1, abs(num) * (FLOAT_DEGREES + 0.0f), bytes - 1);
    return bytes;
}

int assignBytesFromVec3(uint8_t *buf, glm::vec3 vec)
{
    int offset = 0;
    offset += assignBytesFromFloat(buf + offset, vec.x, 4);
    offset += assignBytesFromFloat(buf + offset, vec.y, 4);
    offset += assignBytesFromFloat(buf + offset, vec.z, 4);
    return offset;
}

uint32_t assignNumFromBytes(uint8_t *buf, int bytes)
{
    uint32_t num = 0;
    for (int i = 0; i < bytes; i++)
    {
        num |= ((buf[i] & 0xFF) << (8 * i));
    }
    return num;
}

float assignFloatFromBytes(uint8_t *buf, int bytes)
{
    uint32_t num = 0;
    num = assignNumFromBytes(buf + 1, bytes - 1);
    float f = (float)num;
    f /= (FLOAT_DEGREES + 0.0f);

    if (buf[0]) f *= -1;
    return f;
}

glm::vec3 assignVec3FromBytes(uint8_t *buf, int bytes)
{
    if (bytes != 12)
    {
        fprintf(stderr,
                "Attempted to unserialize a vec3 with a non-standard number of "
                "bytes\n");
        return glm::vec3(0, 0, 0);
    }

	uint8_t offset = 0;
    glm::vec3 result = glm::vec3(0, 0, 0);
        result.x = assignFloatFromBytes(buf + offset, 4);
    offset += 4;
        result.y = assignFloatFromBytes(buf + offset, 4);
    offset += 4;
        result.z = assignFloatFromBytes(buf + offset, 4);
    offset += 4;
}

void handlerAddBox(MessageContext *context,
                   MessageId id,
                   uint8_t *data,
                   uint8_t length)
{
    if (length != 6) return;

    uint32_t iindex = assignNumFromBytes(data + 0, 2);
    uint32_t iangle = assignNumFromBytes(data + 2, 2);
    uint32_t iheight = assignNumFromBytes(data + 4, 2);

    float angle =
        iangle * 0.005f; /*map(iangle, 0, 0x200, -PI_CONST /4, PI_CONST/4);*/
    float height = map(iheight, 0, 400, -5, 5);
    glm::vec3 pos = glm::vec3(0, height, 0);
    pos.x = 25 * sin(angle);
    pos.z = 25 * cos(angle);

    Target cube = Target();
    cube.show = 1;
    cube.hit = 0;
    cube.dosin = 1;
    cube.target.pos = pos;
    cube.target.scale = glm::vec3(0.5, 0.5, 0.1);
    cube.target.rot = glm::vec3(0, angle, 0);
    cube.source = cube.target;
    cube.source.pos.z *= 100;
    cube.source.pos.x *= 100;
    cube.source.scale = glm::vec3(0.1, 0.1, 0.1);
    cube.phase = map(rand() % 1000, 0, 1000, -0.3, 0);
    cube.resetInterp();

    context->mutex_boxes.lock();
    if (iindex >= context->boxes->size())
        fprintf(stderr, "Add command sent too large box index! \n");
    else
        context->boxes->data()[iindex] = cube;
    context->mutex_boxes.unlock();
}

void handlerRemoveBox(MessageContext *context,
                      MessageId id,
                      uint8_t *data,
                      uint8_t length)
{
    uint8_t shooter_id = data[0];
    uint32_t iindex = assignNumFromBytes(data + 1, 2);

    bool error = 0;
    Target cube;

    context->mutex_boxes.lock();
    if (iindex >= context->boxes->size())
    {
        fprintf(stderr, "Remove command sent too large box index! \n");
        error = 1;
    }
    else
    {
        cube = context->boxes->data()[iindex];
        error = 0;
    }
    context->mutex_boxes.unlock();

    if (error) return;

    cube.hit = 1;
    cube.source = cube.postInterp;
    cube.target = cube.source;
    cube.target.scale *= 0.25f;
    cube.target.rot.y = 8 * PI_CONST;
    cube.phase = 0.1;
    cube.dosin = 0;
    cube.bonuscolor = context->color_list.get_color(shooter_id);
    cube.resetInterp();

    context->mutex_boxes.lock();
    if (iindex >= context->boxes->size())
        fprintf(stderr, "Remove command sent too large box index! \n");
    else
        context->boxes->data()[iindex] = cube;
    context->mutex_boxes.unlock();
}

void handlerCursorList(MessageContext *context,
                       MessageId id,
                       uint8_t *data,
                       uint8_t length)
{
    if (length % (1 + 5 + 5) != 0)
    {
        printf("Cursor list gonna break! %d\n", length);
    }
    for (int i = 0; i < length; i += (1 + 5 + 5))
    {
        int index = assignNumFromBytes(data + i, 1);

        context->mutex_cursors.lock();
        if (data[index] < context->cursors->size())
        {
            context->cursors->data()[index].angle =
                assignFloatFromBytes(data + 1 + i, 5);
            context->cursors->data()[index].height =
                assignFloatFromBytes(data + 6 + i, 5);
            context->cursors->data()[index].show = 1;
        }
        context->mutex_cursors.unlock();
    }
}

void handlerSetOwnPid(MessageContext *context,
                      MessageId id,
                      uint8_t *data,
                      uint8_t length)
{
    context->player_id = data[0];
    printf("My pid %u\n", context->player_id);
}

void initMessageHandler(MessageContext *context)
{
    mycontext = context;
    int i;

    for (i = 0; i < 0x100; i++)
    {
        handler_table[i] = handlerDefault;
    }

    handler_table[MSG_ADDBOX] = handlerAddBox;
    handler_table[MSG_CURSOR_LIST] = handlerCursorList;
    handler_table[MSG_REMBOX] = handlerRemoveBox;
    handler_table[MSG_SET_YOUR_PID] = handlerSetOwnPid;
}
