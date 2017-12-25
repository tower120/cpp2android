#pragma once

#include <jni.h>
#include "ThisThread.h"

namespace jni_interface{
    class UniqueUniversalRef;

    class UniqueBaseRef{
        friend UniqueUniversalRef;
    protected:
        jobject m_object{nullptr};

        UniqueBaseRef(){}
        UniqueBaseRef(jobject obj) : m_object(obj){}

        UniqueBaseRef& operator=(UniqueBaseRef&& other) noexcept {
            m_object = std::move(other.m_object);
            other.m_object = nullptr;
            return *this;
        }
    public:
        explicit operator bool() const{
            return m_object != nullptr;
        }
        bool operator==(const UniqueBaseRef& other) const {
            return this_thread.getEnv()->IsSameObject(get(), other.get());
        }
        bool operator!=(const UniqueBaseRef& other) const {
            return !this->operator==(other);
        }

        UniqueBaseRef(const UniqueBaseRef&) = delete;

        UniqueBaseRef(UniqueBaseRef&& other) noexcept
            :m_object(std::move(other.m_object))
        {
            other.m_object = nullptr;
        }

        jobject get() const{
            return m_object;
        }
    };
}