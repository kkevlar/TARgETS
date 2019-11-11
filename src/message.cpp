

#include "message.h"
#include <stdio.h>


static void (*handler_table[0x100])(MessageContext, uint8_t*, int);
static MessageContext* mycontext;

void handleMessage(MessageId id, uint8_t* message, int length)
{

}

void defaultHandler(MessageContext context, uint8_t* data, int length)
{
	fprintf(stderr, "Ignoring message of %d bytes: this message type is unimplemented\n");
}

void initMessageHandler(MessageContext* context)
{
	mycontext = context;
	int i;

	for (i = 0; i < 0x100; i++)
	{
		handler_table[i] = &defaultHandler;
	}
}




