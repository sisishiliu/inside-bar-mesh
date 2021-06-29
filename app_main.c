#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>

// 暴露给业务方的SDK方法，同以前一样，该怎么设计怎么设计。
void mesh_agent_sdk_method() {
    // 在独立进程形式的Agent里，此时sdk会把请求通过网络rpc的形式传给另外一个进程。
    // 而此处，形式还一样，但通过函数调用的形式。
    int response = sdk_call_libagent(int request);

    // 返回或继续处理response
}

int sdk_call_libagent(int request) {
    // sdk call的主要功能有两个
    // 1. 将请求传给libagent执行
    // 2. 管理agent，判断是否加载新的libagent
    //     这里为了介绍，直接用了每次请求都加载新agent，可用的策略可参考文档。
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
            func("agent_io from main", 999);
        }

        dlclose(fHandle);
        sleep(1);
    }
    return 0;
}

// 假设这是APP的主程序
int main(int argc, char **argv)
{
    // 假设这里有无数行APP的业务代码
    

    // 现在APP开始调用轻SDK与mesh agent进行交互。
    mesh_agent_sdk_method();
}