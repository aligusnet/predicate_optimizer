#include <sstream>
#include <vector>

namespace predicate_optimizer {

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    os << '[';
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (i != 0) {
            os << ", ";
        }
        os << v[i];
    }
    os << ']';
    return os;
}

}  // namespace predicate_optimizer
