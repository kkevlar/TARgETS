package com.github.kkevlar.scrubshootsim.server.logic;

public class PlayerUpdater implements Runnable {

	private ShootServer server;
	
	public PlayerUpdater(ShootServer server)
	{
		this.server = server;
		new Thread(this).start();
	}

	
	@Override
	public void run() 
	{
//		long last = System.currentTimeMillis();
//		while(true)
//		{
//			if(System.currentTimeMillis() - last > 20)
//			{
//				last = System.currentTimeMillis();
//				
//				String comb = "pos:";
//				for(Player player : server.getPlayers())
//				{
//					comb += player.getPos() + "~";
//				}
//				if(!comb.contains("~"))
//					continue;
//				comb = comb.substring(0, comb.length()-1);
//				for(Player player : server.getPlayers())
//				{
//					player.getWriter().println(comb);
//					player.getWriter().flush();
//				}				
//				System.out.println(comb);	
//				
//				if(!sendNums())
//					continue;
//				
//				comb = "nums:";
//				for(Player player : server.getPlayers())
//				{
//					comb += player.score + "," + player.chargeCount + "~";
//					player.numsChanged = false;
//				}
//				if(!comb.contains("~"))
//					continue;
//				comb = comb.substring(0, comb.length()-1);
//				for(Player player : server.getPlayers())
//				{
//					player.getWriter().println(comb);
//					player.getWriter().flush();
//				}				
//				System.out.println(comb);
//				
//			}
//		}
	}

}
