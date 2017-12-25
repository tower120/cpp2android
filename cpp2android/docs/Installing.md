Use existing default project, or...



Add `cpp2android` module to your existing android project.

For AndroidStudio 3.0.1:

* Copy to the root of your application project (or where you think it will suits more) `cpp2android` module/folder. And `cpp` folder.

* File -> New -> Import Module `cpp2android`

* Add `cpp2android` [as dependency to your application](https://developer.android.com/studio/build/dependencies.html) . 

  In `build.gradle` file for an app module:

  ```
  apply plugin: 'com.android.application'

  android { ... }
  dependencies {
      compile project(":cpp2android")
      ...
  }
  ```

 * In your MainActivity.OnCreate add (probably at the end):

   ```java
   cpp2android.Main.OnCreate(main_layout.getContext(), main_layout);
   ```

   Where `main_layout` is ViewGroup which will be considered by cpp2android, as root layout.




You may want to move/rename cpp folder somewhere. Make `cpp2android/include_main.cmake`:

```
include(../cpp/CMakeLists.txt)
```

Or leave in cpp folder only one CMakeLists.txt, which will do redirect:

```
include(../may_path_to_cpp/CMakeLists.txt)
```



Your cpp/CMakeLists.txt is part of `native-lib` library. Use Cmake's target_... commands family to add source files, or link libraries.
