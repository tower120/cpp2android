#pragma once

#include <iostream>
#include <sstream>

#include <cpp2android/AndroidViews/ViewGroup.h>
#include <cpp2android/AndroidViews/Button.h>

class Application {
public:
    template <typename T>
    static std::string to_string(T value)
    {
        std::ostringstream os ;
        os << value ;
        return os.str() ;
    }

    void onCreate(const jni_interface::SharedPtr<AndroidViews::ViewGroup>& root){
        using namespace jni_interface;
        using namespace android_interface;
        using namespace AndroidViews;

        auto& vg = root;

        vg->setOnClickListener([i=0](const SharedPtr<View>& view) mutable {
            view->setBackgroundColor(0xFF3F51B5+(++i));

            View* p_view = view.get();
            auto p2 = std::move(p_view);
            ViewGroup* vg = static_cast<ViewGroup*>(p2);

            auto button = Button::make("Hello!");
            button->setOnClickListener([i=0](const SharedPtr<View>& view) mutable {
                Button* btn = static_cast<Button*>(view.get());

                btn->setText(to_string(++i).c_str());
            });
            vg->addView(*button);

        });
    }
};