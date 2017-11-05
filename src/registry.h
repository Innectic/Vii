#pragma once

#include "ir_types.h"

#include <map>
#include <memory>

class Registry {
private:
    std::map<std::string, std::unique_ptr<ViiBaseType*>> registered;
public:
    const bool is_registered(const std::string& name);
    template<typename T>
    const void add(const std::string& name, const std::unique_ptr<ViiType<T>> type);
    template<class T>
    const std::unique_ptr<ViiType<T>> get(const std::string& name);
};
