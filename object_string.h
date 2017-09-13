#pragma once

#include "object.h"
#include <string>

class ObjectString : public Object<std::string> {
public:
	inline const ObjectString(const std::string& value) {
		this->value = value;
	};

	inline const std::string string() override {
		return this->get();
	}
};
