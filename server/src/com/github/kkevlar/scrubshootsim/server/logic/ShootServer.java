package com.github.kkevlar.scrubshootsim.server.logic;
import java.io.IOException;

import java.util.concurrent.Semaphore;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.LinkedList;
import java.util.List;


public class ShootServer extends ServerSocket implements Runnable
{
	private boolean up;
	private LinkedList<Player> users;
	public GameInstance game;
	private Semaphore semaphore;

	public ShootServer(int port) throws IOException
	{
		super(port);
		up = true;
		users = new LinkedList<Player>();
		game = null;
		semaphore = new Semaphore(1,true);
		new Thread(this).start();
	}

	public void run()
	{
		byte id = 0;
		try {
			Thread.sleep(100);
		} catch (InterruptedException e1) {
		}
		while(game == null)
		{
			System.err.println("Cant accept clients till I get context.");
			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
			}
		}
		while (up)
		{
			Socket socket = null;
			try {
				socket = accept();
				Player nuser = new Player(id++,this,socket,game);
				users.add(nuser);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	public List<Player> getPlayers() {
		return users;
	}

	public void sendToAll(byte[] data, int lengthFromRead)
	{
		
			int size = users.size();
		
		for (int i = 0; i <size; i++)
		{
			try {
				users.get(i).getWriter().write(data, 0, lengthFromRead);
				users.get(i).getWriter().flush();
			} catch (IOException e) {
			}		
		}
	}

	public void sendToAll(ToClientMsg msg)
	{
		byte[] buf = new byte[2 + msg.length];
		buf[0] = (byte) msg.code;
		buf[1] = (byte) msg.length;
		for(int i= 0; i < msg.length; i++)
		{
			buf[i+2] = (byte) msg.data[i];
		}
		sendToAll(buf, buf.length);
	}

	public Semaphore getSemaphore() {
		return semaphore;
	}

}