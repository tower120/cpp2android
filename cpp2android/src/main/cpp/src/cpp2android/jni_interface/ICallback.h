#pragma once

namespace jni_interface {
    struct ICallbackBase{
        virtual ~ICallbackBase(){};
    };

    template<class ...Args>
    struct ICallback : ICallbackBase{

        virtual void run(Args...) = 0;

        template<class Closure>
        static ICallback<Args...>* make(Closure&& closure){
            // save 1 pointer in size
            using closure_t = std::decay_t<Closure>;
            struct Callback : ICallback<Args...>, closure_t {
                Callback(const closure_t& lambda) : closure_t(lambda){}
                Callback(closure_t&& lambda) : closure_t(std::move(lambda)){}

                virtual void run(Args... args) override {
                    closure_t::operator()(args...);
                }
            };

            return new Callback(std::forward<Closure>(closure));
        }
    };

    //TODO: ICallbackSBO
}