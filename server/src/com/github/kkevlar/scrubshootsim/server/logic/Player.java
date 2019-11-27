package com.github.kkevlar.scrubshootsim.server.logic;

import java.io.IOException;
import java.net.Socket;
import java.util.Arrays;

public class Player extends GenericServerClient {

	private byte[] pos;
	private byte id;
	private ShootServer server;
	public int score = 0;
	public int chargeCount = 0;
	public boolean numsChanged = false;
	private MessageContext context;

	public Player(byte id, ShootServer server, Socket s) throws IOException 
	{
		super(server, s);
		System.out.printf("Constructing player %d\n", id);
		this.setId(id);
		this.server = server;

		this.sendToPlayer(ToClientMsgFactory.setPlayerIdMessage(getId()));
	}

	@Override
	public void clientInit() 
	{
		//server.sentToAll("id:" + id);
	}

	private void setNums(String nums) 
	{
		try		
		{
			String[] splits = nums.split(",");
			//			int tempScore = Integer.parseInt(splits[0]);
			int tempCharge = Integer.parseInt(splits[1]);
			//			if(tempScore != score)
			//			{
			//				score = tempScore;
			//				numsChanged = true;
			//			}
			if(chargeCount != tempCharge)
			{
				chargeCount = tempCharge;
				numsChanged = true;
			}			
		}
		catch(RuntimeException ex)
		{

		}
	}

	@Override
	public void clientDisconnected() {
		// TODO Auto-generated method stub

	}

	public byte getId() {
		return id;
	}

	private void setId(byte id) {
		this.id = id;
	}

	public MessageContext getContext() {
		return context;
	}

	public void setContext(MessageContext context) {
		this.context = context;
	}

	public byte[] getPos() {
		return pos;
	}

	public void setPos(byte[] pos, int length) {
		this.pos = Arrays.copyOf(pos, length);
	}

	public void sendToPlayer(byte[] data, int lengthFromRead) 
	{
		try {
			getWriter().write(data, 0, lengthFromRead);
			getWriter().flush();
		} catch (IOException e) {
		}
	}


	public void sendToPlayer(ToClientMsg msg)
	{
		byte[] buf = new byte[2 + msg.length];
		buf[0] = (byte) msg.code;
		buf[1] = (byte) msg.length;
		for(int i= 0; i < msg.length; i++)
		{
			buf[i+2] = (byte) msg.data[i];
		}
		sendToPlayer(buf, buf.length);
	}

}
