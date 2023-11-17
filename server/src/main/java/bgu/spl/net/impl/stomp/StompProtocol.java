package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.impl.stomp.ClientFrames.ConnectFrame;
import bgu.spl.net.impl.stomp.ClientFrames.DisconnectFrame;
import bgu.spl.net.impl.stomp.ClientFrames.SendFrame;
import bgu.spl.net.impl.stomp.ClientFrames.SubscribeFrame;
import bgu.spl.net.impl.stomp.ClientFrames.UnsubscribeFrame;
import bgu.spl.net.srv.Connections;
import javafx.util.Pair;

public class StompProtocol implements StompMessagingProtocol<String> {

    int connectionId;
    ConnectionsImpl connections;
    boolean shouldTerminate;

    public StompProtocol() {

    }

    /**
     * Used to initiate the current client protocol with it's personal connection ID
     * and the connections implementation
     **/
    public void start(int connectionId, ConnectionsImpl connections) {
        this.connectionId = connectionId;
        this.connections = (ConnectionsImpl) connections;
        shouldTerminate = false;

    }

    public void process(String message) {
        String[] splitMessage = message.split("\n");
        String command = splitMessage[0];
        //CONNECT Frame
        if (command.equals("CONNECT")) {
            ConnectFrame connectFrame = new ConnectFrame(splitMessage);
            if (connectFrame.getValidHeader() != null) {//check if headers are valid. if not - send error frame.
                 connections.send(this.connectionId, sendErrorFrame(connectFrame.getValidHeader(),connectionId, connectFrame.getReceipt()!=null, connectFrame.getReceipt()));// return error frame and diconnect him
                     connections.disconnect(connectionId);
                     shouldTerminate=true;
                    } else {// procces connect frame              
                connections.send(connectionId, proccessConnectFrame(connectFrame));
            }
        //End of CONNECT Frame



        //SUBSCRIBE Frame    
        } else if (command.equals("SUBSCRIBE")) {
            SubscribeFrame subscribeFrame = new SubscribeFrame(splitMessage);
            if(!connections.connectedUsers.contains(connections.connectionsIDtoUser.get(connectionId))){//check if user is logged in. If not, return error frame
                connections.send(this.connectionId,  sendErrorFrame("You are not logged in",connectionId,subscribeFrame.getReceipt()!=null,subscribeFrame.getReceipt())); //return error frame (not logged in)
                shouldTerminate=true;

            }
            else{//user is logged in
            if (subscribeFrame.getValidHeader() != null) {//check is headers are valid. if not - send error frame
                connections.send(this.connectionId, sendErrorFrame(subscribeFrame.getValidHeader(),connectionId, subscribeFrame.getReceipt()!=null, subscribeFrame.getReceipt()));// return error frame and diconnect him
                connections.disconnect(connectionId);
                shouldTerminate=true;

            } else //procces subscribe frame
            {
                connections.send(connectionId,proccessSubscribeFrame(subscribeFrame));
                        }
                   }
        //End of SUBSCRIBE Frame
        


        //UNSUBSCRIBE Frame
        } else if (command.equals("UNSUBSCRIBE")) {
            UnsubscribeFrame unsubscribeFrame = new UnsubscribeFrame(splitMessage);
            if(!connections.connectedUsers.contains(connections.connectionsIDtoUser.get(connectionId))){//check if user is logged in. If not - send error frame
                connections.send(this.connectionId,  sendErrorFrame("You are not logged in",connectionId,unsubscribeFrame.getReceipt()!=null,unsubscribeFrame.getReceipt())); //return error frame (not logged in)
                shouldTerminate=true;

            }
            else{//user is logged in
            if (unsubscribeFrame.getValidHeader() != null) {//check if headers are valid. if not - send error frame
                connections.send(this.connectionId, sendErrorFrame(unsubscribeFrame.getValidHeader(),connectionId, unsubscribeFrame.getReceipt()!=null, unsubscribeFrame.getReceipt()));// return error frame and diconnect him
                connections.disconnect(connectionId);
                shouldTerminate=true;

            } else {//proccess unsubscribe frame
              
                String toSend = proccesUnsubscribeFrame(unsubscribeFrame);
                connections.send(connectionId,toSend);

            }
        }
        //End of UNSUBSCRIBE Frame



        //SEND Frame
        } else if (command.equals("SEND")) {   
            SendFrame sendFrame = new SendFrame(splitMessage);
            if(!connections.connectedUsers.contains(connections.connectionsIDtoUser.get(connectionId))){//check if user is logged in. if not - send error frame
                connections.send(this.connectionId,  sendErrorFrame("You are not logged in",connectionId,sendFrame.getReceipt()!=null,sendFrame.getReceipt())); 
                shouldTerminate=true;

            }
            else{  //user is logged in
            if (sendFrame.getValidHeader() != null) {//check if headers are valid. if not - send error frame
                connections.send(this.connectionId, sendErrorFrame(sendFrame.getValidHeader(),connectionId, sendFrame.getReceipt()!=null, sendFrame.getReceipt()));// return error frame and diconnect him
                connections.disconnect(connectionId);
                shouldTerminate=true;

            } else {//proccess Send Frame
                proccessSendFrame(sendFrame);//the proccess sends the message to all the users that are subscribed to the topic  
            }
        //End of SEND Frame
        }


        //DISCONNECT Frame
        } else if (command.equals("DISCONNECT")) {
            DisconnectFrame disconnectFrame = new DisconnectFrame(splitMessage);
            if(!connections.connectedUsers.contains(connections.connectionsIDtoUser.get(connectionId))){//check if user is logged in. if not - send error frame
                connections.send(this.connectionId,  sendErrorFrame("You are not logged in",connectionId,disconnectFrame.getReceipt()!=null,disconnectFrame.getReceipt())); //return error frame (not logged in)

            }
            else{//user is logged in
            if (disconnectFrame.getValidHeader() != null) {//check if headers are valid. if not - send error frame
                connections.send(this.connectionId, sendErrorFrame(disconnectFrame.getValidHeader(),connectionId, disconnectFrame.getReceipt()!=null, disconnectFrame.getReceipt()));// return error frame and diconnect him
                connections.disconnect(connectionId);


            } else {//proccess Disconnect Frame
               String toSend = sendReceiptFrame(disconnectFrame.getReceipt());
                connections.send(connectionId,toSend);
              connections.disconnect(connectionId); //should be here

            }
            shouldTerminate=true;

        }
        //End of DISCONNECT Frame


        
        } else {//command is not valid. send error frame
            connections.send(connectionId, sendErrorFrame("malformed frame received",this.connectionId,false,""));

            if(connections.connectedUsers.contains(connections.connectionsIDtoUser.get(connectionId))){//check if user is logged in. if not - send error frame
                connections.disconnect(connectionId);

            }
            shouldTerminate=true;
        }
    }


    public String proccessConnectFrame(ConnectFrame connectFrame) {
        String username = connectFrame.getLogin();

        if (connections.getUserNameToPassword().containsKey(username)) { // check if user is already exist
            if (connections.connectedUsers.contains(username)) { // check if user is already connected. If it does, remove him from list of connected users.
                //disconnect user
                return sendErrorFrame("User already logged in",connectionId,connectFrame.getReceipt()!=null,connectFrame.getReceipt());// return error frame allready log in
            }

            else if (connections.userNameToPassword.get(username).equals(connectFrame.getPasscode())) { // check if password is correct
                    connections.ConnectExistingUser(username, connectionId);                        
                return sendConnectedFrame(connectFrame.getReceipt()!=null,connectFrame.getReceipt());// password and username are match
            }

            else {
                return sendErrorFrame("Wrong password",connectionId,connectFrame.getReceipt()!=null,connectFrame.getReceipt());// return error frame (wrong password)
            }
        } else { // create new user and connect
            connections.createNewUser(username, connectFrame.getPasscode(), connectionId);// creates new user
            
            return sendConnectedFrame(connectFrame.getReceipt()!=null,connectFrame.getReceipt());

        }

    }

    public String proccesUnsubscribeFrame(UnsubscribeFrame unsubscribeFrame){
        int SubId = unsubscribeFrame.getSubId();
        String username =connections.connectionsIDtoUser.get(this.connectionId);
        if ( connections.UserNameandSubIDtoTopic.containsKey(new Pair<>(username,SubId)) 
                && connections.UserNameandSubIDtoTopic.get(new Pair<>(username,SubId))!=null) {
                    connections.unsubFromTopic(SubId,username);

            return sendReceiptFrame(unsubscribeFrame.getReceipt());
        }
        else{
            return sendErrorFrame("You are not subscribed to this topic",connectionId,unsubscribeFrame.getReceipt()!=null,unsubscribeFrame.getReceipt());// return error frame (not subscribed to this topic)
        }

    }



    public String proccessSubscribeFrame(SubscribeFrame subscribeFrame) {
        String topic = subscribeFrame.getDestination();
        int SubId = subscribeFrame.getSubId();
        if (connections.SubscribedTopicsToUsernames.containsKey(topic)) { // check if topic is already exist
            if (connections.SubscribedTopicsToUsernames.get(topic).contains(connections.connectionsIDtoUser.get(connectionId))) { // check if id is already subscribed to this topic
                return sendErrorFrame("You are already subscribed to: " + topic ,connectionId,subscribeFrame.getReceipt()!=null,subscribeFrame.getReceipt());// return error frame (topic already exist)
            } else {
                String username = connections.connectionsIDtoUser.get(connectionId);
                connections.addIdToTopic(SubId ,username ,topic );// add id to topic or creates new topic and add id to it
                return sendReceiptFrame(subscribeFrame.getReceipt());// return receipt frame
            }
        }
        else {

            String username = connections.connectionsIDtoUser.get(connectionId);
            connections.addIdToTopic(SubId ,username ,topic );// add id to topic or creates new topic and add id to it
            return sendReceiptFrame(subscribeFrame.getReceipt());// return receipt frame
            
        } // create new topic and add id to it
        }
    

    public void proccessSendFrame(SendFrame sendFrame){
        String topic = sendFrame.getDestination();
        String body = sendFrame.getBody();
        String username = connections.connectionsIDtoUser.get(connectionId);
       
         if(connections.SubscribedTopicsToUsernames.containsKey(topic)){//check if topic is exist
            if(!connections.SubscribedTopicsToUsernames.get(topic).contains(username)){//check if user is subscribed to this topic            
                connections.send(connectionId, sendErrorFrame("You are not subscribed to this topic",connectionId,sendFrame.getReceipt()!=null,sendFrame.getReceipt()));// return error frame (not subscribed to this topic)
            }
            else{ // send message frame to anyone that subscribe to this topic
            connections.send(topic, body);      
            }
        }
        
        else{//topic doesnt exist
            connections.send(connectionId, sendErrorFrame("Topic does not exist",connectionId,sendFrame.getReceipt()!=null,sendFrame.getReceipt()));// return error frame (topic does not exist)
        }
    }




    public String sendConnectedFrame(boolean receipt, String receiptID) {
        String connectedFrame ="";
        if(receipt)
         connectedFrame = "CONNECTED\nversion:1.2\nreceipt-id:" + receiptID + "\n\n\u0000"; // checkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
     else
         connectedFrame = "CONNECTED\nversion:1.2\n\n\u0000";
       
        return connectedFrame;
    }

    public String sendErrorFrame(String error, int connectionID,boolean receipt, String receiptID) {
        String errorFrame ="";
        if(receipt)
        errorFrame = "ERROR\nreceipt-id:" + receiptID + "\nmessage:" + error + "\n\n\u0000";
        else
        errorFrame = "ERROR\nmessage:" + error + "\n\n\u0000";

        return errorFrame;
    }

    public String sendReceiptFrame(String receipt) {
        String receiptFrame = "RECEIPT\nreceipt-id:" + receipt + "\n\n\u0000";
        return receiptFrame;
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }



    /**
     * @return true if the connection should be terminated
     */
  
   
 
}
