#include "predicate_optimizer/hash.h"
#include <bitset>
#include <iosfwd>
#include <vector>

namespace predicate_optimizer {

using Bitset = std::bitset<16>;

inline Bitset operator""_b(const char* bits, size_t) {
    return Bitset{bits};
}

struct Minterm {
    Minterm() : bitset(0), mask(0){};
    Minterm(const char* bits, const char* mask) : bitset{bits}, mask{mask} {}
    Minterm(size_t bitIndex, bool val) : bitset(0), mask(0) {
        bitset.set(bitIndex, val);
        mask.set(bitIndex, true);
    }
    Minterm(Bitset bitset, Bitset mask) : bitset(bitset), mask(mask) {}
    Bitset bitset;
    Bitset mask;
};

struct Maxterm {
    Maxterm() {}

    Maxterm(std::initializer_list<Minterm> init) : minterms(std::move(init)) {}

    inline Maxterm& operator|=(const Minterm& rhs) {
        minterms.emplace_back(rhs);
        return *this;
    }

    Maxterm& operator|=(const Maxterm& rhs) {
        for (auto& right : rhs.minterms) {
            *this |= right;
        }
        return *this;
    }

    Maxterm& operator&=(const Maxterm& rhs) {
        Maxterm result = *this & rhs;
        minterms.swap(result.minterms);
        return *this;
    }

    friend Maxterm operator&(const Maxterm& lhs, const Maxterm& rhs);

    std::vector<Minterm> minterms;
};

inline Maxterm operator&(const Minterm& lhs, const Minterm& rhs) {
    const bool hasConflictingBits = ((lhs.bitset ^ rhs.bitset) & (lhs.mask & rhs.mask)).any();
    if (hasConflictingBits) {
        return {};
    }
    return {{Minterm(lhs.bitset | rhs.bitset, lhs.mask | rhs.mask)}};
}

inline Maxterm operator&(const Maxterm& lhs, const Maxterm& rhs) {
    Maxterm result{};
    result.minterms.reserve(lhs.minterms.size() * rhs.minterms.size());
    for (const auto& left : lhs.minterms) {
        for (const auto& right : rhs.minterms) {
            result |= left & right;
        }
    }
    return result;
}

bool operator==(const Minterm& lhs, const Minterm& rhs);
std::ostream& operator<<(std::ostream& os, const Minterm& minterm);
bool operator==(const Maxterm& lhs, const Maxterm& rhs);
std::ostream& operator<<(std::ostream& os, const Maxterm& maxterm);

}  // namespace predicate_optimizer

namespace std {
template <>
struct hash<predicate_optimizer::Minterm> {
    using argument_type = predicate_optimizer::Minterm;
    using result_type = size_t;

    result_type operator()(const argument_type& mt) const {
        result_type seed{3037};
        std::hash_combine(seed, mt.bitset);
        std::hash_combine(seed, mt.mask);
        return seed;
    }
};
}  // namespace std
