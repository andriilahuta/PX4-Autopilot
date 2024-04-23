#pragma once

#include <map>
#include <vector>


void append_range(auto& container, const auto& range) {
#ifdef __cpp_lib_containers_ranges
    container.append_range(range);
#else
    container.insert(container.end(), range.cbegin(), range.cend());
#endif
}

template<typename K, typename V>
bool map_contains(const std::map<K, V>& map, const K& key) {
#ifdef __cpp_lib_ranges_contains
    return map.contains(key)
#else
    return map.find(key) != map.end();
#endif
}

#ifdef __cpp_lib_ranges
    #include <ranges>
#else
    namespace std {
        namespace views {
            template<typename K, typename V>
            auto values(const std::map<K, V>& map) {
                std::vector<V> res;
                for (const auto& [_, val] : map) {
                    res.push_back(val);
                }
                return res;
            }
        }
    }
#endif
