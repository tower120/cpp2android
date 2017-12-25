package cpp2android;

import android.util.Log;

public class CppCallbackBase {
    private boolean callbackReleased = false;
    final protected long c_ptr;


    public CppCallbackBase(long c_ptr) {
        this.c_ptr = c_ptr;
    }

    public void releaseCallback(){
        if (!callbackReleased) {
            Main.cppReleaseCallback(c_ptr);
            callbackReleased = true;
        }
    }

    @Override
    protected void finalize() throws Throwable {
        Log.i("cpp2android", "CppCallback.finalize");
        releaseCallback();
        super.finalize();
    }
}
