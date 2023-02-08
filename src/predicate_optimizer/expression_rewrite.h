#pragma once

#include "expression.h"

namespace predicate_optimizer {
/* Remove negate operators from the expression. */
Expression removeNotExpressions(Expression root);

/* Transform the expression to disjunctive normal form. This function does not accept boolean
 * expressions containing negations.*/
Expression transformToDNF(Expression expression);
}  // namespace predicate_optimizer
