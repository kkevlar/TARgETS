package com.github.kkevlar.scrubshootsim.server.logic;

public class GameInstance
{
    private Box[] boxes;
    private ShootServer server;
    private int addIndex;
    private byte winningPlayerId;
    private int winningScore;

    public GameInstance(ShootServer server)
    {
        setBoxes(new Box[0x80]);
        for (int i = 0; i < getBoxes().length; i++)
        {
            getBoxes()[i] = new Box(i);
            getBoxes()[i].visible = false;
        }
        this.setServer(server);
        addIndex = 0;
        winningScore = -1;
        winningPlayerId = -1;
    }

    public Box addNewBox()
    {
        for (int i = 0; i < getBoxes().length; i++)
        {
            if (!getBoxes()[addIndex].visible)
            {
                addIndex += 1;
                if (addIndex >= getBoxes().length)
                {
                    addIndex -= getBoxes().length;
                }
                break;
            }

            addIndex += 1;
            if (addIndex >= getBoxes().length)
            {
                addIndex -= getBoxes().length;
            }
        }

        getBoxes()[addIndex].angle = (int) (Math.random() * 200);
        getBoxes()[addIndex].height = (int) (200 + (200 * Math.random()));

        return getBoxes()[addIndex];
    }

    public Box[] getBoxes()
    {
        return boxes;
    }

    public void setBoxes(Box[] boxes)
    {
        this.boxes = boxes;
    }

    public ShootServer getServer()
    {
        return server;
    }

    public void setServer(ShootServer server)
    {
        this.server = server;
    }

    public void checkSendWinningScore(Player p)
    {
        if (winningScore < p.score)
        {
            winningScore = p.score;
            if (winningPlayerId != p.getId())
            {
                winningPlayerId = p.getId();
                ToClientMsg msg = ToClientMsgFactory.setWinningPlayer(winningPlayerId);
                getServer().sendToAll(msg);
            }
        }
    }
}
