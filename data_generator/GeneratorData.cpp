#include "GeneratorData.h"

#include <string>
#include <random>
#include <iostream>
#include <cctype>
#include <fstream>

const std::string GeneratorData::characters = "abcdefghijklmnopqrstuvwxyz";

GeneratorData::GeneratorData() {}

std::string GeneratorData::getRandomString(const size_t length) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(0, characters.size() - 1);

    std::string result_string;
    if (length > 0) result_string = std::toupper(characters[dist(generator)]);

    for (size_t i = 0; i < length; ++i) {
        result_string += characters[dist(generator)];
    }

    return result_string;
}

std::string GeneratorData::getRandomPhone() {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(0, 9);

    std::string result_phone = "+7";
    for (size_t i = 0; i < phoneNumberLength; ++i) {
        result_phone += std::to_string(dist(generator));
    }

    return result_phone;
}

std::string GeneratorData::getRandomRow() {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(3, 20);

    std::string result_row = getRandomString(dist(generator));

    result_row += " ";
    result_row += getRandomString(dist(generator));

    result_row += " ";
    result_row += getRandomString(dist(generator));

    result_row += " ";
    result_row += getRandomPhone();

    return result_row;
}

void GeneratorData::fillFile(const std::string file_name, const size_t countOfRows) {
    std::ofstream file(file_name);

    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл для записи" << std::endl;
        return;
    }

    for (size_t i = 0; i < countOfRows; ++i) {
        file << getRandomRow();

        if (i != countOfRows - 1) {
            file << "\n";
        }
    }
}
