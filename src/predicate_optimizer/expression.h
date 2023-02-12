#pragma once

#include "predicate_optimizer/hash.h"
#include <iosfwd>
#include <mongodb/polyvalue.h>
#include <string>
#include <vector>

namespace predicate_optimizer {

struct LogicalExpression;
struct ComparisonExpression;
struct InExpression;
struct NotExpression;

using Expression =
    mongodb::PolyValue<LogicalExpression, ComparisonExpression, InExpression, NotExpression>;

using Path = std::string;
using Value = std::string;

enum class LogicalOperator { And, Or };

struct LogicalExpression {
    LogicalOperator op;
    std::vector<Expression> children;

    LogicalExpression(LogicalOperator op, std::vector<Expression> children)
        : op(op), children(std::move(children)) {}

    bool operator==(const LogicalExpression& other) const;

    std::size_t hash() const {
        std::size_t seed = 1823;
        std::hash_combine(seed, op);
        std::hash_combine(seed, children);
        return seed;
    }
};

enum class ComparisonOperator { EQ, NE, GT, GE, LE, LT };

struct ComparisonExpression {
    ComparisonOperator op;
    Path path;
    Value value;

    ComparisonExpression(ComparisonOperator op, Path path, Value value)
        : op(op), path(std::move(path)), value(std::move(value)) {}

    bool operator==(const ComparisonExpression& other) const;

    std::size_t hash() const {
        std::size_t seed = 2311;
        std::hash_combine(seed, op);
        std::hash_combine(seed, path);
        std::hash_combine(seed, value);
        return seed;
    }
};

enum class InOperator { In, NotIn };
struct InExpression {
    InOperator op;
    Path path;
    std::vector<Value> values;

    InExpression(InOperator op, Path path, std::vector<Value> values)
        : op(op), path(std::move(path)), values(std::move(values)) {}

    bool operator==(const InExpression& other) const;

    std::size_t hash() const {
        std::size_t seed = 3181;
        std::hash_combine(seed, op);
        std::hash_combine(seed, path);
        std::hash_combine(seed, values);
        return seed;
    }
};

struct NotExpression {
    Expression child;

    explicit NotExpression(Expression child) : child(std::move(child)) {}

    bool compareEq(const Expression& expr) const;

    bool operator==(const NotExpression& other) const;

    std::size_t hash() const {
        std::size_t seed = 3821;
        std::hash_combine(seed, child);
        return seed;
    }
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

namespace std {
template <>
struct hash<predicate_optimizer::Expression> {
    using argument_type = predicate_optimizer::Expression;
    using result_type = std::size_t;

    result_type operator()(argument_type const& v) const {
        return v.visit(*this);
    }

    template <typename E>
    inline std::size_t operator()(const argument_type&, const E& expr) const {
        return expr.hash();
    }
};
}  // namespace std
