package com.github.kkevlar.scrubshootsim.server.logic;

import java.util.ArrayList;
import java.util.function.Consumer;

public class MessageHandler
{
    public ArrayList<Consumer<ToServerMsg>> table;

    public static final byte MSG_INVALID = 0;
    public static final byte MSG_HEARTBEAT = 1;
    public static final byte MSG_ADDBOX = 2;
    public static final byte MSG_REMBOX = 3;
    public static final byte MSG_CURSOR_LIST = 4;
    public static final byte MSG_CURSOR_UPDATE = 5;
    public static final byte MSG_SET_YOUR_PID = 6;
    public static final byte MSG_ROOM_LIST = 7;
    public static final byte MSG_ROOM_CHOOSE = 8;
    public static final byte MSG_SCORE_LIST = 9;
    public static final byte MSG_CLIENT_QUIT = 10;
    public static final byte MSG_ADDSHOT = 14;
    public static final byte MSG_NEW_SHOT_FROM_CLIENT = 15;
    public static final byte MSG_SET_WINNING = 16;
    public static final byte MSG_REALIGN = 55;

    private static void handleDefault(ToServerMsg m)
    {
        System.err.printf("Ignoring unimplemented message code: %d\n", m.code);
    }

    private static void handleRemove(ToServerMsg m)
    {
        int index = 0xFF & m.data[1];
        index |= (0xFF & m.data[2]) << 8;

        m.context.getGame().getBoxes()[index].visible = false;

        ToClientMsg msg = new ToClientMsg();
        msg.code = MSG_REMBOX;
        msg.data = m.data;
        msg.length = m.length;
        m.context.getGame().getServer().sendToAll(msg);
        m.context.getPlayer().score++;
        m.context.getGame().checkSendWinningScore(m.context.getPlayer());

    }

    private static void handleNewShotFromClient(ToServerMsg m)
    {
        ToClientMsg msg = new ToClientMsg();
        msg.code = MSG_ADDSHOT;

        msg.length = m.length;
        msg.data = new byte[msg.length];
        for (int i = 0; i < m.length; i++)
        {
            msg.data[i] = m.data[i];
        }

        m.context.getGame().getServer().sendToAll(msg);
        
    }

    private static void handleUpdateCursor(ToServerMsg m)
    {
        if (m.length < 10 || m.data == null)
            System.err.println("Didnt allow client to null own position");
        else
        {
            m.context.getPlayer().setPos(m.data, m.length);
            m.context.getPlayer().updateCursorMsgCount++;
        }
    }

    public MessageHandler()
    {
        Consumer<ToServerMsg> defaultConsumer = MessageHandler::handleDefault;
        table = new ArrayList<Consumer<ToServerMsg>>();
        for (int i = 0; i < 0x100; i++)
        {
            table.add(defaultConsumer);
        }

        table.set(MSG_REMBOX, MessageHandler::handleRemove);
        table.set(MSG_CURSOR_UPDATE, MessageHandler::handleUpdateCursor);
        table.set(MSG_NEW_SHOT_FROM_CLIENT, MessageHandler::handleNewShotFromClient);
    }

    public void handleMessage(ToServerMsg m)
    {
        table.get(m.code).accept(m);
    }
}
