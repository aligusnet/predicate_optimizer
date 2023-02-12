#include "Catch2/catch_amalgamated.hpp"
#include "predicate_optimizer/expression.h"
#include "predicate_optimizer/expression_dnf.h"
#include "predicate_optimizer/expression_utils.h"

namespace predicate_optimizer {
TEST_CASE("DNF", "") {
    SECTION("a == 1") {
        auto expr = makeEq("a", "1");
        Maxterm expectedResult{{"1"_b, "1"_b}};
        std::unordered_map<Expression, size_t> expectedMap{
            {makeEq("a", "1"), 0},
        };

        auto [actualResult, actualMap] = transformToNormalForm(expr);
        REQUIRE(expectedResult == actualResult);
        //  REQUIRE(expectedMap == actualMap);
    }

    SECTION("a < 1") {
        auto expr = makeLt("a", "1");
        Maxterm expectedResult{{"0"_b, "1"_b}};
        std::unordered_map<Expression, size_t> expectedMap{
            {makeGe("a", "1"), 0},
        };

        auto [actualResult, actualMap] = transformToNormalForm(expr);
        REQUIRE(expectedResult == actualResult);
        REQUIRE(expectedMap == actualMap);
    }

    SECTION("a > 10 | b <= 5") {
        auto expr = makeOr({
            makeGt("a", "10"),
            makeLe("b", "5"),
        });

        Maxterm expectedResult{
            {"01"_b, "01"_b},
            {"00"_b, "10"_b},
        };
        std::unordered_map<Expression, size_t> expectedMap{
            {makeGt("a", "10"), 0},
            {makeGt("b", "5"), 1},
        };

        auto [actualResult, actualMap] = transformToNormalForm(expr);
        REQUIRE(expectedResult == actualResult);
        REQUIRE(expectedMap == actualMap);
    }

    SECTION("a > 10 | a <= 10") {
        auto expr = makeOr({
            makeGt("a", "10"),
            makeLe("a", "10"),
        });

        Maxterm expectedResult{
            {"1"_b, "1"_b},
            {"0"_b, "1"_b},
        };
        std::unordered_map<Expression, size_t> expectedMap{
            {makeGt("a", "10"), 0},
        };

        auto [actualResult, actualMap] = transformToNormalForm(expr);
        REQUIRE(expectedResult == actualResult);
        REQUIRE(expectedMap == actualMap);
    }

    SECTION("a > 10 & b <= 5") {
        auto expr = makeAnd({
            makeGt("a", "10"),
            makeLe("b", "5"),
        });

        Maxterm expectedResult{
            {"01"_b, "11"_b},
        };
        std::unordered_map<Expression, size_t> expectedMap{
            {makeGt("a", "10"), 0},
            {makeGt("b", "5"), 1},
        };

        auto [actualResult, actualMap] = transformToNormalForm(expr);
        REQUIRE(expectedResult == actualResult);
        REQUIRE(expectedMap == actualMap);
    }

    SECTION("a > 10 & a <= 10") {
        auto expr = makeAnd({
            makeGt("a", "10"),
            makeLe("a", "10"),
        });

        // a > 10 & a <= 10 can never be true
        Maxterm expectedResult{};
        std::unordered_map<Expression, size_t> expectedMap{
            {makeGt("a", "10"), 0},
        };

        auto [actualResult, actualMap] = transformToNormalForm(expr);
        REQUIRE(expectedResult == actualResult);
        REQUIRE(expectedMap == actualMap);
    }

    SECTION("(a > 1 | b > 1) & (a < 2 | b < 2)") {
        auto expr = makeAnd({
            makeOr({makeGt("a", "1"), makeGt("b", "1")}),
            makeOr({makeLt("a", "2"), makeLt("b", "2")}),
        });

        Maxterm expectedResult{
            {"0001"_b, "0101"_b},
            {"0001"_b, "1001"_b},
            {"0010"_b, "0110"_b},
            {"0010"_b, "1010"_b},
        };
        std::unordered_map<Expression, size_t> expectedMap{
            {makeGt("a", "1"), 0},
            {makeGt("b", "1"), 1},
            {makeGe("a", "2"), 2},
            {makeGe("b", "2"), 3},
        };

        auto [actualResult, actualMap] = transformToNormalForm(expr);
        REQUIRE(expectedResult == actualResult);
        REQUIRE(expectedMap == actualMap);
    }

    SECTION("(a > 1 | b > 1) & (a <= 1 | b <= 1)") {
        auto expr = makeAnd({
            makeOr({makeGt("a", "1"), makeGt("b", "1")}),
            makeOr({makeLe("a", "1"), makeLe("b", "1")}),
        });

        Maxterm expectedResult{
            {"01"_b, "11"_b},
            {"10"_b, "11"_b},
        };
        std::unordered_map<Expression, size_t> expectedMap{
            {makeGt("a", "1"), 0},
            {makeGt("b", "1"), 1},
        };

        auto [actualResult, actualMap] = transformToNormalForm(expr);
        REQUIRE(expectedResult == actualResult);
        REQUIRE(expectedMap == actualMap);
    }

    SECTION("a in [1, 2]") {
        auto expr = makeIn("a", {"1", "2"});
        Maxterm expectedResult{{"1"_b, "1"_b}};
        std::unordered_map<Expression, size_t> expectedMap{
            {makeIn("a", {"1", "2"}), 0},
        };

        auto [actualResult, actualMap] = transformToNormalForm(expr);
        REQUIRE(expectedResult == actualResult);
        REQUIRE(expectedMap == actualMap);
    }

    SECTION("a not in [1, 2]") {
        auto expr = makeNotIn("a", {"1", "2"});
        Maxterm expectedResult{{"0"_b, "1"_b}};
        std::unordered_map<Expression, size_t> expectedMap{
            {makeIn("a", {"1", "2"}), 0},
        };

        auto [actualResult, actualMap] = transformToNormalForm(expr);
        REQUIRE(expectedResult == actualResult);
        REQUIRE(expectedMap == actualMap);
    }

    SECTION("(a in [1, 2]) & (a not in [1, 2])") {
        auto expr = makeAnd({
            makeIn("a", {"1", "2"}),
            makeNotIn("a", {"1", "2"}),
        });
        Maxterm expectedResult{};
        std::unordered_map<Expression, size_t> expectedMap{
            {makeIn("a", {"1", "2"}), 0},
        };

        auto [actualResult, actualMap] = transformToNormalForm(expr);
        REQUIRE(expectedResult == actualResult);
        REQUIRE(expectedMap == actualMap);
    }

    SECTION("(a not in [1, 2]) & (a in [1, 2, 3])") {
        auto expr = makeAnd({
            makeNotIn("a", {"1", "2"}),
            makeIn("a", {"1", "2", "3"}),
        });
        Maxterm expectedResult{
            {"10"_b, "11"_b},
        };
        std::unordered_map<Expression, size_t> expectedMap{
            {makeIn("a", {"1", "2"}), 0},
            {makeIn("a", {"1", "2", "3"}), 1},
        };

        auto [actualResult, actualMap] = transformToNormalForm(expr);
        REQUIRE(expectedResult == actualResult);
        REQUIRE(expectedMap == actualMap);
    }

    SECTION("(a not in [1, 2]) | (a in [1, 2, 3])") {
        auto expr = makeOr({
            makeNotIn("a", {"1", "2"}),
            makeIn("a", {"1", "2", "3"}),
        });
        Maxterm expectedResult{
            {"00"_b, "01"_b},
            {"10"_b, "10"_b},
        };
        std::unordered_map<Expression, size_t> expectedMap{
            {makeIn("a", {"1", "2"}), 0},
            {makeIn("a", {"1", "2", "3"}), 1},
        };

        auto [actualResult, actualMap] = transformToNormalForm(expr);
        REQUIRE(expectedResult == actualResult);
        REQUIRE(expectedMap == actualMap);
    }

    SECTION("~(a > 1 | b > 1) & (a < 2 | b < 2)") {
        auto expr = makeNot(makeAnd({
            makeOr({makeGt("a", "1"), makeGt("b", "1")}),
            makeOr({makeLt("a", "2"), makeLt("b", "2")}),
        }));

        Maxterm expectedResult{
            {"0000", "0011"},
            {"1000", "1011"},
            {"0100", "0111"},
            {"1100", "1101"},
            {"1000", "1011"},
            {"1000", "1011"},
            {"1100", "1111"},
            {"1100", "1101"},
            {"0100", "0111"},
            {"1100", "1111"},
            {"0100", "0111"},
            {"1100", "1101"},
            {"1100", "1110"},
            {"1100", "1110"},
            {"1100", "1110"},
            {"1100", "1100"},
        };
        std::unordered_map<Expression, size_t> expectedMap{
            {makeGt("a", "1"), 0},
            {makeGt("b", "1"), 1},
            {makeGe("a", "2"), 2},
            {makeGe("b", "2"), 3},
        };

        auto [actualResult, actualMap] = transformToNormalForm(expr);
        REQUIRE(expectedResult == actualResult);
        REQUIRE(expectedMap == actualMap);
    }
}
}  // namespace predicate_optimizer
