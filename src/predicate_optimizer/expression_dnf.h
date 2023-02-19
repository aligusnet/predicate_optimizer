#pragma once

#include "predicate_optimizer/bitset_algebra.h"
#include "predicate_optimizer/expression.h"
#include <unordered_map>
#include <vector>

namespace predicate_optimizer {
/* Transform the expression to disjunctive normal form. This function does not accept boolean
 * expressions containing negations.*/
std::pair<Maxterm, std::vector<Expression>> transformToNormalForm(Expression expr);
}  // namespace predicate_optimizer
