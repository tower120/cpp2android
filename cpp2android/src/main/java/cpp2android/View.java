package cpp2android;

import cpp2android.android.view.View.OnClickListener;

public class View {
    static public void setOnClickListener(android.view.View view, final long c_ptr){
        view.setOnClickListener(OnClickListener.make(c_ptr));
    }
}
