#pragma once

#include "View.h"
#include "../android_interface/UISession.h"
#include "../jni_interface/JavaVM.h"
#include "../jni_interface/UniqueGlobalClass.h"
#include "../jni_interface/JavaObject.h"
#include "adopt_view.h"
#include "../jni_interface/Proxies.h"


namespace AndroidViews{
    class Button : public View{
    protected:
        inline static struct MethodIds : MethodIdsBase<MethodIds>{
            inline static jni_interface::UniqueGlobalClass clazz{};

            inline static jmethodID constructor;
            inline static jmethodID setText;

            static void init(){
                using namespace android_interface;
                using namespace jni_interface;

                JNIEnv* env = this_thread.getEnv();
                clazz = {ClassFinder::find(env, "android/widget/Button")};
                constructor = env->GetMethodID(clazz.get(), "<init>", "(Landroid/content/Context;)V");
                setText = env->GetMethodID(clazz.get(), "setText", "(Ljava/lang/CharSequence;)V");

                register_proxy<Button>(clazz.get());
            }
        } method_ids;


        static jobject make_jobject(){
            using namespace android_interface;
            using namespace jni_interface;

            JNIEnv* env = this_thread.getEnv();
            jobject jbutton = env->NewObject(method_ids.clazz.get(), method_ids.constructor, ui_session->context() );
            assert(jbutton);

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
}
