#include "expression.h"
#include <iomanip>
#include <sstream>

namespace predicate_optimizer {
namespace {
struct PrintVisitor {
    explicit PrintVisitor(std::ostream& os) : os(os) {}

    template <typename T>
    void operator()(const Expression&, const T& expr) {
        os << expr;
    }

    std::ostream& os;
};
}  // namespace

bool LogicalExpression::operator==(const LogicalExpression& other) const {
    if (op != other.op) {
        return false;
    }

    if (children.size() != other.children.size()) {
        return false;
    }

    for (size_t i = 0; i < children.size(); ++i) {
        if (children[i] != other.children[i]) {
            return false;
        }
    }
    return true;
}

bool ComparisonExpression::operator==(const ComparisonExpression& other) const {
    if (op != other.op) {
        return false;
    }

    if (path != other.path) {
        return false;
    }

    return this->value == other.value;
}

bool InExpression::operator==(const InExpression& other) const {
    if (op != other.op) {
        return false;
    }

    if (path != other.path) {
        return false;
    }

    if (values.size() != other.values.size()) {
        return false;
    }

    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i] != other.values[i]) {
            return false;
        }
    }

    return true;
}

bool NotExpression::operator==(const NotExpression& other) const {
    return child == other.child;
}

std::ostream& operator<<(std::ostream& os, const Expression& expr) {
    expr.visit(PrintVisitor{os});
    return os;
}

std::ostream& operator<<(std::ostream& os, const LogicalExpression& expr) {
    os << '{' << expr.op << ": [";
    for (size_t i = 0; i < expr.children.size(); ++i) {
        if (i != 0) {
            os << ", ";
        }
        os << expr.children[i];
    }
    os << "]}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const ComparisonExpression& expr) {
    os << '{' << std::quoted(expr.path) << ": {" << expr.op << ": " << std::quoted(expr.value)
       << "}}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const InExpression& expr) {
    os << '{' << std::quoted(expr.path) << ": {" << expr.op << ": [";
    for (size_t i = 0; i < expr.values.size(); ++i) {
        if (i != 0) {
            os << ", ";
        }
        os << std::quoted(expr.values[i]);
    }
    os << "]}}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const NotExpression& expr) {
    os << "{\"$not\": " << expr.child << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const LogicalOperator& op) {
    switch (op) {
        case LogicalOperator::And:
            os << std::quoted("$and");
            break;
        case LogicalOperator::Or:
            os << std::quoted("$or");
            break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const ComparisonOperator& op) {
    switch (op) {
        case ComparisonOperator::EQ:
            os << std::quoted("$eq");
            break;
        case ComparisonOperator::GE:
            os << std::quoted("$gte");
            break;
        case ComparisonOperator::GT:
            os << std::quoted("$gt");
            break;
        case ComparisonOperator::LE:
            os << std::quoted("$lte");
            break;
        case ComparisonOperator::LT:
            os << std::quoted("$lt");
            break;
        case ComparisonOperator::NE:
            os << std::quoted("$ne");
            break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const InOperator& op) {
    switch (op) {
        case InOperator::In:
            os << std::quoted("$in");
            break;
        case InOperator::NotIn:
            os << std::quoted("$nin");
            break;
    }
    return os;
}

}  // namespace predicate_optimizer
