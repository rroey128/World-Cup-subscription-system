package bgu.spl.net.impl.stomp.ClientFrames;

public class DisconnectFrame {
    private String Command;
    private String recepitId;
    private String validHeader;





    public DisconnectFrame(String[] splitMessage) {
        this.Command = splitMessage[0];
        this.validHeader = null;
        parser(splitMessage);

    }

    public void parser(String[] splitMessage ){
        String[]splitedRecepitId = splitMessage[1].split(":");
        if(!(splitedRecepitId[0]).equals("receipt"))
        this.validHeader = "receipt is not valid";
        else
        this.recepitId = splitedRecepitId[1];
    }


  
    
    
    public String getrecepitId() {
        return recepitId;
    }
    public String getValidHeader() {
        return validHeader;
    }
    public String getReceipt() {
        return recepitId;
    }
}
