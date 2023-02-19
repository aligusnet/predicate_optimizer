#include "predicate_optimizer/expression_dnf.h"
#include "predicate_optimizer/expression.h"

namespace predicate_optimizer {
namespace {
struct NormalFormVisitor {
    Maxterm operator()(const Expression&, const LogicalExpression& expr) {
        switch (expr.op) {
            case LogicalOperator::And:
                return processAnd(expr);
            case LogicalOperator::Or:
                return processOr(expr);
        }
    }

    Maxterm operator()(const Expression& e, const ComparisonExpression& expr) {
        if (isGreaterEqual(expr)) {
            return processLeafPredicate(e, true);
        } else {
            return processLeafPredicate(makeGreaterEqual(expr), false);
        }
    }

    Maxterm operator()(const Expression& e, const InExpression& expr) {
        switch (expr.op) {
            case InOperator::In:
                return processLeafPredicate(e, true);
            case InOperator::NotIn:
                return processLeafPredicate(
                    Expression::make<InExpression>(InOperator::In, expr.path, expr.values), false);
        }
    }

    Maxterm operator()(const Expression&, const NotExpression& expr) {
        return ~expr.child.visit(*this);
    }

    Maxterm processAnd(const LogicalExpression& expr) {
        if (expr.children.empty()) {
            return {};
        }
        auto result = expr.children.front().visit(*this);
        for (size_t i = 1; i < expr.children.size(); ++i) {
            result &= expr.children[i].visit(*this);
        }
        return result;
    }

    Maxterm processOr(const LogicalExpression& expr) {
        assert(expr.op == LogicalOperator::Or);
        Maxterm result{};
        for (const auto& child : expr.children) {
            result |= child.visit(*this);
        }
        return result;
    }

    Maxterm processLeafPredicate(const Expression& expr, bool isSet) {
        auto bitIndex = getExpressionIndex(expr);
        return {Minterm(bitIndex, isSet)};
    }

    bool isGreaterEqual(const ComparisonExpression& expr) const {
        switch (expr.op) {
            case ComparisonOperator::EQ:
                [[fallthrough]];
            case ComparisonOperator::GE:
                [[fallthrough]];
            case ComparisonOperator::GT:
                return true;
            case ComparisonOperator::LE:
                [[fallthrough]];
            case ComparisonOperator::LT:
                [[fallthrough]];
            case ComparisonOperator::NE:
                return false;
        }
    }

    Expression makeGreaterEqual(const ComparisonExpression& expr) const {
        switch (expr.op) {
            case ComparisonOperator::EQ:
                [[fallthrough]];
            case ComparisonOperator::GE:
                [[fallthrough]];
            case ComparisonOperator::GT:
                return Expression::make<ComparisonExpression>(expr);
            case ComparisonOperator::LE:
                return Expression::make<ComparisonExpression>(
                    ComparisonOperator::GT, expr.path, expr.value);
            case ComparisonOperator::LT:
                return Expression::make<ComparisonExpression>(
                    ComparisonOperator::GE, expr.path, expr.value);
            case ComparisonOperator::NE:
                return Expression::make<ComparisonExpression>(
                    ComparisonOperator::EQ, expr.path, expr.value);
        }
    }

    // maps n expression to the index of its corresponding bit.
    std::unordered_map<Expression, size_t> _map;

    std::vector<Expression> _expressions;

    size_t getExpressionIndex(const Expression& expr) {
        auto pos = _map.find(expr);
        if (pos != _map.end()) {
            return pos->second;
        }

        size_t index = _expressions.size();
        _expressions.emplace_back(expr);
        _map[expr] = index;
        return index;
    }
};

}  // namespace

std::pair<Maxterm, std::vector<Expression>> transformToNormalForm(Expression expr) {
    NormalFormVisitor visitor{};
    auto maxterm = expr.visit(visitor);
    return {maxterm, visitor._expressions};
}

}  // namespace predicate_optimizer
