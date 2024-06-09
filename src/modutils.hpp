#pragma once
#define WIN32_LEAN_AND_MEAN

#include <stdexcept>
#include <vector>

namespace modutils
{

void initialize();
void enable_hooks();
void deinitialize();

struct ScanArgs
{
    const std::string aob;
    void *address = nullptr;
    const ptrdiff_t offset = 0;
    const std::vector<std::pair<ptrdiff_t, ptrdiff_t>> relative_offsets = {};
};

void *scan(const ScanArgs &args);

void hook(void *function, void *detour, void **trampoline);

template <typename ReturnType> inline ReturnType *scan(const ScanArgs &args)
{
    return reinterpret_cast<ReturnType *>(scan(args));
}

template <typename FunctionType>
inline FunctionType *hook(const ScanArgs &args, FunctionType &detour, FunctionType *&trampoline)
{
    auto function = scan<FunctionType>(args);
    if (function == nullptr)
    {
        throw std::runtime_error("Failed to find original function address");
    }
    hook(reinterpret_cast<void *>(function), reinterpret_cast<void *>(&detour),
         reinterpret_cast<void **>(&trampoline));
    return function;
}

};