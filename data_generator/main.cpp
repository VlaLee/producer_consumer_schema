#include "GeneratorData.h"
#include <cstdint>

int32_t main() {
    GeneratorData generator;
    generator.fillFile("bigData.txt", 50000);

    return 0;
}