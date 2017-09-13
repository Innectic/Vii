#pragma once

#include "object.h"
#include <string>

class ObjectInt : public Object<int> {
public:
	inline const ObjectInt(const int value) {
		this->value = value;
	}
	  
	const std::string string() override {
		return std::to_string(this->get());
	}
};
