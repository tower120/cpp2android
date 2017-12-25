#pragma once

#include <mutex>

#include "View.h"
#include "../jni_interface/Proxies.h"
#include "../android_interface/UISession.h"

namespace AndroidViews{
    class ViewGroup : public View{
    protected:
        inline static struct MethodIds : MethodIdsBase<MethodIds>{
            inline static jni_interface::UniqueGlobalClass clazz;

            inline static jmethodID constructor;
            inline static jmethodID addView;

            static void init(){
                using namespace android_interface;
                using namespace jni_interface;

                JNIEnv* env = this_thread.getEnv();
                clazz = {ClassFinder::find(env, "android/view/ViewGroup")};
                constructor = env->GetMethodID(clazz.get(), "<init>", "(Landroid/content/Context;)V");
                addView = env->GetMethodID(clazz.get(), "addView", "(Landroid/view/View;)V");

                register_proxy<ViewGroup>(clazz.get());
            }
        } method_ids;

        using View::View;
    public:

        void addView(View& view){
            JNIEnv* env = jni_interface::this_thread.getEnv();
            env->CallVoidMethod(get_object(), method_ids.addView, view.get_object());
        }
    };
}
