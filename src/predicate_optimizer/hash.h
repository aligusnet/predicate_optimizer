#pragma once

#include <vector>

namespace std {
template <class T>
inline void hash_combine(std::size_t& seed, T const& v) {
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename It>
inline void hash_range(std::size_t& seed, It first, It last) {
    for (; first != last; ++first) {
        hash_combine(seed, *first);
    }
}

template <typename It>
inline std::size_t hash_range(It first, It last) {
    size_t seed = 0;
    hash_range(seed, first, last);
    return seed;
}

template <typename T>
struct hash<vector<T>> {
    using argument_type = vector<T>;
    using result_type = std::size_t;

    result_type operator()(argument_type const& v) const {
        return hash_range(begin(v), end(v));
    }
};
}  // namespace std
