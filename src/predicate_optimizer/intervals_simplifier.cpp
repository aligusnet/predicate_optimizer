#include "predicate_optimizer/intervals_simplifier.h"

#include <sstream>

namespace predicate_optimizer {
namespace {
struct IntervalBound {
    bool isInclusive{false};
    std::optional<Value> value{};
    std::optional<size_t> bitIndex;
};

// if lhs or rhs is empty, infinitySign argument defines whethere is plus or minus infinity.
int compare(const std::optional<Value>& lhs, const std::optional<Value>& rhs, int infinitySign) {
    if (!lhs && !rhs) {
        return 0;
    }

    if (!lhs) {
        return infinitySign;
    }

    if (!rhs) {
        return -infinitySign;
    }

    if (*lhs == *rhs) {
        return 0;
    }

    return *lhs > *rhs ? 1 : -1;
}

struct Interval {
    IntervalBound left;
    IntervalBound right;

    bool intersectWith(const Interval& other) {
        const int leftCmp = compare(left.value, other.left.value, -1);
        if (leftCmp == 0 && left.isInclusive) {
            left = other.left;
        } else if (leftCmp < 0) {
            left = other.left;
        }

        const int rightCmp = compare(right.value, other.right.value, 1);
        if (rightCmp == 0 && right.isInclusive) {
            right = other.right;
        } else if (rightCmp > 0) {
            right = other.right;
        }

        return !empty();
    }

    bool empty() const {
        if (!left.value || !right.value) {
            return false;
        }

        return (*left.value > *right.value) ||
            (*left.value == *right.value &&
             (left.isInclusive == false || right.isInclusive == false));
    }

    bool isPoint() const {
        if (!left.value || !right.value) {
            return false;
        }

        return *left.value == *right.value && left.isInclusive && right.isInclusive;
    }
};

std::ostream& operator<<(std::ostream& os, const Interval& interval) {
    os << (interval.left.isInclusive ? '[' : '(');
    os << (interval.left.value ? *interval.left.value : "---");
    os << ", ";
    os << (interval.right.value ? *interval.right.value : "+++");
    os << (interval.right.isInclusive ? ']' : ')');
    return os;
}

Interval makePointInterval(const Value& value, size_t bitIndex) {
    return Interval{{true, value, bitIndex}, {true, value, bitIndex}};
}

Interval makeInterval(const ComparisonExpression& cmpExpr, size_t bitIndex, bool bitValue) {
    switch (cmpExpr.op) {
        case ComparisonOperator::EQ:
            if (bitValue) {
                return makePointInterval(cmpExpr.value, bitIndex);
            } else {
                throw std::runtime_error("Cannot create an interval from NEQ");
            }
        case ComparisonOperator::GE:
            if (bitValue) {
                return Interval{{true, cmpExpr.value, bitIndex}, {}};
            } else {
                // LT
                return Interval{{}, {false, cmpExpr.value, bitIndex}};
            }
        case ComparisonOperator::GT:
            if (bitValue) {
                return Interval{{false, cmpExpr.value, bitIndex}, {}};
            } else {
                // LE
                return Interval{{}, {true, cmpExpr.value, bitIndex}};
            }
        case ComparisonOperator::LE:
            [[fallthrough]];
        case ComparisonOperator::LT:
            [[fallthrough]];
        case ComparisonOperator::NE:
            throw std::runtime_error("Unexpected negative comparison operator");
    };
}

struct IntervalData {
    Interval interval;
    std::vector<std::pair<Value, size_t>> neqs{};
};

struct Visitor {
    bool operator()(const Expression& expr,
                    const ComparisonExpression& cmpExpr,
                    size_t bitIndex,
                    bool bitValue) {
        if (cmpExpr.op == ComparisonOperator::EQ && !bitValue) {
            intervalsMap[cmpExpr.path].neqs.push_back({cmpExpr.value, bitIndex});
            return true;
        } else {
            auto interval = makeInterval(cmpExpr, bitIndex, bitValue);
            auto res = intervalsMap[cmpExpr.path].interval.intersectWith(interval);
            return res;
        }
    }

    template <typename E>
    bool operator()(const Expression& e, const E&, size_t bitIndex, bool bitValue) {
        // Ignore this expression
        minterm.set(bitIndex, bitValue);
        return true;
    }

    std::unordered_map<Path, IntervalData> intervalsMap{};
    Minterm minterm{};
};
}  // namespace
std::optional<Minterm> simplifyIntervals(const Minterm& minterm,
                                         const std::vector<Expression>& expressions) {
    Visitor visitor{};

    for (size_t i = 0; i < expressions.size(); ++i) {
        if (minterm.mask[i]) {
            const auto& expr = expressions.at(i);
            if (!expr.visit(visitor, i, minterm.bitset[i])) {
                return std::nullopt;
            }
        }
    }

    for (const auto& [path, intervalData] : visitor.intervalsMap) {
        // assert !intervalData.interval.empty()
        if (intervalData.interval.left.value) {
            visitor.minterm.set(*intervalData.interval.left.bitIndex, true);
        }
        if (intervalData.interval.right.value &&
            intervalData.interval.left.value != intervalData.interval.right.value) {
            visitor.minterm.set(*intervalData.interval.right.bitIndex, false);
        }

        for (const auto& [value, bitIndex] : intervalData.neqs) {
            Interval pointInterval = makePointInterval(value, bitIndex);
            // Check if NEQ value is intersected with the interval.
            // If the intersection is not empty we have 2 options:
            // 1. The original interval is point, then we cannot satisfy the given minterm
            // 2. The original inteval is not point, then we have to retain the NEQ point.
            // Otherwise, if not intersection we can simply ignore the NEQ point.
            if (pointInterval.intersectWith(intervalData.interval)) {
                if (intervalData.interval.isPoint()) {
                    return std::nullopt;
                } else {
                    visitor.minterm.set(bitIndex, false);
                }
            }
        }
    }

    return visitor.minterm;
}
}  // namespace predicate_optimizer
