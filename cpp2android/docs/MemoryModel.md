JavaObject lifetime controlled by Java VM. They destroyed, when object's associated finalizer called by VM GC.

Each Java Object which interact with cpp part have it's own JavaObject on cpp side. Either object created from cpp (like `Button::make()`), or its passed as callback parameter from Java. If object comes from Java side and have no cpp JavaObject, proxy object is created.

On each object creation, on Java side (at `Main.addCppObject`) checked native allocated memory, and gc is called, if necessary. In this way Java's gc is aware about native memory usage.



## JavaObject

`JavaObject` represents java object on cpp side:

```c++
struct JavaObject{
  jobject WeakRefference;
  jobject GlobalRefference{nulltpr};
  std::atomic<int> use_count;
}
```

When JavaObject created, weakRefference created. If atomic counter>0 GlobalRefference created from weakRefference, if becomes 0 - GlobalRefference deleted, and JavaObject may be garbage collected.

JavaObject keeped alive by Java GlobalRefference. So you must always use SharedPtr, otherwise it may be gc. 