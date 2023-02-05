#include "Catch2/catch_amalgamated.hpp"
#include "petrick.h"
#include <vector>

namespace predicate_optimization {
TEST_CASE("Petrick", "") {
    std::vector<std::vector<unsigned>> data{
        {0, 1},
        {0, 3},
        {1, 2},
        {3, 4},
        {2, 5},
        {4, 5},
    };

    std::vector<std::vector<unsigned>> expectedResult{
        {0, 3, 4},
        {1, 2, 3, 4},
        {1, 2, 5},
        {0, 1, 4, 5},
        {0, 2, 3, 5},
    };

    auto result = petrick(data);
    REQUIRE(expectedResult == result);
}
}  // namespace predicate_optimization
