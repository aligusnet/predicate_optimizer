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

TEST_CASE("Or Push Up", "") {
    SECTION("InExpression: x in [a, b]") {
        auto expr = makeIn("x", {"a", "b"});
        auto processed = transformToDNF(expr);

        REQUIRE(expr == processed);
    }

    SECTION("ComparisonExpression: x >= 10") {
        auto expr = makeGe("x", "10");
        auto processed = transformToDNF(expr);

        REQUIRE(expr == processed);
    }

    SECTION("And Expression: x >= 10 && y < 5") {
        auto expr = makeAnd({makeGe("x", "10"), makeLt("y", "5")});
        auto processed = transformToDNF(expr);

        REQUIRE(expr == processed);
    }

    SECTION("x > 10 && (y < 10 || y > 11)") {
        auto expr = makeAnd({
            makeGt("x", "10"),
            makeOr({
                makeLt("y", "10"),
                makeGt("y", "11"),
            }),
        });

        auto expected = makeOr({
            makeAnd({
                makeGt("x", "10"),
                makeLt("y", "10"),
            }),
            makeAnd({
                makeGt("x", "10"),
                makeGt("y", "11"),
            }),
        });

        auto processed = transformToDNF(expr);

        REQUIRE(expected == processed);
    }

    SECTION("x > 5 || (x < 11 || x == 5 || x = 111) || x == 9") {
        auto expr = makeOr({
            makeGt("x", "5"),
            makeOr({
                makeLt("x", "11"),
                makeEq("x", "5"),
                makeEq("x", "111"),
            }),
            makeEq("x", "9"),
        });

        auto expected = makeOr({
            makeGt("x", "5"),
            makeLt("x", "11"),
            makeEq("x", "5"),
            makeEq("x", "111"),
            makeEq("x", "9"),
        });

        auto processed = transformToDNF(expr);
        REQUIRE(expected == processed);
    }

    SECTION(
        "(a == 1 && b == 1) && (c == 1 && (d == 2 || d == 3)) && (e == 1 || e == 2 || e == 3)") {
        auto expr = makeAnd({
            makeAnd({makeEq("a", "1"), makeEq("b", "1")}),
            makeAnd({makeEq("c", "1"),
                     makeOr({
                         makeEq("d", "2"),
                         makeEq("d", "3"),
                     })}),
            makeOr({
                makeEq("e", "1"),
                makeEq("e", "2"),
                makeEq("e", "3"),
            }),
        });

        auto expected = makeOr({
            makeAnd({
                makeEq("a", "1"),
                makeEq("b", "1"),
                makeEq("c", "1"),
                makeEq("d", "2"),
                makeEq("e", "1"),
            }),
            makeAnd({
                makeEq("a", "1"),
                makeEq("b", "1"),
                makeEq("c", "1"),
                makeEq("d", "2"),
                makeEq("e", "2"),
            }),
            makeAnd({
                makeEq("a", "1"),
                makeEq("b", "1"),
                makeEq("c", "1"),
                makeEq("d", "2"),
                makeEq("e", "3"),
            }),
            makeAnd({
                makeEq("a", "1"),
                makeEq("b", "1"),
                makeEq("c", "1"),
                makeEq("d", "3"),
                makeEq("e", "1"),
            }),
            makeAnd({
                makeEq("a", "1"),
                makeEq("b", "1"),
                makeEq("c", "1"),
                makeEq("d", "3"),
                makeEq("e", "2"),
            }),
            makeAnd({
                makeEq("a", "1"),
                makeEq("b", "1"),
                makeEq("c", "1"),
                makeEq("d", "3"),
                makeEq("e", "3"),
            }),
        });

        auto processed = transformToDNF(expr);
        REQUIRE(expected == processed);
    }

    SECTION("nested ors") {
        auto expr1 = makeOr({
            makeOr({
                makeOr({makeEq("a", "1"), makeOr({makeEq("b", "1")})}),
                makeOr({makeEq("c", "1"), makeEq("d", "1")}),
            }),
        });

        auto expr2 = makeOr({
            makeOr({
                makeOr({makeEq("a", "2"), makeEq("b", "2")}),
                makeOr({makeEq("c", "2"), makeOr({makeEq("d", "2")})}),
            }),
        });

        auto expr = makeOr({makeAnd({std::move(expr1), std::move(expr2)})});

        auto expected = makeOr({
            makeAnd({makeEq("a", "1"), makeEq("a", "2")}),
            makeAnd({makeEq("a", "1"), makeEq("b", "2")}),
            makeAnd({makeEq("a", "1"), makeEq("c", "2")}),
            makeAnd({makeEq("a", "1"), makeEq("d", "2")}),
            makeAnd({makeEq("b", "1"), makeEq("a", "2")}),
            makeAnd({makeEq("b", "1"), makeEq("b", "2")}),
            makeAnd({makeEq("b", "1"), makeEq("c", "2")}),
            makeAnd({makeEq("b", "1"), makeEq("d", "2")}),
            makeAnd({makeEq("c", "1"), makeEq("a", "2")}),
            makeAnd({makeEq("c", "1"), makeEq("b", "2")}),
            makeAnd({makeEq("c", "1"), makeEq("c", "2")}),
            makeAnd({makeEq("c", "1"), makeEq("d", "2")}),
            makeAnd({makeEq("d", "1"), makeEq("a", "2")}),
            makeAnd({makeEq("d", "1"), makeEq("b", "2")}),
            makeAnd({makeEq("d", "1"), makeEq("c", "2")}),
            makeAnd({makeEq("d", "1"), makeEq("d", "2")}),
        });

        auto processed = transformToDNF(expr);
        REQUIRE(expected == processed);
    }
}
}  // namespace predicate_optimizer
