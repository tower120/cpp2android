#pragma once

#include <unordered_map>
#include <cassert>
#include <deque>
#include <functional>
#include <list>
#include <memory>
#include <algorithm>

#include "ICallback.h"

// Listen signals from java
// aka SignalReceiver
// main thread only!

namespace jni_interface{
    template<class ...Args>
    class SignalReceiver;

    namespace details::SignalReceiver{
        template<class ...Args>
        inline static std::unordered_map<std::string, jni_interface::SignalReceiver<Args...>> signal_map{};
    }

    // non-threadsafe
    // use from main thread only
    template<class ...Args>
    class SignalReceiver{
        using Self = SignalReceiver<Args...>;

        struct hidden_t{};

        SignalReceiver(const SignalReceiver&) = delete;
        SignalReceiver(SignalReceiver&&) = delete;

        std::string id;

        using Callback = ICallback<Args...>;
        std::unique_ptr<Callback> callback;

    public:SignalReceiver(const std::string& id, hidden_t)
            : id(id)
            , callback( Callback::make([&](Args... args){
                    pulse(args...);
                }))
        {}

        const Callback* getCallback() const{
            return callback.get();
        }

    public:     using Delegate = std::function<void(Args...)>;
    private:    std::list<std::unique_ptr<Delegate>> listeners;     // TODO: replace with atomic intrusive list
    public:
        static SignalReceiver<Args...>& link(const std::string& id){
            auto& map = details::SignalReceiver::signal_map<Args...>;

            auto iter = map.find(id);
            if (iter != map.end()){
                return iter->second;
            } else {
                auto result = map.emplace(std::piecewise_construct,
                          std::forward_as_tuple(id),
                          std::forward_as_tuple(id, hidden_t{}));
                const bool inserted = result.second;     assert(inserted);

                return result.first->second;
            }
        }

        template<class ...ArgsT>
        void pulse(ArgsT&&... args){
            auto i = std::begin(listeners);

            while (i != std::end(listeners)) {
                auto& listener = *i;
                ++i;

                (*listener)(std::forward<ArgsT>(args)...);
            }
        }

        void set_unique_listener(Delegate* delegate){
            listeners.clear();
            listeners.emplace_back(delegate);
        }

        template<class Closure, typename = std::enable_if_t<
                !std::is_same<
                        std::decay_t<Closure>, Delegate*
                >::value>>
        void set_unique_listener(Closure&& closure){
            set_unique_listener(new Delegate(std::forward<Closure>(closure)));
        }

        void operator+=(Delegate* delegate){
            listeners.emplace_back(delegate);
        }

        template<class Closure, typename = std::enable_if_t<
            !std::is_same<
                std::decay_t<Closure>, Delegate*
            >::value>>
        void operator+=(Closure&& closure){
            (*this) += new Delegate(std::forward<Closure>(closure));
        };

        void operator-=(Delegate* delegate){
            auto position = std::find(listeners.begin(), listeners.end(), delegate);
            if (position != listeners.end()) // == myVector.end() means the element was not found
                listeners.erase(position);
        }
    };

}