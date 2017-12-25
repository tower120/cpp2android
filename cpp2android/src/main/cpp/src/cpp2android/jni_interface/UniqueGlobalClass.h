#pragma once

#include "UniqueGlobalRef.h"

namespace jni_interface {
    class UniqueGlobalClass : protected UniqueGlobalRef{
    public:
        UniqueGlobalClass(){}
        UniqueGlobalClass(jclass clazz) : UniqueGlobalRef(clazz){}
        UniqueGlobalClass(UniqueGlobalClass&& other) noexcept : UniqueGlobalRef(std::move(other)){}
        UniqueGlobalClass& operator = (UniqueGlobalClass&& other) noexcept {
            UniqueGlobalRef::operator=(std::move(other));
            return *this;
        }

        jclass get() const{
            return reinterpret_cast<jclass>(UniqueGlobalRef::get());
        }
    };
}