#pragma once

#include <jni.h>
#include "UniqueBaseRef.h"
#include "ThisThread.h"

namespace jni_interface{
    class UniqueGlobalRef;

    class UniqueLocalRef : public UniqueBaseRef {
        friend UniqueGlobalRef;
    public:
        UniqueLocalRef(){}
        UniqueLocalRef(jobject object) : UniqueBaseRef(object) {}
        UniqueLocalRef(UniqueLocalRef&& other) noexcept : UniqueBaseRef(std::move(other)) {}

        UniqueLocalRef& operator=(UniqueLocalRef&& other) noexcept {
            this->~UniqueLocalRef();

            m_object = std::move(other.m_object);
            other.m_object = nullptr;

            return *this;
        }

        void release() {
            m_object = nullptr;
        }

        ~UniqueLocalRef() noexcept {
            if (m_object == nullptr) return;
            this_thread.getEnv()->DeleteLocalRef(m_object);
        }
    };
}