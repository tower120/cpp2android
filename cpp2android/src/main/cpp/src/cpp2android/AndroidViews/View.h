#pragma once

#include <jni.h>

#include <cassert>

#include "../jni_interface/JavaVM.h"
#include "../jni_interface/UniqueGlobalClass.h"
#include "../jni_interface/ICallback.h"
#include "../jni_interface/UniqueWeakRef.h"
#include "../jni_interface/JavaObject.h"
#include "../jni_interface/Proxies.h"
#include "../jni_interface/Initializable.h"

namespace AndroidViews{
    template<class Derived>
    using MethodIdsBase = jni_interface::Initializable<Derived>;

    class View : public jni_interface::JavaObject {
    protected:
        template<class T, class ...Args
                , typename = std::enable_if_t<std::is_base_of<View, T>::value>>
        static auto make_view(Args&&...args){
            return jni_interface::JavaObject::make<T>(std::forward<Args>(args)...);
        }

        inline static struct InterfaceMethodIds : MethodIdsBase<InterfaceMethodIds>{
            inline static jni_interface::UniqueGlobalClass clazz;
            inline static jmethodID setOnClickListener;

            static void init(){
                using namespace android_interface;
                using namespace jni_interface;
                JNIEnv* env = this_thread.getEnv();
                clazz = {ClassFinder::find(env, "cpp2android/View")};
                setOnClickListener = env->GetStaticMethodID(clazz.get(), "setOnClickListener", "(Landroid/view/View;J)V");
            }
        } interface_method_ids;


        inline static struct MethodIds : MethodIdsBase<MethodIds>{
            inline static jni_interface::UniqueGlobalClass clazz;
            inline static jmethodID setBackgroundColor;

            static void init(){
                using namespace android_interface;
                using namespace jni_interface;
                JNIEnv* env = this_thread.getEnv();
                clazz = {ClassFinder::find(env, "android/view/View")};

                setBackgroundColor = env->GetMethodID(clazz.get(), "setBackgroundColor", "(I)V");

                register_proxy<View>(clazz.get());
            }
        } method_ids;

        using jni_interface::JavaObject::JavaObject;

        //View(jobject jobj) : jni_interface::JavaObject(jobj){}
    public:
        virtual void setBackgroundColor(int color) {
            JNIEnv* env = jni_interface::this_thread.getEnv();
            env->CallVoidMethod(get_object(), method_ids.setBackgroundColor, color);
        }


        virtual void setOnClickListener(std::function<void(const jni_interface::SharedPtr<View>&)>&& closure) {
            using namespace android_interface;
            using namespace jni_interface;

            auto* callback = ICallback<jlong>::make(
                [closure = std::move(closure)](jlong obj_ptr){
                    SharedPtr<View> view = restore_object<View>(obj_ptr);
                    assert(view);
                    closure(view);
                }
           );

            JNIEnv* env = this_thread.getEnv();
            env->CallStaticVoidMethod(interface_method_ids.clazz.get(), interface_method_ids.setOnClickListener,
                                      get_object(), callback);
        }

        virtual ~View(){}
    };

}