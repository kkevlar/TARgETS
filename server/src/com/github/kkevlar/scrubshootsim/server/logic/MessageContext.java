package com.github.kkevlar.scrubshootsim.server.logic;

public class MessageContext 
{
	private GameInstance game;
	private Player player;
	public MessageContext(GameInstance game2, Player nuser) {
		setGame(game2);
		setPlayer(nuser);
	}
	public GameInstance getGame() {
		return game;
	}
	public void setGame(GameInstance game) {
		this.game = game;
	}
	public Player getPlayer() {
		return player;
	}
	public void setPlayer(Player player) {
		this.player = player;
	}
}
