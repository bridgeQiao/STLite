#pragma once

#include <new>
#include <cstddef>
#include <cstdlib>
#include <climits>
#include <iostream>

namespace jw
{
template <typename T>
inline T *_allocate(std::ptrdiff_t size, T *)
{
    std::set_new_handler(0);
    T *tmp = (T *)(::operator new ((size_t)(size * sizeof(T))));
    if (tmp == 0)
    {
        std::cerr << "Out of memery" << endl;
        exit(0);
    }
    return tmp;
}

template <typename T>
inline void _deallocate(T *buffer)
{
    ::operator delete(buffer);
}

template <typename T1, typename T2>
inline void _construct(T1 *p, const T2 &value)
{
    ::new (p) T1(value);
}

template <typename T>
inline void _destroy(T *ptr)
{
    ptr->~T();
}

template <typename T>
class allocator
{
public:
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef size_t size_type;
    typedef std::ptrdiff_t difference_type;

    // rebind allocator of type U
    template <typename U>
    struct rebind
    {
        typedef allocator<U> other;
    };

    // construct
    allocator() {}
    template<typename U>
    allocator(const allocator<U>&) {}

    // hind used for locality ref.
    pointer allocate(size_type n, const void *hint = 0)
    {
        return _allocate((difference_type)n, (pointer)0);
    }

    void deallocate(pointer p, size_type n)
    {
        _deallocate(p);
    }

    void construct(pointer p, const T &value)
    {
        _construct(p, value);
    }

    void destroy(pointer p) { _destroy(p); }

    pointer address(reference x) { return (pointer)&x; }

    const_pointer address(const_reference x) { return (const_pointer)&x; }

    size_type max_size() const
    {
        return size_type(UINTMAX_MAX / sizeof(T));
    }
};

} // namespace jw
