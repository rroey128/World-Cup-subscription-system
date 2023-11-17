package bgu.spl.net.impl.stomp.ClientFrames;

import javax.print.DocFlavor.STRING;

public class UnsubscribeFrame {
    private String Command;
    private String SubId ;
    private String validHeader;
    private String receipt;

    public UnsubscribeFrame(String[] splitMessage) {
        this.Command = splitMessage[0];
        this.validHeader = null;
        parser(splitMessage);

    }

    public void parser(String[] splitMessage ){
        for (String header : splitMessage){
            if(header.contains("id")){
                String[] splitedId = header.split(":");
                if(splitedId.length!=2||!(splitedId[0]).equals("id"))
                    this.validHeader = "id is not valid";
                this.SubId = splitedId[1];
            }
            else if(header.contains("receipt")){
                String[] splitedReceipt = header.split(":");
                if(splitedReceipt.length!=2||!(splitedReceipt[0]).equals("receipt"))
                    this.validHeader = "receipt is not valid";
                this.receipt = splitedReceipt[1];
            }
        }
         if ( SubId == null)
             this.validHeader = "id is not valid";
            if(receipt==null)
                this.validHeader = "receipt is not valid";
      }
    

    public int getSubId() {
        return Integer.parseInt(SubId);
    }
    public String getValidHeader() {
        return validHeader;
    }

    public String getReceipt() {
        return receipt;
    }

    

}
