#pragma once

#include <string>

class ViiBaseType {
public:
    virtual const std::string name() const = 0;
};

template<typename T>
class ViiType : public ViiBaseType {
public:
    T type;

    virtual const T value() const = 0;
};

class ViiBool : public ViiType<bool> {
private:
    bool the_value;

public:
    inline ViiBool(bool value) : the_value(value) {

    }

    const std::string name() const {
        return "bool";
    }

    const bool value() const {
        return this->the_value;
    }
};
