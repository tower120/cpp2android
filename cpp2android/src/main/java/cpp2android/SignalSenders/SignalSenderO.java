package cpp2android.SignalSenders;

import cpp2android.Main;

public class SignalSenderO {
    long c_callback_ptr;

    public SignalSenderO(String id){
        c_callback_ptr = Main.linkSignalO(id);
    }
    public void send(Object obj){
        Main.cppCallbackO(c_callback_ptr, obj);
    }
}