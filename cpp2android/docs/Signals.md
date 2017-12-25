You can have connection between java and cpp part with signals.



#### From cpp to java

[cpp] `jni_interface::SignalSender<>` -> [java] `cppInterface.SignalRecievers.SignalReciever`

[cpp] `jni_interface::SignalSender<jobject>` -> [java] `cppInterface.SignalRecievers.SignalRecieverO`



#### From java to cpp

[java] `cppInterface.SignalSenders.SignalSender` -> [cpp] `jni_interface::SignalReciever<>`

[java] `cppInterface.SignalSenders.SignalSenderO` -> [cpp] `jni_interface::SignalReciever<jobject>`



If you need to pass something other than `jobject` - add your SignalReceiver Reciever / Listener (it is rather trivially), or pack it in `jobject`. Use Integer, Double, String helpers, if needed on cpp side.





## Using with existing Android application

You can use cpp2android in your existing android application. 

In MainActivity.java:

```java
    SignalSenderO signalSender = new SignalSenderO("btnClick");
    TextView textView; // from existing layout.

    public void btnClick(View view) {
        signalSender.send(textView);
    }
```



In main.cpp:

```c++
        SignalReciever<jobject>& signal = SignalReciever<jobject>::link("btnClick");
        signal.set_unique_listener([](jobject obj){
           auto text_view = adopt_view<TextView>(obj);
           // do some work...
           text_view.setText(result);    // or send message
        });
```

