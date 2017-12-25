#pragma once

namespace cpp2android::utils{
    template<class T>
    struct make_constructable : public T{
        make_constructable(const make_constructable&) = delete;
        make_constructable(make_constructable&&) = delete;

        template<class ...Args>
        make_constructable(Args&&... args) : T(std::forward<Args>(args)...){}
    };
}