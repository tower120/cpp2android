package cpp2android.android.view.View;

import android.view.View;

import cpp2android.CppCallbackBase;
import cpp2android.Main;

public class OnClickListener {
    static public View.OnClickListener make(final long c_ptr){
        class Callback extends CppCallbackBase implements View.OnClickListener{
            Callback(final long c_ptr) {
                super(c_ptr);
            }

            @Override
            public void onClick(View view) {
                Main.cppCallbackL(c_ptr, Main.getCppObject(view));
            }
        };
        return new Callback(c_ptr);
    }
}
