#pragma once

#include "UniqueBaseRef.h"

namespace jni_interface{

    class UniqueWeakRef : public UniqueBaseRef{
    public:
        UniqueWeakRef(std::nullptr_t) : UniqueBaseRef(nullptr){}

        UniqueWeakRef(jobject object)
            :UniqueBaseRef(this_thread.getEnv()->NewWeakGlobalRef(object))
        {
            assert(m_object);
        }

        UniqueWeakRef(UniqueWeakRef&& other) noexcept : UniqueBaseRef(std::move(other)) {}

        UniqueWeakRef& operator=(UniqueWeakRef&& other) noexcept {
            this->~UniqueWeakRef();
            UniqueBaseRef::operator=(std::move(other));
            return *this;
        }

        ~UniqueWeakRef() noexcept {
            if (m_object == nullptr) return;
            this_thread.getEnv()->DeleteWeakGlobalRef(m_object);
        }
    };

}