#pragma once

#include "View.h"
#include "../jni_interface/JavaObject.h"

namespace AndroidViews{
    template<class T, typename = std::enable_if_t<std::is_base_of<View, T>::value> >
    static jni_interface::SharedPtr<T> adopt_view(jobject jobj){
        return jni_interface::adopt_object<T>(jobj);
    }
}