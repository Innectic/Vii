
#include "ir.h"

const std::string ObjectInt::string() {
	return std::to_string(this->value);
}

const std::string ObjectString::string() {
	return this->value;
}
