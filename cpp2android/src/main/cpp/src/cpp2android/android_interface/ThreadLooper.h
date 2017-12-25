#pragma once

#include <unistd.h>
#include <android/looper.h>
#include <android/log.h>

#include <utility>
#include <cassert>
#include <thread>
#include <vector>
#include <mutex>

#include <experimental/optional>

namespace android_interface {
    class ThreadLooper {
        using Lock = std::mutex;        // TODO: put spinLock here
        Lock lock;

        int messagePipe[2];
        const char magic_msg = 100;

        using Callback = std::function<void()>;
        using CallList = std::vector<Callback>;
        CallList call_list;

        bool looperCallbackRequested{false};

        // this will be called on main thread
        static int looperCallback(int fd, int events, void *self_ptr) {
            ThreadLooper &self = *static_cast<ThreadLooper *>(self_ptr);

            assert(self.thread_id == std::this_thread::get_id());

            CallList list;
            {
                std::unique_lock<Lock> l(self.lock);
                std::swap(list, self.call_list);

                // read message from pipe
                char msg;
                const auto readed = read(fd, &msg, 1);
                assert(readed == 1 && msg == self.magic_msg);

                self.looperCallbackRequested = false;
            }

            for (Callback &callback : list) {
                callback();
            }

            return 1; // continue listening for events
        }

        ALooper *mainThreadLooper = nullptr;
        std::thread::id thread_id;
    public:
        ThreadLooper() {
            // get looper for this thread / create new
            mainThreadLooper = ALooper_prepare(0);
            assert(mainThreadLooper != nullptr);

            // add reference to keep object alive
            ALooper_acquire(mainThreadLooper);

            //create send-receive pipe
            {
                const auto pipe_err = pipe(messagePipe);
                assert(pipe_err == 0);
            }

            // register callback
            {
                const auto res = ALooper_addFd(mainThreadLooper, messagePipe[0],
                                               0, ALOOPER_EVENT_INPUT, looperCallback, this);
                assert(res == 1);
            }

            thread_id = std::this_thread::get_id();
        }

        bool onSameThread() const {
            return thread_id == std::this_thread::get_id();
        }

        template<class CallbackT>
        void execute(CallbackT &&callback) {
            if (onSameThread()) {
                callback();
                return;
            }

            std::unique_lock<Lock> l(lock);
            call_list.emplace_back(std::forward<Callback>(callback));

            if (!looperCallbackRequested) {
                looperCallbackRequested = true;

                const auto writed = write(messagePipe[1], &magic_msg, 1);
                assert(writed == 1);
            }
        }

        ~ThreadLooper() {
            if (!mainThreadLooper) return;

            ALooper_removeFd(mainThreadLooper, messagePipe[0]);

            close(messagePipe[0]);
            close(messagePipe[1]);

            ALooper_release(mainThreadLooper);
        }
    };

    static std::experimental::optional<ThreadLooper> uiThreadLooper{};  // start uninited
}