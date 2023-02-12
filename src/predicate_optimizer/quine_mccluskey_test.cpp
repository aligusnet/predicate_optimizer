#include "Catch2/catch_amalgamated.hpp"
#include <unordered_set>

#include "quine_mccluskey.h"

namespace predicate_optimizer {
TEST_CASE("Quine-mcCluskey", "") {
    SECTION("AB | A~B = A") {
        Bitset mask{"11"};
        std::vector<Minterm> minterms{
            {"10"_b, mask},
            {"11"_b, mask},
        };
        std::unordered_set<QMCResult> expectedResult{
            {"10"_b, "10"_b, {0, 1}},
        };

        auto result = quine_mccluskey(std::move(minterms));
        REQUIRE(expectedResult == result);
    }

    SECTION("ABC | A~BC = AC") {
        Bitset mask{"111"};
        std::vector<Minterm> minterms{
            {"111"_b, mask},
            {"101"_b, mask},
        };
        std::unordered_set<QMCResult> expectedResult{
            {"101"_b, "101"_b, {0, 1}},
        };

        auto result = quine_mccluskey(std::move(minterms));
        REQUIRE(expectedResult == result);
    }

    SECTION("ABC | A~BC | AB~C = AC | AB") {
        Bitset mask{"111"};
        std::vector<Minterm> minterms{
            {"111"_b, mask},
            {"101"_b, mask},
            {"110"_b, mask},
        };
        std::unordered_set<QMCResult> expectedResult{
            {"101"_b, "101"_b, {0, 1}},
            {"110"_b, "110"_b, {0, 2}},
        };

        auto result = quine_mccluskey(std::move(minterms));
        REQUIRE(expectedResult == result);
    }

    SECTION("~A~B~C~D | ~A~B~CD | ~AB~C~D | ~AB~CD = ~A~C") {
        Bitset mask{"1111"};
        std::vector<Minterm> minterms{
            {"0000"_b, mask},
            {"0001"_b, mask},
            {"0100"_b, mask},
            {"0101"_b, mask},
        };
        std::unordered_set<QMCResult> expectedResult{
            {"0000"_b, "1010"_b, {0, 1, 2, 3}},
        };

        auto result = quine_mccluskey(std::move(minterms));
        REQUIRE(expectedResult == result);
    }

    SECTION("~A~B~C~D | ~A~B~CD | ~AB~C~D | ~ABCD |~ABCD | A~BCD = A~BCD | ~ABD | ~A~C") {
        Bitset mask{"1111"};
        std::vector<Minterm> minterms{{"0000"_b, mask},
                                      {"0001"_b, mask},
                                      {"0100"_b, mask},
                                      {"0101"_b, mask},
                                      {"0111"_b, mask},
                                      {"1011"_b, mask}};
        std::unordered_set<QMCResult> expectedResult{{"1011"_b, "1111"_b, {5}},
                                                     {"0101"_b, "1101"_b, {3, 4}},
                                                     {"0000"_b, "1010"_b, {0, 1, 2, 3}}};

        auto result = quine_mccluskey(std::move(minterms));
        REQUIRE(expectedResult == result);
    }

    SECTION("") {
        Bitset mask{"111"};
        std::vector<Minterm> minterms{
            {"000"_b, mask},
            {"010"_b, mask},
            {"100"_b, mask},
            {"011"_b, mask},
            {"101"_b, mask},
            {"111"_b, mask},
        };

        std::unordered_set<QMCResult> expectedResult{
            {"000"_b, "101"_b, {0, 1}},
            {"000"_b, "011"_b, {0, 2}},
            {"010"_b, "110"_b, {1, 3}},
            {"100"_b, "110"_b, {2, 4}},
            {"011"_b, "011"_b, {3, 5}},
            {"101"_b, "101"_b, {4, 5}},
        };

        auto result = quine_mccluskey(std::move(minterms));
        REQUIRE(expectedResult == result);
    }
}
}  // namespace predicate_optimizer
