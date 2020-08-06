#ifndef JSONQUERY_UTILS_HPP
#define JSONQUERY_UTILS_HPP

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

#endif
