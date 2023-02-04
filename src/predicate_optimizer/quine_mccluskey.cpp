#include "quine_mccluskey.h"

#include <iostream>

namespace predicate_optimizer {
namespace {

// A utility class that helps to organise minterms by the number of bits set.
struct QmcTable {
    QmcTable() {}

    QmcTable(std::vector<Minterm> minterms) {
        for (auto&& bitset : minterms) {
            insert(std::move(bitset));
        }
    }

    void insert(Minterm minterm) {
        const auto count = minterm.bitset.count();
        if (table.size() <= count) {
            table.resize(count + 1);
        }
        table[count].emplace_back(std::move(minterm));
    }

    bool empty() const {
        return table.empty();
    }

    std::vector<std::vector<Minterm>> table;
};

size_t countDifferentBits(const Bitset& lhs, const Bitset& rhs) {
    return (lhs ^ rhs).count();
}

// Main step of the Quine-McCluskey method. It combines 2 midterms that differ by onnly one bit and
// build new MC table for the next step.
QmcTable combine(QmcTable& table) {
    QmcTable result{};

    for (size_t i = 0; i < table.table.size() - 1; ++i) {
        for (auto& lhs : table.table[i]) {
            for (auto& rhs : table.table[i + 1]) {
                if (lhs.enabledBits != rhs.enabledBits) {
                    continue;
                }
                auto differentBits = lhs.bitset ^ rhs.bitset;
                if (differentBits.count() == 1) {
                    lhs.combined = true;
                    rhs.combined = true;

                    result.insert(
                        Minterm{lhs.bitset & rhs.bitset, lhs.enabledBits & ~differentBits});
                }
            }
        }
    }
    return result;
}
}  // namespace

bool operator==(const Minterm& lhs, const Minterm& rhs) {
    return lhs.bitset == rhs.bitset && lhs.enabledBits == rhs.enabledBits;
}

std::ostream& operator<<(std::ostream& os, const Minterm& minterm) {
    os << '(' << minterm.bitset << ", " << minterm.enabledBits << ')';
    return os;
}

std::unordered_set<Minterm> quine_mccluskey(std::vector<Minterm> minterms) {
    QmcTable table{std::move(minterms)};
    std::unordered_set<Minterm> result{};

    while (!table.empty()) {
        auto combinedTable = combine(table);

        for (auto&& tt : table.table) {
            for (auto&& mt : tt) {
                if (!mt.combined) {
                    result.insert(std::move(mt));
                }
            }
        }

        std::swap(table, combinedTable);
    }

    return result;
}

}  // namespace predicate_optimizer
