#pragma once

#include "ThisThread.h"
#include "UniqueLocalRef.h"
#include "UniqueBaseRef.h"
#include "../android_interface/ThreadLooper.h"

namespace jni_interface{
    class UniqueUniversalRef;

    class UniqueGlobalRef : public UniqueBaseRef{
        friend UniqueUniversalRef;
    public:
        explicit UniqueGlobalRef(){}
        explicit UniqueGlobalRef(std::nullptr_t){}
        explicit UniqueGlobalRef(jobject object){
            if (object == nullptr) return;
            m_object = this_thread.getEnv()->NewGlobalRef(object);
            assert(m_object);
        }

        UniqueGlobalRef(UniqueGlobalRef&& other) noexcept : UniqueBaseRef(std::move(other)) {}

        UniqueGlobalRef& operator=(UniqueGlobalRef&& other) noexcept {
            this->~UniqueGlobalRef();

            m_object = std::move(other.m_object);
            other.m_object = nullptr;

            return *this;
        }

        void reset(){
            this->~UniqueGlobalRef();
            m_object = nullptr;
        }

        // promote to global
        explicit UniqueGlobalRef(UniqueLocalRef&& localRef)
                : UniqueGlobalRef(localRef.get())
        {
            localRef.~UniqueLocalRef();
            localRef.m_object = nullptr;
        }


        ~UniqueGlobalRef() noexcept {
            if (m_object == nullptr) return;
            this_thread.getEnv()->DeleteGlobalRef(m_object);
        }
    };
}