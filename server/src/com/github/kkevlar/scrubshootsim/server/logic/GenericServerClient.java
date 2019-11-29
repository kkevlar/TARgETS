package com.github.kkevlar.scrubshootsim.server.logic;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.Socket;


public abstract class GenericServerClient implements Runnable
{
	private ShootServer server;
	private Socket socket;
	private BufferedOutputStream writer;
	private BufferedInputStream reader;
	private boolean isConnected = true;

	public GenericServerClient(ShootServer server, Socket s) throws IOException
	{
		this.socket = s;
		this.setServer(server);
		this.reader = new BufferedInputStream(socket.getInputStream());
		this.writer = new BufferedOutputStream(socket.getOutputStream());
		new Thread(this).start();
	}

	public abstract void clientInit();

	public void run()
	{

		clientInit();

		int code = 0;
		int length = -1;
		int bufindex = 0;
		byte[] buf = new byte[512];
		byte[] databuf = new byte[256];
		MessageHandler handler = new MessageHandler();

		while( this.getContext() == null)
		{
			
		}

		while(isConnected)
		{
			int bytesread = 0;
			try {
				bytesread = reader.read(buf);
			} catch (IOException e) {

			}

			if (bytesread > 0)
			{
				for (int i = 0; i < bytesread; i++)
				{
					if (code == 0)
					{
						code = buf[i];
						bufindex = 0;
						length = -1;
					}
					else if (length == -1)
					{
						length = buf[i];
						bufindex = 0;
					}
					else if (bufindex < length)
					{						
						databuf[bufindex++]  = buf[i];
					}
					else
					{
						ToServerMsg msg = new ToServerMsg();
						msg.code = code;
						msg.context = this.getContext();
						msg.data = databuf;
						msg.length = length;
						/*
						 * try { server.getSemaphore().acquire(); } catch (InterruptedException e) {
						 * 
						 * e.printStackTrace(); }
						 */					
						handler.handleMessage(msg);
						//server.getSemaphore().release();

						code = 0;
						length = -1;
						bufindex = 0;
						i--;
					}
				}

			}
		}

	}

	protected abstract MessageContext getContext();

	public Socket getSocket()
	{
		return socket;
	}

	public void disconnect()
	{
		try {
			clientDisconnected();
			writer.close();
			reader.close();
			socket.close();
			isConnected = false;
		}
		catch(Exception ex)
		{

		}
	}

	public abstract void clientDisconnected();

	public boolean isConnected() {
		return isConnected;
	}

	public void setConnected(boolean connected) {
		isConnected = connected;
	}

	public BufferedOutputStream getWriter() {
		return writer;
	}

	public ShootServer getServer() {
		return server;
	}

	public void setServer(ShootServer server) {
		this.server = server;
	}
}
