#pragma once

#include "exporter.h"
#include <iostream>
#include <fstream>
#include <vector>

class Export_x64 : public Exporter {
private:
    bool allow_native;
	std::ofstream stream;
public:
	Export_x64(bool allow_native);

    const std::string name() const override;
	const void begin(const AST_SourceFile& source_file) override;
	const void begin(const std::vector<AST_Type*> contained, AST_Function* main_function) override;
    const void go(const std::string& destination_file, const AST_SourceFile& source_file) override;
};
