#include "predicate_optimizer/bitset_algebra.h"
#include "predicate_optimizer/stream_utils.h"

namespace predicate_optimizer {

bool operator==(const Minterm& lhs, const Minterm& rhs) {
    return lhs.bitset == rhs.bitset && lhs.mask == rhs.mask;
}

std::ostream& operator<<(std::ostream& os, const Minterm& minterm) {
    os << '(' << minterm.bitset << ", " << minterm.mask << ")";
    return os;
}

bool operator==(const Maxterm& lhs, const Maxterm& rhs) {
    return lhs.minterms == rhs.minterms;
}

std::ostream& operator<<(std::ostream& os, const Maxterm& maxterm) {
    return os << maxterm.minterms;
}
}  // namespace predicate_optimizer
