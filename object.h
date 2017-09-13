#pragma once

template<typename T> class Object {
protected:
	inline const Object() {}
	T value;
public:
	inline const Object(const T& value) : value(value) {
	};

	inline const T get() const {
		return this->value;
	}

	virtual const std::string string() = 0;
};
