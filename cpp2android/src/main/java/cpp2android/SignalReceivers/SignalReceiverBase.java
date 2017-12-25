package cpp2android.SignalReceivers;

import java.util.HashMap;
import java.util.concurrent.locks.ReentrantLock;

import cpp2android.utils.GrowOnlyArray;


// Using indices, because GlobalRefference are short on supply on cpp side (~56000)
class SignalReceiverBase {
    private static HashMap<String, Integer> map = new HashMap<String, Integer>();
    private static GrowOnlyArray<SignalReceiverBase> memory = new GrowOnlyArray<SignalReceiverBase>();
    private final static ReentrantLock lock = new ReentrantLock();

    @SuppressWarnings("unchecked")
    static protected <T extends SignalReceiverBase> T make(String id, Class<T> clazz){
        return (T)getSignalReceiver(link(id, clazz));
    }

    // called from cpp
    static protected <T extends SignalReceiverBase> int link(String id, Class<T> clazz){
        synchronized (lock){
            Integer receiver_id = map.get(id);
            if (receiver_id == null){
                T newObject = null;
                try {
                    newObject = clazz.newInstance();
                } catch (InstantiationException e) {
                    e.printStackTrace();
                    System.exit(-1);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                    System.exit(-1);
                }
                final int index = memory.add(newObject);
                map.put(id, index);
                return index;
            }
            return receiver_id;
        }
    }

    static protected SignalReceiverBase getSignalReceiver(int index){
        return memory.get(index);
    }

}
