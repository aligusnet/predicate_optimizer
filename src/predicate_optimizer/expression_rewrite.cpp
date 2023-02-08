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

void move(std::vector<Expression>& target, std::vector<Expression>& source) {
    if (target.empty()) {
        target.swap(source);
    } else {
        target.reserve(target.size() + source.size());
        std::move(begin(source), end(source), std::back_inserter(target));
    }
}

struct DNFTransformer {
    Expression operator()(const Expression&, LogicalExpression& expr) {
        return processLogicalExpression(expr);
    }

    Expression processLogicalExpression(LogicalExpression& expr) {
        switch (expr.op) {
            case LogicalOperator::And:
                return processAndExpression(std::move(expr));
            case LogicalOperator::Or:
                return processOrExpression(std::move(expr));
        }
    }

    void flattenAndExprChildren(std::vector<Expression>& children) {
        std::vector<Expression> newChildren{};

        for (auto& child : children) {
            if (child.is<LogicalExpression>()) {
                auto expr = child.cast<LogicalExpression>();
                assert(expr->op == LogicalOperator::And);
                move(newChildren, expr->children);
            } else {
                newChildren.emplace_back(std::move(child));
            }
        }

        children.swap(newChildren);
    }

    void buildAndExpressions(std::vector<Expression> children,
                             const std::vector<LogicalExpression>& ors,
                             size_t orIndex,
                             std::vector<Expression>& result) {
        if (ors.size() == orIndex) {
            flattenAndExprChildren(children);
            result.push_back(
                Expression::make<LogicalExpression>(LogicalOperator::And, std::move(children)));
        } else {
            auto& orExpr = ors[orIndex];
            for (size_t childIndex = 0; childIndex < orExpr.children.size(); ++childIndex) {
                if (childIndex == 0) {
                    children.push_back(orExpr.children[childIndex]);
                } else {
                    children.back() = orExpr.children[childIndex];
                }
                buildAndExpressions(children, ors, orIndex + 1, result);
            }
        }
    }

    Expression processAndExpression(LogicalExpression&& expr) {
        std::vector<Expression> ands{};
        std::vector<LogicalExpression> ors{};

        for (auto&& child : expr.children) {
            if (child.is<LogicalExpression>()) {
                auto processed = processLogicalExpression(*child.cast<LogicalExpression>());
                auto childExpr = processed.cast<LogicalExpression>();

                switch (childExpr->op) {
                    case LogicalOperator::And:
                        move(ands, childExpr->children);
                        break;
                    case LogicalOperator::Or:
                        ors.push_back(std::move(*childExpr));
                        break;
                }
            } else {
                ands.emplace_back(std::move(child));
            }
        }

        if (ors.empty()) {
            return Expression::make<LogicalExpression>(LogicalOperator::And, std::move(ands));
        }

        std::vector<Expression> children{};
        buildAndExpressions(ands, ors, 0, children);

        return Expression::make<LogicalExpression>(LogicalOperator::Or, std::move(children));
    }

    Expression processOrExpression(LogicalExpression&& expr) {
        std::vector<Expression> children{};

        for (auto&& child : expr.children) {
            if (child.is<LogicalExpression>()) {
                auto processed = processLogicalExpression(*child.cast<LogicalExpression>());
                auto childExpr = processed.cast<LogicalExpression>();
                switch (childExpr->op) {
                    case LogicalOperator::And:
                        children.emplace_back(std::move(processed));
                    case LogicalOperator::Or:
                        move(children, childExpr->children);
                }
            } else {
                children.emplace_back(std::move(child));
            }
        }

        return Expression::make<LogicalExpression>(LogicalOperator::Or, std::move(children));
    }

    Expression operator()(const Expression&, ComparisonExpression& expr) {
        return Expression::make<ComparisonExpression>(expr);
    }

    Expression operator()(const Expression&, InExpression& expr) {
        return Expression::make<InExpression>(expr);
    }

    Expression operator()(const Expression&, NotExpression& expr) {
        throw std::runtime_error("NotExpression is not expected");
    }
};
}  // namespace

Expression removeNotExpressions(Expression root) {
    return root.visit(NotRemoval{});
}

Expression transformToDNF(Expression expression) {
    return expression.visit(DNFTransformer{});
}

}  // namespace predicate_optimizer
