#pragma once

#include "JavaVM.h"

/*
Use as:
    inline static struct MethodIds : Initializable<MethodIds>{
         static void init(){
            // do something once, when everything is initialized
        }
    } method_ids;
 */

namespace jni_interface{
    template<class Derived>
    struct Initializable {
        Initializable(){
            jni_interface::init_sequence().add([](){
                Derived::init();
            });
        }
    };
}