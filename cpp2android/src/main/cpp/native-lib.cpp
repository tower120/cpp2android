#include <jni.h>
#include <string>
#include <functional>
#include <string>
#include <sstream>
#include <android/looper.h>
#include <unistd.h>


#include <cpp2android/android_interface/UISession.h>
#include <cpp2android/android_interface/ThreadLooper.h>
#include "cpp2android/jni_interface/ICallback.h"
#include "cpp2android/jni_interface/JavaVM.h"
#include "cpp2android/jni_interface/JavaObject.h"
#include "cpp2android/jni_interface/Proxies.h"
#include "cpp2android/jni_interface/SignalReceiver.h"
#include "cpp2android/AndroidViews/View.h"
#include "cpp2android/AndroidViews/adopt_view.h"
#include "cpp2android/AndroidViews/ViewGroup.h"
#include "cpp2android/AndroidViews/Button.h"
#include "cpp2android/jni_interface/SignalSender.h"
//#include "jni_interface/JavaObject.h"

#include <cpp2android/bootstrap.cpp>

extern "C"
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    jni_interface::java_vm = vm;
    /*JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }*/

    // Get jclass with env->FindClass.
    // Register methods with env->RegisterNatives.

    return jni_interface::JNI_VERSION;
}

/*template <typename T>
std::string to_string(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}*/


// void setOnCreate()


// TODO: separate init from OnCreate

extern "C"
JNIEXPORT void JNICALL
Java_cpp2android_Main_bottstrapJNI(JNIEnv *env, jobject instance
        , jobject contextObject
        , jobject layoutView
) {
    using namespace jni_interface;
    using namespace android_interface;

    uiThreadLooper.emplace();
    ui_session.emplace(env, contextObject);
    ClassFinder::init(env, contextObject);
    init_sequence().run();

    static SignalSender<> s1("s1");
    {
        using namespace AndroidViews;
        SharedPtr<View> root = adopt_view<View>(layoutView);

        SharedPtr<ViewGroup> vg = StaticCast<ViewGroup>(root);

        /*SignalReceiver<jobject>& signal = SignalReceiver<jobject>::link("btnClick");
        signal.set_unique_listener([vg](jobject obj){
            SharedPtr<Button> btn = adopt_object<Button>(obj);
            btn->setText("AAц");
            vg->setBackgroundColor(0xFFFF0000);

            s1.send();
        });


        vg->setOnClickListener([i=0](const SharedPtr<View>& view) mutable {
            view->setBackgroundColor(0xFF3F51B5+(++i));

            View* p_view = view.get();
            auto p2 = std::move(p_view);
            ViewGroup* vg = static_cast<ViewGroup*>(p2);

            auto button = Button::make("Hello!");
            button->setOnClickListener([i=0](const SharedPtr<View>& view) mutable {
                Button* btn = static_cast<Button*>(view.get());

                btn->setText(to_string(++i).c_str());
            });
            vg->addView(*button);

        });*/
        //jni_main(vg);
        application.onCreate(vg);
    }
}




// TODO: Include as callbacks.cxx

extern "C"
JNIEXPORT void JNICALL
Java_cpp2android_Main_cppCallback(JNIEnv *env, jobject instance,
                                                           jlong c_ptr) {
    using namespace jni_interface;

    ICallback<>* callback = reinterpret_cast<ICallback<>*>(c_ptr);
    callback->run();
}

extern "C"
JNIEXPORT void JNICALL
Java_cpp2android_Main_cppCallbackO(JNIEnv *env, jobject instance,
                                                            jlong c_ptr, jobject obj) {
    using namespace jni_interface;

    ICallback<jobject>* callback = reinterpret_cast<ICallback<jobject>*>(c_ptr);
    callback->run(obj);
}

extern "C"
JNIEXPORT void JNICALL
Java_cpp2android_Main_cppCallbackOO(JNIEnv *env, jobject instance,
                                                     jlong c_ptr, jobject obj1, jobject obj2) {
    using namespace jni_interface;

    ICallback<jobject, jobject>* callback = reinterpret_cast<ICallback<jobject, jobject>*>(c_ptr);
    callback->run(obj1, obj2);
}

extern "C"
JNIEXPORT void JNICALL
Java_cpp2android_Main_cppReleaseCallback(JNIEnv *env, jclass type, jlong c_ptr) {
    using namespace jni_interface;

    ICallbackBase* callback = reinterpret_cast<ICallbackBase*>(c_ptr);
    delete callback;
}

extern "C"
JNIEXPORT void JNICALL
Java_cpp2android_Main_cppReleaseObject(JNIEnv *env, jclass type, jlong c_ptr) {
    using namespace jni_interface;

    JavaObjectRef* obj = reinterpret_cast<JavaObjectRef*>(c_ptr);
    delete obj;
}

// -------------------

extern "C"
JNIEXPORT void JNICALL
Java_cpp2android_Main_cppCallbackOL(JNIEnv *env, jobject instance,
                                    jlong callback_ptr, jobject obj, jlong obj_ptr) {
    using namespace jni_interface;

    ICallback<jobject, jlong>* callback = reinterpret_cast< ICallback<jobject, jlong>* >(callback_ptr);
    callback->run(obj, obj_ptr);
}

extern "C"
JNIEXPORT void JNICALL
Java_cpp2android_Main_cppCallbackL(JNIEnv *env, jclass type, jlong callback_ptr, jlong obj_ptr) {
    using namespace jni_interface;

    ICallback<jlong>* callback = reinterpret_cast< ICallback<jlong>* >(callback_ptr);
    callback->run(obj_ptr);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_cpp2android_Main_cppMakeProxyObject(JNIEnv *env, jclass type, jobject jobj) {
    using namespace jni_interface;

    JavaObjectRef* jref = new JavaObjectRef(jobj);

    JavaObject* javaObject = makeProxy(jref);

    jref->obj.reset(javaObject);

    return (jlong)jref;
}

// ---------

template<class ...Args>
static jlong linkSignal(JNIEnv *env, jstring id_){
    using namespace jni_interface;

    const char *id = env->GetStringUTFChars(id_, 0);
    auto& signal = SignalReceiver<Args...>::link(std::string(id));
    env->ReleaseStringUTFChars(id_, id);

    const void* ptr = static_cast<const void*>(signal.getCallback());
    return (jlong)ptr;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_cpp2android_Main_linkSignal(JNIEnv *env, jclass type, jstring id_) {
    return linkSignal(env, id_);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_cpp2android_Main_linkSignalO(JNIEnv *env, jclass type, jstring id_) {
    return linkSignal<jobject>(env, id_);
}