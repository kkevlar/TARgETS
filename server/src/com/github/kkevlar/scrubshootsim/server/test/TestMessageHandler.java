package com.github.kkevlar.scrubshootsim.server.test;

import static org.junit.jupiter.api.Assertions.*;

import org.junit.jupiter.api.Test;

import com.github.kkevlar.scrubshootsim.server.logic.ToServerMsg;
import com.github.kkevlar.scrubshootsim.server.logic.MessageHandler;

class TestMessageHandler
{

	@Test
	void testConstructor()
	{
		MessageHandler h = new MessageHandler();
		
		assertEquals(0x100, h.table.size());
		
		ToServerMsg m = new ToServerMsg();
		m.code = 70;
		h.table.get(0).accept(m);
	}
	
	@Test
	void testRemBox()
	{
		MessageHandler h = new MessageHandler();
		
		ToServerMsg m = new ToServerMsg();
		m.code = MessageHandler.MSG_REMBOX;
		m.context = null;
		
		assertThrows(NullPointerException.class, () -> {
	        h.handleMessage(m);
	    });
	}

}
