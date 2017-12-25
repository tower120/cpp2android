package cpp2android.SignalReceivers;

public class SignalReceiver extends SignalReceiverBase{
    public interface Listener{
        void run();
    };
    private Listener listener = null;

    public void listen(Listener listener){
        this.listener = listener;
    }
    static public SignalReceiver make(String id){
        return make(id, SignalReceiver.class);
    }

    // called from cpp
    static private int link(String id){
        return SignalReceiverBase.link(id, SignalReceiver.class);
    }
    static private void pulse(int id){
        SignalReceiver self = (SignalReceiver)getSignalReceiver(id);
        if (self.listener == null) return;
        self.listener.run();
    }

}