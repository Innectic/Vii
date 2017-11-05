
#include "registry.h"

const bool Registry::is_registered(const std::string& name) {
    for (auto& entry : registered) if (entry.first == name) return true;
    return false;
}

template<typename T>
const void Registry::add(const std::string& name, const std::unique_ptr<ViiType<T>> type) {
    if (this->is_registered(name)) return;

    this->registered[name] = type;
}

template<class T>
const std::unique_ptr<ViiType<T>> Registry::get(const std::string& name) {
    return this->registered[name];
}
