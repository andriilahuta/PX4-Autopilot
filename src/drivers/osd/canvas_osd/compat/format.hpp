#pragma once

#ifdef __cpp_lib_format
    #include <format>
#else
    #define FMT_HEADER_ONLY 1
    #include "fmt/format-inl.h"
    namespace std {
        using fmt::format;
    }
#endif
