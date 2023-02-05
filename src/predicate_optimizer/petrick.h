#include <vector>

namespace predicate_optimization {
/**
 * Takes a coverage list of input to MQ midterms. Every element of outer vector represent the
 * coverage of the output midterm and contains indexes of the covered input midterms. Returns a list
 * of lists of output midterms, where every internal list covers all input midterms.
 */
std::vector<std::vector<unsigned>> petrick(const std::vector<std::vector<unsigned>>& data);
}  // namespace predicate_optimization
