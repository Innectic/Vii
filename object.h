#pragma once

template<typename T> class Object {
public:
	T value;
	inline Object(const T value) : value(value) {
	};
};
