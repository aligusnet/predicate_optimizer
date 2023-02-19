#pragma once

#include "predicate_optimizer/bitset_algebra.h"
#include "predicate_optimizer/expression.h"
#include <optional>

namespace predicate_optimizer {
// Simplify intervals in the given minterm. Return nullopt if it is detected that under no
// conditions the mintern can be satisfied.
std::optional<Minterm> simplifyIntervals(const Minterm& minterm,
                                         const std::vector<Expression>& expressions);
}  // namespace predicate_optimizer
