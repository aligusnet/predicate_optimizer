#include <vector>

namespace predicate_optimization {
/**
 * Takes a coverage list of input to MQ minterms. Every element of outer vector represent the
 * coverage of the output minterm and contains indexes of the covered input minterms. Returns a list
 * of lists of output minterms, where every internal list covers all input minterms.
 */
std::vector<std::vector<unsigned>> petrick(const std::vector<std::vector<unsigned>>& data);
}  // namespace predicate_optimization
