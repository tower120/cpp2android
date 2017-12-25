Write native Android UI directly in c++!

Why to pass complex data structures between java and c side? Access android widgets directly from c++!

## Motivation example

Filling layout from c++:

```c++
    void onCreate(const jni_interface::SharedPtr<AndroidViews::ViewGroup>& root){
        auto btnAdd = Button::make("Add more buttons");
        root->addView(*btnAdd);
      
        btnAdd->setOnClickListener([root](const SharedPtr<View>& view) {
            auto button = Button::make("Hello!");
            root->addView(*button);
            button->setOnClickListener([i=0](const SharedPtr<View>& view) mutable {
                Button* btn = static_cast<Button*>(view.get());
                btn->setText(std::to_string(++i).c_str());
            });          
        });
    }
```



You can easily mix java side logic with c++:

```java
        SignalReceiver hideAd = SignalReceiver.make("hideAd");
        hideAd.listen(new SignalReceiver.Listener() {
            @Override
            public void run() {
                Toast.makeText(getApplicationContext(), "Hiding Ad.", Toast.LENGTH_LONG).show();
            }
        });
```

```c++
SignalSender<> hideAd("hideAd");
hideAd.send();
```



And vice-versa:

```java
    SignalSenderO signalSender = new SignalSenderO("btnClick");

    public void btnClick(View view) {
        signalSender.send(view);
    }
```

```c++
SignalReceiver<jobject>& signal = SignalReceiver<jobject>::link("btnClick");
signal.set_unique_listener([](jobject obj){
    SharedPtr<Button> btn = adopt_object<Button>(obj);
    btn->setText("Clicked");
});
```



---

Target Android devices >=4.0

Documentation at [/cpp2android/docs](/cpp2android/docs)

Your cpp source code in `/cpp` . cpp Entry point in `main.h`