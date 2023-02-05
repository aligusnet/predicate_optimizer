#pragma once

#include "expression.h"

namespace predicate_optimizer {
inline Expression makeOr(std::vector<Expression> children) {
    return Expression::make<LogicalExpression>(LogicalOperator::Or, std::move(children));
}

inline Expression makeAnd(std::vector<Expression> children) {
    return Expression::make<LogicalExpression>(LogicalOperator::And, std::move(children));
}

inline Expression makeEq(Path path, Value value) {
    return Expression::make<ComparisonExpression>(
        ComparisonOperator::EQ, std::move(path), std::move(value));
}

inline Expression makeNe(Path path, Value value) {
    return Expression::make<ComparisonExpression>(
        ComparisonOperator::NE, std::move(path), std::move(value));
}

inline Expression makeGt(Path path, Value value) {
    return Expression::make<ComparisonExpression>(
        ComparisonOperator::GT, std::move(path), std::move(value));
}

inline Expression makeGe(Path path, Value value) {
    return Expression::make<ComparisonExpression>(
        ComparisonOperator::GE, std::move(path), std::move(value));
}

inline Expression makeLt(Path path, Value value) {
    return Expression::make<ComparisonExpression>(
        ComparisonOperator::LT, std::move(path), std::move(value));
}

inline Expression makeLe(Path path, Value value) {
    return Expression::make<ComparisonExpression>(
        ComparisonOperator::LE, std::move(path), std::move(value));
}

inline Expression makeIn(Path path, std::vector<Value> values) {
    return Expression::make<InExpression>(InOperator::In, std::move(path), std::move(values));
}

inline Expression makeNotIn(Path path, std::vector<Value> values) {
    return Expression::make<InExpression>(InOperator::NotIn, std::move(path), std::move(values));
}

inline Expression makeNot(Expression child) {
    return Expression::make<NotExpression>(std::move(child));
}
}  // namespace predicate_optimizer
