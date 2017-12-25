Proxyable View example:

```c++

class Button : public View{
protected:
    inline static struct MethodIds : Initializable<MethodIds>{
        inline static jni_interface::UniqueGlobalClass clazz{};

        inline static jmethodID constructor;
        inline static jmethodID setText;

        static void init(){
            JNIEnv* env = this_thread.getEnv();
            clazz = {ClassFinder::find(env, "android/widget/Button")};
            constructor = env->GetMethodID(clazz.get(), "<init>", "(Landroid/content/Context;)V");
            setText = env->GetMethodID(clazz.get(), "setText", "(Ljava/lang/CharSequence;)V");

            register_proxy<Button>(clazz.get());
        }
    } method_ids;


    static jobject make_jobject(){
        JNIEnv* env = this_thread.getEnv();
        jobject jbutton = env->NewObject(method_ids.clazz.get(), method_ids.constructor, ui_session->context() );
        return jbutton;
    }


    using View::View;
    Button(jni_interface::JavaObjectRef* ref) : View(ref) {
        setText("Default Text");
    }

    Button(jni_interface::JavaObjectRef* ref, const char* text) : View(ref) {
        setText(text);
    }
public:

    template<class ...Args>
    static auto make(Args&&...args){
        return make_view<Button>(make_jobject(), std::forward<Args>(args)...);
    }

    void setText(const char* str){
        JNIEnv* env = jni_interface::this_thread.getEnv();

        jstring jstr = env->NewStringUTF(str);
            env->CallVoidMethod(get_object(), method_ids.setText, jstr);
        env->DeleteLocalRef(jstr);
    }
};

```



keep jobject creation as separate static function - you can easily extend your Proxyable view later.

Hide constructors, construct object by static `make`. Use `make_view`, it will call your custom constructors, with `JavaObjectRef*` as first parameter.



Get your method ids like :

```c++
inline static struct MethodIds : Initializable<MethodIds>{
  static void init(){
      // this will be called when system initialized
      register_proxy<Button>(clazz.get()) // if needed
  }
} method_ids;
```

