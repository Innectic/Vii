#pragma once

#include "exporter.h"
#include <iostream>
#include <fstream>

class Export_x64 : public Exporter {
private:
public:
    std::ofstream stream;
    const std::string name() const override;
    const void begin(const std::string& destination_file, const AST_SourceFile& source_file) override;
    const void go() override;
    const void convert_functions() override;
};
