package com.github.kkevlar.scrubshootsim.server.logic;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

public class ToClientMsgFactory 
{
	public static ToClientMsg addMessage(int index, int angle, int height)
	{
		ToClientMsg m = new ToClientMsg();

		m.code = MessageHandler.MSG_ADDBOX;
		m.data = new byte[
		                  2 /* index */
		                  + 0 /* visible */
		                  + 2 /* angle */
		                  + 2 /* height */];

		m.data[0] = (byte) (0x0000FF & index);
		m.data[1] = (byte) ((0x00FF00 & index)>>8);
		m.data[2] = (byte) (0x0000FF & angle);
		m.data[3] = (byte) ((0x00FF00 & angle)>>8);
		m.data[4] = (byte) (0x0000FF & height);
		m.data[5] = (byte) ((0x00FF00 & height)>>8);
		m.length = m.data.length;
		return m;
	}

	public static Optional<ToClientMsg> cursorListMessage(List<Player> players)
	{
		ArrayList<Byte> poses = new ArrayList<Byte>();
		int psize = players.size();
		for(int i = 0; i < psize; i++)
		{
			Player p = players.get(i);
			if(p.getPos() != null)
			{
				poses.add(p.getId());
				for(int j = 0; j < p.getPos().length; j++) 
				{
					poses.add(p.getPos()[j]);
				}
			}
			else
			{
				System.err.printf("\t\t\tbad bad bad\n");
			}
			
		}
		
		if(poses.size() == 0)
			return Optional.empty();

		ToClientMsg m = new ToClientMsg();
		m.data = new byte[poses.size()];

		m.code = MessageHandler.MSG_CURSOR_LIST;
		m.length = m.data.length;
		for(int i = 0; i < poses.size(); i++)
		{
			m.data[i] = poses.get(i);
		}
		
		if(m.data.length != players.size() * 11)
		{
			System.err.printf("Cursorlistbug\n");
		}
		
		return Optional.of(m);
	}

	public static ToClientMsg setPlayerIdMessage(byte id) 
	{
		ToClientMsg msg = new ToClientMsg();
		
		msg.code = MessageHandler.MSG_SET_YOUR_PID;
		msg.length = 1;
		msg.data = new byte[]{id};
		
		return msg;
	}
	
	
}
