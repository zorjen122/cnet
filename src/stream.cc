#include "stream.h"
#include <iostream>

ssize_t Stream::readFixed(const char *buf, size_t size)
{
    size_t count = size;
    int64_t rt = 0;
    while (count > 0)
    {
        rt = read(buf, size);
        if (rt <= 0)
            return rt;
        count -= rt;
    }

    return size;
}
ssize_t Stream::writeFixed(const char *buf, size_t size)
{
    size_t count = size;
    int64_t rt = 0;
    while (count > 0)
    {
        rt = write(buf, size);
        if (rt <= 0)
            return rt;
        count -= rt;
    }

    return size;
}
