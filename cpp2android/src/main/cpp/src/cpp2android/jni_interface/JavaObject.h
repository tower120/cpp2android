#pragma once

#include "UniqueWeakRef.h"
#include "UniqueGlobalRef.h"
#include "../utils/make_constructable.h"
#include "Initializable.h"
#include "UniqueGlobalClass.h"


namespace jni_interface {
    class JavaObject;
    template<class, bool> class SharedPtr;


    // may be created by java makeProxyObject
    // destructed by java finalizer
    struct JavaObjectRef {
        //template<class> friend class SharedPtr;

        JavaObjectRef(const JavaObjectRef&) = delete;
        JavaObjectRef(JavaObjectRef&&) = delete;

        UniqueWeakRef   weak_ref;
        UniqueGlobalRef global_ref{nullptr};

        std::unique_ptr<JavaObject> obj{nullptr};

        using Lock = std::mutex;                    // TODO: Spinlock here
        Lock lock;

        unsigned int use_count{0};

        void decrease_use_count(){
            std::unique_lock l(lock);
            assert(use_count > 0);
            use_count--;
            if (use_count == 0){
                assert(global_ref);
                global_ref.reset();
            }
        }

        void increase_use_count(){
            std::unique_lock l(lock);
            if (use_count == 0){
                assert(!global_ref);
                global_ref = jni_interface::UniqueGlobalRef(weak_ref.get());
                assert(global_ref);
            }
            use_count++;
        }

        // TODO: hide constructor
        explicit JavaObjectRef(jobject obj)
            :weak_ref(obj)
        {}

        jobject get_object() const {
            assert(global_ref);
            return global_ref.get();
        }
    };

    struct proxy_t{};


    // Object itself
    class JavaObject{
        template<class, bool> friend class SharedPtr;

        JavaObject(const JavaObject&) = delete;
        JavaObject(JavaObject&&) = delete;

        JavaObjectRef* obj_ref{nullptr};
    protected:
        explicit JavaObject(JavaObjectRef* obj_ref)
            :obj_ref(obj_ref)
        {}

        explicit JavaObject(JavaObjectRef* obj_ref, proxy_t)
            :obj_ref(obj_ref)
        {}

        template<class T, class ...Args>
        static SharedPtr<T, true> make(jobject jobj, Args&&...);

    public:
        jobject get_object() const{
            return obj_ref->get_object();
        }

        virtual ~JavaObject(){}
    };



    // use std::shared_ptr with custom destructor, instead?
    template<class ObjectT, bool use_shared_counter = true>
    class SharedPtr {
    protected:
        ObjectT* obj;

        JavaObjectRef* getObjectRef() const{
            return obj->obj_ref;
        }

        void increase_use_count(){
            if (obj){
                getObjectRef()->increase_use_count();
            }
        }

        void decrease_use_count(){
            if (obj){
                getObjectRef()->decrease_use_count();
            }
        }

    public:
        template<bool increase_counter = true>
        SharedPtr(ObjectT* obj, std::integral_constant<bool, increase_counter> m = {})
            : obj(obj)
        {
            if (use_shared_counter && increase_counter) increase_use_count();
        }

        SharedPtr(const SharedPtr& other) noexcept
            : obj(other.obj)
        {
            if (use_shared_counter) increase_use_count();
        }

        SharedPtr(SharedPtr&& other) noexcept
            : obj(other.obj)
        {
            other.obj = nullptr;
        }


        ObjectT* get() const{
            return obj;
        }

        ObjectT* operator->() const{
            return get();
        }
        ObjectT& operator*() const{
            return *get();
        }
        explicit operator bool() const {
            return get() != nullptr;
        }

        ~SharedPtr(){
            if (use_shared_counter) decrease_use_count();
        }


        template<class To, class From>
        friend SharedPtr<To> StaticCast(const SharedPtr<From> &from);

        template<class To, class From>
        friend SharedPtr<To> StaticCast(SharedPtr<From>&& from);
    };


    template<class To, class From>
    static SharedPtr<To> StaticCast(const SharedPtr<From>& from){
        return {static_cast<To*>(from.get())};
    }

    template<class To, class From>
    static SharedPtr<To> StaticCast(SharedPtr<From>&& from){
        SharedPtr<To> out {
            static_cast<To*>(from.get())
            , std::false_type{}
        };

        from.obj = nullptr;

        return std::move(out);
    }


    namespace details::NSJavaObject{
        inline static struct MethodIds : jni_interface::Initializable<MethodIds>{
            inline static jni_interface::UniqueGlobalClass clazz;
            inline static jmethodID addCppObject;
            inline static jmethodID getCppObject;

            static void init(){
                using namespace android_interface;
                using namespace jni_interface;
                JNIEnv* env = this_thread.getEnv();
                clazz = {ClassFinder::find(env, "cpp2android/Main")};
                addCppObject = env->GetStaticMethodID(clazz.get(), "addCppObject", "(Ljava/lang/Object;J)J");   assert(addCppObject);
                getCppObject = env->GetStaticMethodID(clazz.get(), "getCppObject", "(Ljava/lang/Object;)J");    assert(getCppObject);
            }
        } method_ids;

        static void* addCppObject(jobject jobj, void* ptr){
            using namespace jni_interface;
            const long inserted_obj = this_thread.getEnv()->CallStaticLongMethod(method_ids.clazz.get(), method_ids.addCppObject,
                                                                                 jobj, ptr );
            return (void*)inserted_obj;
        }
        static jni_interface::JavaObjectRef* getCppObject(jobject jobj){
            using namespace jni_interface;
            const long obj = this_thread.getEnv()->CallStaticLongMethod(method_ids.clazz.get(), method_ids.getCppObject, jobj);
            return (JavaObjectRef*)obj;
        }

        static jni_interface::JavaObjectRef* make_ref(jobject jobj) {
            using namespace details::NSJavaObject;
            using namespace jni_interface;

            JavaObjectRef* ref = new JavaObjectRef(jobj);
            assert(ref);
            JavaObjectRef* inserted_ref = static_cast<JavaObjectRef*>(addCppObject(jobj, ref));
            assert(ref == inserted_ref);

            return ref;
        }
    }


    template<class T, class ...Args>
    SharedPtr<T, true> JavaObject::make(jobject jobj, Args&&... args) {
        using namespace details::NSJavaObject;
        using namespace cpp2android::utils;

        JavaObjectRef* jref = make_ref(jobj);

        jref->increase_use_count();

        T* obj = new make_constructable<T>(jref, std::forward<Args>(args)...);
        jref->obj.reset(obj);

        return {obj, std::false_type()};
    }

    template<class T = JavaObject>
    static jni_interface::SharedPtr<T> adopt_object(jobject jobj){
        using namespace details::NSJavaObject;
        using namespace jni_interface;

        JavaObjectRef* obj_ref = getCppObject(jobj);
        T* obj = static_cast<T*>(obj_ref->obj.get());
        return {obj};
    }

    template<class T = JavaObject>
    static jni_interface::SharedPtr<T> restore_object(jlong c_ptr){
        using namespace details::NSJavaObject;
        using namespace jni_interface;

        JavaObjectRef* obj_ref = static_cast<JavaObjectRef*>((void*)c_ptr);
        T* obj = static_cast<T*>(obj_ref->obj.get());
        return {obj};
    }

}