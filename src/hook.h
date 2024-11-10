#pragma once
#include <dlfcn.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace hook
{
    using sys_sleep_func_t = unsigned int (*)(unsigned int);
    using sys_read_func_t = ssize_t (*)(int, void *, size_t);
    using sys_write_func_t = ssize_t (*)(int, const void *, size_t);
    using sys_send_func_t = ssize_t (*)(int, const void *, size_t, int);
    using sys_sendto_func_t = ssize_t (*)(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
    using sys_recv_func_t = ssize_t (*)(int, void *, size_t, int);

    // sys_sleep_func_t sys_sleep_func = (sys_sleep_func_t)dlsym(RTLD_NEXT, "sleep");
    // sys_read_func_t sys_read_func = (sys_read_func_t)dlsym(RTLD_NEXT, "read");
    // sys_write_func_t sys_write_func = (sys_write_func_t)dlsym(RTLD_NEXT, "write");
    // sys_send_func_t sys_send_func = (sys_send_func_t)dlsym(RTLD_NEXT, "send");
    // sys_sendto_func_t sys_sendto_func = (sys_sendto_func_t)dlsym(RTLD_NEXT, "sendto");
    // sys_recv_func_t sys_recv_func = (sys_recv_func_t)dlsym(RTLD_NEXT, "recv");

    void setEnableHook(bool flag);
    static thread_local bool enableHook = false;
    unsigned int Sleep(unsigned int second);
}