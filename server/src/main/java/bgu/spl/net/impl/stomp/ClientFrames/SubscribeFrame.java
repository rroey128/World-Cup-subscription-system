package bgu.spl.net.impl.stomp.ClientFrames;


public class SubscribeFrame {   
    private String Command;
    private String destination;
    private String id;
    private String validHeader;
    private String receipt;
    private int subID;  

    public SubscribeFrame(String[] splitMessage) {
        this.Command = splitMessage[0];
        this.validHeader = null;
        parser(splitMessage);
    }
    
    public void parser(String[] splitMessage ){
        for (String header : splitMessage){
            if (header.contains("destination")){
                String[]splitedDestination = header.split(":");
                if(splitedDestination.length!=2||!(splitedDestination[0]).equals("destination"))
                this.validHeader = "destination is not valid";
                this.destination = splitedDestination[1].substring(1);
            }
            else if (header.contains("id")){
                String[]splitedId = header.split(":");
                if(splitedId.length!=2||!(splitedId[0]).equals("id")){
                System.out.println("this is length: "+splitedId.length + " this is the header: "+splitedId[0]);
                this.validHeader = "id is not valid";
            }
                this.id = splitedId[1];
                this.subID = Integer.parseInt(id);
            }
            else if (header.contains("receipt")){
                String[]splitedReceipt = header.split(":");
                if(splitedReceipt.length!=2||!(splitedReceipt[0]).equals("receipt"))
                this.validHeader = "receipt is not valid";
                this.receipt = splitedReceipt[1];
            }

    }

    if( destination == null)
         this.validHeader = "destination is not valid";

    if( id == null)
          this.validHeader = "id is not valid";

    if(receipt==null)
         this.validHeader = "receipt is not valid";


}
    public String getDestination() {
        return destination;
    }
    
    public String getId() {
        return id;
    }
    public String getValidHeader() {
        return validHeader;
    }

    public int getSubId() {
        return subID;
    }
    public String getReceipt(){
        return receipt;
    }
}
