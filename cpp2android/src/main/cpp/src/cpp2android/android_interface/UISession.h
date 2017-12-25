#ifndef CPPTEST1_SESSION_H
#define CPPTEST1_SESSION_H


#include <thread>
#include <cassert>
#include <experimental/optional>
#include "../jni_interface/ThisThread.h"

namespace android_interface{
    // construct/destruct on main thread only
    class UISession{
        jobject m_applicationContext;
        JNIEnv* m_env;

        std::thread::id thread_id;  // for debug purporses only
        void checkThread() const{
            assert(thread_id ==  std::this_thread::get_id());
        }
    public:
        UISession(JNIEnv* env, jobject applicationContext){
            m_env = env;
            m_applicationContext =  env->NewGlobalRef(applicationContext);

            thread_id =  std::this_thread::get_id();
        }

        jobject context(){
            return m_applicationContext;
        }

        ~UISession(){
            if (!m_applicationContext) return;
            checkThread();
            jni_interface::this_thread.getEnv()->DeleteGlobalRef(m_applicationContext);
        }
    };

    static std::experimental::optional<UISession> ui_session{};
}

#endif //CPPTEST1_SESSION_H
