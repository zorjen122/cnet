#pragma once

#include <memory>
#include <string>

namespace net
{

    template <class T>
    class Singleton
    {
    public:
        static T *GetInstance()
        {
            static T v;
            return &v;
        }

        static std::shared_ptr<T> GetInstancePtr()
        {
            static std::shared_ptr<T> v;
            return v;
        }
    };

    // no-mutex
    class Status
    {
    public:
        void setValue(int error, const char *errorMessage)
        {
            err = error;
            errMessage = std::string(errorMessage);
        }
        int err;
        std::string errMessage;
    };

    static thread_local Status status;

};