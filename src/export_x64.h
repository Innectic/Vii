#pragma once

#include "exporter.h"
#include <iostream>
#include <fstream>
#include <vector>

class Export_x64 : public Exporter {
private:
    bool allow_native;
public:
    inline Export_x64(bool allow_native) : allow_native(allow_native) {

    }

    std::ofstream stream;
    const std::string name() const override;
    const void begin(const AST_SourceFile& source_file, std::ofstream& stream) override;
    const void go(const std::string& destination_file, const AST_SourceFile& source_file) override;
};