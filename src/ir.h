#pragma once

#include <string>
#include <vector>
#include "token.h"

// This exists for the soul purpose of needing a generic type.
class AbstractObject {};

template<typename T> class Object : public AbstractObject {
protected:
	T value;
	Object() {}
public:
	inline Object(const T& value) : value(value) {
	}

	inline const T get() const {
		return this->value;
	}

	virtual const std::string string() = 0;
};

class ObjectInt : public Object<int> {
public:
	inline ObjectInt(const int& value) {
		this->value = value;
	}
	const std::string string() override;
};

class ObjectString : public Object<std::string> {
public:
	inline ObjectString(const std::string& value) {
		this->value = value;
	}
	const std::string string() override;
};

struct Decleration {
	int line;
	int column;
	TokenType type;
	
	std::string name;
	std::string scope;
};

struct Function {
	int line;
	int column;

	// TODO: This will only allow for primitives to be returned, custom objects won't work here.
	TokenType returnType;

	std::string name;
	std::vector<Decleration> arguments;

	std::string parentScope;
	std::string scope;
	bool isMain;
};

struct SourceFile {
	std::string fileName;
	int lines;
	std::vector<Function> functions;
	std::vector<Decleration> decls;
};
