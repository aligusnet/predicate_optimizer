#pragma once

#include <bitset>
#include <iosfwd>
#include <unordered_set>
#include <vector>

namespace predicate_optimizer {
using Bitset = std::bitset<16>;

struct Minterm {
    Minterm(Bitset bitset, Bitset mask)
        : bitset(std::move(bitset)), mask(std::move(mask)), coveredMinterms() {}

    Minterm(Bitset bitset, Bitset mask, std::vector<unsigned> coveredMinterms)
        : bitset(std::move(bitset)), mask(std::move(mask)), coveredMinterms(coveredMinterms) {}

    Bitset bitset;
    Bitset mask;
    // Indexes of covered minterms.
    std::vector<unsigned> coveredMinterms;
};

bool operator==(const Minterm& lhs, const Minterm& rhs);
std::ostream& operator<<(std::ostream& os, const Minterm& minterm);

// The Quine-McCluskey method.
std::unordered_set<Minterm> quine_mccluskey(std::vector<Minterm> minterms);

}  // namespace predicate_optimizer

namespace std {
template <>
struct hash<predicate_optimizer::Minterm> {
    using argument_type = predicate_optimizer::Minterm;
    using result_type = size_t;

    result_type operator()(const predicate_optimizer::Minterm& mt) const {
        return mt.bitset.to_ullong() ^ (mt.mask.to_ullong() << 1);
    }
};
}  // namespace std
