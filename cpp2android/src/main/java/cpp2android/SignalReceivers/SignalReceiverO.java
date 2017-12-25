package cpp2android.SignalReceivers;

public class SignalReceiverO extends SignalReceiverBase {
    public interface Listener{
        void run(Object object);
    };
    private Listener listener = null;
    public void listen(Listener listener){
        this.listener = listener;
    }

    protected SignalReceiverO(){}
    static public SignalReceiverO make(String id){
        return make(id, SignalReceiverO.class);
    }

    // called from cpp
    static private int link(String id){
        return SignalReceiverBase.link(id, SignalReceiverO.class);
    }
    static private void pulse(int id, Object object){
        SignalReceiverO self = (SignalReceiverO)getSignalReceiver(id);
        if (self.listener == null) return;
        self.listener.run(object);
    }

}
