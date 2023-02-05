#include "Catch2/catch_amalgamated.hpp"
#include "expression_rewrite.h"
#include "expression_utils.h"

namespace predicate_optimizer {
TEST_CASE("Not Removal", "") {
    SECTION("Trivial") {
        Path path("a");
        Value val{"hello"};
        auto expr = makeNot(makeEq(path, val));
        auto expectedExpr = makeNe(path, val);

        auto processedExpr = removeNotExpressions(std::move(expr));

        REQUIRE(expectedExpr == processedExpr);
    }

    SECTION("Non-trivial") {
        std::vector<Value> inValues{"p1", "p2", "p3"};
        auto and1 = makeAnd({makeEq("a", "1"), makeGt("r.t", "abcd")});
        auto and2 = makeAnd({makeLt("e", "11"), makeNe("r.t", "hello")});

        auto or1 = makeOr({std::move(and1), makeNot(std::move(and2))});
        auto and3 = makeAnd({makeIn("b", inValues), makeNot(makeIn("b", inValues))});

        auto expr = makeNot(makeAnd({std::move(or1), std::move(and3)}));

        auto expected1 = makeOr({makeNe("a", "1"), makeLe("r.t", "abcd")});
        auto expected2 = makeAnd({makeLt("e", "11"), makeNe("r.t", "hello")});
        auto expected3 = makeOr({makeNotIn("b", inValues), makeIn("b", inValues)});
        auto expectedExpr =
            makeOr({makeAnd({std::move(expected1), std::move(expected2)}), std::move(expected3)});

        auto processedExpr = removeNotExpressions(std::move(expr));

        REQUIRE(expectedExpr == processedExpr);
    }
}
}  // namespace predicate_optimizer
