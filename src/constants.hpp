#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
enum class Style { normal, selection, inline_info };

// general version in <utility> in C++23
inline int to_underlying(Style x) { return static_cast<int>(x); }
#endif
