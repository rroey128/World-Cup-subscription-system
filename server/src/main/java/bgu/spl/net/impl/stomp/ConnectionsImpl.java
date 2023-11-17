package bgu.spl.net.impl.stomp;

import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;
import javafx.util.Pair;

public class ConnectionsImpl implements Connections<String> {

    Map<Integer, String> connectionsIDtoUser;// map of connectionId to userName
    Map<String, Integer> userNameToConnectionId;// map of userName to connectionId
    Map<Pair<String, Integer>, String> UserNameandSubIDtoTopic;// map of subID and usernames to Topics;
    Map<String, String> userNameToPassword;// map of userName to password
    Map<Integer, ConnectionHandler<String>> connectionIdToConnectionHandler;// map of connectionIds to connectionHandler
    List<String> connectedUsers;// list of connected users
    List<ConnectionHandler<String>> connectionHandlers;// list of connectionHandlers
    Map<String, List<String>> SubscribedTopicsToUsernames;// map of subscribed topics to usernames
    Map<Pair<String, String>, Integer> TopicAndUsernameToSubID;// map of topics to usernames and subID
    int messageID;

    public ConnectionsImpl() {
        connectionsIDtoUser = new ConcurrentHashMap<>();
        UserNameandSubIDtoTopic = new ConcurrentHashMap<>();
        TopicAndUsernameToSubID = new ConcurrentHashMap<>();
        userNameToPassword = new ConcurrentHashMap<>();
        connectionIdToConnectionHandler = new ConcurrentHashMap<>();
        connectedUsers = new CopyOnWriteArrayList<>();
        connectionHandlers = new CopyOnWriteArrayList<>();
        SubscribedTopicsToUsernames = new ConcurrentHashMap<>();
        userNameToConnectionId = new ConcurrentHashMap<>();
        messageID = 0;

    }

    @Override
    public synchronized boolean send(int connectionId, String msg) {
        connectionIdToConnectionHandler.get(connectionId).send(msg);
        return true;
    }

    @Override
    public synchronized void disconnect(int connectionId) {
        // removes the connection handler from list and maps (it doesnt matter if the
        // user exits or not here)
        connectionHandlers.remove(connectionIdToConnectionHandler.get(connectionId));
        connectionIdToConnectionHandler.remove(connectionId);

        // removes all user related stuff from maps and lists
        if (connectedUsers.contains(connectionsIDtoUser.get(connectionId))) {
            String userName = connectionsIDtoUser.get(connectionId);
            connectedUsers.remove(userName);
            connectionsIDtoUser.remove(connectionId);
            userNameToConnectionId.remove(userName);// check why it disccounts user
            String username = connectionsIDtoUser.get(connectionId);
            for (Pair<String, String> pair : TopicAndUsernameToSubID.keySet()) {
                if (pair.getKey().equals(username)) {
                    TopicAndUsernameToSubID.remove(pair);
                }
            }
            for (Pair<String, Integer> pair : UserNameandSubIDtoTopic.keySet()) {
                if (pair.getKey().equals(userName)) {
                    SubscribedTopicsToUsernames.get(UserNameandSubIDtoTopic.get(pair)).remove(userName);
                    UserNameandSubIDtoTopic.remove(pair);
                }
            }
        }
    }

    @Override
    public synchronized void send(String channel, String msg) {

        for (String username : SubscribedTopicsToUsernames.get(channel)) {
            int userSubID = TopicAndUsernameToSubID.get(new Pair<>(channel, username));
            messageID++;
            String messageFrame = "MESSAGE" + "\n" + "subscription:" + userSubID + "\n" + "message-id:" + messageID
                    + "\n" + "destination:" + "/" + channel + "\n" + "\n" + msg + "\n" + "\u0000";
            connectionIdToConnectionHandler.get(userNameToConnectionId.get(username)).send(messageFrame);
            // }
            // }
        }
    }

    public synchronized void addConnection(int connectionId, String userName) {
        connectionsIDtoUser.put(connectionId, userName);
    }

    public synchronized boolean isConnected(String userName) {
        return connectedUsers.contains(userName);
    }

    public synchronized Map<String, String> getUserNameToPassword() {
        return userNameToPassword;
    }

    public synchronized void addIdToTopic(int subID, String userName, String topic) {
        UserNameandSubIDtoTopic.put(new Pair<>(userName, subID), topic);
        SubscribedTopicsToUsernames.putIfAbsent(topic, new CopyOnWriteArrayList<>());
        SubscribedTopicsToUsernames.get(topic).add(userName);
        TopicAndUsernameToSubID.put(new Pair<>(topic, userName), subID);

    }

    public synchronized void createNewUser(String login, String Password, int connectionId) {
        userNameToPassword.put(login, Password);
        connectedUsers.add(login);
        connectionsIDtoUser.put(connectionId, login);
        userNameToConnectionId.put(login, connectionId);

    }
    public synchronized void ConnectExistingUser(String login, int connectionId) {
        connectedUsers.add(login);
        connectionsIDtoUser.put(connectionId, login);
        userNameToConnectionId.put(login, connectionId);

    }


    public synchronized <T> void addConnectionHandler(ConnectionHandler<T> connectionHandler, int connectionId) {
        connectionHandlers.add((ConnectionHandler<String>) connectionHandler);
        connectionIdToConnectionHandler.put(connectionId, (ConnectionHandler<String>) connectionHandler);
    }

    public synchronized void unsubFromTopic(int SubID, String userName) {
        String topic = UserNameandSubIDtoTopic.get(new Pair<>(userName, SubID));
        SubscribedTopicsToUsernames.get(topic).remove(userName);
        UserNameandSubIDtoTopic.remove(new Pair<>(userName, SubID));
        TopicAndUsernameToSubID.remove(new Pair<>(topic, userName));
    }

}
