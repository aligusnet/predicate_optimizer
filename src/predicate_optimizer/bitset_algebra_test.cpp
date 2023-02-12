#include "Catch2/catch_amalgamated.hpp"
#include "predicate_optimizer/bitset_algebra.h"

namespace predicate_optimizer {
TEST_CASE("Minterm operations") {
    SECTION("a & b") {
        Minterm a{"01", "01"};
        Minterm b{"10", "10"};
        Maxterm expectedResult{{"11", "11"}};

        auto result = a & b;
        REQUIRE(expectedResult == result);
    }

    SECTION("a & ~b") {
        Minterm a{"01", "01"};
        Minterm b{"00", "10"};
        Maxterm expectedResult{{"01", "11"}};

        auto result = a & b;
        REQUIRE(expectedResult == result);
    }

    SECTION("a & ~a") {
        Minterm a{"1", "1"};
        Minterm na{"0", "1"};
        Maxterm expectedResult{};

        auto result = a & na;
        REQUIRE(expectedResult == result);
    }

    SECTION("a & a") {
        Minterm a1{"1", "1"};
        Minterm a2{"1", "1"};
        Maxterm expectedResult{{"1", "1"}};

        auto result = a1 & a2;
        REQUIRE(expectedResult == result);
    }

    SECTION("acd & b") {
        Minterm acd{"1101", "1101"};
        Minterm b{"0010", "0010"};
        Maxterm expectedResult{{"1111", "1111"}};

        auto result = acd & b;
        REQUIRE(expectedResult == result);
    }

    SECTION("ac~bd & b") {
        Minterm acnbd{"1101", "1111"};
        Minterm b{"0010", "0010"};
        Maxterm expectedResult{};

        auto result = acnbd & b;
        REQUIRE(expectedResult == result);
    }
}

TEST_CASE("Maxterm operation") {
    SECTION("AB |= c") {
        Maxterm ab{{"011", "011"}};
        Minterm c{"100", "100"};
        Maxterm expectedResult{
            {"011", "011"},
            {"100", "100"},
        };

        ab |= c;
        REQUIRE(ab == expectedResult);
    }

    SECTION("AB |= C") {
        Maxterm ab{{"011", "011"}};
        Maxterm c{{"100", "100"}};
        Maxterm expectedResult{
            {"011", "011"},
            {"100", "100"},
        };

        ab |= c;
        REQUIRE(ab == expectedResult);
    }

    SECTION("AB |= A") {
        Maxterm ab{{"11", "11"}};
        Maxterm a{{"01", "01"}};
        Maxterm expectedResult{
            {"11", "11"},
            {"01", "01"},
        };

        ab |= a;
        REQUIRE(ab == expectedResult);
    }

    SECTION("(AB | A ) |= (~AC | BD)") {
        Maxterm ab_a{
            {"0011", "0011"},
            {"0001", "0001"},
        };
        Maxterm nac_db{
            {"0100", "0101"},
            {"1001", "1010"},
        };
        Maxterm expectedResult{
            {"0011", "0011"},
            {"0001", "0001"},
            {"0100", "0101"},
            {"1001", "1010"},
        };

        ab_a |= nac_db;
        REQUIRE(ab_a == expectedResult);
    }

    SECTION("(A | B) & C") {
        Maxterm a_b{
            {"001", "001"},
            {"010", "010"},
        };

        Maxterm c{
            {"100", "100"},
        };

        Maxterm expectedResult{
            {"0101", "0101"},
            {"0110", "0110"},
        };

        auto result = a_b & c;
        REQUIRE(expectedResult == result);
    }

    SECTION("(A | B) &= C") {
        Maxterm a_b{
            {"001", "001"},
            {"010", "010"},
        };

        Maxterm c{
            {"100", "100"},
        };

        Maxterm expectedResult{
            {"0101", "0101"},
            {"0110", "0110"},
        };

        a_b &= c;
        REQUIRE(expectedResult == a_b);
    }

    SECTION("(A | B) & (C | ~D)") {
        Maxterm a_b{
            {"0001", "0001"},
            {"0010", "0010"},
        };

        Maxterm c_nd{
            {"0100", "0100"},
            {"0000", "1000"},
        };

        Maxterm expectedResult{
            {"0101", "0101"},
            {"0001", "1001"},
            {"0110", "0110"},
            {"0010", "1010"},
        };

        auto result = a_b & c_nd;
        REQUIRE(expectedResult == result);
    }
}
}  // namespace predicate_optimizer
