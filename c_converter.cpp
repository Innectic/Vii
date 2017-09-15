
#include "c_converter.h"

C_Converter::C_Converter() :
	fileTemplate("int main(int argc, char* argv[]) { realMain(); }\n") {
}


C_Converter::C_Converter(std::string& overrideTemplate) :
	fileTemplate(overrideTemplate) {
}

std::string C_Converter::typeToCType(const Type& type) {
	switch (type) {
	case Type::INT:
		return "int";
	case Type::STRING:
		return "std::string";
	default:
		return "void";
	}
}

void C_Converter::addImport(const std::string& lib, std::ofstream& stream) {
	std::string line = "#include " + lib + "\n";
	this->emit(line, stream);
}

void C_Converter::emitFunctions(const SourceFile& file, std::ofstream& stream) {
	for (auto function : file.functions) {
		std::string functionName = function.isMain ? "realMain" : function.name;
		std::cout << functionName << std::endl;

		std::string returnType = this->typeToCType(function.returnType);
		std::string argumentString = "";

		for (auto arg : function.arguments) {
			std::string argType = this->typeToCType(arg.type);
			argumentString += argType + " " + arg.name;
		}

		this->emit(returnType + " " + functionName + "(" + argumentString +  ") {\n", stream);
		this->emit("std::cout << \"Hello\" << std::endl;\n", stream);
		this->emit("}\n", stream);
	}
}

void C_Converter::convert(const SourceFile& file) {
	std::ofstream fileStream;
	std::string fileName = std::regex_replace(file.fileName, std::regex("\\.vii"), ".cpp");
	fileStream.open(fileName);

	this->addImport("<string>", fileStream);
	this->addImport("<iostream>", fileStream);

	this->emitFunctions(file, fileStream);
	
	this->emit(this->fileTemplate, fileStream);

	fileStream.close();
}

void C_Converter::emit(const std::string& line, std::ofstream& stream) {
	stream << line;
}
