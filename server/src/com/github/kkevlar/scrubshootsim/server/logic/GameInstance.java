package com.github.kkevlar.scrubshootsim.server.logic;

public class GameInstance 
{
	private Box[] boxes;
	private ShootServer server;
	private int addIndex;

	public GameInstance(ShootServer server)
	{
		setBoxes(new Box[0x80]);
		for(int i = 0; i < getBoxes().length; i++)
		{
			getBoxes()[i] = new Box(i);
			getBoxes()[i].visible = false;
		}
		this.setServer(server);
		addIndex = 0;
	}
	
	public Box addNewBox()
	{
		for(int i = 0; i < getBoxes().length; i++)
		{
			if(!getBoxes()[addIndex].visible)
			{
				addIndex+=1;
				if(addIndex >= getBoxes().length)
				{
					addIndex -= getBoxes().length;
				}
				break;
			}
			
			addIndex+=1;
			if(addIndex >= getBoxes().length)
			{
				addIndex -= getBoxes().length;
			}
		}
		
		getBoxes()[addIndex].angle = addIndex *5 ;
		getBoxes()[addIndex].height = (int) (200 + (200*Math.sin((((float)addIndex)/2.0f))));
		
		return getBoxes()[addIndex];
	}	

	public Box[] getBoxes() {
		return boxes;
	}

	public void setBoxes(Box[] boxes) {
		this.boxes = boxes;
	}

	public ShootServer getServer() {
		return server;
	}

	public void setServer(ShootServer server) {
		this.server = server;
	}
}
