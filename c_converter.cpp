
#include "c_converter.h"

C_Converter::C_Converter() :
	fileTemplate("int main(int argc, char* argv[]) { real_main(argc, argv); }") {

}


C_Converter::C_Converter(std::string& overrideTemplate) :
	fileTemplate(overrideTemplate) {

}

std::string C_Converter::typeToCType(const Type& type) {
	if (type == Type::INT) return "int";
	else if (type == Type::STRING) return "std::string";
	else return NULL;
}

void C_Converter::convert(const SourceFile& file) {
	std::ofstream fileStream;
	std::string fileName = std::regex_replace(file.fileName, std::regex("\\.vii"), ".c");
	fileStream.open(fileName);
	this->emit(this->fileTemplate, fileStream);
	fileStream.close();
}

void C_Converter::emit(const std::string& line, std::ofstream& stream) {
	stream << line;
}
