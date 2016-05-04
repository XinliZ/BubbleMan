
#pragma once
#include <mutex>

template<typename T>
class ThreadSafeBox
{
public:
    ThreadSafeBox()
    {
        data = nullptr;
    }

    T GetData()
    {
        std::lock_guard<std::mutex> lock(mutex);
        return data;
    }

    void SetData(T input)
    {
        std::lock_guard<std::mutex> lock(mutex);
        data = input;
    }

private:
    T data;
    std::mutex mutex;
};