#pragma once

namespace from
{

template <typename T> struct list
{
    T *elements;
    unsigned int count;

    inline list() : count(0), elements(nullptr)
    {
    }

    inline list(T *elements, unsigned int count) : elements(elements), count(count)
    {
    }

    template <size_t count> inline list(T (&t)[count]) : elements(t), count(count)
    {
    }

    inline T &operator[](size_t index)
    {
        return elements[index];
    }

    inline T *begin()
    {
        return elements;
    }

    inline T *end()
    {
        return elements + count;
    }
};

}
