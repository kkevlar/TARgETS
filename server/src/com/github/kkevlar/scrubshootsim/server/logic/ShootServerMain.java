package com.github.kkevlar.scrubshootsim.server.logic;
import java.io.IOException;
import java.util.Optional;
import java.util.concurrent.Semaphore;

public class ShootServerMain
{
	/*
	 * public static int getSpecialInt() { double d = Math.random(); if(d < .98) {
	 * return 0; } else if(d < .99) { return 1; } else { return 2; } }
	 */

	public static void main (String[] args) throws IOException 
	{

		ShootServer s = new ShootServer(25567);
		GameInstance game = new GameInstance(s);
		s.game = game;

		//PlayerUpdater u = new PlayerUpdater(s);


		int i = 0;
		while (true)
		{
			/*
			 * String scrubString = String.format("add:%d,%d~%d~%d", (int) (Math.random() *
			 * 550), (int) (Math.random() * 550), getSpecialInt(), i++);
			 * s.sentToAll(scrubString); try { Thread.sleep(150); } catch
			 * (InterruptedException e) { // TODO Auto-generated catch block
			 * e.printStackTrace(); }
			 */


			Box b = game.addNewBox();

			ToClientMsg msg = ToClientMsgFactory.addMessage(b.myIndex, b.angle, b.height);
			try {
				s.getSemaphore().acquire();
			} catch (InterruptedException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			s.sendToAll(msg);
			s.getSemaphore().release();

			for(i = 0; i < 100; i++)
			{
				try {
					s.getSemaphore().acquire();
				} catch (InterruptedException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				Optional<ToClientMsg> omsg = ToClientMsgFactory.cursorListMessage(s.getPlayers());
				omsg.ifPresent(m -> s.sendToAll(m));
				s.getSemaphore().release();
				try {
					Thread.sleep(5);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}

		/*
		 * while (true) { for (String key : s.getRooms().keySet()) { Room room =
		 * s.getRooms().get(key); if(!room.isHasPrintedNominations() &&
		 * System.currentTimeMillis() - room.getStartTime() > 45000) {
		 * 
		 * if (room.checkSuggestion()) room.printNominations(); else {
		 * room.noSuggestions(); room.resetSuggestionPhase(); } }
		 * 
		 * if(room.isHasPrintedNominations() && !room.isHasPrintedFinal() &&
		 * System.currentTimeMillis() - room.getStartTime() > 45000) {
		 * room.printVoteResult(); } if (room.isFinished()) { room.closeRoom();
		 * s.getRooms().remove(room.getKey()); } } }
		 */

	}


}
