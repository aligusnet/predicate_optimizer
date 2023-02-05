#include "quine_mccluskey.h"

#include <cstddef>
#include <iostream>
#include <iterator>

namespace predicate_optimizer {
namespace {

struct MintermData {
    MintermData(Bitset bitset, Bitset mask, std::vector<unsigned> coveredMinterms)
        : bitset(std::move(bitset)),
          mask(std::move(mask)),
          coveredMinterms(std::move(coveredMinterms)),
          combined(false) {}
    Bitset bitset;
    Bitset mask;
    std::vector<unsigned> coveredMinterms;
    bool combined;
};

// A utility class that helps to organise minterms by the number of bits set.
struct QmcTable {
    QmcTable() {}

    QmcTable(std::vector<Minterm> minterms) {
        for (unsigned i = 0; i < static_cast<unsigned>(minterms.size()); ++i) {
            insert(MintermData{std::move(minterms[i].bitset),
                               std::move(minterms[i].mask),
                               std::vector<unsigned>{i}});
        }
    }

    void insert(MintermData minterm) {
        const auto count = minterm.bitset.count();
        if (table.size() <= count) {
            table.resize(count + 1);
        }
        table[count].emplace_back(std::move(minterm));
    }

    bool empty() const {
        return table.empty();
    }

    std::vector<std::vector<MintermData>> table;
};

size_t countDifferentBits(const Bitset& lhs, const Bitset& rhs) {
    return (lhs ^ rhs).count();
}

// Main step of the Quine-McCluskey method. It combines 2 minterms that differ by onnly one bit and
// build new MC table for the next step.
QmcTable combine(QmcTable& table) {
    QmcTable result{};

    for (size_t i = 0; i < table.table.size() - 1; ++i) {
        for (auto& lhs : table.table[i]) {
            for (auto& rhs : table.table[i + 1]) {
                if (lhs.mask != rhs.mask) {
                    continue;
                }
                auto differentBits = lhs.bitset ^ rhs.bitset;
                if (differentBits.count() == 1) {
                    lhs.combined = true;
                    rhs.combined = true;

                    std::vector<unsigned> coveredMinterms{};
                    coveredMinterms.reserve(lhs.coveredMinterms.size() +
                                            rhs.coveredMinterms.size());
                    std::merge(begin(lhs.coveredMinterms),
                               end(lhs.coveredMinterms),
                               begin(rhs.coveredMinterms),
                               end(rhs.coveredMinterms),
                               std::back_inserter(coveredMinterms));
                    result.insert(MintermData{lhs.bitset & rhs.bitset,
                                              lhs.mask & ~differentBits,
                                              std::move(coveredMinterms)});
                }
            }
        }
    }
    return result;
}
}  // namespace

bool operator==(const Minterm& lhs, const Minterm& rhs) {
    return lhs.bitset == rhs.bitset && lhs.mask == rhs.mask &&
        lhs.coveredMinterms == rhs.coveredMinterms;
}

std::ostream& operator<<(std::ostream& os, const Minterm& minterm) {
    os << '(' << minterm.bitset << ", " << minterm.mask << ") ";
    os << '[';
    for (size_t i = 0; i < minterm.coveredMinterms.size(); ++i) {
        if (i != 0) {
            os << ", ";
        }
        os << minterm.coveredMinterms[i];
    }
    os << ']';
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
                    result.emplace(
                        std::move(mt.bitset), std::move(mt.mask), std::move(mt.coveredMinterms));
                }
            }
        }

        std::swap(table, combinedTable);
    }

    return result;
}

}  // namespace predicate_optimizer
