package cpp2android.SignalSenders;


import cpp2android.Main;

public class SignalSender {
    long c_callback_ptr;

    public SignalSender(String id){
        c_callback_ptr = Main.linkSignal(id);
    }
    public void send(){
        Main.cppCallback(c_callback_ptr);
    }
}