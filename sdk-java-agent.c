#include <jni.h>
#include <stdio.h>
#include "SDK.h"

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>

// Implementation of native method agentIOFromJava() of SDK class
JNIEXPORT void JNICALL Java_SDK_agentIOFromJava(JNIEnv *env, jobject thisObj) {
    void *fHandle;
    void (*func)();

    while(true) {
        fHandle = dlopen("./libagent.so",RTLD_LAZY);
        if (!fHandle) {
            fprintf (stderr, "%s\n", dlerror());
            exit(1);
        }
        dlerror();

        func = (void(*)())dlsym(fHandle,"agent_io");
        if (func) {
            func("from java agent proxy", 0);
        }

        dlclose(fHandle);
        sleep(1);
    }
    return;
}