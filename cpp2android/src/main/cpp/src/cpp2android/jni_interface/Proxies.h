#pragma once

#include <cstdlib>
#include <iostream>
#include <string>
#include <functional>
#include <unordered_map>
#include <jni.h>

#include "ThisThread.h"
#include "JavaObject.h"

#include "../utils/make_constructable.h"

#if defined(ANDROID) || defined(__ANDROID__)
    #include <android/log.h>
#endif

namespace jni_interface{
    class JavaObject;
    class JavaObjectRef;

    namespace details::Proxies {
        // always access from main thread, no need to sync
        using ProxyMakerFn = std::function<JavaObject*(JavaObjectRef*)>;
        static std::unordered_map<std::string, ProxyMakerFn> proxy_class_list;

        template<class T>
        inline static ProxyMakerFn defaultProxyMakerFn(
            [](JavaObjectRef* jref) -> JavaObject* {
                using namespace cpp2android::utils;
                return new make_constructable<T>(jref, proxy_t{});
            }
        );

        static std::string getClassName(jclass clazz){
            JNIEnv* env = this_thread.getEnv();

            // First get the class object
            /*jmethodID mid = env->GetMethodID(cls, "getClass", "()Ljava/lang/Class;");
            jobject clsObj = env->CallObjectMethod(obj, mid);*/

            // Now get the class object's class descriptor
            jclass clsClazz = env->GetObjectClass(clazz);

            // Find the getName() method on the class object
            jmethodID mid = env->GetMethodID(clsClazz, "getName", "()Ljava/lang/String;");

            // Call the getName() to get a jstring object back
            jstring strObj = (jstring)env->CallObjectMethod(clazz, mid);

            // Now get the c string from the java jstring object
            const char* chars = env->GetStringUTFChars(strObj, NULL);
            std::string str(chars);


            // Release the memory pinned char array
            env->ReleaseStringUTFChars(strObj, chars);

            env->DeleteLocalRef(clsClazz);
            env->DeleteLocalRef(strObj);

            return std::move(str);
        }

        static bool try_register_proxy(jclass clazz, const details::Proxies::ProxyMakerFn& proxy_maker) {
            using namespace details::Proxies;

            const auto inserted_pair = proxy_class_list.emplace(getClassName(clazz), proxy_maker);
            const bool inserted = inserted_pair.second;
            return inserted;
        }

        template<class T>
        static bool try_register_proxy(jclass clazz){
            using namespace details::Proxies;
            return try_register_proxy(clazz, defaultProxyMakerFn<T>);
        }
    }


    static bool register_proxy(jclass clazz, const details::Proxies::ProxyMakerFn& proxy_maker) {
        using namespace details::Proxies;
        const bool inserted = try_register_proxy(clazz, proxy_maker);

        if (!inserted){
            using namespace details::Proxies;
            const std::string class_name = getClassName(clazz);

            #if defined(ANDROID) || defined(__ANDROID__)
                __android_log_print(ANDROID_LOG_WARN, "cpp2android", "Proxy class %s already exists!", class_name.c_str());
            #endif

            std::cerr << "Proxy class " << class_name << " already exists!";
        }

        return inserted;
    }

    template<class T>
    static void register_proxy(jclass clazz) {
        using namespace details::Proxies;
        register_proxy(clazz, defaultProxyMakerFn<T>);
    }

    static void override_proxy(jclass clazz, const details::Proxies::ProxyMakerFn& proxy_maker) {
        using namespace details::Proxies;

        proxy_class_list.erase(getClassName(clazz));
        register_proxy(clazz, proxy_maker);
    }

    template<class T>
    static void override_proxy(jclass clazz) {
        using namespace details::Proxies;
        override_proxy(clazz, defaultProxyMakerFn<T>);
    }



    static JavaObject* makeProxy(JavaObjectRef* jref){
        using namespace details::Proxies;
        JNIEnv* env = this_thread.getEnv();
        const jobject jobj = jref->weak_ref.get();

        // get class
        jclass clazz = env->GetObjectClass(jobj);
        while(clazz) {
            const std::string class_name = getClassName(clazz);
            const auto result = proxy_class_list.find(class_name);
            if (result != proxy_class_list.end()) {
                // found proxy;
                const ProxyMakerFn &proxy_maker = result->second;
                return proxy_maker(jref);
            }

            // not found, try super.
            clazz = env->GetSuperclass(clazz);
        }

        // found nothing!!!
        {
            const std::string class_name = getClassName(env->GetObjectClass(jobj));

            #if defined(ANDROID) || defined(__ANDROID__)
                __android_log_print(ANDROID_LOG_ERROR, "cpp2android", "Proxy class, or it parents %s not found!", class_name.c_str());
                std::abort();
            #endif

            std::cerr << "Proxy class " << class_name << " already exists!";
            std::abort();
        }
        return nullptr;
    }

}