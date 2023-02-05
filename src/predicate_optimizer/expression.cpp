#include "expression.h"
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
    os << '(';
    for (size_t i = 0; i < expr.children.size(); ++i) {
        if (i != 0) {
            os << ' ' << expr.op << ' ';
        }
        os << expr.children[i];
    }
    os << ')';
    return os;
}

std::ostream& operator<<(std::ostream& os, const ComparisonExpression& expr) {
    os << '(' << expr.path << ' ' << expr.op << ' ' << expr.value << ')';
    return os;
}

std::ostream& operator<<(std::ostream& os, const InExpression& expr) {
    os << '(' << expr.path << ' ' << expr.op << " [";
    for (size_t i = 0; i < expr.values.size(); ++i) {
        if (i != 0) {
            os << ", ";
        }
        os << expr.values[i];
    }
    os << "])";
    return os;
}

std::ostream& operator<<(std::ostream& os, const NotExpression& expr) {
    os << '!' << expr.child;
    return os;
}

std::ostream& operator<<(std::ostream& os, const LogicalOperator& op) {
    switch (op) {
        case LogicalOperator::And:
            os << "&&";
            break;
        case LogicalOperator::Or:
            os << "||";
            break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const ComparisonOperator& op) {
    switch (op) {
        case ComparisonOperator::EQ:
            os << "==";
            break;
        case ComparisonOperator::GE:
            os << ">=";
            break;
        case ComparisonOperator::GT:
            os << ">";
            break;
        case ComparisonOperator::LE:
            os << "<=";
            break;
        case ComparisonOperator::LT:
            os << "<";
            break;
        case ComparisonOperator::NE:
            os << "!=";
            break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const InOperator& op) {
    switch (op) {
        case InOperator::In:
            os << "in";
            break;
        case InOperator::NotIn:
            os << "not in";
            break;
    }
    return os;
}

}  // namespace predicate_optimizer
