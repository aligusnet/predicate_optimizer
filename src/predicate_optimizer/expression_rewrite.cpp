#include "expression_rewrite.h"

namespace predicate_optimizer {
namespace {
struct NotRemoval {
    Expression operator()(const Expression&, LogicalExpression& expr) {
        std::vector<Expression> children{};
        children.reserve(expr.children.size());
        for (auto&& child : expr.children) {
            children.emplace_back(child.visit(*this));
        }

        auto op = inNot ? negate(expr.op) : expr.op;
        return Expression::make<LogicalExpression>(op, std::move(children));
    }

    Expression operator()(const Expression&, ComparisonExpression& expr) {
        auto op = inNot ? negate(expr.op) : expr.op;
        return Expression::make<ComparisonExpression>(
            op, std::move(expr.path), std::move(expr.value));
    }

    Expression operator()(const Expression&, InExpression& expr) {
        auto op = inNot ? negate(expr.op) : expr.op;
        return Expression::make<InExpression>(op, std::move(expr.path), std::move(expr.values));
    }

    Expression operator()(const Expression&, NotExpression& expr) {
        inNot = !inNot;
        auto result = expr.child.visit(*this);
        inNot = !inNot;
        return result;
    }

    Expression operator()(const Expression& expr, ConstExpression&) {
        return expr;
    }

    bool inNot{false};

    static LogicalOperator negate(LogicalOperator op) {
        switch (op) {
            case LogicalOperator::And:
                return LogicalOperator::Or;
            case LogicalOperator::Or:
                return LogicalOperator::And;
        }
    }

    static ComparisonOperator negate(ComparisonOperator op) {
        switch (op) {
            case ComparisonOperator::EQ:
                return ComparisonOperator::NE;
            case ComparisonOperator::GE:
                return ComparisonOperator::LT;
            case ComparisonOperator::GT:
                return ComparisonOperator::LE;
            case ComparisonOperator::LE:
                return ComparisonOperator::GT;
            case ComparisonOperator::LT:
                return ComparisonOperator::GE;
            case ComparisonOperator::NE:
                return ComparisonOperator::EQ;
        }
    }

    static InOperator negate(InOperator op) {
        switch (op) {
            case InOperator::In:
                return InOperator::NotIn;
            case InOperator::NotIn:
                return InOperator::In;
        }
    }
};
}  // namespace

Expression removeNotExpressions(Expression root) {
    return root.visit(NotRemoval{});
}
}  // namespace predicate_optimizer
