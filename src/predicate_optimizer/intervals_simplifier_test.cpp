#include "Catch2/catch_amalgamated.hpp"
#include "predicate_optimizer/expression_utils.h"
#include "predicate_optimizer/intervals_simplifier.h"
#include "predicate_optimizer/stream_utils.h"


namespace predicate_optimizer {
TEST_CASE("intervals simplifier") {
    SECTION("a > 10 && a < 5 && b > 11") {
        Minterm minterm("101", "111");
        std::vector<Expression> expressions{
            makeGt("a", "10"),
            makeGe("a", "05"),
            makeGt("b", "11"),
        };
        std::optional<Minterm> expectedResult{};
        auto actualResult = simplifyIntervals(minterm, expressions);
        REQUIRE(expectedResult == actualResult);
    }

    SECTION("a > 10 && b < 5 && b >= -11 && c in [1, 2] && a == 11 && a < 25 && b >= 0") {
        Minterm minterm("1011101", "1111111");
        std::vector<Expression> expressions{
            makeGt("a", "10"),
            makeGe("b", "05"),
            makeGe("b", "-11"),
            makeIn("c", {"1", "2"}),
            makeEq("a", "11"),
            makeGe("a", "25"),
            makeGe("b", "0"),
        };
        Minterm expectedResult("1011000", "1011010");

        auto actualResult = simplifyIntervals(minterm, expressions);

        REQUIRE(actualResult.has_value());
        REQUIRE(*actualResult == expectedResult);
    }

    SECTION("a >= 5 && a <= 5 && a != 5") {
        Minterm minterm("001", "111");
        std::vector<Expression> expressions{
            makeGe("a", "5"),
            makeGt("a", "5"),
            makeEq("a", "5"),
        };

        std::optional<Minterm> expectedResult{};

        auto actualResult = simplifyIntervals(minterm, expressions);

        REQUIRE(expectedResult == actualResult);
    }

    SECTION("a >= 0 && a <= 9 && a != 5") {
        Minterm minterm("001", "111");
        std::vector<Expression> expressions{
            makeGe("a", "0"),
            makeGt("a", "9"),
            makeEq("a", "5"),
        };

        std::optional<Minterm> expectedResult{{"001", "111"}};

        auto actualResult = simplifyIntervals(minterm, expressions);

        REQUIRE(expectedResult == actualResult);
    }

    SECTION("a >= 0 && a > 5 && a <= 9 && a != 5") {
        Minterm minterm("0011", "1111");
        std::vector<Expression> expressions{
            makeGe("a", "0"),
            makeGt("a", "5"),
            makeGt("a", "9"),
            makeEq("a", "5"),
        };

        std::optional<Minterm> expectedResult{{"0010", "0110"}};

        auto actualResult = simplifyIntervals(minterm, expressions);

        REQUIRE(expectedResult == actualResult);
    }
}

}  // namespace predicate_optimizer
