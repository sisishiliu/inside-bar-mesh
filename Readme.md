# InsideBar：比SideBar性能更好的Mesh架构方案
> 基于shared library的高性能service mesh agent


## 一、当前问题

Service Mesh是微服务化的趋势，但在完整的Mesh架构里，原先SDK模式的调用每一次IO在Mesh方案里都要增加四次额外的进程间通信，即便是采用UnixDomainSocket代替TCP也没有什么本质用处，无法解决其与进程内调用的性能差异，此性能问题也一直是它无法得以大规模应用的拦路虎。

那如何才能即获得 Mesh的架构解耦优势，又能不损失性能（与纯SDK模式相比）呢？在当前的主流的独立进程 sidebar-agent 方案之外，本方案介绍一种新型的 inside-bar mesh agent模式。

## 二、Inside-bar模式的设计目标

- Agent逻辑可独立更新，业务无需关注。
- Agent一套代码支持多语言（C/C++、Java、Go、PHP）。
- Agent处于业务进程内，函数级调用没有性能损耗。
- Agent支持 Go 编写，方便非C团队应用。

## 三、解决方案

简单来讲，就是把当前流行的以独立进程存在的Mesh Agent逻辑，修改为以动态加载库形式，并通过业务透明的轻SDK做好对多语言的支持。

<img src="https://user-images.githubusercontent.com/25544264/123894736-3eade080-d991-11eb-97ab-8ba584d3986b.png" width=500 />

> * **业务逻辑** 这一层就是业务逻辑，如登陆注册、列表获取、消息发送等。
> * **轻SDK** 对上提供API，其对对业务逻辑提供的API与独立进程形式的SDK完全一样。向下管理libagent.so，通过Linux的dlopen\dlsym\dlclose等API实现libagent的动态更新。
> * **libagent.so** 包括agent的所有逻辑，对轻SDK提供一组IO方法，可以分为阻塞、非阻塞两种。

下面分别以C、Java语言的业务项目为例，介绍整个逻辑的基本原理。

## 四、代码示例

### 4.1 C项目代码案例

请参考`app_main.c`、`app_libagent.c`两个文件。

```bash
$ gcc -c app_libagent.c
$ gcc -shared -o libagent.so app_libagent.o
$ gcc app_main.c -o app_main
$ ./app_main
```

### 4.2 Java项目代码案例
因为Java的JNI机制使用动态库只支持load不支持reload，所以需要对上面所述的架构做一定的变更。在轻SDK与真正的libagent.so之间，增加一个libagentproxy.so，并通过它来更新 libagent.so。

参考`SDK.java`、`sdk-java-agent.c`、`app_libagent.c`

```bash
$ gcc -c -I "/jdk/Contents/Home/include/darwin/" -I "/jdk/Contents/Home/include/" sdk-java-agent.c
$ gcc -Wl -add-stdcall-alias -shared -o sdk-java-agent.so sdk-java-agent.o
```
> gcc的-I参数请参考自己的目录。


### 4.3 采用Go组织libagent示例

> 待添加
