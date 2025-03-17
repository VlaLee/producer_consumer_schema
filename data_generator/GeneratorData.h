#ifndef GENERATOR_DATA_H
#define GENERATOR_DATA_H

#include <string>
#include <vector>
#include <random>

class GeneratorData {

private:
    static const std::string characters;
    static constexpr int8_t phoneNumberLength = 10;

    std::string getRandomString(const size_t length);
    std::string getRandomPhone();
    std::string getRandomRow();

public:
    GeneratorData();
    void fillFile(const std::string file_name, const size_t countOfRows);
};

#endif
