package bgu.spl.net.impl.stomp.ClientFrames;

import java.util.HashMap;
import java.util.Map;

import javax.swing.SortOrder;

public class ConnectFrame {
    private String Command;
    private String acceptVersion;
    private String host;
    private String login;
    private String passcode;
    private String receipt;
    private String validHeader;
    
    public ConnectFrame(String[] splitMessage) {
        this.Command = splitMessage[0];
        validHeader = null;
        acceptVersion = null;
        host = null;
        login = null;
        passcode = null;
        receipt=null;
        parser(splitMessage);
       
      }

    public void parser(String[] splitMessage ){
        for (String header : splitMessage){
            if (header.contains("accept-version")){
                String[]splitedAcceptVersion = header.split(":");
                if(splitedAcceptVersion.length==2&&(splitedAcceptVersion[0]).equals("accept-version"))
                this.acceptVersion = splitedAcceptVersion[1];
            }
            else if(header.contains("host")){
                String[]splitedHost = header.split(":");
                if(splitedHost.length==2&&(splitedHost[0]).equals("host"))
                this.host = splitedHost[1];
            }
            else if(header.contains("login")){
                String[]splitedLogin = header.split(":");
                if(splitedLogin.length==2&&(splitedLogin[0]).equals("login"))
                this.login = splitedLogin[1];
            }
            else if(header.contains("passcode")){
                String[]splitedPasscode = header.split(":");
                if(splitedPasscode.length==2&&(splitedPasscode[0]).equals("passcode"))
                this.passcode = splitedPasscode[1];
            }
            else if(header.contains("receipt")){
                String[]splitedReceipt = header.split(":");
                if(splitedReceipt.length==2&&(splitedReceipt[0]).equals("receipt"))
                this.receipt = splitedReceipt[1];
            }
        }
            
               if( acceptVersion == null)
               validHeader  = "accept-version is not valid";
            else {
            }

               if (host == null)
                validHeader  = "host is not valid";
            else {
            }

                if (login == null)
                validHeader  = "login is not valid";
                else {
                }

                if (passcode == null)
                validHeader  = "passcode is not valid";
                else {
                }

                if (receipt == null&&5!=splitMessage.length)
                validHeader  = "receipt is not valid";
        }
       
        

    
        public String getReceipt(){
            return receipt;
        }


    public String getAcceptVersion() {
        return acceptVersion;
    }

    public String getHost() {
        return host;
    }

    public String getLogin() {
        return login;
    }

    public String getPasscode() {
        return passcode;
    }

    public String getValidHeader() {
        return validHeader;
    }
 

    
}
