#pragma once

#include <unistd.h>
#include <assert.h>

class Stream
{
public:
    virtual ssize_t read(const char *buf, size_t size) = 0;
    virtual ssize_t write(const char *buf, size_t size) = 0;

    ssize_t readFixed(const char *buf, size_t size);
    ssize_t writeFixed(const char *buf, size_t size);

    virtual int close() = 0;
};
