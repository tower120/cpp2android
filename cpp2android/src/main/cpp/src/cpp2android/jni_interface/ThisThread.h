#pragma once

#include <jni.h>
#include <cassert>
#include "JavaVM.h"

namespace jni_interface{

    class ThisThread{
        inline static thread_local pthread_key_t p_key;

        static void pthread_dstr(void *arg){
            if (!jni_env) return;
            java_vm->DetachCurrentThread();
            jni_env = nullptr;

            pthread_setspecific(p_key, NULL);
            pthread_key_delete(p_key);
        }

        static void register_dstr(void *arg){
            {
                const int res = pthread_key_create(&p_key, pthread_dstr);
                assert(res != EAGAIN);
                assert(res != ENOMEM);
                assert(res == 0);
            }
            {
                const int res = pthread_setspecific(p_key, arg);
                assert(res == 0);
            }
        }

        inline static thread_local JNIEnv* jni_env{nullptr};
    public:
        JNIEnv* getEnv(){
            if (!jni_env){
                assert(java_vm);
                java_vm->GetEnv((void**)&jni_env, JNI_VERSION);
                java_vm->AttachCurrentThread(&jni_env, NULL);       // safe to call in main thread

                register_dstr(jni_env);
            }
            assert(jni_env);

            return jni_env;
        }
    };

    static thread_local ThisThread this_thread;
}