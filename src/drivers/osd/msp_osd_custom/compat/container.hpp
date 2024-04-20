#pragma once

#include <vector>


void append_range(auto& container, const auto& range) {
#ifdef __cpp_lib_containers_ranges
	container.append_range(range);
#else
	container.insert(container.end(), range.cbegin(), range.cend());
#endif
}
