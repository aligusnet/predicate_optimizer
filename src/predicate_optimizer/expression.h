#pragma once

#include <iosfwd>
#include <mongodb/polyvalue.h>
#include <string>
#include <vector>

namespace predicate_optimizer {

struct LogicalExpression;
struct ComparisonExpression;
struct InExpression;
struct NotExpression;
struct ConstExpression;

using Expression =
    mongodb::PolyValue<LogicalExpression, ComparisonExpression, InExpression, NotExpression>;

using Path = std::string;
using Value = std::string;

enum LogicalOperator { And, Or };

struct LogicalExpression {
    LogicalOperator op;
    std::vector<Expression> children;

    LogicalExpression(LogicalOperator op, std::vector<Expression> children)
        : op(op), children(std::move(children)) {}

    bool operator==(const LogicalExpression& other) const;
};

enum ComparisonOperator { EQ, NE, GT, GE, LE, LT };

struct ComparisonExpression {
    ComparisonOperator op;
    Path path;
    Value value;

    ComparisonExpression(ComparisonOperator op, Path path, Value value)
        : op(op), path(std::move(path)), value(std::move(value)) {}

    bool operator==(const ComparisonExpression& other) const;
};

enum InOperator { In, NotIn };
struct InExpression {
    InOperator op;
    Path path;
    std::vector<Value> values;

    InExpression(InOperator op, Path path, std::vector<Value> values)
        : op(op), path(std::move(path)), values(std::move(values)) {}

    bool operator==(const InExpression& other) const;
};

struct NotExpression {
    Expression child;

    explicit NotExpression(Expression child) : child(std::move(child)) {}

    bool compareEq(const Expression& expr) const;

    bool operator==(const NotExpression& other) const;
};

inline bool operator!=(const Expression& lhs, const Expression& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const Expression& expr);
std::ostream& operator<<(std::ostream& os, const LogicalExpression& expr);
std::ostream& operator<<(std::ostream& os, const ComparisonExpression& expr);
std::ostream& operator<<(std::ostream& os, const InExpression& expr);
std::ostream& operator<<(std::ostream& os, const NotExpression& expr);

std::ostream& operator<<(std::ostream& os, const LogicalOperator& op);
std::ostream& operator<<(std::ostream& os, const ComparisonOperator& op);
std::ostream& operator<<(std::ostream& os, const InOperator& op);

}  // namespace predicate_optimizer
