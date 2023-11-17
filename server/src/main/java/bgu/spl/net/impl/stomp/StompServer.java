package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.ServerUnion;

public class StompServer {
    public static void main(String[] args) {
        
        ServerUnion.threadPerClient(            
            7777, //port
            () -> new StompProtocol(), //protocol factory
            StompLineMessageEncoderDecoder::new //message encoder decoder factory
    ).serve();



    // ServerUnion.Stompreactor(
    //             Runtime.getRuntime().availableProcessors(),
    //             7777, //port
    //             () -> new StompProtocol(), //protocol factory
    //             StompLineMessageEncoderDecoder::new //message encoder decoder factory
    //     ).serve();







        
    }
}