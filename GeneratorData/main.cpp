#include "GeneratorData.h"
#include <cstdint>

int32_t main() {
    GeneratorData generator;
    generator.fillFile("data.txt", 100000);

    return 0;
}