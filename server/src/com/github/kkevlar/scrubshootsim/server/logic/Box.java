package com.github.kkevlar.scrubshootsim.server.logic;

public class Box 
{
	boolean visible;
	int angle;
	int height;
	int myIndex;
	
	public Box(int myIndex)
	{
		visible = false;
		this.myIndex = myIndex;
	}
}
