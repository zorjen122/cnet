#include "stream.h"
#include <iostream>

class FileStream : public Stream
{
public:
    FileStream(int fd) : file(fd) {}
    ssize_t read(const char *buf, size_t size) override
    {
        return ::read(file, (void *)buf, size);
    }
    ssize_t write(const char *buf, size_t size) override
    {
        return ::write(file, (void *)buf, size);
    }

    int close() override
    {
        return ::close(file);
    }

private:
    int file;
};

#include <sys/stat.h>
#include <fcntl.h>
int main()
{

    int fd = open("x.cc", O_RDWR);

    constexpr int size = 5;
    char buf[size] = {};
    FileStream fileStream(fd);

    auto rt = fileStream.readFixed(buf, size);
    if (rt == 0)
        std::cout << "return:0\n", exit(0);
    else if (rt == -1)
        std::cout << "return:-1\n", exit(0);

    assert(rt == size);

    std::cout << "read-buf: " << buf << "\n";
    fileStream.close();

    return 0;
}