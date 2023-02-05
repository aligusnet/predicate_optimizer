#include "petrick.h"
#include <bitset>
#include <cassert>
namespace predicate_optimization {
namespace {
using Implicant = std::bitset<64>;

// Return true if lhs is a non-strict subset of rhs.
bool isSubset(const Implicant& lhs, const Implicant& rhs) {
    return (lhs & rhs) == lhs;
}

Implicant makeImplicant(size_t implicantIndex) {
    Implicant implicant{};
    implicant.set(implicantIndex);
    return implicant;
}

struct ImplicantTable {
    explicit ImplicantTable(const std::vector<std::vector<unsigned>>& data) {
        for (size_t implicantIndex = 0; implicantIndex < data.size(); ++implicantIndex) {
            for (auto mintermIndex : data[implicantIndex]) {
                insert(mintermIndex, implicantIndex);
            }
        }
    }

    void insert(size_t mintermIndex, size_t implicantIndex) {
        if (table.size() <= mintermIndex) {
            table.resize(mintermIndex + 1);
        }

        table[mintermIndex].emplace_back(makeImplicant(implicantIndex));
    };

    std::vector<std::vector<Implicant>> table;
};

void insertImplicant(std::vector<Implicant>& list, Implicant implicant) {
    size_t listSize = list.size();
    size_t pos = 0;
    int implicantCount = static_cast<long>(implicant.count());

    while (pos < listSize) {
        auto& current = list[pos];
        int diff = static_cast<long>(current.count()) - implicantCount;
        if (diff <= 0) {
            if (isSubset(current, implicant)) {
                // current is a non-strict subset of the new implicant, we don't need to add
                // implicant
                assert(listSize == list.size());  // no remavals could be made at this point.
                return;
            }
        } else if (diff > 0) {
            if (isSubset(implicant, current)) {
                // new implicant is a subset of the current, it means we remove the current
                --listSize;
                swap(current, list[listSize]);
                --pos;
            }
        }
        ++pos;
    }

    // erase removed elements and allocate memory for the new one if required
    list.resize(listSize + 1);
    // insert new implicant
    list[list.size() - 1] = std::move(implicant);
}

std::vector<Implicant> product(const std::vector<Implicant>& lhs,
                               const std::vector<Implicant>& rhs) {
    std::vector<Implicant> result{};
    for (const auto& l : lhs) {
        for (const auto& r : rhs) {
            auto implicant = l | r;
            insertImplicant(result, std::move(implicant));
        }
    }

    return result;
}

std::vector<unsigned> getListOfSetBits(const Implicant& implicant) {
    auto value = implicant.to_ullong();
    std::vector<unsigned> result{};
    for (size_t i = 0; i < implicant.size(); ++i) {
        if (implicant[i]) {
            result.emplace_back(i);
        }
    }
    return result;
}
}  // namespace
std::vector<std::vector<unsigned>> petrick(const std::vector<std::vector<unsigned>>& data) {
    ImplicantTable table{data};

    while (table.table.size() >= 2) {
        size_t size = table.table.size();
        auto production = product(table.table[size - 1], table.table[size - 2]);
        table.table.pop_back();
        table.table[table.table.size() - 1].swap(production);
    }

    std::vector<std::vector<unsigned>> result{};
    result.reserve(table.table.front().size());

    for (const auto& implicant : table.table.front()) {
        result.emplace_back(getListOfSetBits(implicant));
    }

    return result;
}
}  // namespace predicate_optimization
