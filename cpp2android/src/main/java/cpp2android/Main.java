package cpp2android;

import android.app.Activity;
import android.content.Context;
import android.os.Debug;
import android.os.Looper;
import android.view.View;
import android.view.ViewGroup;

import java.lang.ref.WeakReference;
import java.util.WeakHashMap;
import java.util.concurrent.locks.ReentrantLock;

public class Main {
    static {
        System.loadLibrary("cpp2android-lib");
    }

    private static WeakReference<Activity> activity   = new WeakReference<Activity>(null);
    private static WeakReference<Looper>   mainLooper = new WeakReference<Looper>(null);
    public static void init(Activity activity){
        if (Main.activity.get() == activity) return;

        Main.activity   = new WeakReference<Activity>(activity);
        Main.mainLooper = new WeakReference<Looper>(Looper.getMainLooper());
    }



    static class CppObject{
        final long c_class_ptr;

        CppObject(long c_class_ptr){
            this.c_class_ptr = c_class_ptr;
        }

        @Override
        protected void finalize() throws Throwable {
            cppReleaseObject(c_class_ptr);
            super.finalize();
        }
    }


    // TODO: replace with Guava.CacheBuilder.weak_keys() #https://stackoverflow.com/a/2256017

    // custom object only (constructed from cpp)
    private static WeakHashMap<Object, CppObject> objects = new WeakHashMap<Object, CppObject>();
    private static ReentrantLock objects_lock = new ReentrantLock();

    private static boolean is_onMainThread(){
        return Thread.currentThread() == mainLooper.get().getThread();
    }

    private static long findCppObject(Object obj){
        if (obj instanceof View && is_onMainThread())
        {
            View view = (View)obj;
            Long c_ptr= (Long)view.getTag(R.id.cpp2android_c_ptr);
            if (c_ptr == null) return 0;
            return c_ptr.longValue();
        }

        synchronized (objects_lock) {
            CppObject res = objects.get(obj);
            if (res == null) return 0;
            return res.c_class_ptr;
        }
    }

    public static long getCppObject(final Object obj){
        long c_obj_ptr = Main.findCppObject(obj);
        if (c_obj_ptr == 0){
            long proxy_ptr = Main.cppMakeProxyObject(obj);
            long added_obj_ptr = Main.addCppObject(obj, proxy_ptr);
            if (added_obj_ptr != proxy_ptr){
                // already added ??
                Main.cppReleaseObject(proxy_ptr);
            }
            c_obj_ptr = added_obj_ptr;
        }
        return c_obj_ptr;
    }

    private static final long memoryThreshold = Math.round(Runtime.getRuntime().maxMemory() * 0.9);
    private static long addCppObject(final Object obj, final long c_ptr){
        if (Debug.getNativeHeapAllocatedSize() + Runtime.getRuntime().totalMemory() >= memoryThreshold) {
            System.gc();
            System.runFinalization();
        }

        synchronized (objects_lock) {
            CppObject object_in_map = objects.get(obj);
            if (object_in_map != null){
                return object_in_map.c_class_ptr;
            }
            objects.put(obj, new CppObject(c_ptr));
        }

        if (obj instanceof View) {
            activity.get().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    View view = (View) obj;
                    view.setTag(R.id.cpp2android_c_ptr, c_ptr);
                }
            });
        }

        return c_ptr;
    }

    static public native void bottstrapJNI(Context applicationContext, ViewGroup rootLayout);

    static public native void cppReleaseObject(long c_ptr);
    static public native long cppMakeProxyObject(Object jobj);
    static public native void cppReleaseCallback(long c_ptr);


    static public native void cppCallback(long c_ptr);
    static public native void cppCallbackO(long c_ptr, Object obj);
    static public native void cppCallbackOO(long c_ptr, Object obj1, Object obj2);
    static public native void cppCallbackOL(long callback_ptr, Object jobj, long obj_ptr);
    static public native void cppCallbackL(long callback_ptr, long obj_ptr);

    /*
    cppCallbackL
    cppCallbackLL
    cppCallbackLLLL
    cppCallbackO
    cppCallbackOO
    cppCallbackOOOO

    cppCallbackLO
    cppCallbackLLOO
     */



    static public native long linkSignal(String id);
    static public native long linkSignalO(String id);

}
