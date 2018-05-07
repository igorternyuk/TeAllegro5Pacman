#pragma once
#include <memory>
#include <functional>

template<typename T>
using my_unique_ptr = std::unique_ptr<T,std::function<void(T*)>>;
