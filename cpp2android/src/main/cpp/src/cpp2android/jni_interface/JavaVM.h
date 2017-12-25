#pragma once

#include <jni.h>
#include <vector>
#include <deque>
#include <mutex>

namespace jni_interface{
    static JavaVM* java_vm{nullptr};
    const static jint JNI_VERSION = JNI_VERSION_1_6;

    class InitSequence{
        using Lock = std::mutex;
        Lock lock;

        std::deque<std::function<void()>> list;

        bool inited = false;
    public:
        template<class Closure>
        void add(Closure&& closure){
            {
                std::unique_lock l(lock);

                if (!inited) {
                    list.emplace_back(std::forward<Closure>(closure));
                    return;
                }
            }

            closure();
        }

        void run(){
            std::unique_lock l(lock);
            if (inited) return;

            for(auto& fn : list){
                fn();
            }

            inited = true;
        }
    };
    static InitSequence& init_sequence(){
        static InitSequence seq;
        return seq;
    }


    struct ClassFinder{
        // exists forever, do not call DeleteGlobalRef
        inline static jobject gApplicationClassLoader;
        inline static jmethodID gApplicationFindClassMethod;

        inline static bool inited{false};

        static void init(JNIEnv* env, jobject applicationContext){
            if (inited) return;

            // findClass from #https://stackoverflow.com/a/16302771/1559666
            jclass classClass = env->GetObjectClass(applicationContext);

            auto classLoaderClass = env->FindClass("java/lang/ClassLoader");
            auto getClassLoaderMethod = env->GetMethodID(classClass, "getClassLoader",
                                                         "()Ljava/lang/ClassLoader;");
            gApplicationClassLoader = env->NewGlobalRef(env->CallObjectMethod(applicationContext, getClassLoaderMethod));
            gApplicationFindClassMethod = env->GetMethodID(classLoaderClass, "findClass",
                                                           "(Ljava/lang/String;)Ljava/lang/Class;");
        }

        static jclass find(JNIEnv* env, const char* name) {
            assert(!env->ExceptionCheck());

            jclass clazz;
            clazz = env->FindClass(name);

            const jboolean flag = env->ExceptionCheck();
            if (!flag) return clazz;

            env->ExceptionClear();
            jstring jstr = env->NewStringUTF(name);
                clazz = static_cast<jclass>(env->CallObjectMethod(gApplicationClassLoader, gApplicationFindClassMethod, jstr));
            env->DeleteLocalRef(jstr);

            return clazz;
        }
    };
}