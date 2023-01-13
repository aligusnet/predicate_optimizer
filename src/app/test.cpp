#include <Catch2/catch_amalgamated.hpp>
#include <mongodb/polyvalue.h>

namespace app {

struct BinaryExpression;
struct ConstantExpression;

using AST = mongodb::PolyValue<BinaryExpression, ConstantExpression>;

enum class BinaryOperator { Add, Subtract, Multiply, Divide };

struct BinaryExpression {
    BinaryExpression(BinaryOperator op, AST left, AST right)
        : op(op), left(std::move(left)), right(std::move(right)) {}

    BinaryOperator op;
    AST left;
    AST right;
};

using Value = double;

struct ConstantExpression {
    explicit ConstantExpression(Value value) : value(value) {}
    Value value;
};

struct EvalVisitor {
    Value operator()(const AST&, const BinaryExpression& node) {
        auto leftValue = node.left.visit(*this);
        auto rightValue = node.right.visit(*this);
        switch (node.op) {
            case BinaryOperator::Add:
                return leftValue + rightValue;
            case BinaryOperator::Subtract:
                return leftValue - rightValue;
            case BinaryOperator::Multiply:
                return leftValue * rightValue;
            case BinaryOperator::Divide:
                return leftValue / rightValue;
        }
    }

    Value operator()(const AST&, const ConstantExpression& node) {
        return node.value;
    }
};

Value eval(const AST& ast) {
    return ast.visit(EvalVisitor{});
}

TEST_CASE("AST", "[eval]") {
    SECTION("Plus") {
        auto ast = AST::make<BinaryExpression>(BinaryOperator::Add,
                                               AST::make<ConstantExpression>(10),
                                               AST::make<ConstantExpression>(25));
        REQUIRE_FALSE(ast.empty());
        auto result = eval(ast);
        REQUIRE_THAT(result, Catch::Matchers::WithinRel(35.0, 1e-5));
    }

    SECTION("Minus") {
        auto ast = AST::make<BinaryExpression>(BinaryOperator::Subtract,
                                               AST::make<ConstantExpression>(10),
                                               AST::make<ConstantExpression>(25));
        REQUIRE_FALSE(ast.empty());
        auto result = eval(ast);
        REQUIRE_THAT(result, Catch::Matchers::WithinRel(-15.0, 1e-8));
    }
}

}  // namespace app
