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

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Main.init(this);

        Log.i("cpp2android", "onCreate");


        final ViewGroup root_view = (ViewGroup)getWindow().getDecorView().getRootView();
        final ViewGroup main_layout = (ViewGroup)findViewById(R.id.main_layout);

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
        signalSender.send(view);
    }
}
