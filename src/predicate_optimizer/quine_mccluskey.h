#pragma once

#include "predicate_optimizer/bitset_algebra.h"
#include <bitset>
#include <iosfwd>
#include <unordered_set>
#include <vector>

namespace predicate_optimizer {
struct QMCResult {
    QMCResult() {}
    QMCResult(Bitset bitset, Bitset mask)
        : minterm(std::move(bitset), std::move(mask)), coveredMinterms() {}

    QMCResult(Bitset bitset, Bitset mask, std::vector<unsigned> coveredMinterms)
        : minterm(std::move(bitset), std::move(mask)), coveredMinterms(coveredMinterms) {}

    Minterm minterm;
    // Indexes of covered minterms.
    std::vector<unsigned> coveredMinterms;
};

bool operator==(const QMCResult& lhs, const QMCResult& rhs);
std::ostream& operator<<(std::ostream& os, const QMCResult& minterm);

// The Quine-McCluskey method.
std::unordered_set<QMCResult> quine_mccluskey(std::vector<Minterm> minterms);

}  // namespace predicate_optimizer

namespace std {
template <>
struct hash<predicate_optimizer::QMCResult> {
    using argument_type = predicate_optimizer::QMCResult;
    using result_type = size_t;

    result_type operator()(const argument_type& mt) const {
        return std::hash<predicate_optimizer::Minterm>{}(mt.minterm);
    }
};
}  // namespace std
