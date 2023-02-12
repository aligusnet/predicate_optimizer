#include "predicate_optimizer/bitset_algebra.h"
#include "predicate_optimizer/stream_utils.h"

namespace predicate_optimizer {

Maxterm::Maxterm() {}

Maxterm::Maxterm(std::initializer_list<Minterm> init) : minterms(std::move(init)) {}

Maxterm& Maxterm::operator|=(const Minterm& rhs) {
    minterms.emplace_back(rhs);
    return *this;
}

Maxterm Maxterm::operator~() const {
    if (minterms.empty()) {
        return {Minterm{}};
    }

    Maxterm result = ~minterms.front();
    for (size_t i = 1; i < minterms.size(); ++i) {
        result &= ~minterms[i];
    }

    return result;
}

Maxterm Minterm::operator~() const {
    Maxterm result = {};
    for (size_t i = 0; i < mask.size(); ++i) {
        if (mask[i]) {
            result |= Minterm(i, !bitset[i]);
        }
    }
    return result;
}

bool operator==(const Minterm& lhs, const Minterm& rhs) {
    return lhs.bitset == rhs.bitset && lhs.mask == rhs.mask;
}

std::ostream& operator<<(std::ostream& os, const Minterm& minterm) {
    os << '(' << minterm.bitset << ", " << minterm.mask << ")";
    return os;
}

Maxterm& Maxterm::operator|=(const Maxterm& rhs) {
    for (auto& right : rhs.minterms) {
        *this |= right;
    }
    return *this;
}

Maxterm& Maxterm::operator&=(const Maxterm& rhs) {
    Maxterm result = *this & rhs;
    minterms.swap(result.minterms);
    return *this;
}

bool operator==(const Maxterm& lhs, const Maxterm& rhs) {
    return lhs.minterms == rhs.minterms;
}

std::ostream& operator<<(std::ostream& os, const Maxterm& maxterm) {
    return os << maxterm.minterms;
}
}  // namespace predicate_optimizer
