Any JavaObject can be proxy. Which means, that it can be "created" from java side.

This is needed to work with objects, which created not from cpp. For example, if some android ViewGroup internally create View's, and you have onClickListener which listen for them.



Proxy object . One Proxy per java class.

```c++
class Button : View{
protected:
    inline static struct MethodIds : Initializable<MethodIds>{
        inline static jni_interface::UniqueGlobalClass clazz{};

         static void init(){
           JNIEnv* env = this_thread.getEnv();
           clazz = {ClassFinder::find(env, "android/widget/Button")};

           ...
           
           register_proxy<Button>(clazz.get());
         }
    } method_ids;   
}
```

To register object as proxy, just add `register_proxy` in `init`. When java side will need to make proxy it will look at class map, and create object associated with that class. If it will not found class, it will try to look its parent. If nothing found it should lands on most general class (like `View`).

`register_proxy` may accept lambda `(JavaObjectRef* jref) -> JavaObject*` as second parameter, if you need to work with something very complex, for example, with generic class.

This self-registration method relies on construction of static object beforehand. If you never ever `#include "Button.h"` , but you have `setOnClickListener`, which listen for `View` (hence `View` included), and you'll receive click from Button, Button will be created as View proxy, not as Button. This should be ok. If somewhere in application, you will cast to Button, you should include that Button first, so you'll have it constructed as a Button.

Proxies should be thin stateless wrappers around java object calls. But if you need, you may overload `MyClass(JavaObjectRef* jref, proxy_t)` constructor.  It will be called, when object will be created as proxy.

For some reasons you may need to override default proxy - you can do this with `override_proxy`.

Make all members as virtual, so you can override them latter, if necessary.



If you need to cache some values, better inherit Proxyable default class. Like:

```c++
class CachedButton : Button{
    std::string m_text;
public:
  virtual void setText(std::string text) override{
    m_text = text;
    Button::setText(text);
  }
  virtual const std::string& getText() const override{
    return m_text;
  }
}
```

And do not register `CachedButton` as proxy. `CachedButton` should be consider as object only changed by cpp side.



Do not cache any values in Proxyable object - if value will be changed on Java side - you'll never know about this.