#pragma once

// Send signal to Java's cpp2android.SignalReceivers.SignalReceiver

#include <jni.h>
#include "Initializable.h"
#include "UniqueGlobalClass.h"


// thread-safe on Java side

namespace jni_interface{
    template<class ...Args>
    class SignalSender;

    template<class Derived, class ...Args>
    class SignalSenderBase{
    protected:
        inline static struct MethodIds : Initializable<MethodIds>{
            inline static UniqueGlobalClass clazz;
            inline static jmethodID pulse;
            inline static jmethodID link;

            static void init(){
                JNIEnv* env = this_thread.getEnv();
                clazz = {ClassFinder::find(env, Derived::class_name)};
                assert(clazz.get());
                pulse = env->GetStaticMethodID(clazz.get(), "pulse", Derived::pulse_sig);
                assert(pulse);
                link  = env->GetStaticMethodID(clazz.get(), "link", "(Ljava/lang/String;)I");
                assert(link);
            }
        } methodIds{};      // another ndk/clang bug: #https://github.com/android-ndk/ndk/issues/594

        static jint link(const std::string& id){
            JNIEnv* env = this_thread.getEnv();
            jstring jstr = env->NewStringUTF(id.c_str());
            jint res = env->CallStaticIntMethod(methodIds.clazz.get(), methodIds.link, jstr);
            env->DeleteLocalRef(jstr);

            return res;
        }

        jint id;

    public:
        SignalSenderBase(const std::string& id)
            : id (link(id))
        {}

        void send(Args... args){
            JNIEnv* env = this_thread.getEnv();
            env->CallStaticVoidMethod(methodIds.clazz.get(), methodIds.pulse, id, args...);
        }
    };


    // ndk issue #https://github.com/android-ndk/ndk/issues/591
    class SignalSenderVoid : public SignalSenderBase<SignalSenderVoid>{
        friend SignalSenderBase;
        static constexpr const char* class_name = "cpp2android/SignalReceivers/SignalReceiver";
        static constexpr const char* pulse_sig  = "(I)V";
    public:
        using SignalSenderBase::SignalSenderBase;
    };
    template<>
    class SignalSender<> : public SignalSenderVoid{
        using SignalSenderVoid::SignalSenderVoid;
    };


    class SignalSenderObject : public SignalSenderBase<SignalSenderObject>{
        friend SignalSenderBase;
        static constexpr const char* class_name = "cpp2android/SignalReceivers/SignalReceiverO";
        static constexpr const char* pulse_sig  = "(ILjava/lang/Object;)V";
    public:
        using SignalSenderBase::SignalSenderBase;
    };
    template<>
    class SignalSender<jobject> : public SignalSenderObject{
        using SignalSenderObject::SignalSenderObject;
    };

}

