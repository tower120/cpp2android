package com.example.cpp2android.test;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import cpp2android.Main;
import cpp2android.SignalReceivers.SignalReceiver;
import cpp2android.SignalSenders.SignalSenderO;

public class MainActivity extends AppCompatActivity {
    static public native void testNative();

    /*class MyObject extends Object {
        char payload[] = new char[20000000];
        @Override
        protected void finalize() throws Throwable {
          //  int t =payload[1];
            Log.i("MEE", "-------------------------------Finalize----------------------");
            super.finalize();
        }
    };
    MyObject list = new MyObject ();*/

    Thread t;

    @Override
    public void onTrimMemory(int level) {
        Log.i("cpp2android","Trimming");

        Runtime.getRuntime().gc();
        Runtime.getRuntime().runFinalization();

        super.onTrimMemory(level);
    }

    @Override
    public void onLowMemory() {
        Log.i("cpp2android","LowMemory");
        super.onLowMemory();
    }


    static long to_kb(long bytes){
        return bytes / 1024;
    }
    static long to_mb(long bytes){
        return to_kb(bytes) / 1024;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Main.init(this);

        Log.i("cpp2android", "onCreate");


        final ViewGroup root_view = (ViewGroup)getWindow().getDecorView().getRootView();
        final ViewGroup main_layout = (ViewGroup)findViewById(R.id.main_layout);

        Log.i("cpp2android", "native allocated heap = " + to_mb(android.os.Debug.getNativeHeapAllocatedSize()));
        Log.i("cpp2android", "native heap = " + to_mb(android.os.Debug.getNativeHeapSize()));
        Log.i("cpp2android", "free heap = " + to_mb(android.os.Debug.getNativeHeapFreeSize()));
        Log.i("cpp2android", "runtime max memory = " + to_mb(Runtime.getRuntime().maxMemory()));
        Log.i("cpp2android", "runtime total memory = " + to_mb(Runtime.getRuntime().totalMemory()));
        Log.i("cpp2android", "runtime free memory = " + to_mb(Runtime.getRuntime().freeMemory()));


        //Runtime.getRuntime().gc();
        /*Runtime.getRuntime().gc();
        Runtime.getRuntime().gc();
        Runtime.getRuntime().gc();
        Runtime.getRuntime().gc();
        Runtime.getRuntime().gc();
        Runtime.getRuntime().gc();
        Runtime.getRuntime().gc();
        Runtime.getRuntime().gc();
        Runtime.getRuntime().gc();
        Runtime.getRuntime().gc();*/

        //Runtime.getRuntime().runFinalization();


        /*final Button text = (Button)findViewById(R.id.sample_text);
        t = new Thread(new Runnable() {
            int i = 0;
            @Override
            public void run() {
                while (true) {
                    i++;
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            text.setText(String.valueOf(i));
                            text.setText(String.valueOf(i));
                            text.setText(String.valueOf(i));
                            text.setText(String.valueOf(i));

                            //text.setText(String.valueOf(i));
                            //text.setText(String.valueOf(i));
                            //text.setText(String.valueOf(i));
                            //text.setText(String.valueOf(i));
                        }
                    });
                    try {
                        Thread.sleep(1);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
        t.start();
        return;*/



        //list = new MyObject();
        /*MyObject l = new MyObject();
        list = null;
        list = l;
        list = new MyObject();
        l = null;*/

        /*Runtime.getRuntime().runFinalization();*/

        // call C bootstrap
        //layout.setOnClickListener(list);

        cpp2android.Main.bottstrapJNI(
                main_layout.getContext(),
                //this.getApplicationContext(),
                main_layout);

        SignalReceiver receiver = SignalReceiver.make("s1");
        receiver.listen(new SignalReceiver.Listener() {
            @Override
            public void run() {
                Toast.makeText(getApplicationContext(), "Toast msg", Toast.LENGTH_LONG).show();
            }
        });
    }


    SignalSenderO signalSender = new SignalSenderO("btnClick");

    public void btnClick(View view) {
        testNative();
        //signalSender.send(view);
    }
}
