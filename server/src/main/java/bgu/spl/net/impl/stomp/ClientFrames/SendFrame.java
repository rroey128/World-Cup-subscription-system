package bgu.spl.net.impl.stomp.ClientFrames;

public class SendFrame {

    private String Command;
    private String destination;
    private String body;
    private String validHeader;
    private String receipt;
    private int headerCounter=0;

    public SendFrame(String[] splitMessage) {
        this.Command = splitMessage[0];
        this.validHeader = null;
        parser(splitMessage);
    }

    public void parser(String[] splitMessage) {
        for (String header : splitMessage) {
            if (header.contains("destination")) {
                String[] splitedDestination = header.split(":");
                if (splitedDestination.length != 2 || !(splitedDestination[0]).equals("destination"))
                    this.validHeader = "destination is not valid";
                this.destination = splitedDestination[1].substring(1);
             }else if (header.contains("receipt")) {
                String[] splitedReceipt = header.split(":");
                if (splitedReceipt.length != 2 || !(splitedReceipt[0]).equals("receipt"))
                    this.validHeader = "receipt is not valid";
                this.receipt = splitedReceipt[1];
            }
        }
        int index= 0;
        for ( int i=0; i<splitMessage.length; i++){
            if (splitMessage[i].contains("user"))
                index=i;
        }
        String messageFromClient = splitMessage[index]+"\n";
        for(int i = index+1; i<splitMessage.length; i++){
            messageFromClient = messageFromClient + splitMessage[i]+"\n";     
            }
            this.body = messageFromClient;

        if (destination == null)
            this.validHeader = "destination is not valid";
            else 
            headerCounter++;
        if (body == null)
            this.validHeader = "body is empty";
            else
            headerCounter++;

        if (receipt == null&&index==4)
            this.validHeader = "receipt is not valid";
  
  
  
        }



    public String getBody() {
        return body;
    }

    public String getDestination() {
        return destination;
    }

    public String getValidHeader() {
        return validHeader;
    }

    public String getReceipt() {
        return receipt;
    }

}
