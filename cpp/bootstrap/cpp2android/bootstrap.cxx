#include "../../main.h"

Application application;

// PUT YOUR user defined callbacks here
extern "C"
JNIEXPORT void JNICALL
Java_com_example_cpp2android_test_MainActivity_testNative(JNIEnv *env, jclass type) {
    __android_log_print(ANDROID_LOG_INFO, "cpp2android", "test native call");
}